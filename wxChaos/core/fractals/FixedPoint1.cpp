#include <complex>
#include "FixedPoint1.h"
using namespace std;

FixedPoint1::FixedPoint1(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -6.15385;
    _maxX = 5.84615;
    _minY = -4;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);
    _hasOrbit = true;

    _type = FractalType::FixedPoint1;
    _hasHighPrecisionRender = true;
    myRender = new FixedPoint1Renderer[_threadNumber];

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("Min step: "), &minStep, "0.001");
    minStep = 0.001;

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithmType::ConvergenceTest);
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

    for (unsigned n = 0; n < _maxIterations; n++)
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
void FixedPoint1::CopyOptionFromPanel()
{
    minStep = *_panelOpt.GetDoubleValue(0);
}
