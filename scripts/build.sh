#! /bin/bash

TARGET="all"

if [ ! -z "$1" ]; then
    TARGET="$1"
fi

clear
cd ../build
cmake --build . -j 10 --target "$TARGET"
