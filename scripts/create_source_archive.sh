#! /bin/bash

function print_help() {
    echo "Usage: create_source_archive.sh <output_directory> <version_string> <binary_file> <icons_directory> <desktop_file> <assets_directories...>"
}

function check_argument() {
    if [[ $1 = "" ]]; then
        print_help
        exit 1
    fi
}

OUTPUT_DIRECTORY=$1
VERSION_STRING=$2
BINARY_FILE=$3
ICONS_DIRECTORY=$4
DESKTOP_FILE=$5
ASSETS_DIRECTORIES="${@: 6}"

OUTPUT_DIRECTORY=$(realpath -m $OUTPUT_DIRECTORY)
ICONS_DIRECTORY=$(realpath -m $ICONS_DIRECTORY)
ASSETS_DIRECTORIES=$(realpath -m $ASSETS_DIRECTORIES)

check_argument $VERSION_STRING
check_argument $BINARY_FILE
check_argument $ICONS_DIRECTORY
check_argument $DESKTOP_FILE

BASE=$OUTPUT_DIRECTORY/ninemorris3d-$VERSION_STRING
BIN=$BASE/usr/local/bin
ICONS=$BASE/usr/local/share/icons/hicolor
DESKTOP=$BASE/usr/local/share/applications
ASSETS=$BASE/usr/local/share/ninemorris3d

mkdir -v -p $BIN
cp -v $BINARY_FILE $BIN/$(basename $BINARY_FILE)
strip -s $BIN/$(basename $BINARY_FILE)

mkdir -v -p $ICONS/32x32/apps
cp -v $ICONS_DIRECTORY/32x32/ninemorris3d.png $ICONS/32x32/apps/ninemorris3d.png
mkdir -v -p $ICONS/64x64/apps
cp -v $ICONS_DIRECTORY/64x64/ninemorris3d.png $ICONS/64x64/apps/ninemorris3d.png
mkdir -v -p $ICONS/128x128/apps
cp -v $ICONS_DIRECTORY/128x128/ninemorris3d.png $ICONS/128x128/apps/ninemorris3d.png
mkdir -v -p $ICONS/256x256/apps
cp -v $ICONS_DIRECTORY/256x256/ninemorris3d.png $ICONS/256x256/apps/ninemorris3d.png
mkdir -v -p $ICONS/512x512/apps
cp -v $ICONS_DIRECTORY/512x512/ninemorris3d.png $ICONS/512x512/apps/ninemorris3d.png

mkdir -v -p $DESKTOP
cp -v $DESKTOP_FILE $DESKTOP/$(basename $DESKTOP_FILE)

for assets_directory in $ASSETS_DIRECTORIES; do
    mkdir -v -p $ASSETS/$(basename $assets_directory)
    cp -v -r --no-target-directory $assets_directory $ASSETS/$(basename $assets_directory)
done

tar -czf $BASE.tar.gz -C $(dirname $BASE) $(basename $BASE)
