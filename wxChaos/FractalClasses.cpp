#include <sstream>
#include <cmath>
#include <limits>
#include "FractalClasses.h"
#include "BmpWriter.h"
#include "StringFuncs.h"
#include "BinaryOps.h"
#include "ConfigParser.h"
#include "CommandFrame.h"

const int stdSpeed = 1;
const GRAD_STYLES defaultGradStyle = RETRO;
const EST_STYLES defaultEstStyle = SUMMER_DAY;


inline double CalcSquaredDist(const double x1, const double y1, const double x2, const double y2)
{ return ((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)); }

inline double CalcDist(const double x1, const double y1, const double x2, const double y2)
{ return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)); }

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
        int desp = moveX;
        int iterations;

        for(unsigned int i=0; i<matrixHeight; i++)
        {
            iterations = matrixWidth - desp;
            for(int j=matrixWidth-1; j>=0; j--)
            {
                if(iterations > 0)
                {
                    matrix[i][j] = matrix[i][j-desp];
                    iterations--;
                }
                else
                {
                    matrix[i][j] = 0;
                }
            }
        }
    }
    else if(moveX < 0)
    {
        int desp = -moveX;
        int iterations;

        for(unsigned int i=0; i<matrixHeight; i++)
        {
            iterations = matrixWidth - desp;
            for(unsigned int j=0; j<matrixWidth; j++)
            {
                if(iterations > 0)
                {
                    matrix[i][j] = matrix[i][j+desp];
                    iterations--;
                }
                else
                {
                    matrix[i][j] = 0;
                }
            }
        }
    }

    // Vertical displacement.
    if(moveY > 0)
    {
        int desp = moveY;
        int iterations;

        for(unsigned int j=0; j<matrixWidth; j++)
        {
            iterations = matrixHeight - desp;
            for(int i=matrixHeight-1; i>=0; i--)
            {
                if(iterations > 0)
                {
                    matrix[i][j] = matrix[i-desp][j];
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
        int desp = -moveY;
        int iterations;

        for(unsigned int j=0; j<matrixWidth; j++)
        {
            iterations = matrixHeight - desp;
            for(unsigned int i=0; i<matrixHeight; i++)
            {
                if(iterations > 0)
                {
                    matrix[i][j] = matrix[i+desp][j];
                    iterations--;
                }
                else
                {
                    matrix[i][j] = 0;
                }
            }
        }
    }
}


/////////////////////////////////////////
////        BEGINS FRACTAL            /////
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
    image.Create(screenWidth, screenHeight, white);

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
    alg = OTHER;
    colorPaletteMode = EST_MODE;
    estStyle = defaultEstStyle;
    gradStyle = defaultGradStyle;
    gradient.fromString(wxString(wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);")));
    gradient.setMin(0);
    gradient.setMax(gradPaletteSize);

    estPaletteSize = paletteSize = 60;
    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;

    redInt = 255;
    greenInt = 201;
    blueInt = 255;

    redMed = 34;
    greenMed = 28;
    blueMed = 12;

    redDes = 8;
    greenDes = 12;
    blueDes = 9;

    for(int i=0; i<paletteSize; i++)
    {
        redPalette[i] = CalcGradient(i, red);
        greenPalette[i] = CalcGradient(i, green);
        bluePalette[i] = CalcGradient(i, blue);
    }
    for(int i=0; i<paletteSize; i++)
    {
        palette[i] = sf::Color(redPalette[i], greenPalette[i], bluePalette[i]);
    }
}
Fractal::Fractal(sf::RenderWindow *Window)
{
    this->SetDefaultOpt();

    // System.
    threadNumber = Get_Cores();

    // Copy window properties.
    screenHeight = Window->GetHeight();
    backScreenWidth = screenWidth = Window->GetWidth();

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
    image.Create(screenWidth, screenHeight, white);
    tempImage = image;
    geomImage.Create(screenWidth, screenHeight, transparent);
    output.SetImage(image);
    tempSprite.SetImage(tempImage);
    outGeom.SetImage(geomImage);
    font.LoadFromFile(GetAbsPath("Resources/DiavloFont.otf"));
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
    alg = OTHER;
    estStyle = defaultEstStyle;
    gradStyle = defaultGradStyle;
    colorPaletteMode = EST_MODE;
    gradient.fromString(wxString(wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);")));
    gradient.setMin(0);
    gradient.setMax(gradPaletteSize);

    estPaletteSize = paletteSize = 60;
    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;

    redInt = 255;
    greenInt = 201;
    blueInt = 255;

    redMed = 34;
    greenMed = 28;
    blueMed = 21;

    redDes = 8;
    greenDes = 12;
    blueDes = 9;

    for(int i=0; i<paletteSize; i++)
    {
        redPalette[i] = CalcGradient(i, red);
        greenPalette[i] = CalcGradient(i, green);
        bluePalette[i] = CalcGradient(i, blue);
    }
    for(int i=0; i<paletteSize; i++)
    {
        palette[i] = sf::Color(redPalette[i], greenPalette[i], bluePalette[i]);
    }
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
    for(int i=0; i<4; i++) movement[i] = false;
}
void Fractal::Show(sf::RenderWindow *Window)
{
    // If the fractal is moving uses the rendered image.
    if(xVel != 0 || yVel != 0)
    {
        output.SetPosition(static_cast<float>(posX), static_cast<float>(posY));
    }
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
                            image.SetPixel(i, j, Color);
                        }
                    }
                }
            }
            varGradChange = false;
        }
    }
    if(!dontDrawTempImage && colorMode)
        Window->Draw(tempSprite);
    Window->Draw(output);

    if(orbitMode)
    {
        if(!this->IsRendering())
        {
            if(!orbitDrawn)
            {
                orbitLines.clear();
                geomImage.Create(screenWidth, screenHeight, transparent);
                this->DrawOrbit();
            }
            Window->Draw(outGeom);
        }
    }
    if(geomFigure)
    {
        if(!this->IsRendering())
        {
            this->DrawGeom(Window);
        }
    }

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
            base.Create(153 + (12*digits), 35, sf::Color(0, 0, 0, 100));
            baseSprite.SetPosition(0, 0);
            baseSprite.SetImage(base);
            baseSprite.Resize(static_cast<float>(153 + (12*digits)), static_cast<float>(35));

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
        Window->Draw(baseSprite);
        Window->Draw(text);
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
        setMap[i] = NULL;
        colorMap[i] = NULL;
        auxMap[i] = NULL;
    }
    delete[] setMap;
    delete[] colorMap;
    delete[] auxMap;
    setMap = NULL;
    colorMap = NULL;
    auxMap = NULL;

    // Copy window properties.
    screenHeight = Window->GetHeight();
    backScreenWidth = screenWidth = Window->GetWidth();
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
    rendered = false;
    rendering = false;
    geomImage.Create(screenWidth, screenHeight, transparent);
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
void Fractal::Resize(sf::Rect<int> scale)
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

    maxX = minX + (scale.Right)*FX;
    minX = minX + scale.Left*FX;
    minY = maxY - (scale.Bottom)*FY;

    maxY = minY + (maxX-minX)*screenHeight/screenWidth;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    rendered = false;
    rendering = false;

    tempImage = image;
    tempSprite.SetImage(tempImage);
    int newHeight = scale.Bottom - (scale.Right-scale.Left)*screenHeight/screenWidth;
    int resize1 = screenWidth*screenWidth/(scale.GetWidth());
    int resize2 = screenHeight*screenHeight/((scale.Bottom - newHeight));
    tempSprite.Resize(resize1, resize2);
    tempSprite.SetCenter(scale.Left, newHeight);

    posY = posX = 0;
    yVel = xVel = 0;
    yMoved = xMoved = 0;

    orbitDrawn = false;
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
        if(movement[LEFT])
            xVel += stdSpeed;
        if(movement[RIGHT])
            xVel -= stdSpeed;
        if(movement[UP])
            yVel += stdSpeed;
        if(movement[DOWN])
            yVel -= stdSpeed;

        // If isn't pressed slow down the image.
        if(!movement[LEFT] && !movement[RIGHT] && !movement[UP] && !movement[DOWN])
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
void Fractal::Move(const sf::Input& input)
{
    if(rendered)
    {
        // If any movement button is pressed move the image and accelerate the movement.
        if(input.IsKeyDown(sf::Key::Left) || input.IsKeyDown(sf::Key::A))
            xVel += stdSpeed;
        else if(input.IsKeyDown(sf::Key::Right) || input.IsKeyDown(sf::Key::D))
            xVel -= stdSpeed;
        else if(input.IsKeyDown(sf::Key::Up) || input.IsKeyDown(sf::Key::W))
            yVel += stdSpeed;
        else if(input.IsKeyDown(sf::Key::Down) || input.IsKeyDown(sf::Key::S))
            yVel -= stdSpeed;
        // If isn't pressed slow down the image.
        else
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
void Fractal::ZoomBack()
{
    bool stillRendering = this->StopRender();

    // Looks for previous zoom coordinates.
    bool thereIsZoom = true;
    for(int i=0; i<4; i++)
    {
        if(zoom[i].empty()) thereIsZoom = false;
    }

    // If they exist, used them.
    if(thereIsZoom)
    {
        minX = zoom[0][zoom[0].size()-1];
        maxX = zoom[1][zoom[1].size()-1];
        minY = zoom[2][zoom[2].size()-1];
        maxY = zoom[3][zoom[3].size()-1];

        for(int i=0; i<4; i++)
        {
            zoom[i].pop_back();
        }
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
    }

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

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
void Fractal::DrawMaps(sf::RenderWindow *Window)
{
    this->PreDrawMaps();

    // Draw maps on the screen.
    if(zoomingBack || dontDrawTempImage || !colorMode)
        image.Create(screenWidth, screenHeight, white);
    else
    {
        image.Create(screenWidth, screenHeight, transparent);
        Window->Draw(tempSprite);
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
                if(colorMap[i][j] > maxColorMapVal) maxColorMapVal = colorMap[i][j];
            }
        }
        if(maxColorMapVal == 0) maxColorMapVal = 1;
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                if(setMap[i][j] == true && colorSet)
                {
                    // Set pixel.
                    image.SetPixel(i, j, fSetColor);
                }
                else
                {
                    if(colorMode)
                    {
                        // Color pixel.
                        if(colorMap[i][j] >= 0)
                        {
                            sf::Color Color = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize + changeGradient);
                            image.SetPixel(i, j, Color);
                        }
                        else if(zoomingBack || dontDrawTempImage)
                        {
                            sf::Color Color = CalcColor(changeGradient);
                            image.SetPixel(i, j, Color);
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
                {
                    // Set pixel.
                    image.SetPixel(i, j, fSetColor);
                }
                else
                {
                    if(colorMode)
                    {
                        // Color pixel.
                        if(colorMap[i][j] >= 0)
                        {
                            sf::Color Color = CalcColor(colorMap[i][j] + changeGradient);
                            image.SetPixel(i, j, Color);
                        }
                        else if(zoomingBack || dontDrawTempImage)
                        {
                            sf::Color Color = CalcColor(changeGradient);
                            image.SetPixel(i, j, Color);
                        }
                    }
                }
            }
        }
    }
    Window->Draw(output);
    if(!juliaMode) Window->Display();
}
void Fractal::Redraw()
{
    if(this->IsRendering())
    {
        dontDrawTempImage = true;
    }
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
        if(Event->Type == sf::Event::KeyPressed)
        {
            switch(Event->Key.Code)
            {
            case sf::Key::L:
                {
                    this->MoreIter();
                    break;
                }
            case sf::Key::K:
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
void Fractal::SpecialSaveRoutine(string filename)
{
    // Do nothing.
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
    if(paused) dontDrawTempImage = true;
    redrawAll = true;
    maxIter += 20;
}
void Fractal::LessIter()
{
    changeFractalIter = true;
    this->DeleteSavedZooms();
    if(paused) dontDrawTempImage = true;
    redrawAll = true;
    int signedMaxIter = (int)maxIter;
    if(signedMaxIter - 20 > 0)
    {
        maxIter -= 20;
    }
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
    else maxY = minY+(maxX-minX)*screenHeight/screenWidth;

    maxIter = opt.maxIter;
    panelOpt = opt.panelOpt;
    changeGradient = opt.changeGradient;
    colorPaletteMode = opt.colorPaletteMode;
    relativeColor = opt.relativeColor;
    gradPaletteSize = opt.gradPaletteSize;
    estPaletteSize = opt.estPaletteSize;
    alg = opt.alg;

    redInt = opt.redInt;
    greenInt = opt.greenInt;
    blueInt = opt.blueInt;

    redMed = opt.redMed;
    greenMed = opt.greenMed;
    blueMed = opt.blueMed;

    redDes = opt.redDes;
    greenDes = opt.greenDes;
    blueDes = opt.blueDes;
    fSetColor = opt.fSetColor;

    gradient = opt.gradient;
    if(colorPaletteMode == GRADIENT)
    {
        paletteSize = gradPaletteSize;
        this->SetGradientSize(paletteSize);
    }
    else
    {
        paletteSize = estPaletteSize;
        this->SetPaletteSize(paletteSize);
    }

    if(hasSmoothRender) smoothRender = opt.smoothRender;

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
    opt.estPaletteSize = estPaletteSize;
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

    opt.redMed = redMed;
    opt.greenMed = greenMed;
    opt.blueMed = blueMed;

    opt.redDes = redDes;
    opt.greenDes = greenDes;
    opt.blueDes = blueDes;
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
FRACTAL_TYPE Fractal::GetType()
{
    return type;
}
bool** Fractal::GetSetMap()
{
    return setMap;
}
bool Fractal::IsMoving()
{
    if(xVel == 0 && yVel == 0) return false;
    else return true;
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
void Fractal::SetMovement(DIRECTION dir)
{
    switch(dir)
    {
    case UP:
        {
            yVel -= stdSpeed;
            movement[UP] = true;
            break;
        }
    case DOWN:
        {
            yVel += stdSpeed;
            movement[DOWN] = true;
            break;
        }
    case LEFT:
        {
            xVel -= stdSpeed;
            movement[LEFT] = true;
            break;
        }
    case RIGHT:
        {
            xVel += stdSpeed;
            movement[RIGHT] = true;
            break;
        }
    default: break;
    }
}
void Fractal::ReleaseMovement(DIRECTION dir)
{
    switch(dir)
    {
    case UP:
        {
            movement[UP] = false;
            break;
        }
    case DOWN:
        {
            movement[DOWN] = false;
            break;
        }
    case LEFT:
        {
            movement[LEFT] = false;
            break;
        }
    case RIGHT:
        {
            movement[RIGHT] = false;
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

    image.Create(screenWidth, screenHeight, white);

    maxColorMapVal = 0;
    if(relativeColor)
    {
        // Search for a color maximum value.
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                if(colorMap[i][j] > maxColorMapVal) maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if(maxColorMapVal == 0) maxColorMapVal = 1;

    for(int i=0; i<screenWidth; i++)
    {
        for(int j=0; j<screenHeight; j++)
        {
            if(setMap[i][j] == true && colorSet)
            {
                // Set pixel.
                image.SetPixel(i, j, fSetColor);
            }
            else
            {
                if(colorMode)
                {
                    // Color pixel.
                    sf::Color Color;
                    if(relativeColor)
                    {
                        Color = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize+changeGradient);
                    }
                    else
                    {
                        Color = CalcColor(colorMap[i][j] + changeGradient);
                    }
                    image.SetPixel(i, j, Color);
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
                if(colorMap[i][j] > maxColorMapVal) maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if(maxColorMapVal == 0) maxColorMapVal = 1;

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
void Fractal::SetInt(int intensidad, COLOR col)
{
    // Changes intensity value.
    colorPaletteMode = EST_MODE;
    if(col == red)
    {
        redInt = intensidad;
    }
    else if(col == green)
    {
        greenInt = intensidad;
    }
    else if(col == blue)
    {
        blueInt = intensidad;
    }
    this->RebuildPalette();
}
void Fractal::SetMed(double med, COLOR col)
{
    // Changes mean value.
    colorPaletteMode = EST_MODE;
    if(col == red)
    {
        redMed = med;
    }
    else if(col == green)
    {
        greenMed = med;
    }
    else if(col == blue)
    {
        blueMed = med;
    }
    this->RebuildPalette();
}
void Fractal::SetDes(double des, COLOR col)
{
    // Changes standard deviation value.
    colorPaletteMode = EST_MODE;
    if(col == red)
    {
        redDes = des;
    }
    else if(col == green)
    {
        greenDes = des;
    }
    else if(col == blue)
    {
        blueDes = des;
    }
    this->RebuildPalette();
}
int Fractal::GetInt(COLOR col)
{
    if(col == red)
    {
        return redInt;
    }
    else if(col == green)
    {
        return greenInt;
    }
    else if(col == blue)
    {
        return blueInt;
    }
    else return 0;
}
double Fractal::GetMed(COLOR col)
{
    if(col == red)
    {
        return redMed;
    }
    else if(col == green)
    {
        return greenMed;
    }
    else if(col == blue)
    {
        return blueMed;
    }
    else return 0;
}
double Fractal::GetDes(COLOR col)
{
    if(col == red)
    {
        return redDes;
    }
    else if(col == green)
    {
        return greenDes;
    }
    else if(col == blue)
    {
        return blueDes;
    }
    else return 0;
}

// Color styles.
void Fractal::SetESTStyle(EST_STYLES _estStyle)
{
    // Changes color palette.
    estStyle = _estStyle;
    ESTFractalColor c;
    c.SetStyle(estStyle);

    // Sets parameters for new palette.
    redInt = c.redInt;
    greenInt = c.greenInt;
    blueInt = c.blueInt;

    redMed = c.redMed;
    greenMed = c.greenMed;
    blueMed = c.blueMed;

    redDes = c.redDes;
    greenDes = c.greenDes;
    blueDes = c.blueDes;

    this->SetPaletteSize(c.paletteSize);
    for(int i=0; i<paletteSize; i++)
    {
        redPalette[i] = CalcGradient(i, red);
        greenPalette[i] = CalcGradient(i, green);
        bluePalette[i] = CalcGradient(i, blue);
    }
    for(int i=0; i<paletteSize; i++)
    {
        palette[i] = sf::Color(redPalette[i], greenPalette[i], bluePalette[i]);
    }
}
EST_STYLES Fractal::GetESTSyle()
{
    return estStyle;
}
void Fractal::SetGradStyle(GRAD_STYLES _gradStyle)
{
    gradStyle = _gradStyle;
}
GRAD_STYLES Fractal::GetGradStyle()
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
    colorPaletteMode = GRADIENT;
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
    colorPaletteMode = GRADIENT;
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
                if(colorMap[i][j] > maxColorMapVal) maxColorMapVal = colorMap[i][j];
            }
        }
    }
    if(maxColorMapVal == 0) maxColorMapVal = 1;

    for(int i=0; i<screenWidth; i++)
    {
        for(int j=0; j<screenHeight; j++)
        {
            if(setMap[i][j] == true && colorSet)
            {
                // Set pixel.
                image.SetPixel(i, j, fSetColor);
            }
            else if(colorMode)
            {
                // Color pixel.
                sf::Color Color;
                if(relativeColor)
                    Color = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize + changeGradient);
                else
                    Color = CalcColor(colorMap[i][j] + changeGradient);
                image.SetPixel(i, j, Color);
            }
            else image.SetPixel(i, j, white);
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
sf::Uint8 Fractal::CalcGradient(int colorNum, COLOR col)
{
    colorNum = colorNum % paletteSize;
    // Return color with a normal distribution.
    if(col == red)
    {
        return static_cast<sf::Uint8>(redInt*NormalDist(colorNum, redMed, redDes) + redInt*NormalDist(colorNum, paletteSize+redMed, redDes));
    }
    if(col == green)
    {
        return static_cast<sf::Uint8>(greenInt*NormalDist(colorNum, greenMed, greenDes) + greenInt*NormalDist(colorNum, paletteSize+greenMed, greenDes));
    }
    if(col == blue)
    {
        return static_cast<sf::Uint8>(blueInt*NormalDist(colorNum, blueMed, blueDes)+blueInt*NormalDist(colorNum, paletteSize+blueMed, blueDes));
    }
    return 0;
}
sf::Color Fractal::CalcColor(int colorNum)
{
    if(colorNum <= 0) colorNum = 0;
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

    if(colorPaletteMode == GRADIENT)
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
            redPalette[i] = CalcGradient(i, red);
            greenPalette[i] = CalcGradient(i, green);
            bluePalette[i] = CalcGradient(i, blue);
        }
    }

    for(int i=0; i<paletteSize; i++)
    {
        palette[i] = sf::Color(redPalette[i], greenPalette[i], bluePalette[i]);
    }

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
                    if(colorMap[i][j] > maxColorMapVal) maxColorMapVal = colorMap[i][j];
                }
            }
            if(maxColorMapVal == 0) maxColorMapVal = 1;

            for(int i=0; i<screenWidth; i++)
            {
                for(int j=0; j<screenHeight; j++)
                {
                    if(setMap[i][j] == false || !colorSet)
                    {
                        sf::Color Color = CalcColor(((double)colorMap[i][j]/(double)maxColorMapVal)*paletteSize + changeGradient);
                        image.SetPixel(i, j, Color);
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
                        image.SetPixel(i, j, Color);
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
    estPaletteSize = paletteSize = size;
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
void Fractal::SetPaletteMode(COLOR_MODE mode)
{
    colorPaletteMode = mode;
    delete[] redPalette;
    delete[] greenPalette;
    delete[] bluePalette;
    delete[] palette;
    if(mode == GRADIENT)
    {
        paletteSize = gradient.getMax()-gradient.getMin();
    }
    else
    {
        paletteSize = estPaletteSize;
    }
    redPalette = new sf::Uint8[paletteSize];
    greenPalette = new sf::Uint8[paletteSize];
    bluePalette = new sf::Uint8[paletteSize];
    palette = new sf::Color[paletteSize];
    varGradientStep = paletteSize/60;
    this->RebuildPalette();
}
COLOR_MODE Fractal::GetColorMode()
{
    return colorPaletteMode;
}

// Algorithm.
ALGORITHM Fractal::GetCurrentAlg()
{
    return alg;
}
vector<ALGORITHM> Fractal::GetAvailableAlg()
{
    return availableAlg;
}
void Fractal::SetAlgorithm(ALGORITHM _alg)
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
        Window->Draw(line);
    }

    // Draw orbit lines.
    for(unsigned int i=0; i<orbitLines.size(); i++)
    {
        float x1 = (orbitLines[i].x1-minX)/xFactor;
        float y1 = (maxY-orbitLines[i].y1)/yFactor;
        float x2 = (orbitLines[i].x2-minX)/xFactor;
        float y2 = (maxY-orbitLines[i].y2)/yFactor;
        sf::Shape line = sf::Shape::Line(x1, y1, x2, y2, 2, orbitLines[i].color);
        Window->Draw(line);
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
        Window->Draw(circle);
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
        if(paused) dontDrawTempImage = true;
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
        if(paused) dontDrawTempImage = true;
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
    if(panelOpt.GetElementsSize() > 0) return true;
    else return false;
}
PanelOptions* Fractal::GetOptPanel()
{
    return &panelOpt;
}

// Command console methods.
wxString Fractal::AskInfo()
{
    // Virtual
    return wxString();
}
wxString Fractal::AskInfo(double real, double imag, int iter)
{
    // Virtual
    return wxString();
}
wxString Fractal::Command(wxString commandText)
{
    wxString error1(wxT("Error: Invalid number of arguments"));
    wxString error2(wxT("Error: Can't recognize function"));
    wxString error3(wxT("Error: Incorrect syntax"));

    // Separates function and arguments.
    int beginParenthesis = -1, endParenthesis = -1;
    wxString function, args;
    vector<wxString> argVector;
    for(unsigned int i=0; i<commandText.length(); i++)
    {
        if(commandText[i] == '(' && beginParenthesis == -1) beginParenthesis = i;
        if(commandText[i] == ')') endParenthesis = i;
    }
    function = commandText.substr(0,beginParenthesis);
    args = commandText.substr(beginParenthesis+1,endParenthesis-(beginParenthesis+1));

    if(beginParenthesis == -1 || endParenthesis == -1)
        return error3;

    // Creates vector with argument list.
    int init = 0;
    wxString sub;
    for(unsigned int i=0; i<args.length(); i++)
    {
        if(args[i] == ',')
        {
            sub = args.substr(init, i-init);
            for(unsigned int j=0; j<sub.length(); j++)
            {
                // Erase white spaces.
                if(sub[j] == ' ') sub.erase(j, 1);
            }
            argVector.push_back(sub);
            init = i+1;
        }
        if(i == args.length()-1)
        {
            sub = args.substr(init, i+1-init);
            for(unsigned int j=0; j<sub.length(); j++)
            {
                if(sub[j] == ' ') sub.erase(j, 1);
            }
            argVector.push_back(sub);
        }
    }


    if(function == wxT("AskInfo") || function == wxT("askinfo"))
    {
        if(argVector.size() == 3)
        {
            wxString output(this->AskInfo(string_to_double(argVector[0]),
                            string_to_double(argVector[1]), string_to_int(argVector[2])));
            return output;
        }
        else return error1;
    }
    if(function == wxT("SetBoundaries") || function == wxT("setboundaries"))
    {
        if(argVector.size() == 4)
        {
            return this->SetBoundaries(string_to_double(argVector[0]), string_to_double(argVector[1]),
                                        string_to_double(argVector[2]), string_to_double(argVector[3]));
        }
        else return error1;
    }
    if(function == wxT("Redraw") || function == wxT("redraw"))
    {
        if(argVector.size() == 0)
        {
            return this->CommandRedraw();
        }
    }
    if(function == wxT("SaveOrbit") || function == wxT("saveorbit"))
    {
        if(argVector.size() == 4)
        {
            return this->SaveOrbit(string_to_double(argVector[0]), string_to_double(argVector[1]),
                                    string_to_int(argVector[2]), argVector[3]);
        }
        else return error1;
    }
    if(function == wxT("SetThreadNumber") || function == wxT("setthreadnumber"))
    {
        if(argVector.size() == 1)
        {
            return this->SetThreadNumber(string_to_int(argVector[0]));
        }
        else return error1;
    }
    if(function == wxT("GetThreadNumber") || function == wxT("getthreadnumber"))
    {
        if(argVector.size() == 0)
        {
            return this->GetThreadNumber();
        }
    }
    if(function == wxT("DrawCircle") || function == wxT("drawcircle"))
    {
        if(argVector.size() == 3)
        {
            this->DrawCircle(string_to_double(argVector[0]), string_to_double(argVector[1]),
                                string_to_double(argVector[2]), sf::Color(0,0,0));
            return wxString(wxT("Done"));
        }
        else return error1;
    }
    if(function == wxT("DrawLine") || function == wxT("drawline"))
    {
        if(argVector.size() == 4)
        {
            this->DrawLine(string_to_double(argVector[0]), string_to_double(argVector[1]),
                                string_to_double(argVector[2]),string_to_double(argVector[3]) , sf::Color(0,0,0));
            return wxString(wxT("Done"));
        }
        else return error1;
    }
    if(function == wxT("DeleteFigures") || function == wxT("deletefigures"))
    {
        if(argVector.size() == 0)
        {
            if(!orbitMode) geomFigure = false;
            circles.clear();
            lines.clear();
            return wxString(wxT("Done"));
        }
        else return error1;
    }
    if(function == wxT("Abort") || function == wxT("abort"))
    {
        if(argVector.size() == 0)
        {
            if(this->StopRender())
            {
                rendered = true;
                return wxString(wxT("Done"));
            }
            else return wxString(wxT("Nothing to abort"));
        }
    }

    // If couldn't find a match..
    return error2;
}
wxString Fractal::SetBoundaries(double iMinRe, double iMaxRe, double iMinIm, double iMaxIm)
{
    this->SaveZoom();
    minX = iMinRe;
    maxX = iMaxRe;
    minY = iMinIm;
    maxY = iMaxIm;

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    rendered = false;
    magnification = 3/(maxX-minX);
    return wxT("Done");
}
wxString Fractal::CommandRedraw()
{
    this->Redraw();
    return wxT("Done");
}
wxString Fractal::SaveOrbit(double real, double imag, int iter, wxString filepath)
{
    return wxT("Error: Command unavailable in this fractal");
}
wxString Fractal::SetThreadNumber( int num )
{
    threadNumber = num;
    this->StopRender();
    watchdog.SetThreadNumber(threadNumber);
    this->ChangeThreadNumber();
    return wxT("Done");
}
wxString Fractal::GetThreadNumber()
{
    wxString out = wxT("This fractal is using ");
    out += num_to_string((int)threadNumber);
    if(threadNumber > 1)
        out += wxT(" threads.");
    else
        out += wxT(" thread.");
    return out;
}


///////////////////
//  ENDS FRACTAL //
///////////////////

// RenderFractal
RenderFractal::RenderFractal()
{
    specialRenderMode = false;
    stopped = false;
    auxMap = NULL;
    threadProgress = 0;
    x = 0;
    y = 0;
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
    if(type != SCRIPT_FRACTAL)
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
Coord RenderFractal::GetCoords()
{
    Coord pos;
    pos.x = 0;
    pos.y = ho;
    return pos;
}
Coord RenderFractal::GetStartPoints()
{
    Coord pos;
    pos.x = wo;
    pos.y = ho;
    return pos;
}
Coord RenderFractal::GetEndPoints()
{
    Coord pos;
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