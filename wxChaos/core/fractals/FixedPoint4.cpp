#include <complex>
#include "FixedPoint4.h"
using namespace std;

FixedPoint4::FixedPoint4(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.8713;
    _maxX = 1.82101;
    _minY = -1.22781;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);
    _hasOrbit = true;

    _type = FractalType::FixedPoint4;
    _hasHighPrecisionRender = true;
    myRender = new FixedPoint4Renderer[_threadNumber];

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, "Min step: ", &minStep, "0.001");
    minStep = 0.001;

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithmType::ConvergenceTest);
}
FixedPoint4::~FixedPoint4()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint4::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(minStep);

    this->SetRendererBounds<FixedPoint4Renderer>(myRender);
}
void FixedPoint4::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    complex<double> z_ant;
    vector< complex<double> > zVector;

    for (unsigned n=0; n<_maxIter; n++)
    {
        zVector.push_back(z);
        z = pow(z,2);

        if ((z_ant.real() - minStep < z.real() && z_ant.real() + minStep > z.real())
            && (z_ant.imag() - minStep < z.imag() && z_ant.imag() + minStep > z.imag()))
            break;

        z_ant = z;
    }

    const auto color = sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}

void FixedPoint4::CopyOptionFromPanel()
{
    minStep = *_panelOpt.GetDoubleElement(0);
}
