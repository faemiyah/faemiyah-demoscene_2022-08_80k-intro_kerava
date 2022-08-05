#!/bin/bash

output_filename=${1:-"combined.wav"}
info_filename="$output_filename.txt"

echo "Combining the .wav files in the directory and writing sample per file counts to $info_filename"

if [ -f $output_filename ]; then
    rm $output_filename
fi
if [ -f $info_filename ]; then
    rm $info_filename
fi

# Try to calculate the number of samples in each wave file
for f in `ls ./*.wav |sort -g`; do
  len_in_secs=$(ffprobe -v error -select_streams a:0 -show_entries format=duration -of default=nokey=1:noprint_wrappers=1 "$f")
  sample_count=`echo $len_in_secs|awk '{print 22050 * $1}'`
  echo "$f => $sample_count samples"
  echo "$f => $sample_count samples" >> $info_filename
done

echo "Concatenating to $output_filename"
# Generate a combined wave by concatenating the files in the directory
ffmpeg -f concat -safe 0 -i <(for f in `ls ./*.wav |sort -g`; do echo "file '$PWD/$f'"; done) -c copy $output_filename

echo "Done."


