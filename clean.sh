#!/bin/bash

dirs=("dijkstra" "bufcache")
for dir in ${dirs[@]}
do
    echo $dir;
    cd $dir
    make fclean
    cd ..
done