#include <complex>
#include "Manowar.h"
using namespace std;

Manowar::Manowar(const sf::RenderWindow* window) : Fractal(window)
{
    // Adjust the scale.
    _minX = -0.4795;
    _maxX = 0.26108;
    _minY = -0.2375;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::Manowar;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    myRender = new ManowarRenderer[_threadNumber];
    SetWatchdog<ManowarRenderer>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
}
Manowar::Manowar(const int width, const int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -0.20;
    _maxX = 0.11;
    _minY = -0.15;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _algorithm = RenderingAlgorithmType::EscapeTime;

    _type = FractalType::Manowar;
    myRender = new ManowarRenderer[_threadNumber];
    SetWatchdog<ManowarRenderer>(myRender, &_watchdog, _threadNumber);
}
Manowar::~Manowar()
{
    this->StopRender();
    delete[] myRender;
}
void Manowar::Render()
{
    this->SetRendererBounds<ManowarRenderer>(myRender);
}
void Manowar::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    const complex<double> constant(_orbitX, _orbitY);
    complex<double> man = z;
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
        const complex<double> temp = z;
        z = pow(z, 2) + man + constant;
        man = temp;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}

