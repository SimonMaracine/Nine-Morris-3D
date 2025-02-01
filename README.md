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

### Tags

- Casual
- Board
- Strategy
- Indie
- 3D

### Distributing on Linux

- Build the binary as per `BUILD.md`
- Export the assets

```txt
cd scripts
./export_assets.py ../build_dist/ ../assets/ ../assets_engine/
```

- Build the archive

```txt
cd scripts
./create_source_archive.sh ../build_dist/ X.Y.Z ../build_dist/nine_morris_3d/nine_morris_3d ../ ../build_dist/assets/icons/ ../distribution/linux/ninemorris3d.desktop ../build_dist/assets/ ../build_dist/assets_engine/
```

- Create a new directory called `Nine-Morris-3D-X.Y.Z-Linux`
- Copy the archive into the directory
- Copy `README.txt` from root into the directory
- Copy `install.sh` and `uninstall.sh` from `distribution/linux/` into the directory
- Archive the directory

```txt
tar -czf Nine-Morris-3D-X.Y.Z-Linux.tar.gz Nine-Morris-3D-X.Y.Z-Linux
```

### Distributing on Windows

- Build the binary as per `BUILD.md`
- Export the assets

```txt
cd scripts
python export_assets.py ..\build ..\assets ..\assets_engine
```

- Create a new folder called whatever
- Copy the game binary from `build\nine_morris_3d\Release` into the folder
- Copy the engine binaries from root into the folder
- Copy the assets directories from `build` into the folder
- Copy `README.txt` from root into the folder
- Copy `installer.nsi` from `distribution\windows` into the folder
- Create an NSIS installer called `Nine-Morris-3D-X.Y.Z-Windows` using the installer script

### Where to update the version

- main.cpp
- README.txt
- ninemorris3d.desktop
- installer.nsi
