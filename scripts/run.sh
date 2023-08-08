#! /bin/bash

clear

cd ../build
cmake --build . -j 8

if [ $? -eq 0 ]; then
    cd ..
    clear
    build/Nine-Morris-3D/Nine-Morris-3D
fi
