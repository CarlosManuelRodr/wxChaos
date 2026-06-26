# Agent Notes

Overview: This project is a C++ fractal generator with a GUI using wxWidgets and SFML. It aims to provide a visually appealing and interactive experience for exploring fractal patterns.
The author of this project created it circa 2012 as a learning exercise and to explore the capabilities of C++ and GUI libraries. As such you will often encounter outdated practices and awful code.

## Code Style

- For helper logic that exists only to support a class, use a private member function so the class declaration shows what the class does. Do not add anonymous-namespace free functions for class-local behavior.
- Avoid adding namespace-scope helper functions unless the behavior is genuinely shared outside a single class.
- If touching an existing anonymous namespace, keep its contents indented, but do not add new anonymous-namespace functions for class helper behavior.

## Escape-Time Renderer Architecture

The escape-time renderer family is intentionally organized around one readable fractal loop per renderer. Keep the formula visible and avoid reintroducing separate loops for normal, smooth, orbit-trap, Gaussian integer, escape-angle, or triangle-inequality coloring.

- Each compatible renderer defines one private templated trace method:

  ```cpp
  template<class MeasurePoint>
  Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;
  ```

- `TracePoint` owns the fractal formula and iteration ordering. It calls the supplied observer at `PointTraceEvent::Started`, `PointTraceEvent::Iterated`, and `PointTraceEvent::Escaped`.
- `Renderer::Point` is the shared result type. It contains common escape state plus optional measurements used by the coloring algorithms.
- `Renderer::RenderPixels`, `RenderFromPoint`, `EscapeTimeRender`, `GaussianIntRender`, `EscapeAngleRender`, and `TriangleInequalityRender` own the repeated pixel scan, observer selection, and coloring.
- Derived `Render()` methods should only create the lambda that invokes their `TracePoint` and dispatch the selected algorithm to the inherited rendering methods.
- Do not add derived color-wrapper functions or per-renderer copies of `RenderFromPoint`. The color functions and common rendering paths belong to `Renderer`.

The observer is templated so MSVC can inline it inside the hot iteration loop. Do not replace it with `std::function` or an indirect runtime callback. Performance depends on each mode measuring only what it needs rather than filling every `Point` field unconditionally.

Special algorithms such as Buddhabrot, Newton, fixed-point renderers, user-defined renderers, and `ScriptFractalRenderer` do not necessarily fit this model and should not be forced into it without a separate design pass.

## Trace Event Invariants

Preserving measurement order is more important than making every trace look identical.

- Preserve whether the original formula tests the bailout before or after updating `z`.
- Emit `Iterated` after the formula update, with the updated `z`, norm, and any formula-specific transformed term needed by triangle inequality.
- Emit `Escaped` exactly where the trace first recognizes escape. Store `escapedZRe`, `escapedZIm`, and `escapedNorm` at that point.
- Keep `point.iterations` compatible with the historical renderer. Update-first formulas commonly record `n + 1` while inside and `n` on escape.
- Pass `wasInside` as the state that applied to the iteration being measured. Gaussian and triangle observers use it to reject unwanted post-escape samples.
- Orbit traps should be sampled only by orbit observers. Smooth interpolation should be calculated only when the selected algorithm needs it.

Gaussian integer coloring needs special care. It interpolates between the current and previous minimum Gaussian distances and requires one measured formula update after escape. `MeasureGaussianInteger` sets `point.measureGaussianAfterEscape` on the `Escaped` event; trace break conditions must honor that flag so the following `Iterated` event can consume and clear it.

For traces that start with `z = pixel` and test before updating, do not declare escape at iteration zero. Use the equivalent of:

```cpp
if (n > 0 && !escaped && point.zNorm > bailout)
```

The initial update supplies the first valid Gaussian sample. Escaping immediately at `n == 0` drops one side of the interpolation and creates a visible discontinuity along the initial bailout frontier. This rule currently matters for Julia, Medusa (called Jellyfish in the UI), Manowar, ManowarJulia, BurningShipJulia, Fractory, and Cell.

When debugging visual regressions, compare the exact update, bailout, and measurement ordering against the implementation on the `dev` branch. Small ordering changes can produce large image differences even when the formula appears algebraically equivalent.

## Fractal UI Capabilities

Renderer support and the fractal definition in `wxChaos/core/fractals/` must be updated together.

- Set `_hasOrbitTrap = true` when the trace and color path support orbit traps.
- Set `_hasSmoothRender = true` when smooth escape-time coloring is supported.
- Add every supported `RenderingAlgorithmType` to `_availableAlg` in the desired UI order.
- Add the matching case to the renderer's `Render()` switch. A menu entry without a dispatch case produces an empty render.

The refactored escape-time family generally supports `EscapeTime`, `GaussianInt`, `EscapeAngle`, and `TriangleInequality`. For triangle inequality, pass the formula's transformed pre-constant term to the observer rather than placeholder zeroes.

## Build Environment

This project is built on Windows with CMake, Ninja, MSVC, wxWidgets, and SFML 2.6 from CMake FetchContent.

Do not run `cmake --build ...` directly unless `cmake` is already on `PATH`. In this workspace, the configured build tree uses CLion's bundled CMake, and plain PowerShell may not have either CMake or the MSVC standard library include paths available.

Use the Visual Studio developer environment before invoking Ninja:

```powershell
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"" -arch=x64 -host_arch=x64 && ninja -C cmake-build-debug wxChaos"
```

This command verified successfully in this workspace.

## Testing Expectations

- When adding a new component or new functionality, add a corresponding unit test unless the change is impossible to exercise without a separate design pass.
- When making changes, run the relevant tests and verify they still pass before handing off.

## Common Build Failures

- `cmake: The term 'cmake' is not recognized`: CMake is not on the shell `PATH`. Use Ninja against the existing build tree, or call the bundled CMake from the configured build files.
- `fatal error C1083: Cannot open include file: 'vector'`, `'complex'`, or `'stddef.h'`: MSVC was invoked outside the Visual Studio developer environment. Re-run the build through `VsDevCmd.bat`.
- `CreateProcess failed ... VerifyGlobs.cmake ... Access is denied`: the sandbox may block CLion's bundled CMake. Re-run the same Ninja command with escalated permissions.
- Link errors that mention both old and renamed types, such as `FormulaOpt` versus `FormulaOptions`, can come from stale incremental object files. Run `ninja -C cmake-build-debug clean` inside the Visual Studio developer environment, then rebuild.

## Verification

Before handing off changes, run:

```powershell
git diff --check
& "C:\Users\fisca\AppData\Local\Programs\CLion\bin\cmake\win\x64\bin\ctest.exe" --test-dir cmake-build-debug --output-on-failure
cmd /c "call ""C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"" -arch=x64 -host_arch=x64 && ninja -C cmake-build-debug wxChaos"
```

The build output should end with linking `bin\wxChaos.exe` and copying application resources.

For renderer changes, also smoke-test representative fractals in normal escape-time, smooth, orbit-trap, smooth plus orbit-trap, Gaussian integer, Gaussian plus orbit-trap, escape-angle, and triangle-inequality modes where exposed by the UI. Pay particular attention to bailout frontiers and iteration-zero behavior.
