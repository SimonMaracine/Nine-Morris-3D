echo off

cls
cd ..\build
cmake --build .

if %ERRORLEVEL% EQU 0 (
    cd ..
    cls
    build\Debug\Nine-Morris-3D.exe
    cd scripts
)

if %ERRORLEVEL% NEQ 0 (
    cd ..\scripts
)
