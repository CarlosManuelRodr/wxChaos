# Agent Notes

## Code Style

- For helper logic that exists only to support a class, prefer a private member function over an anonymous-namespace free function. Avoid adding namespace-scope helper functions unless the behavior is genuinely shared outside a single class.

## Build Environment

This project is built on Windows with CMake, Ninja, MSVC, wxWidgets, and SFML 2.6 from CMake FetchContent.

Do not run `cmake --build ...` directly unless `cmake` is already on `PATH`. In this workspace, the configured build tree uses CLion's bundled CMake, and plain PowerShell may not have either CMake or the MSVC standard library include paths available.

Use the Visual Studio developer environment before invoking Ninja:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"" -arch=x64 -host_arch=x64 && ninja -C cmake-build-debug wxChaos"
```

This command verified successfully in this workspace.

## Common Build Failures

- `cmake: The term 'cmake' is not recognized`: CMake is not on the shell `PATH`. Use Ninja against the existing build tree, or call the bundled CMake from the configured build files.
- `fatal error C1083: Cannot open include file: 'vector'`, `'complex'`, or `'stddef.h'`: MSVC was invoked outside the Visual Studio developer environment. Re-run the build through `VsDevCmd.bat`.
- `CreateProcess failed ... VerifyGlobs.cmake ... Access is denied`: the sandbox may block CLion's bundled CMake. Re-run the same Ninja command with escalated permissions.

## Verification

Before handing off changes, run:

```powershell
git diff --check
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"" -arch=x64 -host_arch=x64 && ninja -C cmake-build-debug wxChaos"
```

The build output should end with linking `bin\wxChaos.exe` and copying application resources.
