#! /bin/bash

# Notice! Execute this from the root directory of the game and with sudo.
# Currently working only on Ubuntu version >= 20.04.

APP_NAME="ninemorris3d"
EXECUTABLE="Nine-Morris-3D"
HELP="HELP_LINUX.txt"
DESKTOP="ninemorris3d.desktop"
ICON="ninemorris3d.png"

EXECUTABLE_DESTINATION_FOLDER="/usr/local/games"
DATA_DESTINATION_FOLDER="/usr/local/share/$APP_NAME"
DESKTOP_FOLDER="/usr/local/share/applications"
ICONS_FOLDER="/usr/local/share/icons/hicolor"

function check_failure() {
    if [[ $? != 0 ]]; then
        echo "Error. Stop installation. Check $HELP."
        exit 1
    fi
}

function copy_icon() {
    mkdir -p "$ICONS_FOLDER/$1/apps/"
    cp -v "$current_dir/data/icons/$1/$ICON" "$ICONS_FOLDER/$1/apps/$ICON"
    check_failure
}

current_dir=$(pwd)
echo "Current working directory: $current_dir"

# Executable
cp -v "$current_dir/$EXECUTABLE" "$EXECUTABLE_DESTINATION_FOLDER/$EXECUTABLE"
check_failure

# Data folder
mkdir -p "$DATA_DESTINATION_FOLDER/data"
check_failure
cp -v -r --no-target-directory "$current_dir/data/" "$DATA_DESTINATION_FOLDER/data/"
check_failure
cp -v -r --no-target-directory "$current_dir/engine_data/" "$DATA_DESTINATION_FOLDER/engine_data/"
check_failure

# Icons
copy_icon 512x512
copy_icon 256x256
copy_icon 128x128
copy_icon 64x64
copy_icon 32x32

gtk-update-icon-cache --force --ignore-theme-index "$ICONS_FOLDER"
check_failure

# Desktop file
touch "$DESKTOP_FOLDER/$DESKTOP"
check_failure
cat << EOF > "$DESKTOP_FOLDER/$DESKTOP"
[Desktop Entry]
Version=0.2.0
Terminal=false
Exec=$EXECUTABLE
Icon=$APP_NAME
Type=Application
Categories=Game;
Name=Nine Morris 3D
Comment=A nine men's morris implementation in 3D
EOF
check_failure
echo "Created desktop file $DESKTOP_FOLDER/$DESKTOP"

# Uninstall script
cp -v "$current_dir/uninstall.sh" "$DATA_DESTINATION_FOLDER/uninstall.sh"
check_failure

# User data removal script
cp -v "$current_dir/user_data_remove.sh" "$DATA_DESTINATION_FOLDER/user_data_remove.sh"
check_failure

# Help file
cp -v "$current_dir/$HELP" "$DATA_DESTINATION_FOLDER/$HELP"
check_failure

echo "Done. Have a nice day. :)"
