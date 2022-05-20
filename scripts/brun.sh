#! /bin/bash

clear

threads=$(grep -c ^processor /proc/cpuinfo)

cd ../build
cmake --build . -j $threads

if [ $? -eq 0 ]; then
    cd ..
    clear
    build/Nine-Morris-3D
fi
