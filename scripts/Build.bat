echo off

cls
cd ..\build
cmake --build . --config Debug -j4
cd ..\scripts
