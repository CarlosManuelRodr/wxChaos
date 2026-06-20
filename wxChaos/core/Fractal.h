#pragma once
#include <algorithm>
#include <limits>
#include <optional>
#include <vector>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <SFML/Graphics.hpp>
#include "wx/wxGradient.h"
#include "types/FractalType.h"
#include "types/RenderingAlgorithmType.h"
#include "geometry/LineData.h"
#include "geometry/CircleData.h"
#include "geometry/Vector2Int.h"
#include "geometry/Rect.h"
#include "ColorPaletteTypes.h"
#include "Options.h"
#include "FormulaOptions.h"
#include "Renderer.h"
#include "ThreadWatchdog.h"
#include "rendering/RenderJob.h"
#include "rendering/RenderRegion.h"
#include "rendering/RenderThreadPool.h"
#include "../gui/wx/PanelOptions.h"

/**
* @class Fractal
* @brief Provides an interface to do the fractal rendering and draw the result to the screen.
*
* This is an abstract class. Its purpose is to provide a set of methods to render the fractal,
* allocate memory for the rendering maps, and provide color data for the drawing.
*/
class Fractal
{
public:
    struct PointSample
    {
        bool inSet;
        unsigned int value;
        bool hasValue;
    };

protected:
    bool** _setMap;                             ///< Stores the points that belong to the fractal set.
    unsigned int** _colorMap;                   ///< Store the color map.
    unsigned int** _auxMap;                     ///< An additional map to perform some auxiliary operations.
    ThreadWatchdog<Renderer> _watchdog;         ///< Watch over the render threads.
    RenderThreadPool _renderPool;               ///< Reusable pool for render jobs.

    // Fractal properties.
    PanelOptions _panelOpt;          ///< List of GUI elements to put into the option panel.
    FractalType _type;               ///< Type of fractal to render.
    double _minX;                    ///< Left numeric limit of the fractal.
    double _maxX;                    ///< Right numeric limit of the fractal.
    double _minY;                    ///< Lower numeric limit of the fractal.
    double _maxY;                    ///< Upper numeric limit of the fractal.
    double _xFactor;                 ///< Conversion factor numberX to pixelX.
    double _yFactor;                 ///< Conversion factor numberY to pixelY.
    mutable PreciseRect _preciseView;
    mutable HighPrecisionReal _preciseXFactor;
    mutable HighPrecisionReal _preciseYFactor;
    mutable bool _preciseViewInitialized;
    unsigned _maxIter;               ///< Maximum number of iterations.
    FormulaOptions _userFormula;         ///< Formula specified by the user.

    // System.
    unsigned int _threadNumber;      ///< Number of threads. By default, is the same as the number of cores in the system.

    // Julia variables.
    double _kReal;
    double _kImaginary;

    unsigned int _screenWidth;
    unsigned int _screenHeight;
    unsigned int _backScreenWidth;
    unsigned int _changeGradient;

    // Color properties.
    RenderingAlgorithmType _algorithm;
    std::vector<RenderingAlgorithmType> _availableAlg;
    wxGradient _gradient;                   ///< Gradient to be used.
    wxColour _fSetColor;                    ///< Color of points belonging to the set.
    std::vector<wxColour> _palette;
    bool _relativeColor;
    bool _colorSet;                         ///< Activates internal coloring.
    bool _colorMode;                        ///< Activates external coloring.
    ColorPaletteTypes _gradStyle;               ///< Grad color palette to be used.
    unsigned int _paletteSize;
    unsigned int _gradPaletteSize;
    unsigned int _varGradientStep;
    unsigned int _maxColorMapVal;
    bool _refreshImage;

    // Status variables.
    Vector2Int _pendingRenderOffset;        ///< Reused map offset used to render only newly exposed areas.
    bool _rendered;
    bool _rendering;
    bool _paused;
    bool _pausing;
    bool _varGradient;                      ///< If this is activated (by the play button) the gradient variation mode starts.
    bool _onSnapshot;
    bool _waitRoutine;
    bool _redrawAll;
    bool _redrawAlways;
    bool _justLaunchThreads;
    bool _varGradChange;
    bool _renderJobCompatible;               ///< Fractal compatible with renderJobs.
    bool _changeFractalProp;
    std::vector<Vector2Int> _endPoints;
    std::vector<Vector2Int> _startPoints;
    std::vector<Vector2Int> _pausePoints;

