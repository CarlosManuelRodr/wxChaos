# Agent Notes

Overview: This project is a C++ fractal generator with a GUI using wxWidgets and SFML. It aims to provide a visually appealing and interactive experience for exploring fractal patterns.
The author of this project created it circa 2012 as a learning exercise and to explore the capabilities of C++ and GUI libraries. As such you will often encounter outdated practices and awful code.

## Code Style

- For helper logic that exists only to support a class, use a private member function so the class declaration shows what the class does. Do not add anonymous-namespace free functions for class-local behavior.
- Avoid adding namespace-scope helper functions unless the behavior is genuinely shared outside a single class.
- If touching an existing anonymous namespace, keep its contents indented, but do not add new anonymous-namespace functions for class helper behavior.
- When a C++ function declaration or definition parameter list must wrap, keep the first parameter on the same line as the function name when it fits, then align continuation lines with the first parameter. Prefer balanced horizontal and vertical space usage over one-parameter-per-line formatting. For example:

  ```cpp
  void MainFrame::OpenJuliaPreviewInMainWindow(const FractalType fractalType, const Options& options,
                                               const ColorPaletteTypes colorPalette, const Rect& view,
                                               const bool automaticIterations)
  ```

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
- When adding a new fractal or changing an existing fractal in a way that changes its formula, options, renderer support, capabilities, history, or user-visible behavior, create or update the matching HTML documentation in `app_resources/Resources/Documents/`.

The refactored escape-time family generally supports `EscapeTime`, `GaussianInt`, `EscapeAngle`, and `TriangleInequality`. For triangle inequality, pass the formula's transformed pre-constant term to the observer rather than placeholder zeroes.

## GUI And Interaction Notes

- `FractalToolbar` is shared by `MainFrame` and `JuliaPreviewFrame`. New interaction tools, toolbar icons, information controls, and color-rotation behavior should usually be added there once instead of separately wiring each frame.
- Keep `FractalCanvas` as the owner of fractal mouse behavior. Toolbar callbacks should select a `FractalInteractionTool`; the canvas should decide how mouse move, click, wheel, capture, and leave events behave for that tool.
- When adding or changing a `FractalInteractionTool`, update the enum, toolbar IDs, icon selection, radio-tool selection handling, cursor selection, and canvas event behavior together.
- Every interaction feature added to the main fractal canvas should also be considered for `JuliaPreviewFrame`, because it embeds a `FractalCanvas` and is expected to feel like the main view unless there is a deliberate reason not to.
- Use theme-specific SVG icons from `app_resources/Resources/Icons/` with `wxBitmapBundle::FromSVGFile(...)`. Pick the light or dark asset with `AppTheme::IsDark()`.
- Avoid runtime-generated toolbar icons when resource icons exist. Keep toolbar hit targets large enough for comfortable clicking; the current fractal toolbar uses 48x48 tool bitmaps.
- For hover inspectors that update rapidly, avoid repeatedly replacing native tooltip text because it can flicker. Use a persistent lightweight popup or another stable widget and update its contents in place.
- `DocumentViewer` uses `wxWebView` and MathJax-capable HTML. Use viewer-side navigation state when immediate Back/Forward button responsiveness matters, because `wxWebView::CanGoBack()` and `CanGoForward()` may lag until the backend commits navigation.
- Documentation pages should share `app_resources/Resources/Documents/fractal_info.css`; the viewer injects the current theme so pages can adapt to light and dark mode. Legacy tutorials under `app_resources/Resources/Tutorials/` can also reuse that stylesheet without changing their content.

## GPU Color Rotation Postmortem

Color rotation was attempted with an SFML fragment shader so palette animation would not require CPU recoloring every pixel on every frame. The narrow shader result was promising: once a complete render was available, rotating colors on the GPU was visually smooth and avoided the CPU thread-pool redraw bottleneck. The broader integration caused regressions and should be treated as an unfinished design problem, not a small presenter optimization.

