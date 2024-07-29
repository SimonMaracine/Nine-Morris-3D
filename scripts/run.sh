#! /bin/bash

./build.sh NineMorris3D

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd ..
build/nine_morris_3d/NineMorris3D
