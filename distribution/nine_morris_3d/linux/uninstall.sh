#! /bin/bash

rm -vf /usr/local/bin/nine_morris_3d
rm -vf /usr/local/bin/nine_morris_3d_engine_*
rm -vf /usr/local/share/applications/ninemorris3d.desktop
rm -vf /usr/local/share/icons/hicolor/*/apps/nine_morris_3d.png
rm -vrf /usr/local/share/ninemorris3d

if [[ -x "$(command -v gtk-update-icon-cache)" ]]; then
    gtk-update-icon-cache --force --ignore-theme-index /usr/local/share/icons/hicolor/
else
    echo "Please refresh your icon cache"
fi
