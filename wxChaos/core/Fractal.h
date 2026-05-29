#pragma once
#ifndef FRACTAL_H
#define FRACTAL_H
#include <vector>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "wx/wxGradient.h"
#include "types/FractalType.h"
#include "types/RenderingAlgorithmType.h"
#include "types/Direction.h"
#include "geometry/LineData.h"
#include "geometry/CircleData.h"
#include "geometry/Vector2Int.h"
#include "geometry/Rect.h"
#include "ColorPalettes.h"
#include "Options.h"
#include "FormulaOpt.h"
#include "Renderer.h"
#include "ThreadWatchdog.h"
#include "rendering/RenderJob.h"
#include "rendering/RenderRegion.h"
#include "rendering/RenderThreadPool.h"
#include "../gui/wx/PanelOptions.h"

class SFMLFractal;

/**
* @class Fractal
* @brief Provides an interface to do the fractal rendering and draw the result to the screen.
*
* This is an abstract class. Its purpose is to provide a set of methods to do the fractal navigation routines, render the result
* to the screen, allocate memory for the rendering maps, provide a color palette for the drawing, and perform zooming operations.
*/
class Fractal
{
    friend class SFMLFractal;

protected:
    bool** _setMap;                             ///< Stores the points that belong to the fractal set.
    int** _colorMap;                            ///< Store the color map.
    unsigned int** _auxMap;                     ///< An additional map to perform some auxiliary operations.
    ThreadWatchdog<Renderer> _watchdog;    ///< Watch over the render threads.
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
    unsigned _maxIter;               ///< Maximum number of iterations.
    FormulaOpt _userFormula;         ///< Formula specified by the user.

    // System.
    unsigned int _threadNumber;      ///< Number of threads. By default, is the same as the number of cores in the system.

    // Julia variables.
    double _kReal;
    double _kImaginary;

    // Image properties.
    int _xVel, _yVel;                 ///< Speed of movement of the fractal image.
    int _posX, _posY;                 ///< Position of the fractal image.
    int _xMoved, _yMoved;             ///< Total movement of the image. Used just before redering a new area.

    std::vector<double> _zoom[4];     ///< Saves the performed zooms.
    Rect _outermostZoom;
    unsigned int _screenWidth;
    unsigned int _screenHeight;
    unsigned int _backScreenWidth;
    int _changeGradient;
    double _magnification;

    // Color properties.
    RenderingAlgorithmType _algorithm;
    std::vector<RenderingAlgorithmType> _availableAlg;
    wxGradient _gradient;                   ///< Gradient to be used.
    wxColour _fSetColor;                    ///< Color of points belonging to the set.
    std::vector<wxColour> _palette;
    bool _relativeColor;
    bool _colorSet;                         ///< Activates internal coloring.
    bool _colorMode;                        ///< Activates external coloring.
    ColorPalettes _gradStyle;               ///< Grad color palette to be used.
    int _paletteSize;
    int _gradPaletteSize;
    int _varGradientStep;
    int _maxColorMapVal;
    bool _refreshImage;

    // Status variables.
    bool _movement[4];
    bool _moving;                           ///< Movement status.
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
    bool _renderJobComp;                    ///< Fractal compatible with renderJobs.
    bool _changeFractalProp;
    bool _onWxCtrl;
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
    ///@brief Saves the rendering area limits to be able to do a zoom-back later.
    void SaveZoom();

    ///@brief Called in child class after constructor.
    void SetOutermostZoom();

    ///@brief Looks into the color palette for the corresponding color.
    ///@param colorNum Color parameter.
    ///@return A struct with the color.
    sf::Color GetColorFromPalette(int colorNum) const;

    ///@brief Rebuilds the color palette
    void RebuildPalette();

    ///@brief If some minor change was made like a color adjustment, redraws the maps.
    void RedrawMaps();

    void SetDefaultOptions();

    ///@brief Copies the current fractal state into a renderer before launch.
    void ConfigureRenderer(Renderer& renderer) const;

    ///@brief Selects the pixel regions that need rendering for the current movement state.
    std::vector<RenderRegion> BuildRenderRegions() const;

