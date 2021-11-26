# Building Nine Morris 3D

## Linux release

    mkdir build-release  
    cd build-release  
    cmake .. -D CMAKE_BUILD_TYPE=Release  
    cmake --build .

## Windows release

### Visual Studio Code
    mkdir build-release  
    cd build-release  
    cmake .. -D CMAKE_BUILD_TYPE=Release -A x64  
    cmake --build . --config Release

### Visual Studio
    cmake . -D CMAKE_BUILD_TYPE=Release -A x64  

## Linux debug

    mkdir build  
    cd scripts  
    setup.sh  
    build.sh  
    # run.sh

## Windows debug

### Visual Studio Code
    mkdir build  
    cd scripts  
    Setup.bat  
    Build.bat  
    :: Run.bat

### Visual Studio
    cmake . -D CMAKE_BUILD_TYPE=Debug -A x64  
