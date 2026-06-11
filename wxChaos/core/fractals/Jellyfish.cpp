#include <complex>
#include "Jellyfish.h"
using namespace std;

Jellyfish::Jellyfish(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    _minX = -1.1342;
    _maxX = 1.7251;
    _minY = -0.90215;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::Medusa;
    _kReal = -0.2;
    _kImaginary = 0;
    _juliaVariety = true;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = true;

    myRender = new JellyfishRenderer[_threadNumber];
    SetWatchdog<JellyfishRenderer>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);
}
Jellyfish::~Jellyfish()
{
    this->StopRender();
    delete[] myRender;
}
void Jellyfish::Render()
{
    this->SetRendererBounds<JellyfishRenderer>(myRender);
}
void Jellyfish::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> k(_kReal, _kImaginary);
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
        z = pow(z, 1.5) + k;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
    {
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);
    }
    _orbitDrawn = true;
}
