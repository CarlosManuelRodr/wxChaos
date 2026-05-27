#include <complex>
#include "Tricorn.h"
using namespace std;

Tricorn::Tricorn(const sf::RenderWindow* window) : Fractal(window)
{
    _minX = -3;
    _maxX = 3;
    _minY = -1.94;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _hasOrbit = true;
    _type = FractalType::Tricorn;
    myRender = new RenderTricorn[_threadNumber];
    SetWatchdog<RenderTricorn>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithm::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithm::GaussianInt);
}
Tricorn::Tricorn(const int width, const int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -2.5;
    _maxX = 2.5;
    _minY = -2.5;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _hasOrbit = true;
    _algorithm = RenderingAlgorithm::EscapeTime;

    _type = FractalType::Tricorn;
    myRender = new RenderTricorn[_threadNumber];
    SetWatchdog<RenderTricorn>(myRender, &_watchdog, _threadNumber);
}
Tricorn::~Tricorn()
{
    this->StopRender();
    delete[] myRender;
}
void Tricorn::Render()
{
    this->TRender<RenderTricorn>(myRender);
}
void Tricorn::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> c = z;
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

        z = pow(conj(z), 2) + c;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
    {
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);
    }

    _orbitDrawn = true;
}

