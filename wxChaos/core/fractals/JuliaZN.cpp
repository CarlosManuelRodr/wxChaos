#include <complex>
#include "JuliaZN.h"
using namespace std;

JuliaZN::JuliaZN(const sf::RenderWindow* window) : Fractal(window)
{
    // Adjust the scale.
    _minX = -1.77437;
    _maxX = 1.6912;
    _minY = -1.06769;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::JuliaZN;
    _kReal = -0.754696;
    _kImaginary = -0.0524231;
    _hasOrbit = true;
    _juliaVariety = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = false;
    myRender = new JuliaZNRenderer[_threadNumber];
    SetWatchdog<JuliaZNRenderer>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("2"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Bailout: "), &bailout, wxT("2"));
    n = 2;
    bailout = 2;
}
JuliaZN::JuliaZN(const int width, const int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.77437;
    _maxX = 1.6912;
    _minY = -1.06769;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _algorithm = RenderingAlgorithmType::EscapeTime;

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("2"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Bailout: "), &bailout, wxT("2"));
    n = 2;
    bailout = 2;

    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _juliaVariety = true;
    _type = FractalType::JuliaZN;
    myRender = new JuliaZNRenderer[_threadNumber];
    SetWatchdog<JuliaZNRenderer>(myRender, &_watchdog, _threadNumber);
}
JuliaZN::~JuliaZN()
{
    this->StopRender();
    delete[] myRender;
}
void JuliaZN::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    const complex<double> k(_kReal, _kImaginary);
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned i=0; i<_maxIter; i++)
    {
        zVector.push_back(z);
        if (z.real()*z.real() + z.imag()*z.imag() > bailout*bailout)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, n) + k;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
void JuliaZN::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(n, bailout);

    this->SetRendererBounds<JuliaZNRenderer>(myRender);
}
void JuliaZN::CopyOptFromPanel()
{
    n = *_panelOpt.GetIntElement(0);
    bailout = *_panelOpt.GetDoubleElement(0);
}

