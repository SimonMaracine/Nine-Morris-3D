# Building Nine Morris 3D

## Requirements

- Git (downloading dependencies)
- Python (meta build system)
- CMake (build system)
- Rust (building engines)
- Visual Studio (build environment - Windows only)
- Packages (dependencies - Linux only)

### Linux packages - dnf

<!-- FIXME update these -->

- GCC: ?
- X11: libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel libXext-devel libXfixes-devel libXrender-devel
- Wayland: wayland-devel libxkbcommon-devel wayland-protocols-devel extra-cmake-modules
- PipeWire: pipewire-devel.x86_64
- OpenGL: mesa-libGL-devel.x86_64
- Asan: libasan libubsan

### Linux packages - apt

- GCC: build-essential
- X11: xorg-dev
- Wayland: libwayland-dev libxkbcommon-dev (wayland-protocols extra-cmake-modules)? libdecor.x86_64
- PulseAudio: libasound2-dev libpulse-dev
- OpenGL: libopengl-dev | libegl1-mesa-dev | mesa-common-dev (one of these)
- zlib: zlib1g-dev
- package-config: pkgconf

## Testing

For building it is usually used the latest toolchain available for the platform.

- Building was last tested on GCC `14.2`, MSVC `19.42` and Rust `1.82`. Support for `C++20` is required.
- Build system was last tested on CMake `3.30` (and requires at least `3.20`).
- Scripts were last tested on Python `3.13`.
- The actual game was last tested on `Fedora Linux 41 (GNOME)`, `Ubuntu Linux 24.10 (GNOME)` and `Windows 10`.

## Downloading

### Linux

```txt
git clone https://github.com/SimonMaracine/Nine-Morris-3D
cd Nine-Morris-3D/scripts
./download_dependencies.py
```

### Windows

```txt
git clone https://github.com/SimonMaracine/Nine-Morris-3D
cd Nine-Morris-3D\scripts
python download_dependencies.py
```

## Building

There are two types of builds:

- Development, where the game is compiled with debug or release mode and it is run within the project;
- Distribution, where the game is compiled with release mode, any additional debug code is stripped and it is
  run from clients' systems (after installation).

### Linux development

```txt
cd scripts
./build_engines.py
./setup.py
./build.py
```

### Linux distribution

```txt
cd scripts
./build_engines.py dist
./setup.py dist
./build.py dist
```

### Windows development

```txt
cd scripts
python build_engines.py
python setup.py
python build.py
:: Or open in Visual Studio and build from there
```

### Windows distribution

```txt
cd scripts
python build_engines.py dist
python setup.py dist
python build.py dist
:: Or open in Visual Studio, change to Release mode and build from there
```

Note: on Windows, you have to rerun setup.py in order to change from development to distribution and back.

## Running

### Linux development

```txt
cd scripts
./run.sh
./run_server.sh
```

### Windows development

From within Visual Studio.

## Caveats

During build there may be warnings emitted.

Running the game is supposed to work only through the scripts on Linux development and only within Visual Studio
on Windows development. Similarly, in distribution, the game is supposed to be installed in the system in order
to work. The reason for this is that it expects the assets and the engines to be in very specific locations.

On Linux, currently, only the X11 protocol is supported.
