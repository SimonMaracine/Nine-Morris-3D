#! /bin/bash

target="all"

if [ ! -z "$1" ]; then
    target="$1"
fi

clear
cd ../build
cmake --build . -j 10 --target "$target"
