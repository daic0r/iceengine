#!/bin/sh

cd src
if [ ! -d "vcpkg" ]
then
   echo Cloning vcpkg repo...
   git clone https://github.com/microsoft/vcpkg.git
   echo Bootstrapping vcpkg...
   ./vcpkg/bootstrap-vcpkg.sh
fi

# We must install these so we don't get audio initialization
# errors from SDL
echo About to install sound libs for SDL, please enter password when prompted
sudo apt-get install libasound2-dev libpulse-dev
echo Installing required packages...
./vcpkg/vcpkg install sdl2
./vcpkg/vcpkg install sdl2-image
./vcpkg/vcpkg install glm
./vcpkg/vcpkg install tinygltf
./vcpkg/vcpkg install crossguid
./vcpkg/vcpkg install rapidxml
./vcpkg/vcpkg install glad[extensions,gl-api-latest] --recurse
./vcpkg/vcpkg install date
./vcpkg/vcpkg install nlohmann-json
cd ..
echo Done.
