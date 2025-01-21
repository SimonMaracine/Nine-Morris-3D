#! /bin/bash

BUILD_DIRECTORY="build"

if [ "$1" = "dist" ]; then
    BUILD_DIRECTORY="build_dist"
fi

clear
cd ../$BUILD_DIRECTORY
cmake --build . -j 10
