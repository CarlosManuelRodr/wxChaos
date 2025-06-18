#pragma once
#ifndef FRACTAL_H
#define FRACTAL_H
#include <complex>
#include "Styles.h"
#include "wx/gradient.h"
#include "../gui/sfml/FractalGUI.h"
#include <mpParser.h>
#include "base/FractalType.h"
#include "base/Color.h"
#include "base/RenderingAlgorithm.h"
#include "base/ColorMode.h"
#include "base/Direction.h"
#include "base/LineData.h"
#include "base/CircleData.h"
#include "base/Options.h"
#include "base/FormulaType.h"
#include "base/FormulaOpt.h"
#include "base/Vector2Int.h"
#include "base/Vector2Double.h"
#include "base/Rect.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// Clases fractal y threadWatchdog
/**
* @class ThreadWatchdog
* @brief Control the execution of the threads.
*
* The watchdog main purpose is to control the execution and flow of the threads. It provides methods to watch their status,
* stop them, reset them and relaunch them.
* @tparam MT Must be a RenderFractal inherited class.
*/
template<class MT> class ThreadWatchdog : public sf::Thread
{
    MT** threadList;               ///< An array with pointers to the execution threads.
    sf::Thread** sfmlThreads;      ///< An array to hold the actual sf::Thread objects.
    bool threadRunning;            ///< State of the threads.
    unsigned int threadCounter;    ///< Number of threads to watch over.
public:
    ThreadWatchdog();
    ~ThreadWatchdog();

    virtual void run();

    ///@brief Changes the number of execution threads. For this it will have to delete the previous ones.
    ///@param nThreads Number of new threads.
    void SetThreadNumber(int nThreads);

    ///@brief Sets a new thread to watch over.
    ///@param threadAdress Pointer to the thread to watch over.
    void SetThread(MT* threadAdress);

    ///@brief Resets the RenderFractal.
    void Reset();

    ///@brief Launch all the threads in the threadList.
    void LaunchThreads();

    ///@brief Stops all the threads in the threadList.
    void StopThreads();

    ///@brief Informs if there is a thread running.
    ///@return true if there is a thread running. false if not.
    bool ThreadRunning();

    ///@brief Ask the RenderFractal the render progress.
    ///@return A integer from 0 to 100 that is the progress.
    int GetThreadProgress();

    ///@brief Get the thread in the specified index.
    ///@param nThread Index of the thread to return.
    ///@return A pointer to the specified thread index.
    MT* GetThread(unsigned int nThread);
};

/**
* @brief Sets the watchdog for the specified threads.
* @param MT Must be a RenderFractal inherited class.
* @param myRender Array of render threads.
* @param watchdog Pointer to the watchdog that will be used.
* @param threadNumber Number of threads to set.
*/
template<class MT> inline void SetWatchdog(MT* myRender, ThreadWatchdog<RenderFractal>* watchdog, unsigned int threadNumber)
{
    watchdog->SetThreadNumber(threadNumber);
    for (unsigned int i = 0; i < threadNumber; i++)
        watchdog->SetThread(&myRender[i]);
}

