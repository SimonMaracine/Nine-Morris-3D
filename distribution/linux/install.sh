#! /bin/bash

SOURCE="ninemorris3d-*.*.*.tar.gz"

if [[ ! -z "$1" ]]; then
    SOURCE="$1"
fi

tar -xzf "$SOURCE" -C /tmp

BASE=$(basename $SOURCE)

cp -v -r /tmp/${BASE%.*.*}/usr/local/bin/* /usr/local/bin
cp -v -r /tmp/${BASE%.*.*}/usr/local/share/* /usr/local/share

if [[ -x "$(command -v gtk-update-icon-cache)" ]]; then
    gtk-update-icon-cache --force --ignore-theme-index /usr/local/share/icons/hicolor/
else
    echo "Please refresh your icon cache"
fi
