#! /bin/bash

threads=$(rep -c ^processor /proc/cpuinfo)

cd ../build
cmake --build . -j $threads
