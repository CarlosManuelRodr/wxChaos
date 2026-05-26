#include <complex>
#include "BurningShip.h"
using namespace std;

BurningShip::BurningShip(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    _minX = -2.36;
    _maxX = 1.79;
    _minY = -1.94;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _hasOrbit = true;
    _type = FractalType::BurningShip;
    myRender = new RenderBurningShip[_threadNumber];
    SetWatchdog<RenderBurningShip>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithm::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
BurningShip::BurningShip(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -2.36;
    _maxX = 1.79;
    _minY = -1.94;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _algorithm = RenderingAlgorithm::EscapeTime;
    _hasOrbit = true;
    _type = FractalType::BurningShip;
    myRender = new RenderBurningShip[_threadNumber];
    SetWatchdog<RenderBurningShip>(myRender, &_watchdog, _threadNumber);
}
BurningShip::~BurningShip()
{
    this->StopRender();
    delete[] myRender;
}
void BurningShip::Render()
{
    this->TRender<RenderBurningShip>(myRender);
}
void BurningShip::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> c = z;
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
        z = pow( complex< double >(abs(z.real()), abs(z.imag())), 2 ) + c;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}