    // Julia Mode variables.
    bool _juliaMode;
    bool _juliaVariety;                    ///< Activate it in derived class if is a Julia variety. False by default.

    // Orbit mode variables.
    bool _hasOrbit;                        ///< False by default. Activate it if the derived class has a DrawOrbit method defined.
    bool _orbitMode;
    bool _orbitDrawn;
    double _orbitX, _orbitY;

    // Geometry variables.
    std::vector<CircleData> _circles;
    std::vector<LineData> _lines, _orbitLines;
    bool _geomFigure;

    // Effect variables.
    bool _hasOrbitTrap;                ///< False by default. Activate it if the derived class has an orbit trap routine.
    bool _orbitTrapMode;
    bool _hasSmoothRender;
    bool _smoothRender;

    // Internal methods.
    ///@brief Looks into the color palette for the corresponding color.
    ///@param index Color parameter.
    ///@return A struct with the color.
    sf::Color GetColorFromPalette(unsigned int index) const;

    ///@brief Rebuilds the color palette
    void RebuildPalette();

    ///@brief If some minor change was made like a color adjustment, redraws the maps.
    void RedrawMaps();

    ///@brief Recalculates the maximum rendered color-map value.
    void UpdateMaxColorMapValue();

    ///@brief Copies the current fractal state into a renderer before launch.
    void ConfigureRenderer(Renderer& renderer) const;
    void EnsurePreciseViewInitialized() const;
    void SyncDoubleViewFromPrecise();
    void UpdatePreciseFactors();
    [[nodiscard]] bool ShouldUseHighPrecision() const;
    [[nodiscard]] bool OptionsPreciseViewMatchesDoubleView(const Options& opt) const;

    ///@brief Selects the pixel regions that need rendering for the current movement state.
    std::vector<RenderRegion> BuildRenderRegions() const;

    ///@brief Splits render regions into jobs used by the selected render backend.
    std::vector<RenderJob> BuildRenderJobs(const std::vector<RenderRegion>& regions, int tileHeight) const;

    ///@brief Moves matrix elements and fills the exposed area with a default value.
    template<class M>
    void MoveMatrix(M** matrix, unsigned int matrixWidth, unsigned int matrixHeight, int moveX, int moveY, M fillValue = M{});

    static wxString FormatNumber(double value);
    static wxString FormatComplex(double real, double imaginary);

public:
    static constexpr unsigned int InvalidColor = std::numeric_limits<unsigned int>::max();

    // Basic methods.
    ///@brief Construct a fractal for the given render dimensions.
    ///@param width Image width.
    ///@param height Image height.
    Fractal(unsigned int width, unsigned int height);

    virtual ~Fractal();

    ///@brief Returns the display name supplied by the concrete fractal.
    virtual wxString GetName() const = 0;

    ///@brief Returns the display name of the selected rendering algorithm.
    wxString GetRenderingAlgorithmName() const;

    ///@brief SetAreaOfView to a specified size.
    ///@param width New width.
    ///@param height New height.
    void Resize(unsigned int width, unsigned int height);

    ///@brief Perform some adjustments needed before the rendering starts.
    void PrepareRender(Vector2Int reusedMapOffset = {0, 0});

    ///@brief Sets the fractal render viewport.
    ///@param worldCoordinates Viewport in world coordinates.
    void SetView(const Rect& worldCoordinates);
    void SetPreciseView(const PreciseRect& worldCoordinates);

    void Redraw();                     ///< Redraws the fractal.

    ///@brief Gets the current render dimensions.
    sf::Vector2u GetScreenSize() const;

    ///@brief Gets the current world-coordinate view.
    Rect GetView() const;
    PreciseRect GetPreciseView() const;

    ///@brief Converts a pixel rectangle into a world-coordinate view with the current aspect ratio.
    Rect GetViewForPixelRect(const sf::Rect<int>& pixelCoordinates) const;
    PreciseRect GetPreciseViewForPixelRect(const sf::Rect<int>& pixelCoordinates) const;

