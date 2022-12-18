#! /bin/bash

# Currently working only on Ubuntu >= 20.04.

APP_NAME="ninemorris3d"
EXECUTABLE="Nine-Morris-3D"
HELP="HELP_LINUX.txt"
DESKTOP="ninemorris3d.desktop"
ICON="ninemorris3d.png"

EXECUTABLE_DESTINATION_FOLDER="/usr/local/games"
DATA_DESTINATION_FOLDER="/usr/local/share/$APP_NAME"
DESKTOP_FOLDER="/usr/local/share/applications"
ICONS_FOLDER="/usr/local/share/icons/hicolor"

error=false

function check_failure() {
    if [[ $? != 0 ]]; then
        echo "Error."
        error=true
    fi
}

function remove_icon() {
    rm -v -f "$ICONS_FOLDER/$1/apps/$ICON"
    check_failure
}

rm -v -f "$EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE"
check_failure

rm -v -rf "$DATA_DESTINATION_FOLDER/"
check_failure

remove_icon 512x512
remove_icon 256x256
remove_icon 128x128
remove_icon 64x64
remove_icon 32x32

rm -v -f "$DESKTOP_FOLDER/$DESKTOP"
check_failure

rm -v -rf "$HOME/.$APP_NAME"
check_failure

if [[ "$error" = true ]]; then
    echo "Uninstallation not successful. There were errors. Check $HELP."
else
    echo "Done. Please send feedback to simonmara.dev@gmail.com, if you didn't like the game."
fi
