# Nine Morris 3D

## A nine men's morris implementation in 3D

### Description

It is a free casual board game with hopefully beautiful 3D graphics. You can play against the computer
or another person. It has five game modes:

- _standard (nine men's morris)_
- _jump_
- _jump+_
- _twelve men's morris_
- _hunt_

And each game mode has its own AI. Games can be saved and be continued later. AI and other
settings are configurable. The background can be changed as you wish. It supports 3D sounds and
it has beautiful music.

The game runs on Linux and Windows.

Hopefully an Android version will be made.

### Tags

- Casual
- Board
- Strategy
- Indie
- 3D

### Exporting on Linux

- Create a new directory called **_Nine-Morris-3D-vX.Y.Z-Linux_**
- Copy the executable from **build-release/Nine-Morris-3D/**
- Make sure that all assets are encrypted
- Use **filter_assets.py** to create the filtered data directories
- Copy the two newly created data directories and rename them from **data-filtered** to **data**
- Copy **HELP_LINUX.txt**
- Copy **README.txt**
- Copy **install.sh**, **uninstall.sh** and **user_data_remove.sh** from **scripts/linux_install/**
- Archive the **_Nine-Morris-3D-vX.Y.Z-Linux_** directory  
  `tar -c --file=Nine-Morris-3D-vX.Y.Z-Linux.tar.gz --gzip Nine-Morris-3D-vX.Y.Z-Linux`

### Exporting on Windows

- Create a new directory called whatever
- Copy the executable from **build\Nine-Morris-3D\Release\\**
- Make sure that all assets are encrypted
- Use **filter_assets.py** to create the filtered data directories
- Copy the two newly created data directories and rename them from **data-filtered** to **data**
- Copy **README.txt**
- Copy **installer.nsi** from **scripts\windows_install\\**
- Create an NSIS installer called **_Nine-Morris-3D-vX.Y.Z-Windows_** using the **installer.nsi** script

### Where to update the version

- main.cpp
- install.sh
- installer.nsi
- README.txt
