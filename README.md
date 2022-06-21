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
- Copy the data directory filtering the unwanted files (keep only encrypted assets)
- Copy HELP_LINUX.txt
- Copy install.sh and uninstall.sh from scripts/linux-install/
- Archive the **_Nine-Morris-3D-vX.Y.Z-Linux_** folder

### Exporting on Windows:
- Create a new folder called **_Nine-Morris-3D_**
- Copy the executable from build-release/
- Make sure that all assets are encrypted
- Copy the data directory filtering the unwanted files (keep only encrypted assets)
- Create the NSIS installer called **_Nine-Morris-3D-vX.Y.Z-Windows_** using the **_Nine-Morris-3D_** folder

### Where to update the version:
- CMakeLists.txt
- nine_morris_3d.h
