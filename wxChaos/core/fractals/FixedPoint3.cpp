#include "FixedPoint3.h"
#include <complex>
#include "Translation.h"
using namespace std;

FixedPoint3::FixedPoint3(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -3.76339;
    _maxX = 3.59018;
    _minY = -2.39204;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);
    _hasOrbit = true;

    _type = FractalType::FixedPoint3;
    _hasHighPrecisionRender = true;
    myRender = new FixedPoint3Renderer[_threadNumber];

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("Min step: "), &minStep, "0.001");
    minStep = 0.001;

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithmType::ConvergenceTest);
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
    this->SetRendererBounds<FixedPoint3Renderer>(myRender);
}
void FixedPoint3::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> z_ant;
    vector< complex<double> > zVector;

    for (unsigned n=0; n<_maxIterations; n++)
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
void FixedPoint3::CopyOptionFromPanel()
{
    minStep = *_panelOpt.GetDoubleValue(0);
}
