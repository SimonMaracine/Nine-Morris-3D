echo off

cls
cd ..
cmake --build . --config Debug -j4
cd scripts