    ///@brief Gets a view expanded around the current one.
    Rect GetExpandedView(double scale = 1.0) const;
    PreciseRect GetPreciseExpandedView(double scale = 1.0) const;

    ///@brief Gets an aspect-correct view centered on a world-coordinate point.
    Rect GetCenteredView(double x, double y, double radius) const;
    PreciseRect GetPreciseCenteredView(const HighPrecisionReal& x, const HighPrecisionReal& y, const HighPrecisionReal& radius) const;

    ///@brief Pans the current view by a pixel delta.
    void PanViewByPixels(int pixelDeltaX, int pixelDeltaY);

    ///@brief Returns true once the current image has completed rendering.
    bool IsRendered() const;

    ///@brief Returns true when rendering has been started by the presenter.
    bool IsRenderStarted() const;

    ///@brief Marks the presenter-driven render pass as started.
    void MarkRenderStarted();

    ///@brief Marks the presenter-driven render pass as complete.
    void MarkRenderComplete();

    ///@brief Marks the rendered image as stale.
    void MarkRenderDirty();

    ///@brief Clears active render and pause presentation state.
    void MarkRenderInterrupted();

    ///@brief Marks an aborted render as visible but requiring a full redraw next time.
    void MarkRenderAborted();

    ///@brief Resumes from a paused pan by clearing pause and render flags.
    void ResumeFromPausedPan();

    ///@brief Returns true when the presenter should treat the fractal as paused.
    bool IsPausedForPresentation() const;

    ///@brief Returns true when a settled pan should leave the paused state.
    bool ShouldResumeFromPausedPan() const;

    ///@brief Consumes the one-frame pause refresh request.
    bool ConsumePausePresentationRefresh();

    ///@brief Consumes the one-frame image refresh request.
    bool ConsumeImageRefreshRequest();

    ///@brief Shifts the rendered maps after panning settles.
    void ReuseRenderedMaps(Vector2Int reusedMapOffset);

    ///@brief Prepares color lookup values before drawing rendered pixels.
    void PrepareDisplayColorLookup();

    ///@brief Returns true when the rendered maps have a display color at the pixel.
    bool HasDisplayPixelColor(unsigned int x, unsigned int y) const;

    ///@brief Gets the display color for a rendered pixel.
    sf::Color GetRenderedPixelColor(unsigned int x, unsigned int y) const;

    ///@brief Gets the color used for missing rendered pixels.
    sf::Color GetInvalidPixelColor() const;

    bool IsExteriorColorEnabled() const;
    bool IsRelativeColorEnabled() const;
    bool IsSetColorEnabled() const;
    bool IsGradientAnimating() const;
    bool ConsumeGradientChangeRequest();
    void AdvanceGradientOffset();
    void RefreshAnimatedColors(sf::Image& image);

    bool ShouldDrawOrbit() const;
    bool IsOrbitDrawn() const;
    void ClearOrbitLines();
    void MarkOrbitDirty();
    bool HasGeometryFigures() const;
    bool IsSnapshotActive() const;
    const std::vector<LineData>& GetLines() const;
    const std::vector<LineData>& GetOrbitLines() const;
    const std::vector<CircleData>& GetCircles() const;

    ///@brief Describes the measurements of the currently recorded orbit.
    wxString DescribeOrbit(bool escaped) const;

    ///@brief Renders the current view synchronously without creating an image.
    void RenderBlocking();

    ///@brief Returns the stored result for a rendered pixel.
    PointSample GetPointSample(unsigned int x, unsigned int y) const;

    ///@brief Evaluates and describes one world-coordinate point using the current fractal settings.
    wxString InspectPoint(double real, double imaginary,
                          std::optional<unsigned int> iterations = std::nullopt) const;

    // Thread control.
    ///@brief Calculate drawing limits of each thread and launches them.
    ///@param myRender Array of RenderFractal.
    template<class MT> void SetRendererBounds(MT* myRender);

    ///@brief Return a pointer to the watchdog.
    ///@return A pointer to the watchdog.
    ThreadWatchdog<Renderer>* GetWatchdog();

    ///@brief Returns progress for the active render backend.
    ///@return A value from 0 to 100.
    int GetRenderProgress();

    ///@brief Pauses or resumes the rendering.
    void PauseContinue();

    ///@brief If there are active threads stops them.
    bool StopRender();

