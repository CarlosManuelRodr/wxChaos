/**
* @file FractalClasses.h
* @brief All the fractal drawing related classes.
*
* For the definition of how each fractal is rendered look into fractalTypes.h
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 9/22/2012
*/

#pragma once
#ifndef _FractalClasses
#define _FractalClasses

#include <complex>
#include "Styles.h"
#include "gradient.h"
#include "FractalGUI.h"
#include "muParserX/mpParser.h"
using namespace std;

/****************************
*                           *
*    Auxiliary functions    *
*                           *
****************************/

int Get_Cores();

/****************************
*                           *
*      Fractal classes      *
*                           *
****************************/

/**
* @enum FractalType
* @brief Enumerates the list of available fractals.
*/
enum class FractalType
{
    Mandelbrot,
    MandelbrotZN,
    Julia,
    JuliaZN,
    NewtonRaphsonMethod,
    Sinoidal,
    Magnetic,
    Medusa,
    Manowar,
    ManowarJulia,
    SierpinskyTriangle,
    FixedPoint1,
    FixedPoint2,
    FixedPoint3,
    FixedPoint4,
    Tricorn,
    BurningShip,
    BurningShipJulia,
    Fractory,
    Cell,
    Logistic,
    HenonMap,
    DoublePendulum,
    UserDefined,
    FixedPointUserDefined,
    ScriptFractal,
    Undefined
};

/**
* @enum Color
* @brief Enumerates the rgb color.
*/
enum class Color
{
    Red,
    Green,
    Blue
};

/**
* @enum RenderingAlgorithm
* @brief List of available render algorithms.
*/
enum class RenderingAlgorithm
{
    EscapeTime,
    GaussianInt,
    Buddhabrot,
    EscapeAngle,
    TriangleInequality,
    ChaoticMap,
    Lyapunov,
    ConvergenceTest,
    Other
};

/**
* @enum ColorMode
* @brief List of available color modes.
*/
enum class ColorMode
{
    Gradient,
    Gaussian
};

/**
* @enum Direction
* @brief Movement direction.
*/
enum Direction
{
    Left = 0,
    Right,
    Up,
    Down
};

/**
* @struct LineData
* @brief Parameters of line shape.
*/
struct LineData
{
    double x1, y1, x2, y2;
    sf::Color color;
};

/**
* @struct CircleData
* @brief Parameters of circle shape.
*/
struct CircleData
{
    double x_center, y_center, radius;
    sf::Color color;
};

/**
* @struct Options
* @brief Used to copy the parameters of the fractal.
*/

struct Options
{
    double minX;
    double maxX;
    double minY;
    double maxY;
    double xFactor;
    double yFactor;
    unsigned maxIter;
    int changeGradient;
    wxGradient gradient;
    ColorMode colorPaletteMode;
    RenderingAlgorithm alg;
    FractalType type;
    int paletteSize;
    int gaussianPaletteSize;
    int gradPaletteSize;
    PanelOptions panelOpt;

    double kReal;
    double kImaginary;

    bool orbitTrapMode;
    bool colorSet;
    bool colorMode;
    bool buddhaMode;
    bool smoothRender;
    bool justLaunchThreads;
    bool relativeColor;
    int redInt, greenInt, blueInt;
    double redMean, greenMean, blueMean;
    double redStdDev, greenStdDev, blueStdDev;
    sf::Color fSetColor;

    int screenHeight;
    int screenWidth;
};

/**
* @enum FormulaType
* @brief Type of user formula.
*/
enum class FormulaType
{
    Complex = 1,
    FixedPoint
};

/**
* @struct FormulaOpt
* @brief User formula parameters.
*/

struct FormulaOpt
{
    wxString userFormula;        ///< String containing the user formula.
    FormulaType type;            ///< Type of user formula (Complex or fixed point).
    bool julia;                  ///< Inform if the fractal is a Julia variety. Only on complex type.
    int bailout;                 ///< Bailout of the complex fractal.
};

/**
* @struct Vector2Int
* @brief Stores x and y coordinates.
*/
struct Vector2Int
{
    int x, y;
};

/**
* @struct Vector2Double
* @brief Stores x and y coordinates. This variant comes with vector arithmetic operators.
*/

struct Vector2Double
{
    double x, y;

    Vector2Double();
    Vector2Double(double _x, double _y);
    Vector2Double(const Vector2Int& v);

