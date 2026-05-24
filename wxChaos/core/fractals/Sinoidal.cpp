#include <complex>
#include "Sinoidal.h"
using namespace std;

Sinoidal::Sinoidal(sf::RenderWindow* window) : Fractal(window)
{
    // Adjust the scale.
    _minX = -8;
    _maxX = 4;
    _minY = -4.12;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::Sinoidal;
    _kReal = 1;
    _kImaginary = 0.25;
    _hasOrbit = true;
    _juliaVariety = true;

    _hasOrbitTrap = true;

    myRender = new RenderSinoidal[_threadNumber];
    SetWatchdog<RenderSinoidal>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _alg = RenderingAlgorithm::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
Sinoidal::Sinoidal(const int width, const int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -6;
    _maxX = 4;
    _minY = -4.5;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _alg = RenderingAlgorithm::EscapeTime;
    _kReal = 1;
    _kImaginary = 0.25;
    _juliaVariety = true;

    _type = FractalType::Sinoidal;
    myRender = new RenderSinoidal[_threadNumber];
    SetWatchdog<RenderSinoidal>(myRender, &_watchdog, _threadNumber);
}
Sinoidal::~Sinoidal()
{
    this->StopRender();
    delete[] myRender;
}
void Sinoidal::Render()
{
    this->TRender<RenderSinoidal>(myRender);
}
void Sinoidal::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    const complex<double> k(_kReal, _kImaginary);
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned n=0; n<_maxIter; n++)
    {
        zVector.push_back(z);
        if (z.real()*z.real() + z.imag()*z.imag() > _maxIter)
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

