#pragma once
#ifndef FRACTAL_H
#define FRACTAL_H
#include <vector>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "wx/gradient.h"
#include "types/FractalType.h"
#include "types/RenderingAlgorithm.h"
#include "types/Direction.h"
#include "geometry/LineData.h"
#include "geometry/CircleData.h"
#include "geometry/Vector2Int.h"
#include "geometry/Vector2Double.h"
#include "geometry/Rect.h"
#include "../gui/wx/PanelOptions.h"
#include "ColorPalettes.h"
#include "Options.h"
#include "FormulaOpt.h"
#include "RenderFractal.h"
#include "ThreadWatchdog.h"

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
    ThreadWatchdog<RenderFractal> _watchdog;    ///< Watch over the render threads.

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
    RenderingAlgorithm _alg;
    std::vector<RenderingAlgorithm> _availableAlg;
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
    bool _moving;                            ///< Movement status.
    bool _rendered;
    bool _rendering;
    bool _paused;
    bool _pausing;
    bool _changeFractalIter;
    bool _varGradient;                       ///< If this is activated (by the play button) the gradient variation mode starts.
    bool _onSnapshot;
    bool _waitRoutine;
    bool _redrawAll;
    bool _redrawAlways;
    bool _justLaunchThreads;
    bool _varGradChange;
    bool _renderJobComp;                     ///< Fractal compatible with renderJobs.
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
    sf::Color CalcColor(int colorNum) const;

    ///@brief Rebuilds the color palette
    void RebuildPalette();

    ///@brief If some minor change was made like a color adjustement, redraws the maps.
    void RedrawMaps();

    void SetDefaultOpt();

public:

    // Basic methods.
    ///@brief Construct a fractal that will not be drawn to the screen. Used to save a image.
    ///@param width Image width.
    ///@param height Image height.
    Fractal(int width, int height);

    ///@brief Construct a fractal that will be drawn to the screen.
    ///@param Window Window to draw the fractal.
    Fractal(sf::RenderWindow* Window);

    virtual ~Fractal();

    ///@brief SetAreaOfView to a specified size.
    ///@param width New width.
    ///@param height New height.
    void Resize(int width, int height);

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
    static void DeleteSavedZooms();    ///< If some image property image has changed, deletes saved zoom images.
    void Redraw();                     ///< Redraws the fractal.

    // Thread control.
    ///@brief Calculate drawing limits of each thread and launches them.
    ///@param myRender Array of RenderFractal.
    template<class MT> void TRender(MT* myRender);

    ///@brief Return a pointer to the watchdog.
    ///@return A pointer to the watchdog.
    ThreadWatchdog<RenderFractal>* GetWatchdog();

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
    virtual void MoreIter();                                       ///< Increases the number of iterations.
    virtual void LessIter();                                       ///< Decreases iterations.
    virtual void ChangeThreadNumber();                             ///< Changes the number of rendering threads.

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
    Options GetOptions();

    ///@brief Forces the fractal to acquire a "rendered" status.
    void SetRendered(bool mode);

    ///@brief Return the farthest zoom viewed by the user.
    Rect GetOutermostZoom();

    ///@brief Return the current zoom rect.
    Rect GetCurrentZoom();

    ///@brief Gets the type of the fractal.
    FractalType GetType();

    ///@brief Returns a pointer to the set map.
    bool** GetSetMap();

    bool IsMoving();
    void SetFractalPropChanged();
    bool GetChangeFractalProp();
    void SetOnWxCtrl(bool mode);
    void SetMovement(Direction dir);
    void ReleaseMovement(Direction dir);

    // Save image.
    sf::Image GetRenderedImage();
    wxBitmap GetRenderedWxBitmap();
    void RenderBMP(const std::string& filename);
    void PrepareSnapshot(bool mode);

    // Color styles.
    void SetColorPalette(ColorPalettes gradStyle);
    ColorPalettes GetColorPalette() const;

    // Color operations.
    sf::Color GetSetColor();
    wxGradient* GetGradient();
    void SetExtColorMode(bool mode);
    void SetFractalSetColorMode(bool mode);
    void SetFractalSetColor(sf::Color color);
    bool GetExteriorColorMode();
    bool GetInteriorColorMode();
    void ChangeVarGradient();
    void SetPaletteSize(int size);
    int GetPaletteSize();
    void SetGradient(wxGradient grad);
    void SetGradientSize(unsigned int size);
    void SetRelativeColor(bool mode);
    bool GetRelativeColorMode() const;
    void SetVarGradient(int n);

    // Algorithm.
    RenderingAlgorithm GetCurrentAlg() const;
    std::vector<RenderingAlgorithm> GetAvailableAlg();
    void SetAlgorithm(RenderingAlgorithm _alg);

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
    void ChangeIterations(int number);
    unsigned int GetIterations();

    // Option panel.
    bool HasOptPanel() const;
    PanelOptions* GetOptPanel();

    // Geometry.
    ///@brief Draws a simple line. Used in orbit mode.
    void DrawLine(double x1, double y1, double x2, double y2, sf::Color color = sf::Color(0, 0, 0), bool orbitLine = false);
    void DrawCircle(double x_center, double y_center, double radius, sf::Color color = sf::Color(0, 0, 0));
    ///@brief By default, it doesn't do anything. Has to be overridden in derived class.
    virtual void DrawOrbit() {}
};