    Vector2Double operator-() const;
    Vector2Double& operator+=(const Vector2Double& v);

    Vector2Double& operator*=(const double t);
    Vector2Double& operator/=(const double t);

    double Length() const;
    double SquaredLength() const;
};

inline Vector2Double operator+(const Vector2Double& u, const Vector2Double& v)
{
    return Vector2Double(u.x + v.x, u.y + v.y);
}
inline Vector2Double operator-(const Vector2Double& u, const Vector2Double& v)
{
    return Vector2Double(u.x - v.x, u.y - v.y);
}
inline Vector2Double operator*(const Vector2Double& u, const Vector2Double& v)
{
    return Vector2Double(u.x * v.x, u.y * v.y);
}
inline Vector2Double operator*(double t, const Vector2Double& v)
{
    return Vector2Double(t * v.x, t * v.y);
}
inline Vector2Double operator*(const Vector2Double& v, double t)
{
    return t * v;
}
inline Vector2Double operator/(Vector2Double v, double t)
{
    return (1.0 / t) * v;
}


/**
* @struct Rect
* @brief Stores rectangle coordinates.
*/
struct Rect
{
    double left, bottom, right, top;

    Rect();
    Rect(double _left, double _bottom, double _right, double _top);
    Vector2Double GetLowerBound();
    Vector2Double GetHigherBound();
    void SetLowerBound(Vector2Double lb);
    void SetHigherBound(Vector2Double hb);
};

/**
* @class RenderFractal
* @brief Handles the multithreading rendering.
*
* This is an abstract class. It's derived classes have to define the Render() method.
*/

class RenderFractal : public sf::Thread
{
protected:
    bool** setMap;                ///< Pointer to the set map.
    int** colorMap;               ///< Pointer to the color map.
    unsigned int** auxMap;        ///< Pointer to the auxiliary map.
    int x;                        ///< Rendering x position.
    int y;                        ///< Rendering y position.
    unsigned int threadProgress;  ///< Last progress counted. Used when the thread is paused.

    int wo;        ///< Left render limit.
    int ho;        ///< Upper render limit.
    int wf;        ///< Right render limit.
    int hf;        ///< Bottom render limit.
    int oldHo;     ///< Previous upper render limit.

    bool threadRunning;         ///< Flag to stop the thread.
    bool stopped;
    bool specialRenderMode;     ///< Controls the rendering modes.
    Options myOpt;              ///< A copy of the parameters found in the Fractal class.

    FractalType type;
    double xFactor;
    double yFactor;
    double minX;
    double maxX;
    double minY;
    double maxY;
    double maxIter;

    double kReal;
    double kImaginary;

public:
    virtual void Render() = 0;            ///< Render the fractal.
    virtual void SpecialRender() {};
    virtual void Run();                   ///< Launches the thread.
    virtual void Stop();
    RenderFractal();                      ///< Constructor.

    ///@brief Sets the rendering limits. This is calculated before the rendering starts by TRender.
    ///@param widthO Left rendering limit.
    ///@param heightO Upper rendering limit.
    ///@param widthF Right rendering limit.
    ///@param heightF Lower rendering limit.
    void SetLimits(int widthO, int heightO, int widthF, int heightF);

    ///@brief Sets the original lower height limit.
    ///@param _oldHo Original lower height limit.
    void SetOldHo(int _oldHo);

    ///@brief Update limits of the rendering area before starting a renderJob.
    ///@param heightO Start position of the renderJob.
    void UpdateLimits(int heightO);

    ///@brief Activates the special rendering mode.
    ///@param mode New mode.
    void SetSpecialRenderMode(bool mode);

    /**
    * @brief Sets the rendering parameters.
    *
    * This is actually here because not every fractal type need to be threaded, so in case of a threaded fractal
    * you need to set the same pack of parameters that a non threaded one would find.
    * @param opt Fractal options.
    */
    void SetOpt(Options opt);

    ///@brief Get the rendering parameters.
    ///@return Fractal options.
    Options GetOpt();

    ///@brief Set the adresses of the maps where the fractal will draw.
    ///@param outSetMap Pointer to Set map.
    ///@param outColorMap Pointer to the color map.
    ///@param outAux Pointer to the auxiliar map.
    void SetRenderOut(bool** outSetMap, int** outColorMap, unsigned int** outAux = nullptr);

