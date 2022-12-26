echo off

cls
cd ..\build
cmake --build . --config Debug -j4

if %ERRORLEVEL% EQU 0 (
    cd ..
    cls
    build\Nine-Morris-3D\Debug\Nine-Morris-3D.exe
    cd scripts
)

if %ERRORLEVEL% NEQ 0 (
    cd ..\scripts
)
