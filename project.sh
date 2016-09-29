#!/bin/bash

video="$1"
dir="${video%.*}"
fps=$2

#echo 'video ' $video ' to frame...'

#mkdir -p DATA/frame/$dir
#mkdir -p DATA/crop/$dir
#START=$(date +%s)
##HACK: fps
#ffmpeg -i ./DATA/raw/$video -r $fps ./DATA/frame/$dir/%3d.jpg
#END=$(date +%s)
#echo "transfer Video takes $(($END-$START)) s"

echo $dir

START=$(date +%s)
cd Cosaliency; matlab -nodesktop -r "cosaliency $dir"; cd ..;
END=$(date +%s)
echo "compute Cosal takes $(($END-$START)) s"

START=$(date +%s)
python photo.py $dir
END=$(date +%s)
echo "python takes $(($END-$START)) s"

