#!/bin/bash

dir=$1/*
for f in $dir
do
    echo "proceeding ${f##*/} ..."
    ./project.sh ${f##*/}
done
