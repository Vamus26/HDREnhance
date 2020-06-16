Building
--------
Required: CMake, Git, Visual Studio 2017 or 2019
Install https://github.com/microsoft/vcpkg
Set environment variable VCPKG_DEFAULT_TRIPLET=x64-windows
Install dependencies with command: ./vcpkg.exe install assimp glm glew glfw3 stb
Open CMake-GUI
set source path to <IBLViewer_root>/src
set build path to <IBLViewer_root>/build
press configure and select your visual studio version and the platform x64
select specify toolchain file for cross-compiling
set toolchain file to: <vcpkg_root>/scripts/buildsystems/vcpkg.cmake
press generate and open solution
select configuration Release x64 and Build Solution

Running
-------
run from <IBLViewer_root> path
<<<<<<< HEAD
command: ./build/Release/IBLViewer.exe ./assets/DamagedHelmet/DamagedHelmet.gltf ./assets/Newport_Loft_Ref.hdr
"./build/Release/IBLViewer.exe" ./assets/DamagedHelmet/DamagedHelmet.gltf ./assets/blackbot.hdr
"./build/Release/IBLViewer.exe" ./assets/DamagedHelmet/DamagedHelmet.gltf ./assets/matlabcomp020_blackbot.hdr ./assets/Newport_Loft_Ref.hdr
"./build/Release/IBLViewer.exe" ./assets/DamagedHelmet/DamagedHelmet.gltf ./assets/matlabcomp020_blackbot.hdr ./assets/kitchenvid-00020.png
"./build/Release/IBLViewer.exe" ./assets/DamagedHelmet/DamagedHelmet.gltf ./assets/videoFrames.txt ./assets/hdrFrames.txt
=======
command: ./build/Release/IBLViewer.exe ./assets/DamagedHelmet/DamagedHelmet.gltf ./assets/Newport_Loft_Ref.hdr
>>>>>>> 5302a5fef9735646a13a986f336553b17cd0cab5