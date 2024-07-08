#! /bin/bash

build_type="Debug"
distribution_mode="OFF"
asan="OFF"

case "$1" in
    "rel")
        build_type="Release"
        distribution_mode="OFF"
        asan="OFF"
        ;;
    "dist")
        build_type="Release"
        distribution_mode="ON"
        asan="OFF"
        ;;
esac

build_directory="build"

if [ "$1" = "dist" ]; then
    build_directory="build_dist"
fi

cd ..
mkdir -p $build_directory
cd $build_directory
cmake .. \
    -DCMAKE_BUILD_TYPE=$build_type \
    -DNM3D_DISTRIBUTION_MODE=$distribution_mode \
    -DNM3D_ASAN=$asan
