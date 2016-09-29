#!/bin/bash

dir=$1/*
fps=$2

for f in $dir
do
    echo "proceeding ${f##*/} ..."
    ./project.sh ${f##*/} $fps
done
