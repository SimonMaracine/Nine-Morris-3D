echo off

cd ..
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNM3D_RELEASE_MODE=OFF -DNM3D_DISTRIBUTION_MODE=OFF -A x64
cd ..\scripts

pause
