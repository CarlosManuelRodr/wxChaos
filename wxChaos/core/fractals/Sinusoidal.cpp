#include <complex>
#include "Sinusoidal.h"
using namespace std;

Sinusoidal::Sinusoidal(const unsigned int width, const unsigned int height) : RasterFractal(width, height)
{
    // Adjust the scale.
    _minX = -8;
    _maxX = 4;
    _minY = -4.12;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::Sinusoidal;
    _hasHighPrecisionRender = true;
    _kReal = 1;
    _kImaginary = 0.25;
    _hasOrbit = true;
    _juliaVariety = true;

    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = true;

    myRender = new SinusoidalRenderer[_threadNumber];

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);
}
Sinusoidal::~Sinusoidal()
{
    this->StopRender();
    delete[] myRender;
}
void Sinusoidal::Render()
{
    this->SetRendererBounds<SinusoidalRenderer>(myRender);
}
void Sinusoidal::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    const complex<double> k(_kReal, _kImaginary);
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned n=0; n<_maxIterations; n++)
    {
        zVector.push_back(z);
        if (z.real()*z.real() + z.imag()*z.imag() > _maxIterations)
        {
            outOfSet = true;
            break;
        }
        z = k*sin(z);
    }
    sf::Color color;
    if (outOfSet)
        color = sf::Color(255, 0, 0);
    else
        color = sf::Color(0, 255, 0);

    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
