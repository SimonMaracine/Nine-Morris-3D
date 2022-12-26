echo off

@REM cd ..\encrypter_helper
@REM cmake . -D CMAKE_BUILD_TYPE=Release -A x64
@REM cmake --build . --config Release
@REM cd ..\scripts

cd ..\build
cmake --build . --target encrypter_helper --config Release
cd ..\scripts
