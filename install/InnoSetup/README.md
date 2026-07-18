# Windows installer

The Windows installer is generated with Inno Setup 7 from the authoritative
CMake install manifest. The packaging script rebuilds the Release target,
creates a clean staging directory, bundles the latest Visual C++ x64
Redistributable installed with Visual Studio, and compiles one English/Spanish
installer.

Run from the repository root:

```powershell
& .\install\InnoSetup\BuildInstaller.ps1
```

The output is written to:

```text
cmake-build-release\package\installer\wxChaos-<version>-windows-x64-setup.exe
```

The application version comes from `project(wxChaos VERSION ...)` in the root
`CMakeLists.txt` and is embedded in `wxChaos.exe`. Inno Setup reads that
embedded product version, so the executable and installer cannot silently
drift to different versions.

The installed application files are read-only under `{autopf}\wxChaos`.
Configuration and editable scripts are stored in the current user's wxChaos
data directory. An empty `portable.flag` beside `wxChaos.exe` switches mutable
data back to the executable directory for portable distributions; configure
with `-DWXCHAOS_PORTABLE_BUILD=ON` to create that marker in a build output.

The generated installer is unsigned unless code signing is configured
separately. Signing requires a certificate and access to its private key.
