#include <complex>
#include "BurningShip.h"
using namespace std;

BurningShip::BurningShip(const unsigned int width, const unsigned int height) : RasterFractal(width, height)
{
    // Adjust the scale.
    _minX = -2.36;
    _maxX = 1.79;
    _minY = -1.94;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _algorithm = RenderingAlgorithmType::EscapeTime;
    _type = FractalType::BurningShip;
    _hasHighPrecisionRender = true;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = true;

    myRender = new BurningShipRenderer[_threadNumber];

    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);
}
BurningShip::~BurningShip()
{
    this->RasterFractal::StopRender();
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

    for (unsigned n=0; n<_maxIterations; n++)
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
