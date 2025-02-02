#! /bin/bash

./build.py dev nine_morris_3d_server

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd ..
build/nine_morris_3d_server/nine_morris_3d_server
