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
    cmake .. -D CMAKE_BUILD_TYPE=Release -A x64  
    cmake --build . --config Release

#### Visual Studio
    mkdir build-release  
    cd build-release  
    cmake .. -D CMAKE_BUILD_TYPE=Release -A x64  
    :: Open in Visual Studio and build from there

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
    mkdir build  
    cd build  
    cmake .. -D CMAKE_BUILD_TYPE=Debug -A x64  
    :: Open in Visual Studio and build from there