template<class MT> ThreadWatchdog<MT>::ThreadWatchdog() : sf::Thread(&ThreadWatchdog<MT>::run, this)
{
    threadCounter = 0;
    threadRunning = false;
    threadList = nullptr;
    sfmlThreads = nullptr;
}
template<class MT> ThreadWatchdog<MT>::~ThreadWatchdog()
{
    if (threadList != nullptr)
        delete[] threadList;
    if (sfmlThreads != nullptr)
    {
        // Ensure threads are stopped and deleted
        if (threadRunning) StopThreads();
        delete[] sfmlThreads;
    }
}
template<class MT> void ThreadWatchdog<MT>::SetThreadNumber(int nThreads)
{
    if (threadList != nullptr)
    {
        delete[] threadList;
    }
    if (sfmlThreads != nullptr)
    {
        delete[] sfmlThreads;
    }
    threadCounter = 0;
    threadRunning = false;

    threadList = new MT * [nThreads];
    sfmlThreads = new sf::Thread * [nThreads]; // Allocate for sf::Thread pointers
    for (int i = 0; i < nThreads; ++i)
    {
        sfmlThreads[i] = nullptr; // Initialize to null
    }
}
template<class MT> void ThreadWatchdog<MT>::SetThread(MT* threadAdress)
{
    threadList[threadCounter++] = threadAdress;
}
template<class MT> void ThreadWatchdog<MT>::run()
{
    // We don't want to collapse our system.
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
    // Wait for every thread to finish and change status.
    for (unsigned int i = 0; i < threadCounter; i++)
    {
        if (sfmlThreads[i])
        {
            sfmlThreads[i]->wait();
            delete sfmlThreads[i]; // Clean up after it's done
            sfmlThreads[i] = nullptr;
        }
    }

    threadRunning = false;
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}
template<class MT> void ThreadWatchdog<MT>::Reset()
{
    for (unsigned int i = 0; i < threadCounter; i++)
        threadList[i]->Reset();

    threadRunning = true;
}
template<class MT> bool ThreadWatchdog<MT>::ThreadRunning()
{
    return threadRunning;
}
template<class MT> void ThreadWatchdog<MT>::LaunchThreads()
{
    // Launches all the threads.
    threadRunning = true;
    for (unsigned int i = 0; i < threadCounter; i++)
    {
        // Create a new thread that will call the run() method of our RenderFractal object
        sfmlThreads[i] = new sf::Thread(&RenderFractal::run, threadList[i]);
        sfmlThreads[i]->launch();
    }
}
template<class MT> void ThreadWatchdog<MT>::StopThreads()
{
    for (unsigned int i = 0; i < threadCounter; i++)
    {
        threadList[i]->PreTerminate();
        threadList[i]->Stop();
    }

    for (unsigned int i = 0; i < threadCounter; i++)
    {
        if (sfmlThreads[i])
        {
            sfmlThreads[i]->wait();
            delete sfmlThreads[i];
            sfmlThreads[i] = nullptr;
        }
    }

    threadRunning = false;

#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}
template<class MT> int ThreadWatchdog<MT>::GetThreadProgress()
{
    int progress = 0;
    for (unsigned int i = 0; i < threadCounter; i++)
        progress += threadList[i]->AskProgress();

    return (double)progress / (double)threadCounter;
}
template<class MT> MT* ThreadWatchdog<MT>::GetThread(unsigned int nThread)
{
    if (nThread >= 0 && nThread < threadCounter)
        return threadList[nThread];
    else
        return nullptr;
}

/**
* @class Fractal
* @brief Provides an interface to do the fractal rendering and draw the result to the screen.
*
* This is an abstract class. It's purpose is to provide a set of methods to do the fractal navigation routines, render the result
* to the screen, allocate memory for the rendering maps, provide a color palette for the drawing, and perform zooming operations.
*/
class Fractal
{
protected:
    bool** setMap;                             ///< Stores the points that belong to the fractal set.
    int** colorMap;                            ///< Store the color map.
    unsigned int** auxMap;                     ///< An additional map to perform some auxiliary operations.
    ThreadWatchdog<RenderFractal> watchdog;    ///< Watch over the render threads.

    // Fractal properties.
    PanelOptions panelOpt;          ///< List of GUI elements to put into the option panel.
    FractalType type;               ///< Type of fractal to render.
    double minX;                    ///< Left numeric limit of the fractal.
    double maxX;                    ///< Right numeric limit of the fractal.
    double minY;                    ///< Lower numeric limit of the fractal.
    double maxY;                    ///< Upper numeric limit of the fractal.
    double xFactor;                 ///< Conversion factor numberX to pixelX.
    double yFactor;                 ///< Conversion factor numberY to pixelY.
    unsigned maxIter;               ///< Maximum number of iterations.
    FormulaOpt userFormula;         ///< Formula specified by the user.

    // System.
    unsigned int threadNumber;      ///< Number of threads. By default is the same as the number of cores in the system.

    // Julia variables.
    double kReal;
    double kImaginary;

    // Image properties.
    int xVel, yVel;                 ///< Speed of movement of the fractal image.
    int posX, posY;                 ///< Positionof the fractal image.
    int xMoved, yMoved;             ///< Total movement of the image. Used just before redering a new area.

    sf::Image image;                ///< Layer where the output image is created.
    sf::Texture texture;
    sf::Sprite output;              ///< Sprite to draw the output image.
    vector<sf::Image> imgVector;    ///< Vector of rendering images that are loaded on zoomback.
    sf::Font font;
    sf::Text text;
    wxString tempText;
    sf::Image tempImage;            ///< Temporary image. Shows low res image while renering.
    sf::Texture tempTexture;
    sf::Sprite tempSprite;          ///< tempImage sprite.

    vector<double> zoom[4];         ///< Saves the performed zooms.
    Rect outermostZoom;
    int screenWidth;
    int screenHeight;
    int backScreenWidth;
    int changeGradient;
    double magnification;

    // Color properties.
    RenderingAlgorithm alg;
    vector<RenderingAlgorithm> availableAlg;
    ColorMode colorPaletteMode;
    wxGradient gradient;                    ///< Gradient to be used.
    sf::Color white;
    sf::Color fSetColor;                    ///< Color of points belonging to the set.
    sf::Color transparent;
    sf::Uint8* redPalette;
    sf::Uint8* greenPalette;
    sf::Uint8* bluePalette;
    sf::Color* palette;
    int redInt, greenInt, blueInt;                ///< Intensity parameters.
    double redMean, greenMean, blueMean;          ///< Mean parameters.
    double redStdDev, greenStdDev, blueStdDev;    ///< Standard deviation parameters.
    bool relativeColor;
    bool colorSet;                                ///< Activates internal coloring.
    bool colorMode;                               ///< Activates external coloring.
    GaussianColorStyles gaussianStyle;            ///< Gaussian color palette to be used.
    GradientColorStyles gradStyle;                ///< Grad color palette to be used.
    int paletteSize;
    int gaussianPaletteSize;
    int gradPaletteSize;
    int varGradientStep;
    int maxColorMapVal;

    // Status variables.
    bool movement[4];
    bool moving;                            ///< Movement status.
    bool rendered;
    bool rendering;
    bool paused;
    bool pausing;
    bool changeFractalIter;
    bool varGradient;                       ///< If this is activated (by the play button) the gradient variation mode starts.
    bool onSnapshot;
    bool waitRoutine;
    bool redrawAll;
    bool redrawAlways;
    bool imgInVector;
    bool usingRenderImage;;
    bool renderingToScreen;
    bool justLaunchThreads;
    bool zoomingBack;
    bool dontDrawTempImage;
    bool varGradChange;
    bool renderJobComp;                     ///< Fractal compatible with renderJobs.
    bool changeFractalProp;
    bool onWxCtrl;
    vector<Vector2Int> endPoints;
    vector<Vector2Int> startPoints;
    vector<Vector2Int> pausePoints;

    // Julia Mode variables.
    bool juliaMode;
    bool juliaVariety;                    ///< Activate it in derived class if is a Julia variety. False by default.

    // Orbit mode variables.
    bool hasOrbit;                        ///< False by default. Activate it if the derived class has a DrawOrbit method defined.
    bool orbitMode;
    bool orbitDrawn;
    double orbitX, orbitY;

    // Geometry variables.
    vector<CircleData> circles;
    vector<LineData> lines, orbitLines;
    bool geomFigure;
    sf::Image geomImage;
    sf::Texture geomTexture;
    sf::Sprite outGeom;

    // Effect variables.
    bool hasOrbitTrap;                ///< False by default. Activate it if the derived class has a orbit trap routine.
    bool orbitTrapMode;
    bool hasSmoothRender;
    bool smoothRender;

    // Internal methods.
    ///@brief Saves the rendering area limits to be able to do a zoomback later.
    void SaveZoom();

    ///@brief Called in child class after constructor.
    void SetOutermostZoom();

    ///@brief Calculate a EST color in the specified channel.
    ///@param colorNum Color parameter.
    ///@param col Channel to calculate.
    ///@return A 8 bit unsigned integer with the calculated color.
    sf::Uint8 CalcGradient(int colorNum, Color col);

    ///@brief Looks into the color palette for the corresponding color.
    ///@param colorNum Color parameter.
    ///@return A struct with the color.
    sf::Color CalcColor(int colorNum);

    ///@brief Calculates a normal distribution.
    double NormalDist(int x, double mean, double stdDev);

    ///@brief Rebuilds the color palette in the colorPaletteMode.
    void RebuildPalette();

    ///@brief Draws the maps into the screen.
    void DrawMaps(sf::RenderWindow* Window);

    ///@brief If some minor change was made like a color adjustement redraws the maps.
    void RedrawMaps();

    // Geometry.
    ///@brief Draws a simple line. Used in orbit mode.
    void DrawLine(double x1, double y1, double x2, double y2, sf::Color color = sf::Color(0, 0, 0), bool orbitLine = false);
    void DrawCircle(double x_center, double y_center, double radius, sf::Color color = sf::Color(0, 0, 0));
    void DrawGeom(sf::RenderWindow* Window);

    ///@brief By default it doesn't do anything. Has to be overriden in derived class.
    virtual void DrawOrbit() {}

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

    ~Fractal();

    ///@brief Draws the fractal into the screen.
    ///@param Window Window to draw the fractal.
    void Show(sf::RenderWindow* Window);

    ///@brief Resizes the fractal.
    ///@param Window Window to draw the fractal.
    void Resize(sf::RenderWindow* Window);

    ///@brief SetAreaOfView to specified size.
    ///@param width New width.
    ///@param height New height.
    void Resize(int width, int height);

    ///@brief Perform some adjustements needed before the rendering starts.
    void PrepareRender();

    ///@brief Resizes the viewing area of the fractal.
    ///@param pixelCoordinates Selection area in pixel coordinates.
    void SetAreaOfView(sf::Rect<int> pixelCoordinates);

    ///@brief Resizes the viewing area of the fractal.
    ///@param worldCoordinates Selection area in world coordinates.
    void SetAreaOfView(Rect worldCoordinates);

    void Move();                ///< Moves the fractal image.
    void MoveKeyboard();
    void ZoomBack();            ///< Does a zoomback in the selection area.
    void DeleteSavedZooms();    ///< If some image property image has changed deletes saved zoom images.
    void Redraw();              ///< Redraws the fractal.

    // Thread control.
    ///@brief Calculate drawing limits of each thread and launches them.
    ///@param myRender Array of RenderFractal.
    template<class MT> inline void TRender(MT* myRender);

    ///@brief Return a pointer to the watchdog.
    ///@return A pointer to the watchdog.
    ThreadWatchdog<RenderFractal>* GetWatchdog();

    ///@brief Pauses or resumes the rendering.
    void PauseContinue();

    ///@brief If there are active threads stops them.
    bool StopRender();

    ///@brief Get pause status.
    ///@return true if paused, false if not.
    bool IsPaused();

    // Métodos virtuales.
    virtual void Render() = 0;                               ///< Start the fractal rendering. Defined in derived class.
    virtual void PreRender();                                ///< Perform necessary operations before starting to render.
    virtual void PreDrawMaps();                              ///< Perform necessary operations before drawing the maps.
    virtual void PostRender();                               ///< Perform necessary operations after the rendering is finished.
    virtual void PreRestartRender();                         ///< Perform necessary operations before restarting.
    virtual void HandleEvents(sf::Event* Event);             ///< SFML event handler.

    ///@brief Verifies watchdog status.
    ///@return true if there is an active thread. false if not.
    virtual bool IsRendering();
    virtual void SetFormula(FormulaOpt formula);                   ///< Sets user formula.
    virtual void CopyOptFromPanel();                               ///< Copy options from the option panel.
    virtual void MoreIter();                                       ///< Increases the number of iterations.
    virtual void LessIter();                                       ///< Decreases interations.
    virtual void ChangeThreadNumber();                             ///< Changes the number of rendering threads.

    // Communication methods.
    ///@brief Get value of X number in the plane at selected pixel.
    ///@param Pixel_X Pixel to inspect.
    ///@return Numerical value corresponding to the pixel position.
    double GetX(int Pixel_X);

    ///@brief Get value of Y number in the plane at selected pixel.
    ///@param Pixel_Y Pixel to inspect.
    ///@return Numerical value corresponding to the pixel position.
    double GetY(int Pixel_Y);

    ///@brief Gets pixel corresponding to the specified numerical position.
    ///@param xNum Numerical position.
    ///@return Pixel corresponding to number.
    int GetPixelX(double xNum);

    ///@brief Gets pixel corresponding to the specified numerical position.
    ///@param yNum Numerical position.
    ///@return Pixel corresponding to number.
    int GetPixelY(double yNum);

    ///@brief Sets fractal options.
    ///@param opt Fractal options.
    ///@param keepSize If true doesn't copy new resolution.
    void SetOptions(Options opt, bool keepSize = false);

    ///@brief Gets fractal options.
    ///@return a Options struct with the fractal options.
    Options GetOptions();

    ///@brief Forces the fractal to adquire a "rendered" status.
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
    void RenderBMP(string filename);
    void PrepareSnapshot(bool mode);

    // Color styles.
    void SetGaussianColorStyle(GaussianColorStyles _gaussianStyle);
    void SetGradStyle(GradientColorStyles _gradStyle);
    GaussianColorStyles GetGaussianColorStyle();
    GradientColorStyles GetGradStyle();

    // Color operations.
    sf::Color GetSetColor();
    wxGradient* GetGradient();
    ColorMode GetColorMode();
    void SetGaussianColorIntensity(int intensity, Color col);
    void SetGaussianColorMean(double med, Color col);
    void SetGaussianColorStdDev(double des, Color col);
    int GetGaussianColorIntensity(Color col);
    double GetGaussianColorMean(Color col);
    double GetGaussianColorStdDev(Color col);
    void SetExtColorMode(bool mode);
    void SetFractalSetColorMode(bool mode);
    void SetFractalSetColor(sf::Color color);
    bool GetExtColorMode();
    bool GetSetColorMode();
    void ChangeVarGradient();
    void SetPaletteSize(int size);
    int GetPaletteSize();
    void SetGradient(wxGradient grad);
    void SetGradientSize(unsigned int size);
    void SetPaletteMode(ColorMode mode);
    void SetRelativeColor(bool mode);
    bool GetRelativeColorMode();
    void SetVarGradient(int n);

    // Algorithm.
    RenderingAlgorithm GetCurrentAlg();
    vector<RenderingAlgorithm> GetAvailableAlg();
    void SetAlgorithm(RenderingAlgorithm _alg);

    // Julia mode operations.
    bool IsJuliaVariety();
    void SetJuliaMode(bool mode);
    void SetK(double _real, double _imaginary);
    double GetKReal();
    double GetKImaginary();

    // Orbit mode operations.
    void SetOrbitMode(bool mode);
    void SetOrbitPoint(double x, double y);
    bool HasOrbit();
    void SetOrbitChange();

    // Orbit trap operations.
    void SetOrbitTrapMode(bool mode);
    bool HasOrbitTrapMode();
    bool OrbitTrapActivated();

    // SmoothRender.
    void SetSmoothRender(bool mode);
    bool HasSmoothRenderMode();
    bool SmoothRenderActivated();

    // Menu operations.
    void ChangeIterations(int number);
    unsigned int GetIterations();

    // Option panel.
    bool HasOptPanel();
    PanelOptions* GetOptPanel();
};

template<class MT> inline void Fractal::TRender(MT* myRender)
{
    watchdog.Reset();
    // If the image has been moved divides the rendering area so threads will draw the missing part.
    if (xMoved != 0 || yMoved != 0)
    {
        if (xMoved && yMoved)
        {
            for (unsigned int i = 0; i < threadNumber; i++)
            {
                myRender[i].SetOpt(this->GetOptions());
                myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                if (orbitTrapMode || smoothRender)
                    myRender[i].SetSpecialRenderMode(true);
                else
                    myRender[i].SetSpecialRenderMode(false);

                myRender[i].SetK(kReal, kImaginary);
            }

            if (xMoved > 0 && yMoved < 0)
            {
                // First thread pack.
                unsigned int localThreadN = ceil((double)threadNumber / 2.0);
                int Div = static_cast<int>(floor((screenHeight + yMoved) / (double)localThreadN));
                int Step = Div;
                for (unsigned int i = 0; i < localThreadN; i++)
                {
                    if (i + 2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step - Div, xMoved, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, xMoved, screenHeight + yMoved);
                }

                // Second thread pack.
                Div = static_cast<int>(floor(abs(yMoved) / (double)(threadNumber - localThreadN)));
                Step = Div;
                int start = screenHeight + yMoved;
                for (unsigned int i = localThreadN; i < threadNumber; i++)
                {

                    if (i + 2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step - Div, screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, screenWidth, screenHeight);
                }
            }
            else if (xMoved > 0 && yMoved > 0)
            {
                // First thread pack.
                unsigned int localThreadN = ceil((double)threadNumber / 2.0);
                int Div = static_cast<int>(floor(abs(yMoved) / (double)localThreadN));
                int Step = Div;
                for (unsigned int i = 0; i < localThreadN; i++)
                {
                    if (i + 2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step - Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, screenWidth, yMoved);
                }

                // Second thread pack.
                int start = yMoved;
                Div = static_cast<int>(floor((screenHeight - yMoved) / (double)(threadNumber - localThreadN)));
                Step = Div;
                for (unsigned int i = localThreadN; i < threadNumber; i++)
                {
                    if (i + 2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step - Div, xMoved, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, xMoved, screenHeight);
                }
            }
            else if (xMoved < 0 && yMoved < 0)
            {
                // First thread pack.
                unsigned int localThreadN = ceil((double)threadNumber / 2.0);
                int Div = static_cast<int>(floor((screenHeight + yMoved) / (double)localThreadN));
                int Step = Div;
                for (unsigned int i = 0; i < localThreadN; i++)
                {
                    if (i + 2 != localThreadN)
                    {
                        myRender[i].SetLimits(screenWidth + xMoved, Step - Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(screenWidth + xMoved, Step, screenWidth, screenHeight + yMoved);
                }

                // Second thread pack.
                Div = static_cast<int>(floor(abs(yMoved) / (double)(threadNumber - localThreadN)));
                Step = Div;
                int start = screenHeight + yMoved;
                for (unsigned int i = localThreadN; i < threadNumber; i++)
                {

                    if (i + 2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step - Div, screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, screenWidth, screenHeight);
                }
            }
            else if (xMoved < 0 && yMoved > 0)
            {
                // First thread pack.
                unsigned int localThreadN = ceil((double)threadNumber / 2.0);
                int Div = static_cast<int>(floor(abs(yMoved) / (double)localThreadN));
                int Step = Div;
                for (unsigned int i = 0; i < localThreadN; i++)
                {
                    if (i + 2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step - Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, screenWidth, yMoved);
                }

                // Second thread pack.
                int start = yMoved;
                Div = static_cast<int>(floor((screenHeight - yMoved) / (double)(threadNumber - localThreadN)));
                Step = Div;
                for (unsigned int i = localThreadN; i < threadNumber; i++)
                {
                    if (i + 2 != threadNumber)
                    {
                        myRender[i].SetLimits(screenWidth + xMoved, start + Step - Div, screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(screenWidth + xMoved, start + Step, screenWidth, screenHeight);
                }
            }
        }
        else if (xMoved)
        {
            int Div = static_cast<int>(floor(screenHeight / (double)threadNumber));
            int Step = Div;
            if (xMoved > 0)
            {
                for (unsigned int i = 0; i < threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                    if (orbitTrapMode || smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(kReal, kImaginary);

                    if (i + 2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, Step - Div, xMoved, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, xMoved, screenHeight);
                }
            }
            else
            {
                for (unsigned int i = 0; i < threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                    if (orbitTrapMode || smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(kReal, kImaginary);

                    if (i + 2 != threadNumber)
                    {
                        myRender[i].SetLimits(screenWidth + xMoved, Step - Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(screenWidth + xMoved, Step, screenWidth, screenHeight);
                }
            }
        }
        else if (yMoved)
        {
            if (yMoved > 0)
            {
                int Div = static_cast<int>(floor(yMoved / (double)threadNumber));
                int Step = Div;

                for (unsigned int i = 0; i < threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                    if (orbitTrapMode || smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(kReal, kImaginary);

                    if (i + 2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, Step - Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, screenWidth, yMoved);
                }
            }
            else
            {
                int Div = static_cast<int>(floor(abs(yMoved) / (double)threadNumber));
                int Step = Div;

                for (unsigned int i = 0; i < threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                    if (orbitTrapMode || smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(kReal, kImaginary);

                    int start = screenHeight + yMoved;

                    if (i + 2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step - Div, screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, screenWidth, screenHeight);
                }
            }
        }
    }
    else
    {
        // Draws all the screen.
        int Div = static_cast<int>(floor(screenHeight / (double)threadNumber));
        int Step = Div;

        for (unsigned int i = 0; i < threadNumber; i++)
        {
            myRender[i].SetOpt(this->GetOptions());
            myRender[i].SetRenderOut(setMap, colorMap, auxMap);

            if (orbitTrapMode || smoothRender)
                myRender[i].SetSpecialRenderMode(true);
            else
                myRender[i].SetSpecialRenderMode(false);

            myRender[i].SetK(kReal, kImaginary);

            if (!justLaunchThreads)
            {
                if (i + 2 != threadNumber)
                {
                    myRender[i].SetLimits(0, Step - Div, screenWidth, Step);
                    Step += Div;
                }
                else
                    myRender[i].SetLimits(0, Step, screenWidth, screenHeight);
            }
            else
            {
                if (i + 2 != threadNumber)
                {
                    myRender[i].SetOldHo(Step - Div);
                    Step += Div;
                }
                else
                    myRender[i].SetOldHo(Step);
            }
        }
    }

    watchdog.LaunchThreads();
    watchdog.launch();

    if (waitRoutine)
    {
        watchdog.wait();
    }
}

#endif