template<class MT> void Fractal::TRender(MT* myRender)
{
    _watchdog.Reset();
    // If the image has been moved, divides the rendering area so threads will draw the missing part.
    if (_xMoved != 0 || _yMoved != 0)
    {
        if (_xMoved && _yMoved)
        {
            for (unsigned int i = 0; i < _threadNumber; i++)
            {
                myRender[i].SetOpt(this->GetOptions());
                myRender[i].SetRenderOut(_setMap, _colorMap, _auxMap);

                if (_orbitTrapMode || _smoothRender)
                    myRender[i].SetSpecialRenderMode(true);
                else
                    myRender[i].SetSpecialRenderMode(false);

                myRender[i].SetK(_kReal, _kImaginary);
            }

            if (_xMoved > 0 && _yMoved < 0)
            {
                // First thread group.
                unsigned int localThreadN = ceil(static_cast<double>(_threadNumber) / 2.0);
                int Div = static_cast<int>(floor((_screenHeight + _yMoved) / static_cast<double>(localThreadN)));
                int Step = Div;
                for (unsigned int i = 0; i < localThreadN; i++)
                {
                    if (i + 2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step - Div, _xMoved, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, _xMoved, _screenHeight + _yMoved);
                }

                // Second thread group.
                Div = static_cast<int>(floor(abs(_yMoved) / static_cast<double>(_threadNumber - localThreadN)));
                Step = Div;
                int start = _screenHeight + _yMoved;
                for (unsigned int i = localThreadN; i < _threadNumber; i++)
                {

                    if (i + 2 != _threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step - Div, _screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, _screenWidth, _screenHeight);
                }
            }
            else if (_xMoved > 0 && _yMoved > 0)
            {
                // First thread group.
                unsigned int localThreadN = ceil(static_cast<double>(_threadNumber) / 2.0);
                int Div = static_cast<int>(floor(abs(_yMoved) / static_cast<double>(localThreadN)));
                int Step = Div;
                for (unsigned int i = 0; i < localThreadN; i++)
                {
                    if (i + 2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step - Div, _screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, _screenWidth, _yMoved);
                }

                // Second thread group.
                int start = _yMoved;
                Div = static_cast<int>(floor((_screenHeight - _yMoved) / static_cast<double>(_threadNumber - localThreadN)));
                Step = Div;
                for (unsigned int i = localThreadN; i < _threadNumber; i++)
                {
                    if (i + 2 != _threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step - Div, _xMoved, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, _xMoved, _screenHeight);
                }
            }
            else if (_xMoved < 0 && _yMoved < 0)
            {
                // First thread group.
                unsigned int localThreadN = ceil(static_cast<double>(_threadNumber) / 2.0);
                int Div = static_cast<int>(floor((_screenHeight + _yMoved) / static_cast<double>(localThreadN)));
                int Step = Div;
                for (unsigned int i = 0; i < localThreadN; i++)
                {
                    if (i + 2 != localThreadN)
                    {
                        myRender[i].SetLimits(_screenWidth + _xMoved, Step - Div, _screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(_screenWidth + _xMoved, Step, _screenWidth, _screenHeight + _yMoved);
                }

                // Second thread group.
                Div = static_cast<int>(floor(abs(_yMoved) / (double)(_threadNumber - localThreadN)));
                Step = Div;
                int start = _screenHeight + _yMoved;
                for (unsigned int i = localThreadN; i < _threadNumber; i++)
                {

                    if (i + 2 != _threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step - Div, _screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, _screenWidth, _screenHeight);
                }
            }
            else if (_xMoved < 0 && _yMoved > 0)
            {
                // First thread group.
                unsigned int localThreadN = ceil(static_cast<double>(_threadNumber) / 2.0);
                int Div = static_cast<int>(floor(abs(_yMoved) / static_cast<double>(localThreadN)));
                int Step = Div;
                for (unsigned int i = 0; i < localThreadN; i++)
                {
                    if (i + 2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step - Div, _screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, _screenWidth, _yMoved);
                }

                // Second thread pack.
                int start = _yMoved;
                Div = static_cast<int>(floor((_screenHeight - _yMoved) / static_cast<double>(_threadNumber - localThreadN)));
                Step = Div;
                for (unsigned int i = localThreadN; i < _threadNumber; i++)
                {
                    if (i + 2 != _threadNumber)
                    {
                        myRender[i].SetLimits(_screenWidth + _xMoved, start + Step - Div, _screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(_screenWidth + _xMoved, start + Step, _screenWidth, _screenHeight);
                }
            }
        }
        else if (_xMoved)
        {
            int Div = static_cast<int>(floor(_screenHeight / static_cast<double>(_threadNumber)));
            int Step = Div;
            if (_xMoved > 0)
            {
                for (unsigned int i = 0; i < _threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(_setMap, _colorMap, _auxMap);

                    if (_orbitTrapMode || _smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(_kReal, _kImaginary);

                    if (i + 2 != _threadNumber)
                    {
                        myRender[i].SetLimits(0, Step - Div, _xMoved, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, _xMoved, _screenHeight);
                }
            }
            else
            {
                for (unsigned int i = 0; i < _threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(_setMap, _colorMap, _auxMap);

                    if (_orbitTrapMode || _smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(_kReal, _kImaginary);

                    if (i + 2 != _threadNumber)
                    {
                        myRender[i].SetLimits(_screenWidth + _xMoved, Step - Div, _screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(_screenWidth + _xMoved, Step, _screenWidth, _screenHeight);
                }
            }
        }
        else if (_yMoved)
        {
            if (_yMoved > 0)
            {
                int Div = static_cast<int>(floor(_yMoved / static_cast<double>(_threadNumber)));
                int Step = Div;

                for (unsigned int i = 0; i < _threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(_setMap, _colorMap, _auxMap);

                    if (_orbitTrapMode || _smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(_kReal, _kImaginary);

                    if (i + 2 != _threadNumber)
                    {
                        myRender[i].SetLimits(0, Step - Div, _screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, _screenWidth, _yMoved);
                }
            }
            else
            {
                int Div = static_cast<int>(floor(abs(_yMoved) / static_cast<double>(_threadNumber)));
                int Step = Div;

                for (unsigned int i = 0; i < _threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(_setMap, _colorMap, _auxMap);

                    if (_orbitTrapMode || _smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(_kReal, _kImaginary);

                    int start = _screenHeight + _yMoved;

                    if (i + 2 != _threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step - Div, _screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, _screenWidth, _screenHeight);
                }
            }
        }
    }
    else
    {
        // Draws all the screen.
        int Div = static_cast<int>(floor(_screenHeight / static_cast<double>(_threadNumber)));
        int Step = Div;

        for (unsigned int i = 0; i < _threadNumber; i++)
        {
            myRender[i].SetOpt(this->GetOptions());
            myRender[i].SetRenderOut(_setMap, _colorMap, _auxMap);

            if (_orbitTrapMode || _smoothRender)
                myRender[i].SetSpecialRenderMode(true);
            else
                myRender[i].SetSpecialRenderMode(false);

            myRender[i].SetK(_kReal, _kImaginary);

            if (!_justLaunchThreads)
            {
                if (i + 2 != _threadNumber)
                {
                    myRender[i].SetLimits(0, Step - Div, _screenWidth, Step);
                    Step += Div;
                }
                else
                    myRender[i].SetLimits(0, Step, _screenWidth, _screenHeight);
            }
            else
            {
                if (i + 2 != _threadNumber)
                {
                    myRender[i].SetOldHo(Step - Div);
                    Step += Div;
                }
                else
                    myRender[i].SetOldHo(Step);
            }
        }
    }

    _watchdog.LaunchThreads();
    _watchdog.launch();

    if (_waitRoutine)
    {
        _watchdog.wait();
    }
}

#endif