What made the attempt fail:

- The current presentation model assumes that the CPU-owned `sf::Image _image` is the canonical visible image. Zoom previews, zoom history, zoom-back, partial render previews, panning reuse, export-adjacent code paths, and fallback drawing all expect a CPU image that already contains final display colors.
- The shader path instead makes encoded render data plus palette state canonical. Final colors only exist after drawing through `sf::Shader`, so any code that needs an `sf::Image` either sees stale/unrotated colors or must force a GPU-to-CPU readback with `copyToImage()`.
- That readback became the interaction killer. Capturing a shader-colored preview before zooming required rendering to an `sf::RenderTexture`, calling `copyToImage()`, then sometimes walking the whole screen on the CPU to merge partial render pixels. With antialiasing or larger windows, zoom and interactive zoom developed a visible pause before the animation even started.
- Avoiding the readback by using a GPU-backed temporary preview fixed part of the delay, but introduced a second representation for temporary previews. Some paths now had `_tempSprite` backed by `sf::Texture` from `_tempImage`, while others had it backed by `sf::RenderTexture`. That split made cancellation, redraw, zoom-back, cached history, and preview layering fragile.
- Partial render preview and shader colorization pull in opposite directions. The renderer reveals CPU-computed regions incrementally, while the shader wants a coherent encoded texture. Uploading dirty regions can make progressive preview possible, but then every render, pan reuse, reset, and completion path must precisely invalidate or update the encoded texture. Missing one transition creates stale pixels, holes, or sudden full-frame swaps.
- Panning reuse is especially delicate because the visible display map and the supersampled/internal render map have different coordinate spaces. Any GPU-side cache has to track the same stitched regions and offsets as the CPU maps or visual seams appear.
- The fallback paths multiplied. The code had to handle shader unavailable, shader compile errors, max texture size limits, partial data not uploaded yet, rendering vs rendered states, exterior color disabled, set color enabled, antialiasing scale, zoom animation active, and cached zoom images. This made it easy for one feature to work only by regressing another.
- SFML/OpenGL object lifetime is risky inside this wxWidgets integration. Adding or reordering SFML members in `FractalPresenter` caused startup crashes until a clean rebuild, and previous direct ownership changes such as switching shader resources to `std::unique_ptr` also produced instability. Treat SFML resource lifetime and context ownership as part of the design, not a mechanical refactor.

Lessons for a future attempt:

- Do not bolt a shader-only color path onto the existing CPU-image presentation model. First define a single authoritative frame model: either CPU final-color frames, GPU encoded-data frames, or an explicit two-layer model with strict ownership and synchronization rules.
- Separate "current visual frame for interaction" from "CPU image for history/export/cache". Zoom animation should never depend on a blocking GPU-to-CPU readback. If CPU snapshots are still needed, make them asynchronous, lower priority, or explicitly unavailable while GPU color rotation is active.
- Design render preview, panning reuse, zoom previews, and zoom history together. A GPU colorizer that handles only final completed renders is easy; a GPU colorizer that preserves wxChaos' perceived responsiveness must support dirty-region updates and panned-map reuse as first-class behavior.
- Add observability before optimizing: log when the GPU path is active, when it falls back, when full texture uploads occur, when dirty-region uploads occur, and when any GPU readback is performed. Treat unexpected readbacks during interaction as bugs.
- Keep a CPU fallback path simple and intact. If the GPU path cannot support a feature without complicated bridge code, prefer temporarily disabling GPU color rotation for that feature over adding another mixed CPU/GPU representation.
- If this is attempted again, prototype it behind a narrow feature flag and manually test color rotation, render-in-progress preview, wheel zoom, box zoom, interactive zoom, zoom-back, panning while rendered, panning while rendering, antialiasing 1x/2x/4x, exterior color disabled, set color enabled, and shader failure fallback before considering it integrated.

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
