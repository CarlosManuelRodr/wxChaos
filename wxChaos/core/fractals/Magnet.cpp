#include <complex>
#include "Magnet.h"
using namespace std;

Magnet::Magnet(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.8;
    _maxX = 4.4;
    _minY = -2.2;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::Magnetic;
    _hasHighPrecisionRender = true;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;

    myRender = new MagnetRenderer[_threadNumber];

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);
}
Magnet::~Magnet()
{
    this->StopRender();
    delete[] myRender;
}
void Magnet::Render()
{
    this->SetRendererBounds<MagnetRenderer>(myRender);
}
void Magnet::DrawOrbit()
{
    complex<double> z(0, 0);
    complex<double> constant(_orbitX, _orbitY);
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
        z = pow((pow(z, 2) + constant - complex<double>(1, 0))/(complex<double>(2, 0)*z + constant - complex<double>(2,0)), 2);
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
