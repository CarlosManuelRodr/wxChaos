#include <complex>
#include "ManowarJulia.h"
using namespace std;

ManowarJulia::ManowarJulia(sf::RenderWindow* window) : Fractal(window)
{
    _minX = -2.0;
    _maxX = 1.0;
    _minY = -1.2;
    _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
    this->SetOutermostZoom();

    _kReal = 0.0272873;
    _kImaginary = -0.0432547;
    _type = FractalType::ManowarJulia;
    _juliaVariety = true;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    myRender = new RenderManowarJulia[_threadNumber];
    SetWatchdog<RenderManowarJulia>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _alg = RenderingAlgorithm::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
ManowarJulia::ManowarJulia(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -2.0;
    _maxX = 1.0;
    _minY = -1.2;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);
    this->SetOutermostZoom();

    _kReal = 0.0272873;
    _kImaginary = -0.0432547;

    _juliaVariety = true;
    _alg = RenderingAlgorithm::EscapeTime;

    _type = FractalType::ManowarJulia;
    myRender = new RenderManowarJulia[_threadNumber];
    SetWatchdog<RenderManowarJulia>(myRender, &_watchdog, _threadNumber);
}
ManowarJulia::~ManowarJulia()
{
    this->StopRender();
    delete[] myRender;
}
void ManowarJulia::Render()
{
    this->TRender<RenderManowarJulia>(myRender);
}
void ManowarJulia::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    const complex<double> k(_kReal, _kImaginary);
    complex<double> man = z;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned n=0; n<_maxIter; n++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > 4)
        {
            outOfSet = true;
            break;
        }
        const complex<double> temp = z;
        z = pow(z, 2) + man + k;
        man = temp;
    }
    sf::Color color;
    if (outOfSet)
        color = sf::Color(255, 0, 0);
    else
        color = sf::Color(0, 255, 0);

    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}

