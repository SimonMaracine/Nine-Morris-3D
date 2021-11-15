#! /bin/bash

# Notice! To be executed from the root directory of the game and with sudo.

APP_NAME="ninemorris3d"
EXECUTABLE_NAME="Nine-Morris-3D"

EXECUTABLE_DESTINATION_FOLDER="/usr/games"
DATA_DESTINATION_FOLDER="/usr/share/$APP_NAME"
SHORTCUT_FOLDER="/usr/share/applications"

function check_failure() {
    if [ $? != 0 ]; then
        echo "Error. Stop installation. Check HELP.txt."
        exit 1
    fi
}

current_dir=$(pwd)
echo "Working directory: $current_dir"

cp $current_dir/$EXECUTABLE_NAME $EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE_NAME
check_failure
echo "Copied $current_dir/$EXECUTABLE_NAME to $EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE_NAME"

mkdir -p $DATA_DESTINATION_FOLDER/
cp -r $current_dir/data/ $DATA_DESTINATION_FOLDER/data/
check_failure
echo "Copied $current_dir/data/ to $DATA_DESTINATION_FOLDER/data/"

touch $SHORTCUT_FOLDER/$APP_NAME.desktop
check_failure
cat << EOF > $SHORTCUT_FOLDER/$APP_NAME.desktop
[Desktop Entry]
Version=0.1.0
Terminal=false
Exec=$EXECUTABLE_NAME
Icon=$DATA_DESTINATION_FOLDER/data/icons/icon.png
Type=Application
Categories=Game;BoardGame;
Name=Nine Morris 3D
Comment=A Nine Men's Morris Implementation In 3D
EOF
check_failure
echo "Created shortcut $SHORTCUT_FOLDER/$APP_NAME.desktop"

cp $current_dir/uninstall.sh $DATA_DESTINATION_FOLDER/uninstall.sh
check_failure
echo "Copied $current_dir/uninstall.sh to $DATA_DESTINATION_FOLDER/uninstall.sh"

echo "Done. Have a nice day. :)"
