ECHO OFF
cd ..\build
cmake .. -D CMAKE_BUILD_TYPE=Debug -D VSCODE=ON -A x64
cd ..\scripts
