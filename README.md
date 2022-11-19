# Nine Morris 3D

## A nine men's morris implementation in 3D

### Description
It is a free casual board game with hopefully beautiful 3D graphics. It has five game modes:
_standard (nine men's morris)_, _jump_, _jump+_, _twelve men's morris_ and _hunt_, each with their AIs.
Games can be saved and be continued later. AI and other settings are configurable. The background
can be changed as you wish. The game runs on Linux and Windows.

Hopefully an Android version will be made.

### Tags
- Casual
- Board
- Strategy
- Indie
- 3D

### Exporting on Linux
- Create a new folder called **_Nine-Morris-3D-vX.Y.Z-Linux_**
- Copy the executable from build-release/
- Make sure that all assets are encrypted
- Use filter_assets.py to create the filtered data folders
- Copy the two newly created data directories and rename them from ***_data-filtered_** to ***_data_**
- Copy HELP_LINUX.txt
- Copy README.txt
- Copy install.sh and uninstall.sh from scripts/linux-install/
- Archive the **_Nine-Morris-3D-vX.Y.Z-Linux_** folder  
  (`tar -c --file=Nine-Morris-3D-vX.Y.Z-Linux.tar.gz --gzip Nine-Morris-3D-vX.Y.Z-Linux`)

### Exporting on Windows
- Create a new folder called whatever
- Copy the executable from Release/
- Make sure that all assets are encrypted
- Use filter_assets.py to create the filtered data folders
- Copy the two newly created data directories and rename them from ***_data-filtered_** to ***_data_**
- Copy README.txt
- Copy installer.nsi from scripts/windows-install/
- Create an NSIS installer called **_Nine-Morris-3D-vX.Y.Z-Windows_** using the **_installer.nsi_** script

### Where to update the version
- CMakeLists.txt
- main.cpp
- install.sh
- installer.nsi
- README.txt
