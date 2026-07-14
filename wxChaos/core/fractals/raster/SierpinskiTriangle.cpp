#include "SierpinskiTriangle.h"
using namespace std;

SierpinskiTriangle::SierpinskiTriangle(const unsigned int width, const unsigned int height) : RasterFractal(width, height)
{
    _minX = -0.5;
    _maxX = 1.5;
    _minY = -0.2;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    myRender = new SierpinskiTriangleRenderer[_threadNumber];

    _type = FractalType::SierpinskiTriangle;
    _hasHighPrecisionRender = true;
}
void SierpinskiTriangle::Render()
{
    this->SetRendererBounds<SierpinskiTriangleRenderer>(myRender);
}
