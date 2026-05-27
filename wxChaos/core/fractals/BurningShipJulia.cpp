#include <complex>
#include "BurningShipJulia.h"
using namespace std;

BurningShipJulia::BurningShipJulia(const sf::RenderWindow* window) : Fractal(window)
{
    _minX = -2.77051;
    _maxX = 2.77682;
    _minY = -1.75939;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    _juliaVariety = true;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _hasOrbit = true;
    _type = FractalType::BurningShip;
    myRender = new RenderBurningShipJulia[_threadNumber];
    SetWatchdog<RenderBurningShipJulia>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithm::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
BurningShipJulia::BurningShipJulia(int width, int height) : Fractal(width, height)
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

    _algorithm = RenderingAlgorithm::EscapeTime;
    _type = FractalType::BurningShip;
    myRender = new RenderBurningShipJulia[_threadNumber];
    SetWatchdog<RenderBurningShipJulia>(myRender, &_watchdog, _threadNumber);
}
BurningShipJulia::~BurningShipJulia()
{
    this->StopRender();
    delete[] myRender;
}
void BurningShipJulia::Render()
{
    this->TRender<RenderBurningShipJulia>(myRender);
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

