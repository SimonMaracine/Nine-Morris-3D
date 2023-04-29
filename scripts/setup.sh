#! /bin/bash

cd ..
mkdir -p build
cd build
cmake .. -D CMAKE_BUILD_TYPE=Debug -D GLFW_USE_WAYLAND=OFF
