#!/bin/bash

dirs=("dijkstra" "bufcache" "mysh")

for dir in ${dirs[@]}
do
    echo $dir;
    cd $dir
    make
    make fclean
    cd ..
done