    ///@brief Get pause status.
    ///@return true if paused, false if not.
    bool IsPaused() const;

    // Virtual methods.
    virtual void Render() = 0;                               ///< Start the fractal rendering. Defined in derived class.
    virtual void PreRender();                                ///< Perform necessary operations before starting to render.
    virtual void PreDrawMaps();                              ///< Perform necessary operations before drawing the maps.
    virtual void PostRender();                               ///< Perform necessary operations after the rendering is finished.
    virtual void PreRestartRender();                         ///< Perform necessary operations before restarting.

    ///@brief Verifies watchdog status.
    ///@return true if there is an active thread. false if not.
    virtual bool IsRendering();
    virtual void SetFormula(FormulaOptions formula);                   ///< Sets user formula.
    virtual void CopyOptFromPanel();                               ///< Copy options from the option panel.

    // Communication methods.
    ///@brief Get value of X number in the plane at the selected pixel.
    ///@param pixelX Pixel to inspect.
    ///@return Numerical value corresponding to the pixel position.
    double GetX(int pixelX) const;

    ///@brief Get value of Y number in the plane at the selected pixel.
    ///@param pixelY Pixel to inspect.
    ///@return Numerical value corresponding to the pixel position.
    double GetY(int pixelY) const;

    ///@brief Gets pixel corresponding to the specified numerical position.
    ///@param xNum Numerical position.
    ///@return Pixel corresponding to number.
    int GetPixelX(double xNum) const;

    ///@brief Gets pixel corresponding to the specified numerical position.
    ///@param yNum Numerical position.
    ///@return Pixel corresponding to number.
    int GetPixelY(double yNum) const;

    ///@brief Sets fractal options.
    ///@param opt Fractal options.
    ///@param keepSize If true doesn't copy new resolution.
    void SetOptions(const Options& opt, bool keepSize = false);

    ///@brief Gets fractal options.
    ///@return a Options struct with the fractal options.
    Options GetOptions() const;

    ///@brief Forces the fractal to acquire a "rendered" status.
    void SetRendered(bool mode);

    ///@brief Gets the type of the fractal.
    FractalType GetType() const;

    ///@brief Returns a pointer to the set map.
    bool** GetSetMap() const;

    void SetFractalPropChanged();
    bool GetChangeFractalProp();

    // Save image.
    sf::Image GetRenderedImage();
    wxBitmap GetRenderedWxBitmap();
    bool SaveBmp(const std::string& filename);
    void PrepareSnapshot(bool mode);

    // Color styles.
    void SetColorPalette(ColorPaletteTypes gradStyle);
    ColorPaletteTypes GetColorPalette() const;

    // Color operations.
    sf::Color GetSetColor() const;
    wxGradient* GetGradient();
    void SetExteriorColorMode(bool mode);
    void SetFractalSetColorMode(bool mode);
    void SetFractalSetColor(sf::Color color);
    bool GetExteriorColorMode() const;
    bool GetInteriorColorMode() const;
    void ChangeVarGradient();
    void SetPaletteSize(unsigned int size);
    unsigned int GetPaletteSize() const;
    void SetGradient(const wxGradient& grad);
    void SetGradientSize(unsigned int size);
    void SetRelativeColor(bool mode);
    bool GetRelativeColorMode() const;
    void SetVarGradient(unsigned int n);

    // Algorithm.
    RenderingAlgorithmType GetCurrentAlg() const;
    std::vector<RenderingAlgorithmType> GetAvailableAlg();
    void SetAlgorithm(RenderingAlgorithmType algorithm);

    // Julia mode operations.
    bool IsJuliaVariety() const;
    void SetJuliaMode(bool mode);
    void SetK(double real, double imaginary);
    double GetKReal() const;
    double GetKImaginary() const;

    // Orbit mode operations.
    void SetOrbitMode(bool mode);
    void SetOrbitPoint(double x, double y);
    bool HasOrbit() const;
    void SetOrbitChange();

    // Orbit trap operations.
    void SetOrbitTrapMode(bool mode);
    bool HasOrbitTrapMode() const;
    bool OrbitTrapActivated() const;

    // SmoothRender.
    void SetSmoothRender(bool mode);
    bool HasSmoothRenderMode() const;
    bool SmoothRenderActivated() const;

