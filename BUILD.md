# Building Nine Morris 3D

## Requirements

- Git
- CMake (version >= 3.20)

### On Windows

- Visual Studio (version >= 2022)

### On Linux

- GCC (version >= 11)
- libxrandr
- libxinerama
- libxcursor
- libxi
- libopengl-dev | libegl1-mesa-dev | mesa-common-dev (one of these)
- libasound2-dev
- libpulse-dev

## Linux release

    mkdir build-release
    cd build-release
    cmake .. -D CMAKE_BUILD_TYPE=Release
    cmake --build . -j4

## Windows release

### Visual Studio Code

    mkdir build
    cd build
    cmake .. -D CMAKE_BUILD_TYPE=Release -A x64
    cmake --build . --config Release -j4

### Visual Studio

    mkdir build
    cd build
    cmake .. -D CMAKE_BUILD_TYPE=Release -A x64
    :: Open in Visual Studio, change to Release mode and build from there

## Linux debug

    cd scripts
    setup.sh
    build.sh
    # test.sh

## Windows debug

### Visual Studio Code

    cd scripts
    Setup.bat
    Build.bat
    :: Test.bat

### Visual Studio

    mkdir build
    cd build
    cmake .. -D CMAKE_BUILD_TYPE=Debug -A x64
    :: Open in Visual Studio and build from there
