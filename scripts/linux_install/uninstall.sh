#! /bin/bash

APP_NAME="ninemorris3d"
EXECUTABLE_NAME="Nine-Morris-3D"
HELP="HELP_LINUX.txt"
SHORTCUT="ninemorris3d.desktop"
ICON="ninemorris3d.png"

EXECUTABLE_DESTINATION_FOLDER="/usr/games"
DATA_DESTINATION_FOLDER="/usr/share/$APP_NAME"
SHORTCUT_FOLDER="/usr/share/applications"
ICONS_FOLDER="/usr/share/icons/hicolor"

error=false

function check_failure() {
    if [ $? != 0 ]; then
        echo "Error."
        error=true
    else
        echo "$1"
    fi
}

function remove_icon() {
    rm -f $ICONS_FOLDER/$1/apps/$ICON
    check_failure "Removed $ICONS_FOLDER/$1/apps/$ICON"
}

rm -f $EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE_NAME
check_failure "Removed $EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE_NAME"

rm -rf $DATA_DESTINATION_FOLDER/
check_failure "Removed $DATA_DESTINATION_FOLDER/"

remove_icon 512x512
remove_icon 256x256
remove_icon 128x128
remove_icon 64x64
remove_icon 32x32

rm -f $SHORTCUT_FOLDER/$SHORTCUT
check_failure "Removed $SHORTCUT_FOLDER/$SHORTCUT"

rm -rf ${HOME}/.$APP_NAME
check_failure "Removed ${HOME}/.$APP_NAME"

if [ "$error" == true ]; then
    echo "Uninstallation not successful. There were errors. Check $HELP."
else
    echo "Done. Please send feedback to simonmara.dev@gmail.com, if you didn't like the game."
fi
