echo off

cd ..
cmake . -D CMAKE_BUILD_TYPE=Debug -A x64
cd scripts
