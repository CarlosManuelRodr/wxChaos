#include <complex>
#include "FixedPoint3.h"
using namespace std;

FixedPoint3::FixedPoint3(const sf::RenderWindow* window) : Fractal(window)
{
    // Adjust the scale.
    _minX = -3.76339;
    _maxX = 3.59018;
    _minY = -2.39204;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);
    _hasOrbit = true;

    _type = FractalType::FixedPoint3;
    myRender = new RenderFixedPoint3[_threadNumber];
    SetWatchdog<RenderFixedPoint3>(myRender, &_watchdog, _threadNumber);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Min step: "), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    _algorithm = RenderingAlgorithm::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FixedPoint3::FixedPoint3(const int width, const int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -3.76339;
    _maxX = 3.59018;
    _minY = -2.39204;
    _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
    this->SetOutermostZoom();

    minStep = 0.001;

    _type = FractalType::FixedPoint3;
    myRender = new RenderFixedPoint3[_threadNumber];
    SetWatchdog<RenderFixedPoint3>(myRender, &_watchdog, _threadNumber);
}
FixedPoint3::~FixedPoint3()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint3::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(minStep);
    this->TRender<RenderFixedPoint3>(myRender);
}
void FixedPoint3::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> z_ant;
    vector< complex<double> > zVector;

    for (unsigned n=0; n<_maxIter; n++)
    {
        zVector.push_back(z);
        z = tan(z);

        if ((z_ant.real() - minStep < z.real() && z_ant.real() + minStep > z.real())
            && (z_ant.imag() - minStep < z.imag() && z_ant.imag() + minStep > z.imag()))
        {
            break;
        }
        z_ant = z;
    }

    const auto color = sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
    {
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);
    }
    _orbitDrawn = true;
}
void FixedPoint3::CopyOptFromPanel()
{
    minStep = *_panelOpt.GetDoubleElement(0);
}

