#include <complex>
#include "BurningShipJulia.h"
using namespace std;

BurningShipJulia::BurningShipJulia(unsigned int width, unsigned int height) : Fractal(width, height)
{
    _minX = -2.77051;
    _maxX = 2.77682;
    _minY = -1.75939;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();
    _juliaVariety = true;
    _hasOrbit = true;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _algorithm = RenderingAlgorithmType::EscapeTime;
    _type = FractalType::BurningShip;
    myRender = new BurningShipJuliaRenderer[_threadNumber];
    SetWatchdog<BurningShipJuliaRenderer>(myRender, &_watchdog, _threadNumber);
}
BurningShipJulia::~BurningShipJulia()
{
    this->StopRender();
    delete[] myRender;
}
void BurningShipJulia::Render()
{
    this->SetRendererBounds<BurningShipJuliaRenderer>(myRender);
}
void BurningShipJulia::DrawOrbit()
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
        z = pow( complex< double >(abs(z.real()), abs(z.imag())), 2 ) + k;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
