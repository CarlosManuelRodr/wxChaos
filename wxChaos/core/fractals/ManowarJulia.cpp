#include <complex>
#include "ManowarJulia.h"
using namespace std;

ManowarJulia::ManowarJulia(const unsigned int width, const unsigned int height) : RasterFractal(width, height)
{
    _minX = -2.0;
    _maxX = 1.0;
    _minY = -1.2;
    _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;

    _kReal = 0.0272873;
    _kImaginary = -0.0432547;
    _type = FractalType::ManowarJulia;
    _hasHighPrecisionRender = true;
    _juliaVariety = true;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = true;

    myRender = new ManowarJuliaRenderer[_threadNumber];

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);
}
ManowarJulia::~ManowarJulia()
{
    this->StopRender();
    delete[] myRender;
}
void ManowarJulia::Render()
{
    this->SetRendererBounds<ManowarJuliaRenderer>(myRender);
}
void ManowarJulia::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    const complex<double> k(_kReal, _kImaginary);
    complex<double> man = z;
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
        const complex<double> temp = z;
        z = pow(z, 2) + man + k;
        man = temp;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
