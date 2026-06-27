# Build

wxChaos currently builds on Windows with MSVC, CMake, Ninja, and vcpkg-managed
dependencies including wxWidgets and SFML 2.6.2.

These notes describe the known-good setup used by the current development tree.

## Prerequisites

- Visual Studio 2026 or newer with the Desktop development with C++ workload.
- CLion, or another CMake/Ninja-capable IDE.
- Ninja.
- vcpkg.

The current local setup uses:

- vcpkg at `C:\Users\fisca\.vcpkg-clion\vcpkg`
- CLion bundled CMake at
  `C:\Users\fisca\AppData\Local\Programs\CLion\bin\cmake\win\x64\bin\cmake.exe`
- CLion bundled Ninja at
  `C:\Users\fisca\AppData\Local\Programs\CLion\bin\ninja\win\x64\ninja.exe`

Adjust paths for a different machine.

## Install vcpkg Dependencies

The repository contains a `vcpkg.json` manifest. Install dependencies with the
same vcpkg instance that CMake will use:

```powershell
C:\Users\fisca\.vcpkg-clion\vcpkg\vcpkg.exe install --triplet x64-windows --x-manifest-root=C:\Users\fisca\OneDrive\Documents\Programacion\wxChaos
```

This installs dependencies such as:

- doctest
- mpfr
- SFML 2.6.2
- symengine
- wxWidgets

The generated `vcpkg_installed` directory is build output. Do not edit it by
hand.

The manifest pins SFML with a vcpkg override. Keep that override in place while
the codebase remains on the SFML 2 API; `version>= 2.6.2` alone is only a
minimum and may otherwise resolve to SFML 3 on newer registries.

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
