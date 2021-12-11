#! /bin/bash

threads=$(grep -c ^processor /proc/cpuinfo)

cd ../build
cmake --build . -j $threads
