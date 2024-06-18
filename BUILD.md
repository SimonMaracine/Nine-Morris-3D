# Building Nine Morris 3D

## Requirements

- Git
- CMake (version >= 3.20)

### Windows

- Visual Studio (version >= 2022)

### Linux - Fedora

- GCC (version >= 13)
- X11: libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel
- Wayland: wayland-devel libxkbcommon-devel wayland-protocols-devel extra-cmake-modules
- PipeWire: pipewire-devel.x86_64
- mesa-libGL-devel.x86_64
- libasan libubsan

### Linux - Ubuntu

- GCC (version >= 13)
- X11: xorg-dev
- Wayland: libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules
- PulseAudio: libasound2-dev libpulse-dev
- libopengl-dev | libegl1-mesa-dev | mesa-common-dev (one of these)

## Linux distribution

    mkdir build-release
    cd build-release
    cmake .. -DCMAKE_BUILD_TYPE=Release -DNM3D_DISTRIBUTION_MODE=ON
    cmake --build . -j8

## Windows distribution

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DNM3D_DISTRIBUTION_MODE=ON -A x64
    cmake --build . --config Release -j8
    :: Or open in Visual Studio, change to Release mode and build from there

## Linux debug

    cd scripts
    setup.sh <type>
    build.sh
    # test.sh

## Windows debug

    cd scripts
    setup.bat
    cmake --build . --config Release -j8
    :: Or open in Visual Studio and build from there
