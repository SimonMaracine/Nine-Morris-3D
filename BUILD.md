# Building Nine Morris 3D

## Requirements

- Git
- CMake (version >= 3.20)

### Windows

- Visual Studio (version >= 2022)

### Linux (dnf)

<!-- FIXME update these -->

- GCC (version >= 13)
- X11: libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel
- Wayland: wayland-devel libxkbcommon-devel wayland-protocols-devel extra-cmake-modules
- PipeWire: pipewire-devel.x86_64
- mesa-libGL-devel.x86_64
- libasan libubsan

### Linux (apt)

- GCC (version >= 13)
- X11: xorg-dev
- Wayland: libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules
- PulseAudio: libasound2-dev libpulse-dev
- libopengl-dev | libegl1-mesa-dev | mesa-common-dev (one of these)

## Downloading

```txt
git clone https://github.com/SimonMaracine/Nine-Morris-3D
cd Nine-Morris-3D
git submodule update --init
```

## Building

### Linux debug

```txt
cd scripts
./setup.py
./build.sh
```

### Linux distribution

```txt
cd scripts
./setup.py dist
./build.sh dist
```

### Windows debug

```txt
cd scripts
python setup.py
cd ..\build
cmake --build . --config Debug -j8
:: Or open in Visual Studio and build from there
```

### Windows distribution

```txt
cd scripts
python setup.py dist
cd ..\build
cmake --build . --config Release -j8
:: Or open in Visual Studio, change to Release mode and build from there
```
