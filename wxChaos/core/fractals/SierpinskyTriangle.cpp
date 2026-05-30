#include "SierpinskyTriangle.h"
using namespace std;

SierpinskyTriangle::SierpinskyTriangle(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    _minX = -0.5;
    _maxX = 1.5;
    _minY = -0.2;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    myRender = new SierpinskyTriangleRenderer[_threadNumber];
    SetWatchdog<SierpinskyTriangleRenderer>(myRender, &_watchdog, _threadNumber);

    _type = FractalType::SierpinskyTriangle;
}
void SierpinskyTriangle::Render()
{
    this->SetRendererBounds<SierpinskyTriangleRenderer>(myRender);
}
