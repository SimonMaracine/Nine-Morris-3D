#! /bin/bash

cd ..
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DGLFW_USE_WAYLAND=OFF
