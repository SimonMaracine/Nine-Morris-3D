echo off

cd ..
mkdir build
cd build
cmake .. -D CMAKE_BUILD_TYPE=Debug -A x64
cd ..\scripts
