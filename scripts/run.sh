#! /bin/bash

./build.sh

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd ..
build/nine_morris_3d/NineMorris3D
