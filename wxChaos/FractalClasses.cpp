#include <sstream>
#include <cmath>
#include <limits>
#include "FractalClasses.h"
#include "BmpWriter.h"
#include "StringFuncs.h"
#include "BinaryOps.h"
#include "ConfigParser.h"
#include "Filesystem.h"
#include "global.h"

const int stdSpeed = 1;
const GradientColorStyles defaultGradStyle = GradientColorStyles::Retro;
const GaussianColorStyles defaultEstStyle = GaussianColorStyles::SummerDay;


inline double CalcSquaredDist(const double x1, const double y1, const double x2, const double y2)
{
    return ((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

inline double CalcDist(const double x1, const double y1, const double x2, const double y2)
{
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

int Get_Cores()
{
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    return sysinfo.dwNumberOfProcessors;
#endif

#ifdef linux
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
    return 1;
}

/**
* @brief Moves matrix elements and fills with zeros.
*
* When the fractal image is moved it needs to move the elements in the maps so the program doesn't have to redraw the whole screen.
* @param matrix Matrix to move.
* @param moveX Elements to move in the X axis.
* @param moveY Elements to move in the Y axis.
*/
template<class M> inline void MoveMatrix(M** matrix, const unsigned int matrixWidth, const unsigned int matrixHeight, const int moveX, const int moveY)
{
    // Horizontal displacement.
    if(moveX > 0)
    {
        int displacement = moveX;
        int iterations;

        for(unsigned int i=0; i<matrixHeight; i++)
        {
            iterations = matrixWidth - displacement;
            for(int j=matrixWidth-1; j>=0; j--)
            {
                if(iterations > 0)
                {
                    matrix[i][j] = matrix[i][j-displacement];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }
    else if(moveX < 0)
    {
        int displacement = -moveX;
        int iterations;

        for(unsigned int i=0; i<matrixHeight; i++)
        {
            iterations = matrixWidth - displacement;
            for(unsigned int j=0; j<matrixWidth; j++)
            {
                if(iterations > 0)
                {
                    matrix[i][j] = matrix[i][j+displacement];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }

    // Vertical displacement.
    if(moveY > 0)
    {
        int displacement = moveY;
        int iterations;

        for(unsigned int j=0; j<matrixWidth; j++)
        {
            iterations = matrixHeight - displacement;
            for(int i=matrixHeight-1; i>=0; i--)
            {
                if(iterations > 0)
                {
                    matrix[i][j] = matrix[i-displacement][j];
                    iterations--;
                }
                else
                {
                    matrix[i][j] = 0;
                }
            }
        }
    }
    else if(moveY < 0)
    {
        int displacement = -moveY;
        int iterations;

        for(unsigned int j=0; j<matrixWidth; j++)
        {
            iterations = matrixHeight - displacement;
            for(unsigned int i=0; i<matrixHeight; i++)
            {
                if(iterations > 0)
                {
                    matrix[i][j] = matrix[i+displacement][j];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }
}

// Vector2Double implementation
Vector2Double::Vector2Double()
{
    x = y = 0.0;
}
Vector2Double::Vector2Double(double _x, double _y)
{
    x = _x;
    y = _y;
}
Vector2Double::Vector2Double(const Vector2Int& v)
{
    x = static_cast<double>(v.x);
    y = static_cast<double>(v.y);
}

Vector2Double Vector2Double::operator-() const
{
    return Vector2Double(-x, -y);
}
Vector2Double& Vector2Double::operator+=(const Vector2Double& v)
{
    x += v.x;
    y += v.y;
    return *this;
}
Vector2Double& Vector2Double::operator*=(const double t)
{
    x *= t;
    y *= t;
    return *this;
}
Vector2Double& Vector2Double::operator/=(const double t)
{
    return *this *= 1.0 / t;
}

double Vector2Double::Length() const
{
    return sqrt(this->SquaredLength());
}
double Vector2Double::SquaredLength() const
{
    return x * x + y * y;
}


// Rect implementation
Rect::Rect()
{
    left = top = right = bottom = 0.0;
}
Rect::Rect(double _left, double _bottom, double _right, double _top)
{
    left = _left;
    bottom = _bottom;
    right = _right;
    top = _top;
}
Vector2Double Rect::GetLowerBound()
{
    return Vector2Double(left, bottom);
}
Vector2Double Rect::GetHigherBound()
{
    return Vector2Double(right, top);
}
void Rect::SetLowerBound(Vector2Double lb)
{
    left = lb.x;
    bottom = lb.y;
}
void Rect::SetHigherBound(Vector2Double hb)
{
    right = hb.x;
    top = hb.y;
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

    // SFML variables.
    white = sf::Color(255, 255, 255);
    fSetColor = sf::Color(0, 0, 0);
    transparent = sf::Color(255, 255, 255, 0);
    image.create(screenWidth, screenHeight, white);

    // Allocates memory for the maps.
    setMap = new bool*[screenWidth];
    colorMap = new int*[screenWidth];
    auxMap = new unsigned int*[screenWidth];
    for(int i=0; i<screenWidth; i++)
    {
        setMap[i] = new bool[screenHeight];
        colorMap[i] = new int[screenHeight];
        auxMap[i] = new unsigned int[screenHeight];
    }

    // Initializes maps.
    for(int i=0; i<screenWidth; i++)
    {
        for(int j=0; j<screenHeight; j++)
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
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
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
    imgInVector = false;
    usingRenderImage = false;
    rendering = false;
    paused = false;
    pausing = false;
    justLaunchThreads = false;
    renderingToScreen = false;
    zoomingBack = false;
    dontDrawTempImage = false;
    maxIter = 100;
    moving = false;
    varGradChange = false;

    // Creates default color palette.
    relativeColor = false;
    gradPaletteSize = 300;
    alg = RenderingAlgorithm::Other;
    colorPaletteMode = ColorMode::Gaussian;
    gaussianStyle = defaultEstStyle;
    gradStyle = defaultGradStyle;
    gradient.fromString(wxString(wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);")));
    gradient.setMin(0);
    gradient.setMax(gradPaletteSize);

    gaussianPaletteSize = paletteSize = 60;
    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;

    redInt = 255;
    greenInt = 201;
    blueInt = 255;

    redMean = 34;
    greenMean = 28;
    blueMean = 12;

    redStdDev = 8;
    greenStdDev = 12;
    blueStdDev = 9;

    for(int i=0; i<paletteSize; i++)
    {
        redPalette[i] = CalcGradient(i, Color::Red);
        greenPalette[i] = CalcGradient(i, Color::Green);
        bluePalette[i] = CalcGradient(i, Color::Blue);
    }
    for(int i=0; i<paletteSize; i++)
        palette[i] = sf::Color(redPalette[i], greenPalette[i], bluePalette[i]);

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

    // SFML variables.
    white = sf::Color(255, 255, 255);
    fSetColor = sf::Color(0, 0, 0);
    transparent = sf::Color(255, 255, 255, 0);
    image.create(screenWidth, screenHeight, white);
    tempImage = image;
    geomImage.create(screenWidth, screenHeight, transparent);
    output.SetImage(image);
    tempSprite.SetImage(tempImage);
    outGeom.SetImage(geomImage);
    font.loadFromFile(GetAbsPath({ "Resources", "PublicSans-Regular.otf" }));
    text.SetFont(font);

    // Set fractal properties.
    minX = -2.0;
    maxX = 1.0;
    minY = -1.2;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
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
    imgInVector = false;
    usingRenderImage = false;
    rendering = false;
    paused = false;
    pausing = false;
    justLaunchThreads = false;
    renderingToScreen = true;
    zoomingBack = false;
    dontDrawTempImage = true;
    varGradChange = false;

    // Allocates memory for the maps.
    setMap = new bool*[screenWidth];
    colorMap = new int*[screenWidth];
    auxMap = new unsigned int*[screenWidth];
    for(int i=0; i<screenWidth; i++)
    {
        setMap[i] = new bool[screenHeight];
        colorMap[i] = new int[screenHeight];
        auxMap[i] = new unsigned int[screenHeight];
    }

    // Initializes maps.
    for(int i=0; i<screenWidth; i++)
    {
        for(int j=0; j<screenHeight; j++)
        {
            setMap[i][j] = false;
            colorMap[i][j] = -1;
            auxMap[i][j] = 0;
        }
    }

    // Creates default color palette.
    relativeColor = false;
    gradPaletteSize = 300;
    alg = RenderingAlgorithm::Other;
    gaussianStyle = defaultEstStyle;
    gradStyle = defaultGradStyle;
    colorPaletteMode = ColorMode::Gaussian;
    gradient.fromString(wxString(wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);")));
    gradient.setMin(0);
    gradient.setMax(gradPaletteSize);

    gaussianPaletteSize = paletteSize = 60;
    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;

    redInt = 255;
    greenInt = 201;
    blueInt = 255;

    redMean = 34;
    greenMean = 28;
    blueMean = 21;

    redStdDev = 8;
    greenStdDev = 12;
    blueStdDev = 9;

    for(int i=0; i<paletteSize; i++)
    {
        redPalette[i] = CalcGradient(i, Color::Red);
        greenPalette[i] = CalcGradient(i, Color::Green);
        bluePalette[i] = CalcGradient(i, Color::Blue);
    }
    for(int i=0; i<paletteSize; i++)
        palette[i] = sf::Color(redPalette[i], greenPalette[i], bluePalette[i]);

    this->SetOutermostZoom();
}
Fractal::~Fractal()
{
    // Cleanup.
    for(int i=0; i< backScreenWidth; i++)
    {
        delete[] setMap[i];
        delete[] colorMap[i];
        delete[] auxMap[i];
    }

    delete[] setMap;
    delete[] colorMap;
    delete[] auxMap;
    delete[] redPalette;
    delete[] greenPalette;
    delete[] bluePalette;
    delete[] palette;
}
void Fractal::SetDefaultOpt()
{
    renderJobComp = true;
    changeFractalProp = false;
    geomFigure = false;
    onWxCtrl = false;

    for(int i=0; i<4; i++)
        movement[i] = false;
}
void Fractal::Show(sf::RenderWindow *Window)
{
    // If the fractal is moving uses the rendered image.
    if(xVel != 0 || yVel != 0)
        output.SetPosition(static_cast<float>(posX), static_cast<float>(posY));
    else
    {
        if(moving)
        {
            MoveMatrix<bool>(setMap, screenHeight, screenWidth, yMoved, xMoved);
            MoveMatrix<int>(colorMap, screenHeight, screenWidth, yMoved, xMoved);
            MoveMatrix<unsigned int>(auxMap, screenHeight, screenWidth, yMoved, xMoved);
            moving = false;
            rendered = false;
        }
        if(!rendered)
        {
            if(usingRenderImage)
            {
                moving = false;
                usingRenderImage = false;
                xMoved = 0;
                yMoved = 0;
            }

            if(!rendering)
            {
                rendering = true;
                this->PrepareRender();
                this->Render();
            }

            xMoved = 0;
            yMoved = 0;

            // Draw maps on the screen.
            this->DrawMaps(Window);

            // Check in the threads have finished.
            if(!this->IsRendering())
            {
                rendered = true;
                rendering = false;
                dontDrawTempImage = false;
                zoomingBack = false;
                this->PostRender();
                this->DrawMaps(Window);
            }
        }

        // Pause mode.
        if(pausing)
        {
            this->DrawMaps(Window);
            pausing = false;
        }

        // Color change mode.
        if(varGradient || varGradChange)
        {
            if(changeGradient < paletteSize)
                changeGradient += varGradientStep;
            else
                changeGradient = 0;

            // Redraw colors.
            if(rendered)
            {
                double coef = (double)paletteSize/(double)maxColorMapVal;
                for(int i=0; i<screenWidth; i++)
                {
                    for(int j=0; j<screenHeight; j++)
                    {
                        if(setMap[i][j] == false || !colorSet)
                        {
                            // Color pixel.
                            sf::Color Color;
                            if(relativeColor)
                                Color = CalcColor(colorMap[i][j]*coef + changeGradient);
                            else
                                Color = CalcColor(colorMap[i][j] + changeGradient);

                            image.setPixel(i, j, Color);
                        }
                    }
                }
            }
            varGradChange = false;
        }
    }

    if(!dontDrawTempImage && colorMode)
        Window->draw(tempSprite);

    Window->draw(output);

    if(orbitMode && !this->IsRendering())
    {
        if (!orbitDrawn)
        {
            orbitLines.clear();
            geomImage.create(screenWidth, screenHeight, transparent);
            this->DrawOrbit();
        }
        Window->draw(outGeom);
    }
    if (geomFigure && !this->IsRendering())
        this->DrawGeom(Window);

    // Writes iteration text.
    if(!onSnapshot && !rendering)
    {
        static sf::Image base;
        static sf::Sprite baseSprite;
        if(changeFractalIter)
        {
            // Calculate the numbers of digits in the number.
            int number = maxIter;
            int digits = 1;
            while(number >= 10)
            {
                number /= 10;
                digits++;
            }

            // Creates support layer.
            base.create(148 + (12*digits), 35, sf::Color(0, 0, 0, 100));
            baseSprite.SetPosition(0, 0);
            baseSprite.SetImage(base);
            baseSprite.Resize(static_cast<float>(148 + (12*digits)), static_cast<float>(35));

            // Writes text.
            string temp;
            std::ostringstream oss;
            oss << maxIter;
            temp = fractIterationsTxt;
            temp += oss.str();
            text.SetText(temp);
            text.SetSize(25);
            text.SetPosition(0, 0);
            changeFractalIter = false;
        }
        Window->draw(baseSprite);
        Window->draw(text);
    }
}
void Fractal::Resize(sf::RenderWindow *Window)
{
    // Stop threads if they are still rendering.
    this->StopRender();
    paused = false;

    // Frees memory.
    for(int i=0; i< backScreenWidth; i++)
    {
        delete[] setMap[i];
        delete[] colorMap[i];
        delete[] auxMap[i];
        setMap[i] = nullptr;
        colorMap[i] = nullptr;
        auxMap[i] = nullptr;
    }
    delete[] setMap;
    delete[] colorMap;
    delete[] auxMap;
    setMap = nullptr;
    colorMap = nullptr;
    auxMap = nullptr;

    // Copy window properties.
    screenHeight = Window->getSize().y;
    backScreenWidth = screenWidth = Window->getSize().x;
    dontDrawTempImage = true;
    tempSprite.SetSubRect(sf::Rect<int>(0,0,screenWidth, screenHeight));

    // Allocate memory.
    setMap = new bool*[screenWidth];
    colorMap = new int*[screenWidth];
    auxMap = new unsigned int*[screenWidth];
    for(int i=0; i<screenWidth; i++)
    {
        setMap[i] = new bool[screenHeight];
        colorMap[i] = new int[screenHeight];
        auxMap[i] = new unsigned int[screenHeight];
    }

    // Initialize maps.
    for(int i=0; i<screenWidth; i++)
    {
        for(int j=0; j<screenHeight; j++)
        {
            setMap[i][j] = false;
            colorMap[i][j] = -1;
            auxMap[i][j] = 0;
        }
    }

    // Sets SFML variables and scales.
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    this->SetOutermostZoom();

    rendered = false;
    rendering = false;
    geomImage.create(screenWidth, screenHeight, transparent);
    imgVector.clear();
    imgInVector = false;
    orbitDrawn = false;

    // Fix the scales in the zoom vectors.
    for(unsigned int i=0; i<zoom[3].size(); i++)
    {
        zoom[3][i] = zoom[2][i] + (zoom[1][i] - zoom[0][i])*screenHeight/screenWidth;
    }

    // Changes output layer properties.
    sf::Rect<int> Size;
    Size.Right = screenWidth;
    Size.Bottom = screenHeight;
    output.SetSubRect(Size);
    outGeom.SetSubRect(Size);
}
void Fractal::SetAreaOfView(sf::Rect<int> pixelCoordinates)
{
    if(paused)
    {
        this->DeleteSavedZooms();
        dontDrawTempImage = true;
        paused = false;
    }
    else
    {
        imgVector.push_back(image);
        imgInVector = true;
        dontDrawTempImage = false;
    }
    this->SaveZoom();

    // Changes the scale.
    double FX = (maxX - minX)/screenWidth;
    double FY = (maxY - minY)/screenHeight;

    maxX = minX + (pixelCoordinates.Right)*FX;
    minX = minX + pixelCoordinates.Left*FX;
    minY = maxY - (pixelCoordinates.Bottom)*FY;

    maxY = minY + (maxX-minX)*screenHeight/screenWidth;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    rendered = false;
    rendering = false;

    tempImage = image;
    tempSprite.SetImage(tempImage);
    int newHeight = pixelCoordinates.Bottom - (pixelCoordinates.Right-pixelCoordinates.Left)*screenHeight/screenWidth;
    int resize1 = screenWidth*screenWidth/(pixelCoordinates.getSize().x);
    int resize2 = screenHeight*screenHeight/((pixelCoordinates.Bottom - newHeight));
    tempSprite.Resize(resize1, resize2);
    tempSprite.SetCenter(pixelCoordinates.Left, newHeight);

    posY = posX = 0;
    yVel = xVel = 0;
    yMoved = xMoved = 0;

    orbitDrawn = false;
}
void Fractal::SetAreaOfView(Rect worldCoordinates)
{
    minX = worldCoordinates.left;
    maxX = worldCoordinates.right;
    minY = worldCoordinates.bottom;
    maxY = worldCoordinates.top;

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
    for(int i=0; i< backScreenWidth; i++)
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
    setMap = new bool*[screenWidth];
    colorMap = new int*[screenWidth];
    auxMap = new unsigned int*[screenWidth];
    for(int i=0; i<screenWidth; i++)
    {
        setMap[i] = new bool[screenHeight];
        colorMap[i] = new int[screenHeight];
        auxMap[i] = new unsigned int[screenHeight];
    }

    // Initialize maps.
    for(int i=0; i<screenWidth; i++)
    {
        for(int j=0; j<screenHeight; j++)
        {
            setMap[i][j] = false;
            colorMap[i][j] = -1;
            auxMap[i][j] = 0;
        }
    }

    // Sets SFML variables and scales.
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
}
void Fractal::Move()
{
    if(rendered)
    {
        // If any movement button is pressed move the image and accelerate the movement.
        if(movement[Direction::Left])
            xVel += stdSpeed;
        if(movement[Direction::Right])
            xVel -= stdSpeed;
        if(movement[Direction::Up])
            yVel += stdSpeed;
        if(movement[Direction::Down])
            yVel -= stdSpeed;

        // If isn't pressed slow down the image.
        if(!movement[Direction::Left] && !movement[Direction::Right] && !movement[Direction::Up] && !movement[Direction::Down])
        {
            if(xVel > 0) xVel -= stdSpeed;
            if(xVel < 0) xVel += stdSpeed;
            if(yVel > 0) yVel -= stdSpeed;
            if(yVel < 0) yVel += stdSpeed;
        }

        // Updates the coordinates.
        if(xVel != 0 || yVel != 0)
        {
            double FX = (maxX - minX)/screenWidth;
            double FY = (maxY - minY)/screenHeight;

            minX -= xVel*FX;
            maxX -= xVel*FX;
            minY += yVel*FY;
            maxY += yVel*FY;

            posX += xVel;
            posY += yVel;

            // A new render is needed.
            //rendered = false;
            moving = true;
            dontDrawTempImage = true;
        }
        else if(posX != 0 || posY != 0)
        {
            if(paused && !pausing)
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
    bool stillRendering = this->StopRender();

    // Looks for previous zoom coordinates.
    bool thereIsZoom = true;
    for(int i=0; i<4; i++)
    {
        if(zoom[i].empty())
            thereIsZoom = false;
    }

    // If they exist, use them.
    if(thereIsZoom)
    {
        minX = zoom[0][zoom[0].size()-1];
        maxX = zoom[1][zoom[1].size()-1];
        minY = zoom[2][zoom[2].size()-1];
        maxY = zoom[3][zoom[3].size()-1];

        for(int i=0; i<4; i++)
            zoom[i].pop_back();
    }
    // If they don't, expand the drawing area.
    else
    {
        double scaleX = abs(maxX-minX);
        double scaleY = abs(maxY-minY);
        minX -= scaleX;
        maxX += scaleX;
        minY -= scaleY;
        maxY = minY+(maxX-minX)*screenHeight/screenWidth;
        this->SetOutermostZoom();
    }

    xFactor = (maxX - minX)/(screenWidth - 1);
    yFactor = (maxY - minY)/(screenHeight - 1);

    // If there are no changes uses previous image.
    if(imgInVector && !varGradient && imgVector.size() > 0 && !stillRendering)
    {
        image = imgVector[imgVector.size()-1];
        imgVector.pop_back();
        usingRenderImage = true;
    }
    else
    {
        if(stillRendering && imgVector.size() > 0) imgVector.pop_back();
        rendered = false;
        zoomingBack = true;
    }
    magnification = 3/(maxX-minX);
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
    if(imgVector.size() > 0)
    {
        imgVector.clear();
        imgInVector = false;
    }
}
void Fractal::PrepareRender()
{
    this->PreRender();
    usingRenderImage = false;

    // Checks if the movement is valid.
    if((abs(xMoved) >= screenWidth) || (abs(yMoved) >= screenHeight)) redrawAll = true;

    // Clear maps.
    if((!xMoved && !yMoved) || redrawAll || redrawAlways)
    {
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
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
void Fractal::DrawMaps(sf::RenderWindow* Window)
{
    this->PreDrawMaps();

    // Draw maps on the screen.
    if(zoomingBack || dontDrawTempImage || !colorMode)
        image.create(screenWidth, screenHeight, white);
    else
    {
        image.create(screenWidth, screenHeight, transparent);
        Window->draw(tempSprite);
    }
    output.SetPosition(0, 0);
    if(relativeColor)
    {
        // Search for a color maximum value.
        maxColorMapVal = 0;
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                if(colorMap[i][j] > maxColorMapVal)
                    maxColorMapVal = colorMap[i][j];
            }
        }

        if(maxColorMapVal == 0)
            maxColorMapVal = 1;

        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                if(setMap[i][j] == true && colorSet)
                    image.setPixel(i, j, fSetColor);
                else
                {
                    if(colorMode)
                    {
                        // Color pixel.
                        if(colorMap[i][j] >= 0)
                        {
                            sf::Color Color = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize + changeGradient);
                            image.setPixel(i, j, Color);
                        }
                        else if(zoomingBack || dontDrawTempImage)
                        {
                            sf::Color Color = CalcColor(changeGradient);
                            image.setPixel(i, j, Color);
                        }
                    }
                }
            }
        }
    }
    else
    {
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                if(setMap[i][j] == true && colorSet)
                    image.setPixel(i, j, fSetColor);
                else
                {
                    if(colorMode)
                    {
                        // Color pixel.
                        if(colorMap[i][j] >= 0)
                        {
                            sf::Color Color = CalcColor(colorMap[i][j] + changeGradient);
                            image.setPixel(i, j, Color);
                        }
                        else if(zoomingBack || dontDrawTempImage)
                        {
                            sf::Color Color = CalcColor(changeGradient);
                            image.setPixel(i, j, Color);
                        }
                    }
                }
            }
        }
    }

    Window->draw(output);

    if(!juliaMode)
        Window->display();
}
void Fractal::Redraw()
{
    if(this->IsRendering())
        dontDrawTempImage = true;
    else if(colorMode)
    {
        tempImage = image;
        tempSprite.SetCenter(0,0);
        tempSprite.Resize(screenWidth, screenHeight);
    }

    this->StopRender();
    redrawAll = true;
    imgVector.clear();
    imgInVector = false;
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
    if(paused)
    {
        this->PreRestartRender();
        rendered = false;
        rendering = true;
        watchdog.LaunchThreads();
        watchdog.Launch();
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
    if(this->IsRendering())
    {
        watchdog.Terminate();
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
void Fractal::HandleEvents(sf::Event *Event)
{
    if(!this->IsRendering())
    {
        if(Event->type == sf::Event::KeyPressed)
        {
            switch(Event->key.code)
            {
            case sf::Keyboard::L:
                {
                    this->MoreIter();
                    break;
                }
            case sf::Keyboard::K:
                {
                    this->LessIter();
                    break;
                }
            default:
                break;
            }
        }
    }

    if(!onWxCtrl && Event->Type == sf::Event::MouseButtonPressed)
    {
        if(Event->MouseButton.Button == sf::Mouse::Right)
        {
            if(xVel == 0 && yVel == 0) ZoomBack();
        }
    }
}
bool Fractal::IsRendering()
{
    if(waitRoutine) return false;
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

    if(paused)
        dontDrawTempImage = true;

    redrawAll = true;
    maxIter += 100;

    rendered = false;
}
void Fractal::LessIter()
{
    changeFractalIter = true;
    this->DeleteSavedZooms();

    if(paused) 
        dontDrawTempImage = true;

    redrawAll = true;
    int signedMaxIter = (int)maxIter;

    if(signedMaxIter - 100 > 0)
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
    return minX + Pixel_X*xFactor;
}
double Fractal::GetY(int Pixel_Y)
{
    return maxY - Pixel_Y*yFactor;
}
int Fractal::GetPixelX(double xNum)
{
    return (xNum-minX)/xFactor;
}
int Fractal::GetPixelY(double yNum)
{
    return (maxY-yNum)/yFactor;
}
void Fractal::ChangeIterations(int number)
{
    if(number > 0)
    {
        maxIter = static_cast<unsigned int>(number);
        rendered = false;
        changeFractalIter = true;
        dontDrawTempImage = true;
    }
}
void Fractal::SetOptions(Options opt, bool keepSize)
{
    if(!keepSize)
    {
        minX = opt.minX;
        maxX = opt.maxX;
        minY = opt.minY;
        maxY = opt.maxY;
    }
    else
        maxY = minY+(maxX-minX)*screenHeight/screenWidth;

    maxIter = opt.maxIter;
    panelOpt = opt.panelOpt;
    changeGradient = opt.changeGradient;
    colorPaletteMode = opt.colorPaletteMode;
    relativeColor = opt.relativeColor;
    gradPaletteSize = opt.gradPaletteSize;
    gaussianPaletteSize = opt.gaussianPaletteSize;
    alg = opt.alg;

    redInt = opt.redInt;
    greenInt = opt.greenInt;
    blueInt = opt.blueInt;

    redMean = opt.redMean;
    greenMean = opt.greenMean;
    blueMean = opt.blueMean;

    redStdDev = opt.redStdDev;
    greenStdDev = opt.greenStdDev;
    blueStdDev = opt.blueStdDev;
    fSetColor = opt.fSetColor;

    gradient = opt.gradient;
    if(colorPaletteMode == ColorMode::Gradient)
    {
        paletteSize = gradPaletteSize;
        this->SetGradientSize(paletteSize);
    }
    else
    {
        paletteSize = gaussianPaletteSize;
        this->SetPaletteSize(paletteSize);
    }

    if(hasSmoothRender)
        smoothRender = opt.smoothRender;

    kReal = opt.kReal;
    kImaginary = opt.kImaginary;

    orbitTrapMode = opt.orbitTrapMode;
    colorSet = opt.colorSet;
    colorMode = opt.colorMode;
    justLaunchThreads = opt.justLaunchThreads;

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

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
    opt.gaussianPaletteSize = gaussianPaletteSize;
    opt.gradPaletteSize = gradPaletteSize;
    opt.panelOpt = panelOpt;
    opt.type = type;

    opt.kReal = kReal;
    opt.kImaginary = kImaginary;

    opt.orbitTrapMode = orbitTrapMode;
    opt.colorSet = colorSet;
    opt.colorMode = colorMode;
    opt.justLaunchThreads = justLaunchThreads;

    opt.redInt = redInt;
    opt.greenInt = greenInt;
    opt.blueInt = blueInt;

    opt.redMean = redMean;
    opt.greenMean = greenMean;
    opt.blueMean = blueMean;

    opt.redStdDev = redStdDev;
    opt.greenStdDev = greenStdDev;
    opt.blueStdDev = blueStdDev;
    opt.fSetColor = fSetColor;

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
    if(xVel == 0 && yVel == 0)
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
    switch(dir)
    {
    case Direction::Up:
        {
            yVel -= stdSpeed;
            movement[Direction::Up] = true;
            break;
        }
    case Direction::Down:
        {
            yVel += stdSpeed;
            movement[Direction::Down] = true;
            break;
        }
    case Direction::Left:
        {
            xVel -= stdSpeed;
            movement[Direction::Left] = true;
            break;
        }
    case Direction::Right:
        {
            xVel += stdSpeed;
            movement[Direction::Right] = true;
            break;
        }
    default: break;
    }
}
void Fractal::ReleaseMovement(Direction dir)
{
    switch(dir)
    {
    case Direction::Up:
        {
            movement[Direction::Up] = false;
            break;
        }
    case Direction::Down:
        {
            movement[Direction::Down] = false;
            break;
        }
    case Direction::Left:
        {
            movement[Direction::Left] = false;
            break;
        }
    case Direction::Right:
        {
            movement[Direction::Right] = false;
            break;
        }
    default: break;
    }
}

// Save image.
sf::Image Fractal::GetRenderedImage()
{
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
    onSnapshot = true;
    waitRoutine = true;
    if(!rendered)
    {
        this->PrepareRender();
        this->Render();
    }
    this->PreDrawMaps();

    image.create(screenWidth, screenHeight, white);

    maxColorMapVal = 0;
    if(relativeColor)
    {
        // Search for a color maximum value.
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                if(colorMap[i][j] > maxColorMapVal)
                    maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if(maxColorMapVal == 0)
        maxColorMapVal = 1;

    for(int i=0; i<screenWidth; i++)
    {
        for(int j=0; j<screenHeight; j++)
        {
            if(setMap[i][j] == true && colorSet)
                image.setPixel(i, j, fSetColor);
            else
            {
                if(colorMode)
                {
                    // Color pixel.
                    sf::Color Color;
                    if(relativeColor)
                        Color = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize+changeGradient);
                    else
                        Color = CalcColor(colorMap[i][j] + changeGradient);

                    image.setPixel(i, j, Color);
                }
            }
        }
    }

    onSnapshot = false;
    waitRoutine = false;
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
    return image;
}
wxBitmap Fractal::GetRenderedWxBitmap()
{
    sf::Image renderedImage = this->GetRenderedImage();
    wxBitmap output(renderedImage.getSize().x, renderedImage.getSize().y);
    wxMemoryDC dc(output);

    for (unsigned i = 0; i < renderedImage.getSize().x; i++)
    {
        for (unsigned j = 0; j < renderedImage.getSize().y; j++)
        {
            const sf::Color pixel = renderedImage.GetPixel(i, j);
            dc.SetPen(wxColour(pixel.r, pixel.g, pixel.b));
            dc.SetBrush(wxColour(pixel.r, pixel.g, pixel.b));
            dc.DrawPoint(i, j);
        }
    }

    return output;
}
void Fractal::RenderBMP(string filename)
{
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
    waitRoutine = true;
    onSnapshot = true;
    BMPWriter writer(filename.c_str(), screenWidth, screenHeight);
    if(!rendered)
    {
        this->PrepareRender();
        this->Render();
    }
    this->PreDrawMaps();
    BMPPixel* data = new BMPPixel[screenWidth];

    maxColorMapVal = 0;
    if(relativeColor)
    {
        // Search for a color maximum value.
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                if(colorMap[i][j] > maxColorMapVal)
                    maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if(maxColorMapVal == 0)
        maxColorMapVal = 1;

    // Copy maps values to BMPWriter.
    for(int j=screenHeight-1; j>=0; j--)
    {
        for(int i=0; i<screenWidth; i++)
        {
            if(setMap[i][j] != 0 && colorSet)
            {
                data[i].r = 0;
                data[i].g = 0;
                data[i].b = 0;
            }
            else if(colorMode)
            {
                if(relativeColor)
                {
                    data[i].r = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize+changeGradient).r;
                    data[i].g = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize+changeGradient).g;
                    data[i].b = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize+changeGradient).b;
                }
                else
                {
                    data[i].r = CalcColor(colorMap[i][j]+changeGradient).r;
                    data[i].g = CalcColor(colorMap[i][j]+changeGradient).g;
                    data[i].b = CalcColor(colorMap[i][j]+changeGradient).b;
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

#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}
void Fractal::PrepareSnapshot(bool mode)
{
    onSnapshot = mode;
}

// EST Color.
void Fractal::SetGaussianColorIntensity(int intensity, Color col)
{
    // Changes intensity value.
    colorPaletteMode = ColorMode::Gaussian;
    if(col == Color::Red)
        redInt = intensity;
    else if(col == Color::Green)
        greenInt = intensity;
    else if(col == Color::Blue)
        blueInt = intensity;

    this->RebuildPalette();
}
void Fractal::SetGaussianColorMean(double med, Color col)
{
    // Changes mean value.
    colorPaletteMode = ColorMode::Gaussian;
    if(col == Color::Red)
        redMean = med;
    else if(col == Color::Green)
        greenMean = med;
    else if(col == Color::Blue)
        blueMean = med;

    this->RebuildPalette();
}
void Fractal::SetGaussianColorStdDev(double des, Color col)
{
    // Changes standard deviation value.
    colorPaletteMode = ColorMode::Gaussian;
    if(col == Color::Red)
        redStdDev = des;
    else if(col == Color::Green)
        greenStdDev = des;
    else if(col == Color::Blue)
        blueStdDev = des;

    this->RebuildPalette();
}
int Fractal::GetGaussianColorIntensity(Color col)
{
    if(col == Color::Red)
        return redInt;
    else if(col == Color::Green)
        return greenInt;
    else if(col == Color::Blue)
        return blueInt;
    else
        return 0;
}
double Fractal::GetGaussianColorMean(Color col)
{
    if(col == Color::Red)
        return redMean;
    else if(col == Color::Green)
        return greenMean;
    else if(col == Color::Blue)
        return blueMean;
    else
        return 0;
}
double Fractal::GetGaussianColorStdDev(Color col)
{
    if(col == Color::Red)
        return redStdDev;
    else if(col == Color::Green)
        return greenStdDev;
    else if(col == Color::Blue)
        return blueStdDev;
    else
        return 0;
}

// Color styles.
void Fractal::SetGaussianColorStyle(GaussianColorStyles _gaussianStyle)
{
    // Changes color palette.
    gaussianStyle = _gaussianStyle;
    GaussianColorPalette c;
    c.SetStyle(gaussianStyle);

    // Sets parameters for new palette.
    redInt = c.redInt;
    greenInt = c.greenInt;
    blueInt = c.blueInt;

    redMean = c.redMean;
    greenMean = c.greenMean;
    blueMean = c.blueMean;

    redStdDev = c.redStdDev;
    greenStdDev = c.greenStdDev;
    blueStdDev = c.blueStdDev;

    this->SetPaletteSize(c.paletteSize);
    for(int i=0; i<paletteSize; i++)
    {
        redPalette[i] = CalcGradient(i, Color::Red);
        greenPalette[i] = CalcGradient(i, Color::Green);
        bluePalette[i] = CalcGradient(i, Color::Blue);
    }
    for(int i=0; i<paletteSize; i++)
        palette[i] = sf::Color(redPalette[i], greenPalette[i], bluePalette[i]);
}
GaussianColorStyles Fractal::GetGaussianColorStyle()
{
    return gaussianStyle;
}
void Fractal::SetGradStyle(GradientColorStyles _gradStyle)
{
    gradStyle = _gradStyle;
}
GradientColorStyles Fractal::GetGradStyle()
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
    delete[] redPalette;
    delete[] greenPalette;
    delete[] bluePalette;
    delete[] palette;
    gradPaletteSize = paletteSize = gradient.getMax()-gradient.getMin();
    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;
    this->RebuildPalette();
}
void Fractal::SetGradientSize(unsigned int size)
{
    colorPaletteMode = ColorMode::Gradient;
    delete[] redPalette;
    delete[] greenPalette;
    delete[] bluePalette;
    delete[] palette;
    gradient.setMax(size);
    gradPaletteSize = paletteSize = size;
    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;
    this->RebuildPalette();
}

// Color operations.
void Fractal::RedrawMaps()
{
    maxColorMapVal = 0;

    if(this->IsRendering())
        dontDrawTempImage = true;

    if(relativeColor)
    {
        // Search for color maximum.
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                if(colorMap[i][j] > maxColorMapVal) 
                    maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if(maxColorMapVal == 0)
        maxColorMapVal = 1;

    for(int i=0; i<screenWidth; i++)
    {
        for(int j=0; j<screenHeight; j++)
        {
            if(setMap[i][j] == true && colorSet)
                image.setPixel(i, j, fSetColor);
            else if(colorMode)
            {
                // Color pixel.
                sf::Color Color;
                if(relativeColor)
                    Color = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize + changeGradient);
                else
                    Color = CalcColor(colorMap[i][j] + changeGradient);
                image.setPixel(i, j, Color);
            }
            else
                image.setPixel(i, j, white);
        }
    }
}
void Fractal::SetExtColorMode(bool mode)
{
    // Changes external color mode.
    if(colorMode != mode)
    {
        colorMode = mode;
        if(usingRenderImage)
            rendered = false;
        else
            this->RedrawMaps();
    }
    this->DeleteSavedZooms();
}
void Fractal::SetFractalSetColorMode(bool mode)
{
    // Changes internal color mode.
    if(colorSet != mode)
    {
        colorSet = mode;
        if(usingRenderImage)
            rendered = false;
        else
            this->RedrawMaps();
    }
    this->DeleteSavedZooms();
}
void Fractal::SetFractalSetColor(sf::Color color)
{
    // Changes the color of the set.
    fSetColor = color;

    if(this->IsRendering())
        dontDrawTempImage = true;

    if(usingRenderImage)
    {
        rendered = false;
        dontDrawTempImage = true;
    }
    else
        this->RedrawMaps();
    this->DeleteSavedZooms();
}
sf::Color Fractal::GetSetColor()
{
    return fSetColor;
}
bool Fractal::GetExtColorMode()
{
    return colorMode;
}
bool Fractal::GetSetColorMode()
{
    return colorSet;
}
double Fractal::NormalDist(int x, double mean, double stdDev)
{
    return exp(-(pow(x-mean,2)/(2*pow(stdDev,2))));
}
sf::Uint8 Fractal::CalcGradient(int colorNum, Color col)
{
    colorNum = colorNum % paletteSize;

    // Return color with a normal distribution.
    if(col == Color::Red)
        return static_cast<sf::Uint8>(redInt*NormalDist(colorNum, redMean, redStdDev) + redInt*NormalDist(colorNum, paletteSize+redMean, redStdDev));
    if(col == Color::Green)
        return static_cast<sf::Uint8>(greenInt*NormalDist(colorNum, greenMean, greenStdDev) + greenInt*NormalDist(colorNum, paletteSize+greenMean, greenStdDev));
    if(col == Color::Blue)
        return static_cast<sf::Uint8>(blueInt*NormalDist(colorNum, blueMean, blueStdDev)+blueInt*NormalDist(colorNum, paletteSize+blueMean, blueStdDev));

    return 0;
}
sf::Color Fractal::CalcColor(int colorNum)
{
    if(colorNum <= 0)
        colorNum = 0;

    colorNum = colorNum % paletteSize;
    return palette[colorNum];
}
void Fractal::ChangeVarGradient()
{
    varGradient = !varGradient;
    if(usingRenderImage)
    {
        rendered = false;
        dontDrawTempImage = true;
    }
    imgVector.clear();
    imgInVector = false;
}
void Fractal::RebuildPalette()
{
    this->DeleteSavedZooms();

    if(this->IsRendering())
        dontDrawTempImage = true;

    if(colorPaletteMode == ColorMode::Gradient)
    {
        wxColour myWxColor;
        for(int i=0; i<paletteSize; i++)
        {
            myWxColor = gradient.getColorAt(i);
            redPalette[i] = myWxColor.Red();
            greenPalette[i] = myWxColor.Green();
            bluePalette[i] = myWxColor.Blue();
        }
    }
    else
    {
        for(int i=0; i<paletteSize; i++)
        {
            redPalette[i] = CalcGradient(i, Color::Red);
            greenPalette[i] = CalcGradient(i, Color::Green);
            bluePalette[i] = CalcGradient(i, Color::Blue);
        }
    }

    for(int i=0; i<paletteSize; i++)
        palette[i] = sf::Color(redPalette[i], greenPalette[i], bluePalette[i]);

    if(usingRenderImage)
    {
        rendered = false;
        dontDrawTempImage = true;
    }
    else if(renderingToScreen)
    {
        // Redraw colors.
        if(relativeColor)
        {
            // Search for color maximum.
            maxColorMapVal = 0;
            for(int i=0; i<screenWidth; i++)
            {
                for(int j=0; j<screenHeight; j++)
                {
                    if(colorMap[i][j] > maxColorMapVal)
                        maxColorMapVal = colorMap[i][j];
                }
            }
            if(maxColorMapVal == 0)
                maxColorMapVal = 1;

            for(int i=0; i<screenWidth; i++)
            {
                for(int j=0; j<screenHeight; j++)
                {
                    if(setMap[i][j] == false || !colorSet)
                    {
                        sf::Color Color = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize + changeGradient);
                        image.setPixel(i, j, Color);
                    }
                }
            }
        }
        else
        {
            for(int i=0; i<screenWidth; i++)
            {
                for(int j=0; j<screenHeight; j++)
                {
                    if(setMap[i][j] == false || !colorSet)
                    {
                        sf::Color Color = CalcColor(colorMap[i][j] + changeGradient);
                        image.setPixel(i, j, Color);
                    }
                }
            }
        }
    }
}
int Fractal::GetPaletteSize()
{
    return paletteSize;
}
void Fractal::SetPaletteSize(int size)
{
    delete[] redPalette;
    delete[] greenPalette;
    delete[] bluePalette;
    delete[] palette;
    gaussianPaletteSize = paletteSize = size;
    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;
    this->RebuildPalette();
}
void Fractal::SetVarGradient(int n)
{
    varGradChange = true;
    changeGradient = n % paletteSize;

    if(this->IsRendering())
        dontDrawTempImage = true;

    if(usingRenderImage)
    {
        rendered = false;
        dontDrawTempImage = true;
    }
    this->DeleteSavedZooms();
}

// ColorMode.
void Fractal::SetPaletteMode(ColorMode mode)
{
    colorPaletteMode = mode;
    delete[] redPalette;
    delete[] greenPalette;
    delete[] bluePalette;
    delete[] palette;

    if(mode == ColorMode::Gradient)
        paletteSize = gradient.getMax()-gradient.getMin();
    else
        paletteSize = gaussianPaletteSize;

    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;
    this->RebuildPalette();
}
ColorMode Fractal::GetColorMode()
{
    return colorPaletteMode;
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
    dontDrawTempImage = true;
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
    if(watchdog.ThreadRunning())
    {
        watchdog.Terminate();
        watchdog.StopThreads();
        rendering = false;
    }
    if(_real != kReal || _imaginary != kImaginary) rendered = false;

    kReal = _real;
    kImaginary = _imaginary;

    if(imgInVector) this->DeleteSavedZooms();

    dontDrawTempImage = true;
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

    if(orbitLine)
        orbitLines.push_back(data);
    else
        lines.push_back(data);

    geomFigure = true;
}
void Fractal::DrawCircle( double x_center, double y_center, double radius, sf::Color color )
{
    CircleData data;
    data.x_center = x_center;
    data.y_center = y_center;
    data.radius = radius;
    data.color = color;
    circles.push_back(data);
    geomFigure = true;
}
void Fractal::DrawGeom(sf::RenderWindow *Window)
{
    // Draw lines.
    for(unsigned int i=0; i<lines.size(); i++)
    {
        float x1 = (lines[i].x1-minX)/xFactor;
        float y1 = (maxY-lines[i].y1)/yFactor;
        float x2 = (lines[i].x2-minX)/xFactor;
        float y2 = (maxY-lines[i].y2)/yFactor;
        sf::Shape line = sf::Shape::Line(x1, y1, x2, y2, 2, lines[i].color);
        Window->draw(line);
    }

    // Draw orbit lines.
    for(unsigned int i=0; i<orbitLines.size(); i++)
    {
        float x1 = (orbitLines[i].x1-minX)/xFactor;
        float y1 = (maxY-orbitLines[i].y1)/yFactor;
        float x2 = (orbitLines[i].x2-minX)/xFactor;
        float y2 = (maxY-orbitLines[i].y2)/yFactor;
        sf::Shape line = sf::Shape::Line(x1, y1, x2, y2, 2, orbitLines[i].color);
        Window->draw(line);
    }

    // Draw circles.
    for(unsigned int i=0; i<circles.size(); i++)
    {
        float x0 = (circles[i].x_center-minX)/xFactor;
        float y0 = (maxY-circles[i].y_center)/yFactor;
        float right = (circles[i].x_center + circles[i].radius-minX)/xFactor;
        float r = right - x0;
        sf::Shape circle = sf::Shape::Circle(x0, y0, r, circles[i].color, 2, sf::Color(0,0,0));
        circle.EnableFill(false);
        circle.EnableOutline(true);
        Window->draw(circle);
    }
}

// Orbit mode operations.
void Fractal::SetOrbitMode(bool mode)
{
    if(hasOrbit)
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
    if(!orbitDrawn)
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
    if(hasOrbitTrap)
    {
        this->DeleteSavedZooms();
        if(paused)
            dontDrawTempImage = true;
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
    if(hasSmoothRender)
    {
        this->DeleteSavedZooms();
        if(paused)
            dontDrawTempImage = true;
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
bool Fractal::HasOptPanel()
{
    if(panelOpt.GetElementsSize() > 0)
        return true;
    else
        return false;
}
PanelOptions* Fractal::GetOptPanel()
{
    return &panelOpt;
}


///////////////////
//  ENDS FRACTAL //
///////////////////

// RenderFractal
RenderFractal::RenderFractal()
{
    setMap = nullptr;
    colorMap = nullptr;
    auxMap = nullptr;
    x = y =  0;
    threadProgress = 0;
    wo = ho = wf = hf = oldHo = 0;

    specialRenderMode = false;
    stopped = false;
    threadRunning = false;

    type = FractalType::Undefined;
    xFactor = 0.0;
    yFactor = 0.0;
    minX = maxX = minY = maxY = maxIter = 0.0;
    kReal = kImaginary = 0.0;
}
void RenderFractal::SetOpt(Options opt)
{
    myOpt = opt;
    xFactor = opt.xFactor;
    yFactor = opt.yFactor;
    minX = opt.minX;
    maxX = opt.maxX;
    minY = opt.minY;
    maxY = opt.maxY;
    maxIter = opt.maxIter;
    type = opt.type;
}
void RenderFractal::SetLimits(int widthO, int heightO, int widthF, int heightF)
{
    wo = widthO;
    oldHo = ho = heightO;
    hf = heightF;
    wf = widthF;
}
void RenderFractal::UpdateLimits(int heightO)
{
    ho = heightO;
}
void RenderFractal::SetOldHo(int _oldHo)
{
    oldHo = _oldHo;
}
void RenderFractal::Run()
{
    y = ho;
    threadRunning = true;
    stopped = false;

    if(specialRenderMode)
        this->SpecialRender();
    else
        this->Render();

    threadRunning = false;
}
void RenderFractal::Stop()
{
    if(type != FractalType::ScriptFractal)
    {
        stopped = true;

        if(y != 0)
            ho = y;

        x = wf - 1;
        y = hf - 1;
    }
}
void RenderFractal::SetSpecialRenderMode(bool mode)
{
    specialRenderMode = mode;
}
void RenderFractal::SetRenderOut(bool** outSetMap, int** outColorMap, unsigned int** outAux)
{
    setMap = outSetMap;
    colorMap = outColorMap;
    auxMap = outAux;
}
void RenderFractal::SetK(double re, double im)
{
    kReal = re;
    kImaginary = im;
}
void RenderFractal::Reset()
{
    x = 0;
    y = 0;
}
void RenderFractal::PreTerminate()
{
    // Do nothing.
}
int RenderFractal::AskProgress()
{
    if(!stopped)
        threadProgress = static_cast<int>( floor(100.0*((double)(y+1-oldHo)/(double)(hf-oldHo))) );

    return threadProgress;
}
Vector2Int RenderFractal::GetCoords()
{
    Vector2Int pos;
    pos.x = 0;
    pos.y = ho;
    return pos;
}
Vector2Int RenderFractal::GetStartPoints()
{
    Vector2Int pos;
    pos.x = wo;
    pos.y = ho;
    return pos;
}
Vector2Int RenderFractal::GetEndPoints()
{
    Vector2Int pos;
    pos.x = wf;
    pos.y = hf;
    return pos;
}
bool RenderFractal::IsRunning()
{
    return threadRunning;
}
Options RenderFractal::GetOpt()
{
    return myOpt;
}