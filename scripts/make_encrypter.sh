#! /bin/bash

cd ../encrypter_helper
mkdir -p build
cd build
cmake .. -D CMAKE_BUILD_TYPE=Release
cmake --build .
