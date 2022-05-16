# Building Nine Morris 3D

## Linux release

    mkdir build-release  
    cd build-release  
    cmake .. -D CMAKE_BUILD_TYPE=Release  
    cmake --build .

## Windows release

#### Visual Studio Code
    mkdir build-release  
    cd build-release  
    cmake .. -D CMAKE_BUILD_TYPE=Release -D VSCODE=ON -A x64  
    cmake --build . --config Release

#### Visual Studio
    cmake . -D CMAKE_BUILD_TYPE=Release -D VSCODE=OFF -A x64  

## Linux debug

    cd scripts  
    setup.sh  
    build.sh  
    # run.sh

## Windows debug

#### Visual Studio Code
    cd scripts  
    Setup.bat  
    Build.bat  
    :: Run.bat

#### Visual Studio
    cmake . -D CMAKE_BUILD_TYPE=Debug -D VSCODE=OFF -A x64  
