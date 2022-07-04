#! /bin/bash

# Notice! To be executed from the root directory of the game and with sudo.

APP_NAME="ninemorris3d"
EXECUTABLE_NAME="Nine-Morris-3D"
HELP="HELP_LINUX.txt"
SHORTCUT="ninemorris3d.desktop"
ICON="ninemorris3d.png"

EXECUTABLE_DESTINATION_FOLDER="/usr/games"
DATA_DESTINATION_FOLDER="/usr/share/$APP_NAME"
SHORTCUT_FOLDER="/usr/share/applications"
ICONS_FOLDER="/usr/share/icons/hicolor"

function check_failure() {
    if [ $? != 0 ]; then
        echo "Error. Stop installation. Check $HELP."
        exit 1
    fi
}

function copy_icon() {
    cp $current_dir/data/icons/$1/$ICON $ICONS_FOLDER/$1/apps/$ICON
    check_failure
    echo "Copied $current_dir/data/icons/$1/$ICON to $ICONS_FOLDER/$1/apps/$ICON"
}

current_dir=$(pwd)
echo "Working directory: $current_dir"

# Executable
cp $current_dir/$EXECUTABLE_NAME $EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE_NAME
check_failure
echo "Copied $current_dir/$EXECUTABLE_NAME to $EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE_NAME"

# Data folder
mkdir -p $DATA_DESTINATION_FOLDER/data
cp -r --no-target-directory $current_dir/data/ $DATA_DESTINATION_FOLDER/data/
check_failure
echo "Copied $current_dir/data/ to $DATA_DESTINATION_FOLDER/data/"

# Icons
copy_icon 512x512
copy_icon 256x256
copy_icon 128x128
copy_icon 64x64
copy_icon 32x32

gtk-update-icon-cache -f $ICONS_FOLDER

# Shortcut
touch $SHORTCUT_FOLDER/$SHORTCUT
check_failure
cat << EOF > $SHORTCUT_FOLDER/$SHORTCUT
[Desktop Entry]
Version=0.2.0
Terminal=false
Exec=$EXECUTABLE_NAME
Icon=$APP_NAME
Type=Application
Categories=Game;BoardGame;
Name=Nine Morris 3D
Comment=A Nine Men's Morris Implementation In 3D
EOF
check_failure
echo "Created shortcut $SHORTCUT_FOLDER/$SHORTCUT"

# Uninstall script
cp $current_dir/uninstall.sh $DATA_DESTINATION_FOLDER/uninstall.sh
check_failure
echo "Copied $current_dir/uninstall.sh to $DATA_DESTINATION_FOLDER/uninstall.sh"

# Help file
cp $current_dir/$HELP $DATA_DESTINATION_FOLDER/$HELP
check_failure
echo "Copied $current_dir/$HELP to $DATA_DESTINATION_FOLDER/$HELP"

echo "Done. Have a nice day. :)"