    ///@brief Sets the K constant.
    ///@param re Real parameter.
    ///@param im Imaginary parameter.
    void SetK(double re, double im);

    ///@brief Resets x and y positions.
    void Reset();

    ///@brief To be overriden in case that a special behaviour is needed before terminanting the thread.
    virtual void PreTerminate();

    ///@brief Gets the pixel coordinates where the rendering was paused.
    ///@return A struct with the coordinates.
    Vector2Int GetCoords();

    ///@brief Get the pixel coordinates where the rendering was started.
    ///@return A struct with the coordinates.
    Vector2Int GetStartPoints();

    ///@brief Get the pixel coordinates where the rendering has to end.
    ///@return A struct with the coordinates.
    Vector2Int GetEndPoints();

    bool IsRunning();

    ///@brief Ask the percentage of rendering progress,
    ///@return A integer from 0 to 100 that is the progress.
    virtual int AskProgress();
};

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
    bool threadRunning;            ///< State of the threads.
    unsigned int threadCounter;    ///< Number of threads to watch over.

public:
    ThreadWatchdog();
    ~ThreadWatchdog();

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

    ///@brief Launches the threads.
    virtual void Run();
};

/**
* @brief Sets the watchdog for the specified threads.
* @param MT Must be a RenderFractal inherited class.
* @param myRender Array of render threads.
* @param watchdog Pointer to the watchdog that will be used.
* @param threadNumber Number of threads to set.
*/
template<class MT> inline void SetWatchdog(MT* myRender, ThreadWatchdog<RenderFractal>* watchdog ,unsigned int threadNumber)
{
    watchdog->SetThreadNumber(threadNumber);
    for(unsigned int i=0; i<threadNumber; i++)
        watchdog->SetThread(&myRender[i]);
}

