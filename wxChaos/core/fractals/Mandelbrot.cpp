#include <complex>
#include "Mandelbrot.h"
using namespace std;

Mandelbrot::Mandelbrot(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -2.45296;
    _maxX = 1.1624;
    _minY = -1.169;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::Mandelbrot;
    _hasHighPrecisionRender = true;
    _hasFastPrecisionRender = true;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = true;
    myRender = new MandelbrotRenderer[_threadNumber];

    // Creates panel.
    _panelOpt.SetForceShow(false);
    _panelOpt.LinkInteger(PanelOptionType::Spin, "Number of buddhabrot\nrandom points:", &buddhaRandomP, "1000000");
    buddhaRandomP = 1000000;

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);
    _availableAlg.push_back(RenderingAlgorithmType::Buddhabrot);
}
Mandelbrot::~Mandelbrot()
{
    this->StopRender();
    delete[] myRender;
}
void Mandelbrot::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetBuddhaRandomP(buddhaRandomP);

    this->SetRendererBounds<MandelbrotRenderer>(myRender);
}
void Mandelbrot::DrawOrbit()
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
        z = pow(z, 2) + c;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
void Mandelbrot::CopyOptionFromPanel()
{
    buddhaRandomP = *_panelOpt.GetIntegerValue(0);
}
void Mandelbrot::PreRender()
{
}
void Mandelbrot::PreDrawMaps()
{
}
