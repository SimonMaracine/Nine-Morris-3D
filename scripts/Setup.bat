ECHO OFF
export CC=/usr/bin/gcc-9
export CXX=/usr/bin/g++-9
cd ..\build
cmake ..
cd ..\scripts
