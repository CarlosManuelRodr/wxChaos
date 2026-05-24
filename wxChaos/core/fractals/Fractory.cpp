#include <complex>
#include "Fractory.h"
using namespace std;

Fractory::Fractory(sf::RenderWindow* Window) : Fractal(Window)
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
    myRender = new RenderFractory[_threadNumber];
    SetWatchdog<RenderFractory>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _alg = RenderingAlgorithm::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
Fractory::Fractory(int width, int height) : Fractal(width, height)
{
    _minX = 0.837154;
    _maxX = 1.14419;
    _minY = -0.102209;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _alg = RenderingAlgorithm::EscapeTime;
    _hasOrbit = true;

    _type = FractalType::Fractory;
    myRender = new RenderFractory[_threadNumber];
    SetWatchdog<RenderFractory>(myRender, &_watchdog, _threadNumber);
}
Fractory::~Fractory()
{
    this->StopRender();
    delete[] myRender;
}
void Fractory::Render()
{
    this->TRender<RenderFractory>(myRender);
}
void Fractory::DrawOrbit()
{
    complex<double> c(_orbitX, _orbitY);
    complex<double> b, z;
    z = c;
    b = c - sin(c);

    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned n=0; n<_maxIter; n++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > 4)
        {
            outOfSet = true;
            break;
        }
        b = c + b/c - z;
        z = z*c + b/z;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}

