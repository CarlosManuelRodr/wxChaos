#include <complex>
#include "Julia.h"
using namespace std;

Julia::Julia(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.77437;
    _maxX = 1.6912;
    _minY = -1.06769;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::Julia;
    _kReal = -0.754696;
    _kImaginary = -0.0524231;
    _hasOrbit = true;
    _juliaVariety = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = true;
    myRender = new JuliaRenderer[_threadNumber];
    SetWatchdog<JuliaRenderer>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);
}
Julia::~Julia()
{
    this->StopRender();
    delete[] myRender;
}
void Julia::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    const complex<double> k(_kReal, _kImaginary);
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned n=0; n<_maxIter; n++)
    {
        zVector.push_back(z);
        if (z.real()*z.real() + z.imag()*z.imag() > 4)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, 2) + k;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
void Julia::Render()
{
    this->SetRendererBounds<JuliaRenderer>(myRender);
}
