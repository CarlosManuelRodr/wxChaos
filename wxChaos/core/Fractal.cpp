#include <sstream>
#include <complex>
#include <mpParser.h>
#include "Fractal.h"
#include "BmpWriter.h"
#include "StringFuncs.h"
#include "SystemUtils.h"
using namespace std;

const int stdSpeed = 1;
const ColorPalettes defaultGradientStyle = Retro;
const wxString defaultGradientString = wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);");


inline double CalcSquaredDist(const double x1, const double y1, const double x2, const double y2)
{
    return ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

inline double CalcDist(const double x1, const double y1, const double x2, const double y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

/**
* @brief Moves matrix elements and fills with zeros.
*
* When the fractal image is moved, it needs to move the elements in the maps so the program doesn't have to redraw the whole screen.
* @param matrix Matrix to move.
* @param moveX Elements to move in the X axis.
* @param moveY Elements to move in the Y axis.
*/
template<class M> void MoveMatrix(M** matrix, const unsigned int matrixWidth, const unsigned int matrixHeight, const int moveX, const int moveY)
{
    // Horizontal displacement.
    if (moveX > 0)
    {
        int displacement = moveX;
        int iterations;

        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            iterations = matrixWidth - displacement;
            for (int j = matrixWidth - 1; j >= 0; j--)
            {
                if (iterations > 0)
                {
                    matrix[i][j] = matrix[i][j - displacement];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }
    else if (moveX < 0)
    {
        int displacement = -moveX;
        int iterations;

        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            iterations = matrixWidth - displacement;
            for (unsigned int j = 0; j < matrixWidth; j++)
            {
                if (iterations > 0)
                {
                    matrix[i][j] = matrix[i][j + displacement];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }

    // Vertical displacement.
    if (moveY > 0)
    {
        int displacement = moveY;
        int iterations;

        for (unsigned int j = 0; j < matrixWidth; j++)
        {
            iterations = matrixHeight - displacement;
            for (int i = matrixHeight - 1; i >= 0; i--)
            {
                if (iterations > 0)
                {
                    matrix[i][j] = matrix[i - displacement][j];
                    iterations--;
                }
                else
                {
                    matrix[i][j] = 0;
                }
            }
        }
    }
    else if (moveY < 0)
    {
        int displacement = -moveY;
        int iterations;

        for (unsigned int j = 0; j < matrixWidth; j++)
        {
            iterations = matrixHeight - displacement;
            for (unsigned int i = 0; i < matrixHeight; i++)
            {
                if (iterations > 0)
                {
                    matrix[i][j] = matrix[i + displacement][j];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }
}


/////////////////////////////////////////
////        BEGINS FRACTAL          /////
/////////////////////////////////////////

// Basic methods.
Fractal::Fractal(int width, int height)
{
    // System.
    threadNumber = Get_Cores();

    // Copy window properties.
    screenHeight = height;
    screenWidth = width;
    backScreenWidth = screenWidth;

    fSetColor = wxColour(0, 0, 0);

    // Allocates memory for the maps.
    setMap = new bool* [screenWidth];
    colorMap = new int* [screenWidth];
    auxMap = new unsigned int* [screenWidth];
    for (int i = 0; i < screenWidth; i++)
    {
        setMap[i] = new bool[screenHeight];
        colorMap[i] = new int[screenHeight];
        auxMap[i] = new unsigned int[screenHeight];
    }

    // Initializes maps.
    for (int i = 0; i < screenWidth; i++)
    {
        for (int j = 0; j < screenHeight; j++)
        {
            setMap[i][j] = false;
            colorMap[i][j] = -1;
            auxMap[i][j] = 0;
        }
    }

    // Set position and velocities.
    xVel = 0;
    yVel = 0;
    posX = 0;
    posY = 0;
    xMoved = 0;
    yMoved = 0;

    // Set fractal properties.
    this->SetDefaultOpt();
    minX = -2.0;
    maxX = 1.0;
    minY = -1.2;
    maxY = minY + (maxX - minX) * (double)screenHeight / screenWidth;
    xFactor = (maxX - minX) / (screenWidth - 1);
    yFactor = (maxY - minY) / (screenHeight - 1);
    kReal = 0;
    kImaginary = 0;
    magnification = 0;
    changeGradient = 0;
    rendered = false;
    changeFractalIter = false;
    varGradient = false;
    colorMode = true;
    juliaMode = false;
    hasOrbit = false;
    orbitMode = false;
    orbitDrawn = false;
    onSnapshot = false;
    juliaVariety = false;
    colorSet = true;
    orbitTrapMode = false;
    hasOrbitTrap = false;
    smoothRender = false;
    hasSmoothRender = false;
    waitRoutine = false;
    redrawAll = false;
    redrawAlways = false;
    rendering = false;
    paused = false;
    pausing = false;
    justLaunchThreads = false;
    maxIter = 100;
    moving = false;
    varGradChange = false;
    refreshImage = false;

    // Creates default color palette.
    relativeColor = false;
    gradPaletteSize = paletteSize = 300;
    alg = RenderingAlgorithm::Other;
    colorPaletteMode = ColorMode::Gradient;
    gradStyle = defaultGradientStyle;
    gradient.fromString(defaultGradientString);
    gradient.setMin(0);
    gradient.setMax(gradPaletteSize);

    palette.resize(paletteSize);
    varGradientStep = paletteSize / 60;
    this->RebuildPalette();

    this->SetOutermostZoom();
}
Fractal::Fractal(sf::RenderWindow* Window)
{
    this->SetDefaultOpt();

    // System.
    threadNumber = Get_Cores();

    // Copy window properties.
    screenHeight = Window->getSize().y;
    backScreenWidth = screenWidth = Window->getSize().x;

    // Set position and velocities.
    xVel = 0;
    yVel = 0;
    posX = 0;
    posY = 0;
    xMoved = 0;
    yMoved = 0;

    fSetColor = wxColour(0, 0, 0);

    // Set fractal properties.
    minX = -2.0;
    maxX = 1.0;
    minY = -1.2;
    maxY = minY + (maxX - minX) * (double)screenHeight / screenWidth;
    xFactor = (maxX - minX) / (screenWidth - 1);
    yFactor = (maxY - minY) / (screenHeight - 1);
    kReal = 0;
    kImaginary = 0;
    magnification = 0;
    changeGradient = 0;
    rendered = false;
    changeFractalIter = true;
    varGradient = false;
    colorMode = true;
    juliaMode = false;
    hasOrbit = false;
    orbitMode = false;
    orbitDrawn = false;
    onSnapshot = false;
    juliaVariety = false;
    colorSet = true;
    orbitTrapMode = false;
    hasOrbitTrap = false;
    smoothRender = false;
    hasSmoothRender = false;
    waitRoutine = false;
    maxIter = 100;
    moving = false;
    redrawAll = false;
    redrawAlways = false;
    rendering = false;
    paused = false;
    pausing = false;
    justLaunchThreads = false;
    varGradChange = false;
    refreshImage = false;

    // Allocates memory for the maps.
    setMap = new bool* [screenWidth];
    colorMap = new int* [screenWidth];
    auxMap = new unsigned int* [screenWidth];
    for (int i = 0; i < screenWidth; i++)
    {
        setMap[i] = new bool[screenHeight];
        colorMap[i] = new int[screenHeight];
        auxMap[i] = new unsigned int[screenHeight];
    }

    // Initializes maps.
    for (int i = 0; i < screenWidth; i++)
    {
        for (int j = 0; j < screenHeight; j++)
        {
            setMap[i][j] = false;
            colorMap[i][j] = -1;
            auxMap[i][j] = 0;
        }
    }

    // Creates default color palette.
    relativeColor = false;
    gradPaletteSize = paletteSize = 300;
    alg = RenderingAlgorithm::Other;
    gradStyle = defaultGradientStyle;
    colorPaletteMode = ColorMode::Gradient;
    gradient.fromString(defaultGradientString);
    gradient.setMin(0);
    gradient.setMax(gradPaletteSize);

    palette.resize(paletteSize);
    varGradientStep = paletteSize / 60;
    this->RebuildPalette();

    this->SetOutermostZoom();
}
Fractal::~Fractal()
{
    // Cleanup.
    for (int i = 0; i < backScreenWidth; i++)
    {
        delete[] setMap[i];
        delete[] colorMap[i];
        delete[] auxMap[i];
    }

    delete[] setMap;
    delete[] colorMap;
    delete[] auxMap;
}
void Fractal::SetDefaultOpt()
{
    renderJobComp = true;
    changeFractalProp = false;
    geomFigure = false;
    onWxCtrl = false;

    for (int i = 0; i < 4; i++)
        movement[i] = false;
}
void Fractal::SetAreaOfView(sf::Rect<int> pixelCoordinates)
{
    if (paused)
    {
        paused = false;
    }
    this->SaveZoom();

    // Changes the scale.
    double FX = (maxX - minX) / screenWidth;
    double FY = (maxY - minY) / screenHeight;

    maxX = minX + (pixelCoordinates.left + pixelCoordinates.width) * FX;
    minX = minX + pixelCoordinates.left * FX;
    minY = maxY - (pixelCoordinates.top + pixelCoordinates.height) * FY;

    maxY = minY + (maxX - minX) * (double)screenHeight / screenWidth;
    xFactor = (maxX - minX) / (screenWidth - 1);
    yFactor = (maxY - minY) / (screenHeight - 1);

    rendered = false;
    rendering = false;

    posY = posX = 0;
    yVel = xVel = 0;
    yMoved = xMoved = 0;

    orbitDrawn = false;
}
void Fractal::SetAreaOfView(Rect worldCoordinates)
{
    minX = worldCoordinates._left;
    maxX = worldCoordinates._right;
    minY = worldCoordinates._bottom;
    maxY = worldCoordinates._top;

    xFactor = (maxX - minX) / (screenWidth - 1);
    yFactor = (maxY - minY) / (screenHeight - 1);

    rendered = false;
    rendering = false;

    posY = posX = 0;
    yVel = xVel = 0;
    yMoved = xMoved = 0;
}
void Fractal::Resize(int width, int height)
{
    // Stop threads if they are still rendering.
    this->StopRender();

    // Frees memory.
    for (int i = 0; i < backScreenWidth; i++)
    {
        delete[] setMap[i];
        delete[] colorMap[i];
        delete[] auxMap[i];
    }
    delete[] setMap;
    delete[] colorMap;
    delete[] auxMap;

    // Copy window properties.
    screenHeight = height;
    backScreenWidth = screenWidth = width;

    // Allocate memory.
    setMap = new bool* [screenWidth];
    colorMap = new int* [screenWidth];
    auxMap = new unsigned int* [screenWidth];
    for (int i = 0; i < screenWidth; i++)
    {
        setMap[i] = new bool[screenHeight];
        colorMap[i] = new int[screenHeight];
        auxMap[i] = new unsigned int[screenHeight];
    }

    // Initialize maps.
    for (int i = 0; i < screenWidth; i++)
    {
        for (int j = 0; j < screenHeight; j++)
        {
            setMap[i][j] = false;
            colorMap[i][j] = -1;
            auxMap[i][j] = 0;
        }
    }

    // Sets SFML variables and scales.
    xFactor = (maxX - minX) / (screenWidth - 1);
    yFactor = (maxY - minY) / (screenHeight - 1);
}
void Fractal::Move()
{
    if (rendered)
    {
        // If any movement button is pressed move the image and accelerate the movement.
        if (movement[Left])
            xVel += stdSpeed;
        if (movement[Right])
            xVel -= stdSpeed;
        if (movement[Up])
            yVel += stdSpeed;
        if (movement[Down])
            yVel -= stdSpeed;

        // If isn't pressed slow down the image.
        if (!movement[Left] && !movement[Right] && !movement[Up] && !movement[Down])
        {
            if (xVel > 0) xVel -= stdSpeed;
            if (xVel < 0) xVel += stdSpeed;
            if (yVel > 0) yVel -= stdSpeed;
            if (yVel < 0) yVel += stdSpeed;
        }

        // Updates the coordinates.
        if (xVel != 0 || yVel != 0)
        {
            double FX = (maxX - minX) / screenWidth;
            double FY = (maxY - minY) / screenHeight;

            minX -= xVel * FX;
            maxX -= xVel * FX;
            minY += yVel * FY;
            maxY += yVel * FY;

            posX += xVel;
            posY += yVel;

            // A new render is needed.
            //rendered = false;
            moving = true;
        }
        else if (posX != 0 || posY != 0)
        {
            if (paused && !pausing)
            {
                rendering = false;
                rendered = false;
                paused = false;
                xMoved = 0;
                yMoved = 0;
                moving = false;
            }
            // If the image has stopped saves the total amount of movement.
            else
            {
                xMoved = posX;
                yMoved = posY;
                posX = 0;
                posY = 0;
            }
        }
    }
}
void Fractal::SaveZoom()
{
    zoom[0].push_back(minX);
    zoom[1].push_back(maxX);
    zoom[2].push_back(minY);
    zoom[3].push_back(maxY);
}
void Fractal::SetOutermostZoom()
{
    outermostZoom = Rect(minX, minY, maxX, maxY);
}
Rect Fractal::GetOutermostZoom()
{
    return outermostZoom;
}
Rect Fractal::GetCurrentZoom()
{
    return Rect(minX, minY, maxX, maxY);
}
void Fractal::ZoomBack()
{
    this->StopRender();

    // Looks for previous zoom coordinates.
    bool thereIsZoom = true;
    for (int i = 0; i < 4; i++)
    {
        if (zoom[i].empty())
            thereIsZoom = false;
    }

    // If they exist, use them.
    if (thereIsZoom)
    {
        minX = zoom[0][zoom[0].size() - 1];
        maxX = zoom[1][zoom[1].size() - 1];
        minY = zoom[2][zoom[2].size() - 1];
        maxY = zoom[3][zoom[3].size() - 1];

        for (int i = 0; i < 4; i++)
            zoom[i].pop_back();
    }
    // If they don't, expand the drawing area.
    else
    {
        double scaleX = abs(maxX - minX);
        double scaleY = abs(maxY - minY);
        minX -= scaleX;
        maxX += scaleX;
        minY -= scaleY;
        maxY = minY + (maxX - minX) * (double)screenHeight / screenWidth;
        this->SetOutermostZoom();
    }

    xFactor = (maxX - minX) / (screenWidth - 1);
    yFactor = (maxY - minY) / (screenHeight - 1);

    rendered = false;
    magnification = 3 / (maxX - minX);
    orbitDrawn = false;
    paused = false;

    xMoved = 0;
    yMoved = 0;
    posX = 0;
    posY = 0;

    orbitDrawn = false;
}
void Fractal::DeleteSavedZooms()
{
}
void Fractal::PrepareRender()
{
    this->PreRender();

    // Checks if the movement is valid.
    if ((abs(xMoved) >= screenWidth) || (abs(yMoved) >= screenHeight)) redrawAll = true;

    // Clear maps.
    if ((!xMoved && !yMoved) || redrawAll || redrawAlways)
    {
        for (int i = 0; i < screenWidth; i++)
        {
            for (int j = 0; j < screenHeight; j++)
            {
                setMap[i][j] = false;
                colorMap[i][j] = -1;
                auxMap[i][j] = 0;
            }
        }
        xMoved = 0;
        yMoved = 0;
        redrawAll = false;
    }
}
void Fractal::Redraw()
{
    this->StopRender();
    redrawAll = true;
    rendered = false;
    rendering = false;
    paused = false;
}

// Thread control
ThreadWatchdog<RenderFractal>* Fractal::GetWatchdog()
{
    return &watchdog;
}
void Fractal::PauseContinue()
{
    if (paused)
    {
        this->PreRestartRender();
        rendered = false;
        rendering = true;
        watchdog.LaunchThreads();
        watchdog.launch();
        paused = false;
    }
    else
    {
        this->StopRender();
        rendered = true;
        paused = true;
        pausing = true;
    }
}
bool Fractal::StopRender()
{
    if (this->IsRendering())
    {
        watchdog.terminate();
        watchdog.StopThreads();
        rendering = false;
        return true;
    }
    return false;
}
bool Fractal::IsPaused()
{
    return paused;
}

// Virtual methods.
void Fractal::PreRender()
{
    // Do nothing.
}
void Fractal::PreDrawMaps()
{
    // Do nothing.
}
void Fractal::PostRender()
{
    // Do nothing.
}
void Fractal::PreRestartRender()
{
    // Do nothing.
}
bool Fractal::IsRendering()
{
    if (waitRoutine) return false;
    else return watchdog.ThreadRunning();
}
void Fractal::SetFormula(FormulaOpt formula)
{
    userFormula = formula;
}
void Fractal::CopyOptFromPanel()
{
    // Do nothing.
}
void Fractal::MoreIter()
{
    changeFractalIter = true;
    this->DeleteSavedZooms();

    redrawAll = true;
    maxIter += 100;

    rendered = false;
}
void Fractal::LessIter()
{
    changeFractalIter = true;
    this->DeleteSavedZooms();

    redrawAll = true;
    int signedMaxIter = (int)maxIter;

    if (signedMaxIter - 100 > 0)
        maxIter -= 100;

    rendered = false;
}
void Fractal::ChangeThreadNumber()
{
    // Do nothing.
}

// Communication methods.
double Fractal::GetX(int Pixel_X)
{
    return minX + Pixel_X * xFactor;
}
double Fractal::GetY(int Pixel_Y)
{
    return maxY - Pixel_Y * yFactor;
}
int Fractal::GetPixelX(double xNum)
{
    return (xNum - minX) / xFactor;
}
int Fractal::GetPixelY(double yNum)
{
    return (maxY - yNum) / yFactor;
}
void Fractal::ChangeIterations(int number)
{
    if (number > 0)
    {
        maxIter = static_cast<unsigned int>(number);
        rendered = false;
        changeFractalIter = true;
    }
}
void Fractal::SetOptions(Options opt, bool keepSize)
{
    if (!keepSize)
    {
        minX = opt.minX;
        maxX = opt.maxX;
        minY = opt.minY;
        maxY = opt.maxY;
    }
    else
        maxY = minY + (maxX - minX) * (double)screenHeight / screenWidth;

    maxIter = opt.maxIter;
    panelOpt = opt.panelOpt;
    changeGradient = opt.changeGradient;
    colorPaletteMode = ColorMode::Gradient;
    relativeColor = opt.relativeColor;
    gradPaletteSize = opt.gradPaletteSize;
    alg = opt.alg;
    fSetColor = wxColour(opt.fSetColor.r, opt.fSetColor.g, opt.fSetColor.b, opt.fSetColor.a);

    gradient = opt.gradient;
    paletteSize = gradPaletteSize;
    this->SetGradientSize(paletteSize);

    if (hasSmoothRender)
        smoothRender = opt.smoothRender;

    kReal = opt.kReal;
    kImaginary = opt.kImaginary;

    orbitTrapMode = opt.orbitTrapMode;
    colorSet = opt.colorSet;
    colorMode = opt.colorMode;
    justLaunchThreads = opt.justLaunchThreads;

    xFactor = (maxX - minX) / (screenWidth - 1);
    yFactor = (maxY - minY) / (screenHeight - 1);

    this->CopyOptFromPanel();
}
Options Fractal::GetOptions()
{
    Options opt;

    opt.minX = minX;
    opt.maxX = maxX;
    opt.minY = minY;
    opt.maxY = maxY;
    opt.xFactor = xFactor;
    opt.yFactor = yFactor;
    opt.maxIter = maxIter;
    opt.changeGradient = changeGradient;
    opt.smoothRender = smoothRender;
    opt.alg = alg;
    opt.gradient = gradient;
    opt.relativeColor = relativeColor;
    opt.colorPaletteMode = colorPaletteMode;
    opt.paletteSize = paletteSize;
    opt.gradPaletteSize = gradPaletteSize;
    opt.panelOpt = panelOpt;
    opt.type = type;

    opt.kReal = kReal;
    opt.kImaginary = kImaginary;

    opt.orbitTrapMode = orbitTrapMode;
    opt.colorSet = colorSet;
    opt.colorMode = colorMode;
    opt.justLaunchThreads = justLaunchThreads;

    opt.fSetColor = GetSetColor();

    opt.screenWidth = screenWidth;
    opt.screenHeight = screenHeight;

    return opt;
}
unsigned int Fractal::GetIterations()
{
    return maxIter;
}
void Fractal::SetRendered(bool mode)
{
    rendered = mode;
}
FractalType Fractal::GetType()
{
    return type;
}
bool** Fractal::GetSetMap()
{
    return setMap;
}
bool Fractal::IsMoving()
{
    if (xVel == 0 && yVel == 0)
        return false;
    else
        return true;
}
void Fractal::SetFractalPropChanged()
{
    changeFractalProp = true;
}
bool Fractal::GetChangeFractalProp()
{
    bool temp = changeFractalProp;
    changeFractalProp = false;
    return temp;
}
void Fractal::SetOnWxCtrl(bool mode)
{
    onWxCtrl = mode;
}
void Fractal::SetMovement(Direction dir)
{
    switch (dir)
    {
    case Up:
    {
        yVel -= stdSpeed;
        movement[Up] = true;
        break;
    }
    case Down:
    {
        yVel += stdSpeed;
        movement[Down] = true;
        break;
    }
    case Left:
    {
        xVel -= stdSpeed;
        movement[Left] = true;
        break;
    }
    case Right:
    {
        xVel += stdSpeed;
        movement[Right] = true;
        break;
    }
    default: break;
    }
}
void Fractal::ReleaseMovement(Direction dir)
{
    switch (dir)
    {
    case Up:
    {
        movement[Up] = false;
        break;
    }
    case Down:
    {
        movement[Down] = false;
        break;
    }
    case Left:
    {
        movement[Left] = false;
        break;
    }
    case Right:
    {
        movement[Right] = false;
        break;
    }
    default: break;
    }
}

// Save image.
sf::Image Fractal::GetRenderedImage()
{
    onSnapshot = true;
    waitRoutine = true;
    if (!rendered)
    {
        this->PrepareRender();
        this->Render();
    }
    this->PreDrawMaps();

    sf::Image image;
    image.create(screenWidth, screenHeight, sf::Color(255, 255, 255));

    maxColorMapVal = 0;
    if (relativeColor)
    {
        // Search for a color maximum value.
        for (int i = 0; i < screenWidth; i++)
        {
            for (int j = 0; j < screenHeight; j++)
            {
                if (colorMap[i][j] > maxColorMapVal)
                    maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if (maxColorMapVal == 0)
        maxColorMapVal = 1;

    for (int i = 0; i < screenWidth; i++)
    {
        for (int j = 0; j < screenHeight; j++)
        {
            if (setMap[i][j] == true && colorSet)
                image.setPixel(i, j, GetSetColor());
            else
            {
                if (colorMode)
                {
                    // Color pixel.
                    sf::Color Color;
                    if (relativeColor)
                        Color = CalcColor(((double)colorMap[i][j] / (double)maxColorMapVal) * paletteSize + changeGradient);
                    else
                        Color = CalcColor(colorMap[i][j] + changeGradient);

                    image.setPixel(i, j, Color);
                }
            }
        }
    }

    onSnapshot = false;
    waitRoutine = false;
    return image;
}
wxBitmap Fractal::GetRenderedWxBitmap()
{
    sf::Image renderedImage = this->GetRenderedImage();
    const sf::Vector2u imageSize = renderedImage.getSize();
    const sf::Uint8* rgbaPixels = renderedImage.getPixelsPtr();
    unsigned char* rgbPixels = new unsigned char[imageSize.x * imageSize.y * 3];

    // wxImage expects packed RGB data, while SFML exposes RGBA pixels.
    for (unsigned int i = 0, j = 0; i < imageSize.x * imageSize.y * 4; i += 4, j += 3)
    {
        rgbPixels[j] = rgbaPixels[i];
        rgbPixels[j + 1] = rgbaPixels[i + 1];
        rgbPixels[j + 2] = rgbaPixels[i + 2];
    }

    wxImage wximg(imageSize.x, imageSize.y, rgbPixels, true);
    wxBitmap output(wximg);
    return output;
}
void Fractal::RenderBMP(string filename)
{
    waitRoutine = true;
    onSnapshot = true;
    BMPWriter writer(filename.c_str(), screenWidth, screenHeight);
    if (!rendered)
    {
        this->PrepareRender();
        this->Render();
    }
    this->PreDrawMaps();
    BMPPixel* data = new BMPPixel[screenWidth];

    maxColorMapVal = 0;
    if (relativeColor)
    {
        // Search for a color maximum value.
        for (int i = 0; i < screenWidth; i++)
        {
            for (int j = 0; j < screenHeight; j++)
            {
                if (colorMap[i][j] > maxColorMapVal)
                    maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if (maxColorMapVal == 0)
        maxColorMapVal = 1;

    // Copy maps values to BMPWriter.
    for (int j = screenHeight - 1; j >= 0; j--)
    {
        for (int i = 0; i < screenWidth; i++)
        {
            if (setMap[i][j] != 0 && colorSet)
            {
                data[i].r = 0;
                data[i].g = 0;
                data[i].b = 0;
            }
            else if (colorMode)
            {
                if (relativeColor)
                {
                    sf::Color c = CalcColor(((double)colorMap[i][j] / (double)maxColorMapVal) * paletteSize + changeGradient);
                    data[i].r = c.r;
                    data[i].g = c.g;
                    data[i].b = c.b;
                }
                else
                {
                    sf::Color c = CalcColor(colorMap[i][j] + changeGradient);
                    data[i].r = c.r;
                    data[i].g = c.g;
                    data[i].b = c.b;
                }
            }
            else
            {
                data[i].r = static_cast<unsigned>(0xFF);
                data[i].g = static_cast<unsigned>(0xFF);
                data[i].b = static_cast<unsigned>(0xFF);
            }
        }
        writer.WriteLine(data);
    }
    writer.CloseBMP();
    delete[] data;
}
void Fractal::PrepareSnapshot(bool mode)
{
    onSnapshot = mode;
}

void Fractal::SetColorPalette(ColorPalettes _gradStyle)
{
    gradStyle = _gradStyle;
}
ColorPalettes Fractal::GetColorPalette()
{
    return gradStyle;
}

// Gradient color.
wxGradient* Fractal::GetGradient()
{
    return &gradient;
}
void Fractal::SetGradient(wxGradient grad)
{
    // Copy gradient.
    colorPaletteMode = ColorMode::Gradient;
    gradient = grad;
    gradPaletteSize = paletteSize = gradient.getMax() - gradient.getMin();
    palette.resize(paletteSize);
    varGradientStep = paletteSize / 60;
    this->RebuildPalette();
}
void Fractal::SetGradientSize(unsigned int size)
{
    colorPaletteMode = ColorMode::Gradient;
    gradient.setMax(size);
    gradPaletteSize = paletteSize = size;
    palette.resize(paletteSize);
    varGradientStep = paletteSize / 60;
    this->RebuildPalette();
}

// Color operations.
void Fractal::RedrawMaps()
{
    maxColorMapVal = 0;

    if (relativeColor)
    {
        // Search for color maximum.
        for (int i = 0; i < screenWidth; i++)
        {
            for (int j = 0; j < screenHeight; j++)
            {
                if (colorMap[i][j] > maxColorMapVal)
                    maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if (maxColorMapVal == 0)
        maxColorMapVal = 1;
    refreshImage = true;
}
void Fractal::SetExtColorMode(bool mode)
{
    // Changes external color mode.
    if (colorMode != mode)
    {
        colorMode = mode;
        this->RedrawMaps();
    }
    this->DeleteSavedZooms();
}
void Fractal::SetFractalSetColorMode(bool mode)
{
    // Changes internal color mode.
    if (colorSet != mode)
    {
        colorSet = mode;
        this->RedrawMaps();
    }
    this->DeleteSavedZooms();
}
void Fractal::SetFractalSetColor(sf::Color color)
{
    // Changes the color of the set.
    fSetColor = wxColour(color.r, color.g, color.b, color.a);
    this->RedrawMaps();
    this->DeleteSavedZooms();
}
sf::Color Fractal::GetSetColor()
{
    return sf::Color(fSetColor.Red(), fSetColor.Green(), fSetColor.Blue(), fSetColor.Alpha());
}
bool Fractal::GetExteriorColorMode()
{
    return colorMode;
}
bool Fractal::GetInteriorColorMode()
{
    return colorSet;
}
sf::Color Fractal::CalcColor(int colorNum) const
{
    if (colorNum <= 0)
        colorNum = 0;

    colorNum = colorNum % paletteSize;
    const wxColour& color = palette[colorNum];
    return sf::Color(color.Red(), color.Green(), color.Blue(), color.Alpha());
}
void Fractal::ChangeVarGradient()
{
    varGradient = !varGradient;
}
void Fractal::RebuildPalette()
{
    this->DeleteSavedZooms();

    wxColour myWxColor;
    for (int i = 0; i < paletteSize; i++)
    {
        myWxColor = gradient.getColorAt(i);
        palette[i] = myWxColor;
    }
    this->RedrawMaps();
}
int Fractal::GetPaletteSize()
{
    return paletteSize;
}
void Fractal::SetPaletteSize(int size)
{
    this->SetGradientSize(size);
}
void Fractal::SetVarGradient(int n)
{
    varGradChange = true;
    changeGradient = n % paletteSize;
    this->DeleteSavedZooms();
}

ColorMode Fractal::GetColorMode()
{
    return ColorMode::Gradient;
}

// Algorithm.
RenderingAlgorithm Fractal::GetCurrentAlg()
{
    return alg;
}
vector<RenderingAlgorithm> Fractal::GetAvailableAlg()
{
    return availableAlg;
}
void Fractal::SetAlgorithm(RenderingAlgorithm _alg)
{
    alg = _alg;
    this->DeleteSavedZooms();
    this->StopRender();
    rendered = false;
    rendering = false;
}

// RelativeColor.
void Fractal::SetRelativeColor(bool mode)
{
    relativeColor = mode;
    this->RebuildPalette();
}
bool Fractal::GetRelativeColorMode()
{
    return relativeColor;
}

// Julia mode operations.
void Fractal::SetK(double _real, double _imaginary)
{
    if (watchdog.ThreadRunning())
    {
        watchdog.terminate();
        watchdog.StopThreads();
        rendering = false;
    }
    if (_real != kReal || _imaginary != kImaginary) rendered = false;

    kReal = _real;
    kImaginary = _imaginary;
    this->DeleteSavedZooms();
}
bool Fractal::IsJuliaVariety()
{
    return juliaVariety;
}
void Fractal::SetJuliaMode(bool mode)
{
    juliaMode = waitRoutine = mode;
}
double Fractal::GetKReal()
{
    return kReal;
}
double Fractal::GetKImaginary()
{
    return kImaginary;
}

// Geometry operations.
void Fractal::DrawLine(double x1, double y1, double x2, double y2, sf::Color color, bool orbitLine)
{
    LineData data;
    data.x1 = x1;
    data.x2 = x2;
    data.y1 = y1;
    data.y2 = y2;
    data.color = color;

    if (orbitLine)
        orbitLines.push_back(data);
    else
        lines.push_back(data);

    geomFigure = true;
}
void Fractal::DrawCircle(double x_center, double y_center, double radius, sf::Color color)
{
    CircleData data;
    data.x_center = x_center;
    data.y_center = y_center;
    data.radius = radius;
    data.color = color;
    circles.push_back(data);
    geomFigure = true;
}
// Orbit mode operations.
void Fractal::SetOrbitMode(bool mode)
{
    if (hasOrbit)
    {
        orbitMode = mode;
        orbitX = 0;
        orbitY = 0;
        orbitLines.clear();
    }
}
bool Fractal::HasOrbit()
{
    return hasOrbit;
}
void Fractal::SetOrbitPoint(double x, double y)
{
    if (!orbitDrawn)
    {
        orbitX = x;
        orbitY = y;
    }
}
void Fractal::SetOrbitChange()
{
    orbitDrawn = false;
}

// Orbit trap operations.
void Fractal::SetOrbitTrapMode(bool mode)
{
    if (hasOrbitTrap)
    {
        this->DeleteSavedZooms();
        orbitTrapMode = mode;
    }
}
bool Fractal::HasOrbitTrapMode()
{
    return hasOrbitTrap;
}
bool Fractal::OrbitTrapActivated()
{
    return orbitTrapMode;
}

// SmoothRender
void Fractal::SetSmoothRender(bool mode)
{
    if (hasSmoothRender)
    {
        this->DeleteSavedZooms();
        smoothRender = mode;
    }
}
bool Fractal::HasSmoothRenderMode()
{
    return hasSmoothRender;
}
bool Fractal::SmoothRenderActivated()
{
    return smoothRender;
}

// Option panel.
bool Fractal::HasOptPanel() {
    return panelOpt.GetElementsSize() > 0;
}

PanelOptions* Fractal::GetOptPanel()
{
    return &panelOpt;
}


