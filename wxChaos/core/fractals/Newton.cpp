#include <complex>
#include "Newton.h"
using namespace std;

Newton::Newton(const int width, const int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.5721;
    _maxX = 1.4086;
    _minY = -1;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::NewtonRaphsonMethod;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    myRender = new NewtonRenderer[_threadNumber];
    SetWatchdog<NewtonRenderer>(myRender, &_watchdog, _threadNumber);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Min step: "), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithmType::ConvergenceTest);
}
Newton::~Newton()
{
    this->StopRender();
    delete[] myRender;
}
void Newton::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);

    if (_orbitX != 0 && _orbitY != 0)
    {
        for (unsigned n=0; n<_maxIter; n++)
        {
            complex<double> previous = z;
            z = z - (pow(z, 3) - complex<double>(1, 0))/(complex<double>(2, 0)*pow(z,2));

            this->DrawLine(previous.real(), previous.imag(), z.real(), z.imag(), sf::Color(0,255,0), true);

            if ((previous.real() - minStep < z.real() && previous.real() + minStep > z.real())
                && (previous.imag() - minStep < z.imag() && previous.imag() + minStep > z.imag()))
                break;
        }
    }

    _orbitDrawn = true;
}
void Newton::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(minStep);

    this->SetRendererBounds<NewtonRenderer>(myRender);
}
void Newton::CopyOptFromPanel()
{
    minStep = *_panelOpt.GetDoubleElement(0);
}
