#!/usr/bin/env python3

import os
import re
import subprocess
import sys

def is_windows():
    """Tell if we are in Windows."""
    return os.name == "nt"

def popen(op):
    """Execute a command. Return output if executed successfully or None."""
    so = b''
    try:
        proc = subprocess.Popen(op, stdout=subprocess.PIPE, universal_newlines=True)
        while True:
            line = proc.stdout.buffer.read(1)
            ret = proc.poll()
            if (not line) and (ret is not None):
                break
            so += line
            sys.stdout.buffer.write(line)
            sys.stdout.flush()
        if ret != 0:
            raise RuntimeError("command %s returned %i" % (str(op), ret))
    except OSError:
        return None
    return so

def parse_camera(op):
    """Parses camera settings string from input."""
    for ii in op.splitlines():
        match = re.match(r'.*(--camera=.*)$', ii, re.I)
        if match:
            return match.group(1)
    return None

def parse_seed(op):
    """Parse seed setting string from input."""
    for ii in op.splitlines():
        match = re.match(r'.*(--seed=\d+).*$', ii, re.I)
        if match:
            return match.group(1)
    return None

def parse_ticks(op):
    """Parse ticks setting string from input."""
    for ii in op.splitlines():
        match = re.match(r'.*(--ticks=\d+).*$', ii, re.I)
        if match:
            return match.group(1)
    return None

if __name__ == "__main__":
    mesh = ""
    camera_settings = None
    seed_settings = None
    ticks_settings = None
    additional_options = []

    ii = 1
    while ii < len(sys.argv):
        arg = sys.argv[ii]
        if arg == "-s":
            seed_settings = "--seed=%i" % (int(sys.argv[ii + 1]))
            ii += 2
        elif arg == "-r":
            additional_options += sys.argv[ii:ii+2]
            ii += 2
            continue
        elif mesh:
            raise RuntimeError("only one mesh can be previewed")
        else:
            mesh = arg
        ii += 1

    while True:
        # Run make.
        if is_windows():
            make_cmd = ["msbuild", "kerava.vcxproj", "-p:Configuration=Release"]
        else:
            make_cmd = ["make"]
        make_result = popen(make_cmd)
        if make_result is None:
            raise RuntimeError("make failed")
        # Run command.
        if is_windows():
            cmd = [".\\Release\\kerava.exe"]
        else:
            cmd = ["./kerava"]
        if mesh:
            cmd += ["-m", mesh]
        else:
            cmd += ["-d"]
        if additional_options:
            cmd += additional_options
        if camera_settings:
            cmd += [camera_settings]
        if seed_settings:
            cmd += [seed_settings]
        if ticks_settings:
            cmd += [ticks_settings]
        so = popen(cmd)
        if so is None:
            break
        # Parse amera and/or ticks settings, if they are not found, abort.
        so = so.decode()
        camera_settings = parse_camera(so)
        ticks_settings = parse_ticks(so)
        if (not camera_settings) and (not ticks_settings):
            break
        # Other settings are not obligatory.
        seed_settings = parse_seed(so)

    sys.exit(0)
