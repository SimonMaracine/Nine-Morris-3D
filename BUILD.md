# Building

## Linux

mkdir build-release  
cd build-release  
cmake .. -D CMAKE_BUILD_TYPE=Release  
cmake --build .

## Windows

mkdir build-release  
cd build-release  
cmake .. -D CMAKE_BUILD_TYPE=Release -A x64  
cmake --build . --config Release

## Debug build

Go into the scripts directory and run the appropriate scripts.

mkdir build  
cd scripts  
setup.sh  
build.sh  
run.sh
