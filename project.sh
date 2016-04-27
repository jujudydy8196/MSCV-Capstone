#!/bin/bash

video="$1"
dir="${video%.*}"

echo 'video ' $video ' to frame...'

mkdir -p DATA/frame/$dir
mkdir -p DATA/crop/$dir
ffmpeg -i ./DATA/raw/$video -r 3 ./DATA/frame/$dir/%3d.jpg
echo $dir

cd Cosaliency; matlab -nodesktop -r "cosaliency $dir"; cd ..;
python photo.py $dir

