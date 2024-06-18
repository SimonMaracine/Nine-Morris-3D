#! /bin/bash

build_type="Debug"
distribution_mode="OFF"

case "$1" in
    "rel")
        build_type="Release"
        distribution_mode="OFF"
        ;;
    "dist")
        build_type="Release"
        distribution_mode="ON"
        ;;
esac

cd ..
mkdir -p build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=$build_type \
    -DNM3D_DISTRIBUTION_MODE=$distribution_mode