    ///@brief Splits render regions into jobs used by the selected render backend.
    std::vector<RenderJob> BuildRenderJobs(const std::vector<RenderRegion>& regions, int tileHeight) const;

public:

    // Basic methods.
    ///@brief Construct a fractal that will not be drawn to the screen. Used to save a image.
    ///@param width Image width.
    ///@param height Image height.
    Fractal(int width, int height);

    ///@brief Construct a fractal that will be drawn to the screen.
    ///@param window Window to draw the fractal.
    explicit Fractal(const sf::RenderWindow* window);

    virtual ~Fractal();

    ///@brief SetAreaOfView to a specified size.
    ///@param width New width.
    ///@param height New height.
    void Resize(unsigned int width, unsigned int height);

    ///@brief Perform some adjustments needed before the rendering starts.
    void PrepareRender();

    ///@brief Resizes the viewing area of the fractal.
    ///@param pixelCoordinates Selection area in pixel coordinates.
    void SetAreaOfView(sf::Rect<int> pixelCoordinates);

    ///@brief Resizes the viewing area of the fractal.
    ///@param worldCoordinates Selection area in world coordinates.
    void SetAreaOfView(const Rect& worldCoordinates);

    void Move();                       ///< Moves the fractal image.
    void ZoomBack();                   ///< Does a zoom-back in the selection area.
    void Redraw();                     ///< Redraws the fractal.

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
    virtual void SetFormula(FormulaOpt formula);                   ///< Sets user formula.
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

    ///@brief Return the farthest zoom viewed by the user.
    Rect GetOutermostZoom() const;

    ///@brief Return the current zoom rect.
    Rect GetCurrentZoom() const;

    ///@brief Gets the type of the fractal.
    FractalType GetType() const;

    ///@brief Returns a pointer to the set map.
    bool** GetSetMap() const;

    bool IsMoving() const;
    void SetFractalPropChanged();
    bool GetChangeFractalProp();
    void SetOnWxCtrl(bool mode);
    void SetMovement(Direction direction);
    void ReleaseMovement(Direction direction);

    // Save image.
    sf::Image GetRenderedImage();
    wxBitmap GetRenderedWxBitmap();
    void RenderBMP(const std::string& filename);
    void PrepareSnapshot(bool mode);

    // Color styles.
    void SetColorPalette(ColorPalettes gradStyle);
    ColorPalettes GetColorPalette() const;

    // Color operations.
    sf::Color GetSetColor() const;
    wxGradient* GetGradient();
    void SetExtColorMode(bool mode);
    void SetFractalSetColorMode(bool mode);
    void SetFractalSetColor(sf::Color color);
    bool GetExteriorColorMode() const;
    bool GetInteriorColorMode() const;
    void ChangeVarGradient();
    void SetPaletteSize(int size);
    int GetPaletteSize() const;
    void SetGradient(const wxGradient& grad);
    void SetGradientSize(unsigned int size);
    void SetRelativeColor(bool mode);
    bool GetRelativeColorMode() const;
    void SetVarGradient(int n);

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
    void DrawCircle(double x_center, double y_center, double radius, sf::Color color = sf::Color(0, 0, 0));
    virtual void DrawOrbit() {}
};

template<class DerivedRenderer> void Fractal::SetRendererBounds(DerivedRenderer* myRender)
{
    const std::vector<RenderRegion> regions = this->BuildRenderRegions();
    const bool useRenderPool = _renderJobComp && !_justLaunchThreads;
    const int tileHeight = useRenderPool ? 16 : 0;
    const std::vector<RenderJob> jobs = this->BuildRenderJobs(regions, tileHeight);
    const bool relaunchExistingWork = _justLaunchThreads && _xMoved == 0 && _yMoved == 0;

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
        {
            myRender[i].SetOldHeightOrigin(job.GetProgressOriginY());
        }
        else if (job.IsEmpty())
        {
            myRender[i].SetLimits(0, 0, 0, 1);
        }
        else
        {
            myRender[i].SetLimits(region.GetLeft(), region.GetTop(), region.GetRight(), region.GetBottom());
        }
    }

    _watchdog.LaunchThreads();
    _watchdog.launch();

    if (_waitRoutine)
        _watchdog.wait();
}

#endif
