#include <complex>
#include "FixedPoint1.h"
using namespace std;

FixedPoint1::FixedPoint1(const sf::RenderWindow* window) : Fractal(window)
{
    // Adjust the scale.
    _minX = -6.15385;
    _maxX = 5.84615;
    _minY = -4;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);
    _hasOrbit = true;

    _type = FractalType::FixedPoint1;
    myRender = new FixedPoint1Renderer[_threadNumber];
    SetWatchdog<FixedPoint1Renderer>(myRender, &_watchdog, _threadNumber);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Min step: "), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithmType::ConvergenceTest);
}
FixedPoint1::FixedPoint1(const int width, const int height) : Fractal(width, height)
{
    minStep = 0.01;
    _minX = -6.15385;
    _maxX = 5.84615;
    _minY = -4;
    _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
    this->SetOutermostZoom();

    minStep = 0.001;

    _type = FractalType::FixedPoint1;
    myRender = new FixedPoint1Renderer[_threadNumber];
    SetWatchdog<FixedPoint1Renderer>(myRender, &_watchdog, _threadNumber);
}
FixedPoint1::~FixedPoint1()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint1::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(minStep);

    this->SetRendererBounds<FixedPoint1Renderer>(myRender);
}
void FixedPoint1::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> z_prev;
    vector< complex<double> > zVector;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        zVector.push_back(z);
        z = sin(z);

        if ((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
            && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
            break;

        z_prev = z;
    }

    const auto color = sf::Color(0, 255, 0);
    for (unsigned int i = 0; i < zVector.size() - 1; i++)
    {
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i + 1].real(), zVector[i + 1].imag(), color, true);
    }
    _orbitDrawn = true;
}
void FixedPoint1::CopyOptFromPanel()
{
    minStep = *_panelOpt.GetDoubleElement(0);
}

