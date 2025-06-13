#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
SierpTriangle::SierpTriangle(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = -0.5;
    maxX = 1.5;
    minY = -0.2;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    myRender = new RenderSierpTriangle[threadNumber];
    SetWatchdog<RenderSierpTriangle>(myRender, &watchdog, threadNumber);

    type = FractalType::SierpinskyTriangle;
}
SierpTriangle::SierpTriangle(int width, int height) : Fractal(width, height)
{
    minX = -0.5;
    maxX = 1.5;
    minY = -0.2;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    renderJobComp = false;
    type = FractalType::SierpinskyTriangle;
    myRender = new RenderSierpTriangle[threadNumber];
    SetWatchdog<RenderSierpTriangle>(myRender, &watchdog, threadNumber);
}
void SierpTriangle::Render()
{
    this->TRender<RenderSierpTriangle>(myRender);
}

