#!/usr/bin/env python3

import argparse
import os
import re
import subprocess
import sys
import wave

########################################
# Template #############################
########################################

class Template:
    """Class for templated string generation."""

    def __init__(self, content):
        """Constructor."""
        self.__content = content

    def format(self, substitutions=None):
        """Return formatted output."""
        ret = self.__content
        if substitutions:
            for kk in substitutions:
                vv = substitutions[kk].replace("\\", "\\\\")
                (ret, num) = re.subn(r'\[\[\s*%s\s*\]\]' % (kk), vv, ret)
                if not num:
                    print("WARNING: substitution '%s' has no matches" % (kk))
        unmatched = list(set(re.findall(r'\[\[([^\]]+)\]\]', ret)))
        (ret, num) = re.subn(r'\[\[[^\]]+\]\]', "", ret)
        return ret

    def __str__(self):
        """String representation."""
        return self.__content

g_header_file = Template("""#ifndef __[[HEADER]]__
#define __[[HEADER]]__\n
static uint8_t g_sample_data[] =
{
[[SAMPLE_DATA]]
};\n
static unsigned g_sample_sizes[] =
{
[[SAMPLE_SIZES]]
};\n
#endif""")

########################################
# Functions ############################
########################################

def popen(op):
    """Execute a command. Return output if executed successfully or None."""
    proc = subprocess.Popen(op)
    proc.communicate()
    if proc.returncode != 0:
        raise RuntimeError("command %s returned %i" % (str(op), proc.returncode))

def data_str(op):
    """Generate data string (80 characters wide) from input byte array."""
    ret = ""
    line = ""
    for ii in op:
        line_add = str(ii) + ","
        if len(line + line_add) > 78:
            if ret:
                ret += "\n"
            ret += line
            line = ""
        if not line:
            line = "    "
        else:
            line += " "
        line += line_add
    if ret:
        return ret + "\n" + line
    return line

########################################
# Main #################################
########################################

if __name__ == "__main__":
    program_name = os.path.basename(sys.argv[0])

    parser = argparse.ArgumentParser(usage="%s [args] <wave file(s)> [-o output]" % (program_name), description="Concatenates and combines wave files into a single opus file that is embedded into a C header.", add_help=False)
    parser.add_argument("-b", "--bitrate", default=6.0, type=float, help="Bitrate to encode to.\n(default: %(default)s)")
    parser.add_argument("-h", "--help", action="store_true", help="Print this help string and exit.")
    parser.add_argument("-o", "--output-file", default=None, help="Name of header file to generate.")
    parser.add_argument("-r", "--opus2raw", default=None, help="Path to opus2raw binary.")
    parser.add_argument("-t", "--tmpdir", default="/tmp", help="Temporary directory to save the .opus file to.\n(default: %(default)s)")
    parser.add_argument("waves", default=[], nargs="*", help="Wave file(s) to process.")

    args = parser.parse_args()

    if args.help:
        print(parser.format_help().strip())
        sys.exit(0)

    if not args.output_file:
        raise RuntimeError("output header file not defined")
    if args.output_file in args.waves:
        raise RuntimeError("cannot write output to file '%s' that is among wave files %s" %
                (args.output_file, str(args.waves)))

    audio = []
    sample_sizes = []
    output_settings = None

    for ii in args.waves:
        fd = wave.open(ii, "rb")
        settings = (fd.getnchannels(), fd.getframerate(), fd.getsampwidth())
        if output_settings and (output_settings != settings):
            raise RuntimeError("wave file '%s' has a new settings (%i vs. %i)" % (ii, str(settings), str(output_settings)))
        output_settings = settings
        nframes = fd.getnframes()
        audio += [fd.readframes(nframes)]
        sample_sizes += [int(float(nframes) / float(settings[1]) * 48000.0) ]
        fd.close()

    # Write the wave file.
    tmpfile = os.path.normpath(args.tmpdir + "/generate_sample_header.wav")
    fd = wave.open(tmpfile, "wb")
    fd.setnchannels(output_settings[0])
    fd.setframerate(output_settings[1])
    fd.setsampwidth(output_settings[2])
    for ii in audio:
        fd.writeframes(ii)
    fd.close()

    # Compress the wave.
    opusfile = os.path.normpath(args.tmpdir + "/generate_sample_header.opus")
    popen(["opusenc", "--bitrate", str(args.bitrate), "--downmix-mono", tmpfile, opusfile])

    # Convert to raw if opus2raw was provided.
    if args.opus2raw:
        (pth, ext) = os.path.splitext(opusfile)
        opusraw = pth + ".opus.raw"
        popen([args.opus2raw, "-o", opusraw, opusfile])
        opusfile = opusraw

    # Generate data blocks.
    sample_data = []
    fd = open(opusfile, "rb")
    while True:
        bb = fd.read(1)
        if not bb:
            break
        sample_data += [int.from_bytes(bb, byteorder="little", signed=False)]
    fd.close()
    sample_data_str = data_str(sample_data)
    sample_size_str = data_str(sample_sizes)

    # Write header.
    fd = open(args.output_file, "w")
    header_name = re.sub(r'\.', r'_', os.path.basename(args.output_file).lower())
    fd.write(g_header_file.format({"HEADER": header_name, "SAMPLE_DATA": sample_data_str, "SAMPLE_SIZES": sample_size_str}))
    fd.close()

    print("Wrote '%s' with %i samples: %s" % (args.output_file, len(args.waves), str(args.waves)))

    sys.exit(0)