    // Menu operations.
    void SetIterations(unsigned int iterations);
    unsigned int GetIterations() const;

    // Option panel.
    bool HasOptPanel() const;
    PanelOptions* GetOptPanel();

    // Geometry.
    ///@brief Draws a simple line. Used in orbit mode.
    void DrawLine(double x1, double y1, double x2, double y2, sf::Color color = sf::Color(0, 0, 0), bool orbitLine = false);
    void DrawCircle(double xCenter, double yCenter, double radius, sf::Color color = sf::Color(0, 0, 0));
    void DrawCircle(double xCenter, double yCenter, double radius, sf::Color color, bool filled);
    void ClearGeometryFigures();
    virtual void DrawOrbit() {}
};

template<class DerivedRenderer> void Fractal::SetRendererBounds(DerivedRenderer* myRender)
{
    const std::vector<RenderRegion> regions = this->BuildRenderRegions();
    const bool useRenderPool = _renderJobCompatible && !_justLaunchThreads;
    const int tileHeight = useRenderPool ? 16 : 0;
    const std::vector<RenderJob> jobs = this->BuildRenderJobs(regions, tileHeight);
    const bool relaunchExistingWork = _justLaunchThreads && _pendingRenderOffset.x == 0 && _pendingRenderOffset.y == 0;
    _pendingRenderOffset = {0, 0};

    if (useRenderPool)
    {
        std::vector<Renderer*> renderers;
        renderers.reserve(_threadNumber);

        for (unsigned int i = 0; i < _threadNumber; i++)
        {
            this->ConfigureRenderer(myRender[i]);
            renderers.push_back(&myRender[i]);
        }

        _renderPool.Render(renderers, jobs);

        if (_waitRoutine)
            _renderPool.Wait();

        return;
    }

    _watchdog.Reset();

    for (unsigned int i = 0; i < _threadNumber; i++)
    {
        this->ConfigureRenderer(myRender[i]);

        const RenderJob& job = jobs[i];
        const RenderRegion& region = job.GetRegion();

        if (relaunchExistingWork)
            myRender[i].SetOldHeightOrigin(job.GetProgressOriginY());
        else if (job.IsEmpty())
            myRender[i].SetLimits(0, 0, 0, 1);
        else
            myRender[i].SetLimits(region.GetLeft(), region.GetTop(), region.GetRight(), region.GetBottom());
    }

    _watchdog.LaunchThreads();
    _watchdog.launch();

    if (_waitRoutine)
        _watchdog.wait();
}

template<class M> void Fractal::MoveMatrix(M** matrix, const unsigned int matrixWidth, const unsigned int matrixHeight,
                                           const int moveX, const int moveY, const M fillValue)
{
    if (matrix == nullptr || matrixWidth == 0 || matrixHeight == 0)
        return;

    if (std::abs(moveX) >= static_cast<int>(matrixWidth) || std::abs(moveY) >= static_cast<int>(matrixHeight))
    {
        for (unsigned int i = 0; i < matrixHeight; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);

        return;
    }

    if (moveX > 0)
    {
        const auto displacement = static_cast<unsigned int>(moveX);
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            std::move_backward(matrix[i], matrix[i] + matrixWidth - displacement, matrix[i] + matrixWidth);
            std::fill(matrix[i], matrix[i] + displacement, fillValue);
        }
    }
    else if (moveX < 0)
    {
        const auto displacement = static_cast<unsigned int>(-moveX);
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            std::move(matrix[i] + displacement, matrix[i] + matrixWidth, matrix[i]);
            std::fill(matrix[i] + matrixWidth - displacement, matrix[i] + matrixWidth, fillValue);
        }
    }

    if (moveY > 0)
    {
        const auto displacement = static_cast<unsigned int>(moveY);
        std::rotate(matrix, matrix + matrixHeight - displacement, matrix + matrixHeight);
        for (unsigned int i = 0; i < displacement; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);
    }
    else if (moveY < 0)
    {
        const auto displacement = static_cast<unsigned int>(-moveY);
        std::rotate(matrix, matrix + displacement, matrix + matrixHeight);
        for (unsigned int i = matrixHeight - displacement; i < matrixHeight; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);
    }
}
