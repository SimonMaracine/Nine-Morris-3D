# Building Nine Morris 3D

## Requirements

- Git
- CMake (version >= 3.20)

**On Linux, prefer building on Wayland instead of X11.**

### On Windows

- Visual Studio (version >= 2022)

### On Linux - Fedora

- GCC (version >= 13)
- X11: libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel
- Wayland: wayland-devel libxkbcommon-devel wayland-protocols-devel extra-cmake-modules
- PipeWire: pipewire-devel.x86_64
- mesa-libGL-devel.x86_64
- libasan libubsan

### On Linux - Ubuntu

- GCC (version >= 11)
- X11: xorg-dev
- Wayland: libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules
- PulseAudio: libasound2-dev libpulse-dev
- libopengl-dev | libegl1-mesa-dev | mesa-common-dev (one of these)

## Linux release distribution

    mkdir build-release
    cd build-release
    cmake .. -D CMAKE_BUILD_TYPE=Release -D NM3D_FOR_DISTRIBUTION=ON -D GLFW_USE_WAYLAND=OFF
    cmake --build . -j4

## Windows release distribution

### Visual Studio Code

    mkdir build
    cd build
    cmake .. -D CMAKE_BUILD_TYPE=Release -D NM3D_FOR_DISTRIBUTION=ON -A x64
    cmake --build . --config Release -j4

### Visual Studio

    mkdir build
    cd build
    cmake .. -D CMAKE_BUILD_TYPE=Release -D NM3D_FOR_DISTRIBUTION=ON -A x64
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
