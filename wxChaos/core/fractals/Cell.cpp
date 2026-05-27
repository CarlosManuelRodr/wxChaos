#include <complex>
#include "Cell.h"
using namespace std;

Cell::Cell(const sf::RenderWindow* window) : Fractal(window)
{
    _minX = -1.95533;
    _maxX = 1.91967;
    _minY = -1.2495;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _hasOrbit = true;
    _type = FractalType::Cell;
    myRender = new RenderCell[_threadNumber];
    SetWatchdog<RenderCell>(myRender, &_watchdog, _threadNumber);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Bailout: "), &bailout, wxT("2"));
    bailout = 2;

    // Specify algorithms.
    _algorithm = RenderingAlgorithm::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithm::GaussianInt);
}
Cell::Cell(int width, int height) : Fractal(width, height)
{
    _minX = -1.3;
    _maxX = 1.3;
    _minY = -1.2495;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Bailout: "), &bailout, wxT("2"));
    bailout = 2;

    _hasOrbit = true;
    _algorithm = RenderingAlgorithm::EscapeTime;
    _type = FractalType::Cell;
    myRender = new RenderCell[_threadNumber];
    SetWatchdog<RenderCell>(myRender, &_watchdog, _threadNumber);
}
Cell::~Cell()
{
    this->StopRender();
    delete[] myRender;
}
void Cell::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(bailout);

    this->TRender<RenderCell>(myRender);
}
void Cell::DrawOrbit()
{
    complex<double> c(_orbitX, _orbitY);
    complex<double> b, z;
    z = c;
    b = c - sin(c);

    vector< complex<double> > zVector;
    bool outOfSet = false;
    double squaredBail = bailout*bailout;

    for (unsigned n=0; n<_maxIter; n++)
    {
        zVector.push_back(z);
        if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
        {
            outOfSet = true;
            break;
        }
        b /= c;
        z = z*c + b/z;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
void Cell::CopyOptFromPanel()
{
    bailout = *_panelOpt.GetDoubleElement(0);
}

