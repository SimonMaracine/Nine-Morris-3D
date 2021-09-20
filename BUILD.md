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
