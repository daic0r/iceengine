@echo off
cd src
echo Cloning vcpkg repo...
git clone https://github.com/microsoft/vcpkg.git
echo Bootstrapping vcpkg...
.\vcpkg\bootstrap-vcpkg.bat
echo Installing required packages...
.\vcpkg\vcpkg.exe install sdl2 --triplet x64-windows
.\vcpkg\vcpkg.exe install sdl2-image --triplet x64-windows
.\vcpkg\vcpkg.exe install glm --triplet x64-windows
.\vcpkg\vcpkg.exe install tinygltf --triplet x64-windows
.\vcpkg\vcpkg.exe install crossguid --triplet x64-windows
.\vcpkg\vcpkg.exe install rapidxml --triplet x64-windows
.\vcpkg\vcpkg.exe install glad --triplet x64-windows
.\vcpkg\vcpkg.exe install date --triplet x64-windows
.\vcpkg\vcpkg.exe install nlohmann_json --triplet x64-windows
cd ..
echo Done.