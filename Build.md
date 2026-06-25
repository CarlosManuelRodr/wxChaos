# Build

wxChaos currently builds on Windows with MSVC, CMake, Ninja, wxWidgets, SFML 2.6
through CMake FetchContent, and several vcpkg dependencies.

These notes describe the known-good setup used by the current development tree.

## Prerequisites

- Visual Studio 2026 or newer with the Desktop development with C++ workload.
- CLion, or another CMake/Ninja-capable IDE.
- Ninja.
- wxWidgets 3.3.x built locally with MSVC x64.
- vcpkg.

The current local setup uses:

- wxWidgets at `C:\wxWidgets-3.3.2`
- vcpkg at `C:\Users\fisca\.vcpkg-clion\vcpkg`
- CLion bundled CMake at
  `C:\Users\fisca\AppData\Local\Programs\CLion\bin\cmake\win\x64\bin\cmake.exe`
- CLion bundled Ninja at
  `C:\Users\fisca\AppData\Local\Programs\CLion\bin\ninja\win\x64\ninja.exe`

Adjust paths for a different machine.

## Build wxWidgets

Install wxWidgets and build the MSVC x64 static libraries. The project expects
wxWidgets to provide the components used in `CMakeLists.txt`:

- core
- base
- adv
- html
- stc
- richtext
- xml

For the current local setup, CMake finds libraries under:

```text
C:\wxWidgets-3.3.2\lib\vc_x64_lib
```

If wxWidgets is installed somewhere else, set `wxWidgets_ROOT_DIR` or the
standard wxWidgets CMake variables in your CMake profile.

## Install vcpkg Dependencies

The repository contains a `vcpkg.json` manifest. Install dependencies with the
same vcpkg instance that CMake will use:

```powershell
C:\Users\fisca\.vcpkg-clion\vcpkg\vcpkg.exe install --triplet x64-windows --x-manifest-root=C:\Users\fisca\OneDrive\Documents\Programacion\wxChaos
```

This installs dependencies such as:

- doctest
- mpfr
- symengine

The generated `vcpkg_installed` directory is build output. Do not edit it by
hand.

## Configure with CMake

When using CLion, configure the CMake profile with the vcpkg toolchain file:

```text
-DCMAKE_TOOLCHAIN_FILE=C:/Users/fisca/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake
```

If CMake cannot find SymEngine after vcpkg install, explicitly point CMake at
the manifest install directory:

```text
-DVCPKG_MANIFEST_MODE=ON
-DVCPKG_MANIFEST_DIR=C:/Users/fisca/OneDrive/Documents/Programacion/wxChaos
-DVCPKG_INSTALLED_DIR=C:/Users/fisca/OneDrive/Documents/Programacion/wxChaos/vcpkg_installed
```

Alternatively, set only the SymEngine package directory:

```text
-DSymEngine_DIR=C:/Users/fisca/OneDrive/Documents/Programacion/wxChaos/vcpkg_installed/x64-windows/share/symengine
```

Equivalent command-line configure:

```powershell
& "C:\Users\fisca\AppData\Local\Programs\CLion\bin\cmake\win\x64\bin\cmake.exe" `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_MAKE_PROGRAM=C:/Users/fisca/AppData/Local/Programs/CLion/bin/ninja/win/x64/ninja.exe `
  -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=C:/Users/fisca/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -S C:/Users/fisca/OneDrive/Documents/Programacion/wxChaos `
  -B C:/Users/fisca/OneDrive/Documents/Programacion/wxChaos/cmake-build-debug
```

## SymEngine and CMake 4

SymEngine's exported vcpkg CMake config currently declares compatibility with
CMake 2.8.12. CMake 4 rejects that unless a compatibility floor is set.

The project sets this before `find_package(SymEngine CONFIG REQUIRED)`:

```cmake
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()
```

If this workaround is removed, configure can fail with:

```text
Compatibility with CMake < 3.5 has been removed from CMake.
```

## Build

Build from a Visual Studio developer environment so MSVC headers and libraries
are on the path:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"" -arch=x64 -host_arch=x64 && ninja -C cmake-build-debug wxChaos"
```

The build should finish by linking:

```text
bin\wxChaos.exe
```

## Run Tests

Run the configured test suite with CLion's bundled CTest:

```powershell
& "C:\Users\fisca\AppData\Local\Programs\CLion\bin\cmake\win\x64\bin\ctest.exe" --test-dir cmake-build-debug --output-on-failure
```

Before handing off code changes, also run:

```powershell
git diff --check
```