template<class MT> ThreadWatchdog<MT>::ThreadWatchdog()
{
    threadCounter = 0;
    threadRunning = false;
    threadList = nullptr;
}
template<class MT> ThreadWatchdog<MT>::~ThreadWatchdog()
{
    if(threadList != nullptr)
        delete[] threadList;
}
template<class MT> void ThreadWatchdog<MT>::SetThreadNumber(int nThreads)
{
    if(threadList != nullptr)
    {
        delete[] threadList;
        threadCounter = 0;
        threadRunning = false;
    }
    threadList = new MT*[nThreads];
}
template<class MT> void ThreadWatchdog<MT>::SetThread(MT* threadAdress)
{
    threadList[threadCounter++] = threadAdress;
}
template<class MT> void ThreadWatchdog<MT>::Run()
{
    // We don't want to collapse our system.
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
    // Wait for every thread to finish and change status.
    for(unsigned int i=0; i<threadCounter; i++)
        threadList[i]->Wait();

    threadRunning = false;
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}
template<class MT> void ThreadWatchdog<MT>::Reset()
{
    for(unsigned int i=0; i<threadCounter; i++)
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
    for(unsigned int i=0; i<threadCounter; i++)
        threadList[i]->Launch();
}
template<class MT> void ThreadWatchdog<MT>::StopThreads()
{
    for(unsigned int i=0; i<threadCounter; i++)
    {
        threadList[i]->PreTerminate();  // Do the necessary operations before terminating a thread.
        threadList[i]->Stop();
    }

    for(unsigned int i=0; i<threadCounter; i++)
    {
        while(threadList[i]->IsRunning())
            threadList[i]->Wait();
    }

    threadRunning = false;

#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}
template<class MT> int ThreadWatchdog<MT>::GetThreadProgress()
{
    int progress = 0;
    for(unsigned int i=0; i<threadCounter; i++)
        progress += threadList[i]->AskProgress();

    return (double)progress/(double)threadCounter;
}
template<class MT> MT* ThreadWatchdog<MT>::GetThread(unsigned int nThread)
{
    if(nThread >= 0 && nThread < threadCounter)
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
    sf::Sprite output;              ///< Sprite to draw the output image.
    vector<sf::Image> imgVector;    ///< Vector of rendering images that are loaded on zoomback.
    sf::Font font;
    sf::String text;
    wxString tempText;
    sf::Image tempImage;            ///< Temporary image. Shows low res image while renering.
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
    void DrawMaps(sf::RenderWindow *Window);

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
    void Move(const sf::Input& input);
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

/**
* @class ScreenPointer
* @brief A selection cross to point to a number. Used in Slider, JuliaMode, and OrbitMode.
*/

class ScreenPointer
{
    int x, y;
    unsigned int screenWidth;
    unsigned int screenHeight;
    sf::Image texture;
    sf::Sprite output;
    sf::Color color;
    bool rendered;
    bool inSelection;

    void Render();

public:
    ScreenPointer(sf::RenderWindow* Window);
    void Show(sf::RenderWindow* Window);
    void Resize(sf::RenderWindow* Window);
    bool HandleEvents(sf::Event Event);

    // WX events.
    bool ClickEvent(wxMouseEvent& event);
    void UnClickEvent(wxMouseEvent& event);
    bool MoveEvent(wxMouseEvent& event);

    double GetX(Fractal* target);
    double GetY(Fractal* target);
    void AdjustPosition(Fractal* target, double numX, double numY);
};

template<class MT> inline void Fractal::TRender(MT* myRender)
{
    watchdog.Reset();
    // If the image has been moved divides the rendering area so threads will draw the missing part.
    if(xMoved != 0 || yMoved != 0)
    {
        if(xMoved && yMoved)
        {
            for(unsigned int i=0; i<threadNumber; i++)
            {
                myRender[i].SetOpt(this->GetOptions());
                myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                if(orbitTrapMode || smoothRender)
                    myRender[i].SetSpecialRenderMode(true);
                else
                    myRender[i].SetSpecialRenderMode(false);

                myRender[i].SetK(kReal, kImaginary);
            }

            if(xMoved > 0 && yMoved < 0)
            {
                // First thread pack.
                unsigned int localThreadN = ceil((double)threadNumber/2.0);
                int Div = static_cast<int>(floor((screenHeight+yMoved)/(double)localThreadN));
                int Step = Div;
                for(unsigned int i=0; i<localThreadN; i++)
                {
                    if(i+2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step-Div, xMoved, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, xMoved, screenHeight + yMoved);
                }

                // Second thread pack.
                Div = static_cast<int>(floor(abs(yMoved)/(double)(threadNumber - localThreadN)));
                Step = Div;
                int start = screenHeight+yMoved;
                for(unsigned int i=localThreadN; i<threadNumber; i++)
                {

                    if(i+2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step-Div, screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, screenWidth, screenHeight);
                }
            }
            else if(xMoved > 0 && yMoved > 0)
            {
                // First thread pack.
                unsigned int localThreadN = ceil((double)threadNumber/2.0);
                int Div = static_cast<int>(floor(abs(yMoved)/(double)localThreadN));
                int Step = Div;
                for(unsigned int i=0; i<localThreadN; i++)
                {
                    if(i+2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step-Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, screenWidth, yMoved);
                }

                // Second thread pack.
                int start = yMoved;
                Div = static_cast<int>(floor((screenHeight-yMoved)/(double)(threadNumber-localThreadN)));
                Step = Div;
                for(unsigned int i=localThreadN; i<threadNumber; i++)
                {
                    if(i+2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step-Div, xMoved, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, xMoved, screenHeight);
                }
            }
            else if(xMoved < 0 && yMoved < 0)
            {
                // First thread pack.
                unsigned int localThreadN = ceil((double)threadNumber/2.0);
                int Div = static_cast<int>(floor((screenHeight+yMoved)/(double)localThreadN));
                int Step = Div;
                for(unsigned int i=0; i<localThreadN; i++)
                {
                    if(i+2 != localThreadN)
                    {
                        myRender[i].SetLimits(screenWidth+xMoved, Step-Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(screenWidth+xMoved, Step, screenWidth, screenHeight + yMoved);
                }

                // Second thread pack.
                Div = static_cast<int>(floor(abs(yMoved)/(double)(threadNumber-localThreadN)));
                Step = Div;
                int start = screenHeight+yMoved;
                for(unsigned int i=localThreadN; i<threadNumber; i++)
                {

                    if(i+2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step-Div, screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, screenWidth, screenHeight);
                }
            }
            else if(xMoved < 0 && yMoved > 0)
            {
                // First thread pack.
                unsigned int localThreadN = ceil((double)threadNumber/2.0);
                int Div = static_cast<int>(floor(abs(yMoved)/(double)localThreadN));
                int Step = Div;
                for(unsigned int i=0; i<localThreadN; i++)
                {
                    if(i+2 != localThreadN)
                    {
                        myRender[i].SetLimits(0, Step-Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, screenWidth, yMoved);
                }

                // Second thread pack.
                int start = yMoved;
                Div = static_cast<int>(floor((screenHeight-yMoved)/(double)(threadNumber - localThreadN)));
                Step = Div;
                for(unsigned int i=localThreadN; i<threadNumber; i++)
                {
                    if(i+2 != threadNumber)
                    {
                        myRender[i].SetLimits(screenWidth+xMoved, start + Step-Div, screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(screenWidth+xMoved, start + Step, screenWidth, screenHeight);
                }
            }

            for(unsigned int i=0; i<threadNumber; i++)
                myRender[i].Launch();
        }
        else if(xMoved)
        {
            int Div = static_cast<int>(floor(screenHeight / (double)threadNumber));
            int Step = Div;
            if(xMoved > 0)
            {
                for(unsigned int i=0; i<threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                    if(orbitTrapMode || smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(kReal, kImaginary);

                    if(i+2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, Step-Div, xMoved, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, xMoved, screenHeight);

                    myRender[i].Launch();
                }
            }
            else
            {
                for(unsigned int i=0; i<threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                    if(orbitTrapMode || smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(kReal, kImaginary);

                    if(i+2 != threadNumber)
                    {
                        myRender[i].SetLimits(screenWidth+xMoved, Step - Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(screenWidth+xMoved, Step, screenWidth, screenHeight);

                    myRender[i].Launch();
                }
            }
        }
        else if(yMoved)
        {
            if(yMoved > 0)
            {
                int Div = static_cast<int>(floor(yMoved / (double)threadNumber));
                int Step = Div;

                for(unsigned int i=0; i<threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                    if(orbitTrapMode || smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(kReal, kImaginary);

                    if(i+2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, Step-Div, screenWidth, Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, Step, screenWidth, yMoved);

                    myRender[i].Launch();
                }
            }
            else
            {
                int Div = static_cast<int>(floor(abs(yMoved) / (double)threadNumber));
                int Step = Div;

                for(unsigned int i=0; i<threadNumber; i++)
                {
                    myRender[i].SetOpt(this->GetOptions());
                    myRender[i].SetRenderOut(setMap, colorMap, auxMap);

                    if(orbitTrapMode || smoothRender)
                        myRender[i].SetSpecialRenderMode(true);
                    else
                        myRender[i].SetSpecialRenderMode(false);

                    myRender[i].SetK(kReal, kImaginary);

                    int start = screenHeight+yMoved;

                    if(i+2 != threadNumber)
                    {
                        myRender[i].SetLimits(0, start + Step-Div, screenWidth, start + Step);
                        Step += Div;
                    }
                    else
                        myRender[i].SetLimits(0, start + Step, screenWidth, screenHeight);

                    myRender[i].Launch();
                }
            }
        }
    }
    else
    {
        // Draws all the screen.
        int Div = static_cast<int>(floor(screenHeight/(double)threadNumber));
        int Step = Div;

        for(unsigned int i=0; i<threadNumber; i++)
        {
            myRender[i].SetOpt(this->GetOptions());
            myRender[i].SetRenderOut(setMap, colorMap, auxMap);

            if(orbitTrapMode || smoothRender)
                myRender[i].SetSpecialRenderMode(true);
            else
                myRender[i].SetSpecialRenderMode(false);

            myRender[i].SetK(kReal, kImaginary);

            if(!justLaunchThreads)
            {
                if(i+2 != threadNumber)
                {
                    myRender[i].SetLimits(0, Step-Div, screenWidth, Step);
                    Step += Div;
                }
                else
                    myRender[i].SetLimits(0, Step, screenWidth, screenHeight);
            }
            else
            {
                if(i+2 != threadNumber)
                {
                    myRender[i].SetOldHo(Step-Div);
                    Step += Div;
                }
                else
                    myRender[i].SetOldHo(Step);
            }
            myRender[i].Launch();
        }
    }
    if(waitRoutine)
    {
        for(unsigned int i=0; i<threadNumber; i++)
            myRender[i].Wait();
    }
    watchdog.Launch();
}

#endif