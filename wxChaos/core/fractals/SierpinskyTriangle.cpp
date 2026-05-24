#include "SierpinskyTriangle.h"
using namespace std;

SierpinskyTriangle::SierpinskyTriangle(sf::RenderWindow* Window) : Fractal(Window)
{
    _minX = -0.5;
    _maxX = 1.5;
    _minY = -0.2;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    myRender = new RenderSierpinskyTriangle[_threadNumber];
    SetWatchdog<RenderSierpinskyTriangle>(myRender, &_watchdog, _threadNumber);

    _type = FractalType::SierpinskyTriangle;
}
SierpinskyTriangle::SierpinskyTriangle(int width, int height) : Fractal(width, height)
{
    _minX = -0.5;
    _maxX = 1.5;
    _minY = -0.2;
    _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
    this->SetOutermostZoom();

    _renderJobComp = false;
    _type = FractalType::SierpinskyTriangle;
    myRender = new RenderSierpinskyTriangle[_threadNumber];
    SetWatchdog<RenderSierpinskyTriangle>(myRender, &_watchdog, _threadNumber);
}
void SierpinskyTriangle::Render()
{
    this->TRender<RenderSierpinskyTriangle>(myRender);
}

