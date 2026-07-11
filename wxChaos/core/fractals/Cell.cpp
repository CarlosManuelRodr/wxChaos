#include <complex>
#include "Cell.h"
using namespace std;

Cell::Cell(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    _minX = -1.3;
    _maxX = 1.3;
    _minY = -1.2495;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    // Creates panel.
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("Bailout: "), &bailout, "2");
    bailout = 2;

    _hasOrbit = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _type = FractalType::Cell;
    _hasHighPrecisionRender = true;
    myRender = new CellRenderer[_threadNumber];

    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);
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

    this->SetRendererBounds<CellRenderer>(myRender);
}
void Cell::DrawOrbit()
{
    const complex<double> c(_orbitX, _orbitY);
    complex<double> z = c;
    complex<double> b = c - sin(c);

    vector< complex<double> > zVector;
    bool outOfSet = false;
    double squaredBail = bailout*bailout;

    for (unsigned n=0; n<_maxIterations; n++)
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
void Cell::CopyOptionFromPanel()
{
    bailout = *_panelOpt.GetDoubleValue(0);
}
