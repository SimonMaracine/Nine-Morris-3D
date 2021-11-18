#! /bin/bash

APP_NAME="ninemorris3d"
EXECUTABLE_NAME="Nine-Morris-3D"
HELP="HELP.txt"

EXECUTABLE_DESTINATION_FOLDER="/usr/games"
DATA_DESTINATION_FOLDER="/usr/share/$APP_NAME"
SHORTCUT_FOLDER="/usr/share/applications"

error=false

function check_failure() {
    if [ $? != 0 ]; then
        echo "Error."
        error=true
    else
        echo "$1"
    fi
}

rm -f $EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE_NAME
check_failure "Removed $EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE_NAME"

rm -rf $DATA_DESTINATION_FOLDER/
check_failure "Removed $DATA_DESTINATION_FOLDER/"

rm -f $SHORTCUT_FOLDER/$APP_NAME.desktop
check_failure "Removed $SHORTCUT_FOLDER/$APP_NAME.desktop"

rm -rf /home/$(logname)/.$APP_NAME
check_failure "Removed /home/$(logname)/.$APP_NAME"

if [ "$error" == true ]; then
    echo "Uninstallation not successful. There were errors. Check $HELP."
else
    echo "Done. Please send feedback to simonmara.dev@gmail.com, if you didn't like the game."
fi
