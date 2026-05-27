#include <complex>
#include "MandelbrotZN.h"
using namespace std;

MandelbrotZN::MandelbrotZN(const sf::RenderWindow* window) : Fractal(window)
{
    // Adjust the scale.
    _minX = -1.87078;
    _maxX = 1.74458;
    _minY = -1.169;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    // Adjust options.
    _type = FractalType::MandelbrotZN;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = false;
    myRender = new MandelbrotZNRenderer[_threadNumber];
    SetWatchdog<MandelbrotZNRenderer>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("3"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Bailout: "), &bailout, wxT("2"));
    n = 3;
    bailout = 2;
}
MandelbrotZN::MandelbrotZN(const int width, const int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.5;
    _maxX = 1.25;
    _minY = -1.3;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("3"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Bailout: "), &bailout, wxT("2"));
    n = 3;
    bailout = 2;

    _algorithm = RenderingAlgorithmType::EscapeTime;
    _hasSmoothRender = false;
    _hasOrbitTrap = true;
    _type = FractalType::MandelbrotZN;
    myRender = new MandelbrotZNRenderer[_threadNumber];
    SetWatchdog<MandelbrotZNRenderer>(myRender, &_watchdog, _threadNumber);
}
MandelbrotZN::~MandelbrotZN()
{
    this->StopRender();
    delete[] myRender;
}
void MandelbrotZN::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(n, bailout);

    this->SetRendererBounds<MandelbrotZNRenderer>(myRender);
}
void MandelbrotZN::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> c = z;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned i=0; i<_maxIter; i++)
    {
        zVector.push_back(z);
        if (z.real()*z.real() + z.imag()*z.imag() > 4)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, n) + c;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
void MandelbrotZN::CopyOptFromPanel()
{
    n = *_panelOpt.GetIntElement(0);
    bailout = *_panelOpt.GetDoubleElement(0);
}

