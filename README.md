# Eclipse

Eclipse is a C++ game engine project

## Build Tetris on Windows

Install Visual Studio Build Tools with the MSVC C++ toolchain, CMake, Ninja,
and vcpkg. Open a Developer PowerShell, or initialize MSVC in your existing
PowerShell session. `VCPKG_ROOT` must point to the vcpkg installation.

From the repository root, configure the project:

```powershell
cmake -S . -B build -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DBUILD_TESTING=OFF `
  -DVCPKG_TARGET_TRIPLET=x64-windows `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```

The vcpkg toolchain reads `vcpkg.json` and installs SDL2 and GLM for the
project. CMake stores the toolchain setting in `build/CMakeCache.txt`, so later
builds need only this command:

```powershell
cmake --build build --target tetrisgame --parallel
```

Run Tetris:

```powershell
.\build\tetrisgame\tetrisgame.exe
```

If you delete `build`, run the configure command again before you build.
