#!/bin/bash
dir_path="."
dirs=`find $dir_path -mindepth 1 -maxdepth 1 -type d`

for dir in $dirs;
do
    echo $dir;
    cd $dir
    make fclean
    cd ..
done