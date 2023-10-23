#! /bin/bash

./build.sh Nine-Morris-3D

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd ..
build/Nine-Morris-3D/Nine-Morris-3D
