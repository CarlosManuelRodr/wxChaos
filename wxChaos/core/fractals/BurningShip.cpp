#include <complex>
#include "BurningShip.h"
using namespace std;

BurningShip::BurningShip(unsigned int width, unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -2.36;
    _maxX = 1.79;
    _minY = -1.94;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _algorithm = RenderingAlgorithmType::EscapeTime;
    _hasOrbit = true;
    _type = FractalType::BurningShip;
    myRender = new BurningShipRenderer[_threadNumber];
    SetWatchdog<BurningShipRenderer>(myRender, &_watchdog, _threadNumber);
}
BurningShip::~BurningShip()
{
    this->StopRender();
    delete[] myRender;
}
void BurningShip::Render()
{
    this->SetRendererBounds<BurningShipRenderer>(myRender);
}
void BurningShip::DrawOrbit()
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
        z = pow( complex< double >(abs(z.real()), abs(z.imag())), 2 ) + c;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
