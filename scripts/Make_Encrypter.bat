echo off

cd ..\encrypter_helper
cmake . -D CMAKE_BUILD_TYPE=Release -A x64
cmake --build . --config Release
cd ..\scripts
