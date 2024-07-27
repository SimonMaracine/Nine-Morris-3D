#! /bin/bash

build_directory="build"

if [ "$1" = "dist" ]; then
    build_directory="build_dist"
fi

clear
cd ../$build_directory
cmake --build . -j 10
