#include <complex>
#include "FixedPoint2.h"
using namespace std;

FixedPoint2::FixedPoint2(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -5.88462;
    _maxX = 6.11538;
    _minY = -4;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _hasOrbit = true;
    _type = FractalType::FixedPoint2;
    _hasHighPrecisionRender = true;
    myRender = new FixedPoint2Renderer[_threadNumber];

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, "Min step: ", &minStep, "0.001");
    minStep = 0.001;

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithmType::ConvergenceTest);
}
FixedPoint2::~FixedPoint2()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint2::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(minStep);

    this->SetRendererBounds<FixedPoint2Renderer>(myRender);
}
void FixedPoint2::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> z_prev;
    vector< complex<double> > zVector;

    for (unsigned n=0; n<_maxIter; n++)
    {
        zVector.push_back(z);
        z = cos(z);

        if ((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
            && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
            break;

        z_prev = z;
    }

    const auto color = sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
void FixedPoint2::CopyOptionFromPanel()
{
    minStep = *_panelOpt.GetDoubleElement(0);
}
