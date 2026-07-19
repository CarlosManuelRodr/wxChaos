# AppImage packaging

Build the x86-64 AppImage from the repository root:

```bash
./scripts/build-appimage.sh
```

The script builds in an Ubuntu 24.04 container and writes the artifact to
`dist/wxChaos-<version>-x86_64.AppImage`. Docker is the only host dependency.
Set `JOBS` to limit build parallelism.

The container deliberately owns the complete build and packaging process so a
future GitHub Actions job can invoke the same script without duplicating its
commands.

The build uses the vcpkg commit recorded by `vcpkg.json`. wxWidgets, GTK,
GStreamer, and WebKitGTK come from the Ubuntu 24.04 build environment; the
remaining C++ dependencies come from the manifest.

## Compatibility

Ubuntu 24.04 establishes the binary's glibc baseline. The AppImage bundles the
application libraries and WebKitGTK helper executables that linuxdeploy can
discover or that wxWebView starts dynamically. It still relies on standard
low-level libraries and graphics drivers supplied by the target Linux system.

Before publishing, smoke-test the artifact on at least Ubuntu 24.04 and a
non-Ubuntu distribution. Documentation pages, video export, SFML rendering, and
X11 operation under a Wayland desktop are the most important integration paths.
