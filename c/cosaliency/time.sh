#!/bin/bash

START=$(date +%s)
./cosal
END=$(date +%s)
echo "c++  takes $(($END-$START)) s"

cd ../../Cosaliency;
START=$(date +%s)
matlab -nodesktop -r "cosaliency IMG_5094";
END=$(date +%s)
echo "matlab takes $(($END-$START)) s"

