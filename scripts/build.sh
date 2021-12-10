#! /bin/bash

no_threads=$(rep -c ^processor /proc/cpuinfo)

cd ../build
cmake --build . -j $no_threads
