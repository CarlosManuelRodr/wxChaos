#include <complex>
#include "Fractory.h"
using namespace std;

Fractory::Fractory(const int width, const int height) : Fractal(width, height)
{
    _minX = 0.837154;
    _maxX = 1.14419;
    _minY = -0.102209;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _hasOrbit = true;
    _type = FractalType::Fractory;
    myRender = new FractoryRenderer[_threadNumber];
    SetWatchdog<FractoryRenderer>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
}
Fractory::~Fractory()
{
    this->StopRender();
    delete[] myRender;
}
void Fractory::Render()
{
    this->SetRendererBounds<FractoryRenderer>(myRender);
}
void Fractory::DrawOrbit()
{
    const complex<double> c(_orbitX, _orbitY);
    complex<double> z = c;
    complex<double> b = c - sin(c);

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
        b = c + b/c - z;
        z = z*c + b/z;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
