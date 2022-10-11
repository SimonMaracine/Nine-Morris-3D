echo off

cls
cd ..
cmake --build . --config Debug

if %ERRORLEVEL% EQU 0 (
    cls
    Debug\Nine-Morris-3D.exe
    cd scripts
)

if %ERRORLEVEL% NEQ 0 (
    cd scripts
)
