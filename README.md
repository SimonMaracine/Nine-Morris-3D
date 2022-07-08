# Nine Morris 3D

## A Nine Men's Morris implementation in 3D.

### Details:
It is a free casual board game with hopefully beautiful 3D graphics. It has three game modes: standard, final and hunter, each with their AIs. Games can be saved and be continued later. AI and other settings are configurable. Works on Linux and Windows.

### Tags:
- Casual
- Board
- Strategy
- Indie
- 3D

### Exporting on Linux:
- Create a new folder called **_Nine-Morris-3D-vX.Y.Z-Linux_**
- Copy the executable from build-release/
- Make sure that all assets are encrypted
- Use filter_assets.py to create the data folder filtered
- Copy the newly created data directory and rename it from **_data-filtered_** to **_data_**
- Copy HELP_LINUX.txt
- Copy install.sh and uninstall.sh from scripts/linux-install/
- Archive the **_Nine-Morris-3D-vX.Y.Z-Linux_** folder

### Exporting on Windows:
- Create a new folder called whatever
- Copy the executable from Release/
- Make sure that all assets are encrypted
- Use filter_assets.py to create the data folder filtered
- Copy the newly created data directory and rename it from **_data-filtered_** to **_data_**
- Copy installer.nsi from scripts/windows-install/
- Create the NSIS installer called **_Nine-Morris-3D-vX.Y.Z-Windows_** using the **_installer.nsi_** script

### Where to update the version:
- CMakeLists.txt
- nine_morris_3d.h
- install.sh
- installer.nsi
- README.txt
