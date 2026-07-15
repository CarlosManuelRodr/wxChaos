#include "JuliaZM.h"
#include <complex>
#include "Translation.h"
using namespace std;

JuliaZM::JuliaZM(const unsigned int width, const unsigned int height) : RasterFractal(width, height)
{
    // Adjust the scale.
    _minX = -1.77437;
    _maxX = 1.6912;
    _minY = -1.06769;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::JuliaZM;
    _hasHighPrecisionRender = true;
    _kReal = -0.754696;
    _kImaginary = -0.0524231;
    _hasOrbit = true;
    _juliaVariety = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = true;
    _myRender = new JuliaZMRenderer[_threadNumber];

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDouble(PanelOptionType::SpinDouble, WXCHAOS_TRANSLATE_NOOP("m: "), &_m, "3", 0.1);
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("Bailout: "), &_bailout, "2");
    _m = 3;
    _bailout = 2;
}
JuliaZM::~JuliaZM()
{
    this->RasterFractal::StopRender();
    delete[] _myRender;
}
void JuliaZM::DrawOrbit()
{
    complex<double> z(_orbitX, _orbitY);
    const complex<double> k(_kReal, _kImaginary);
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned i=0; i<_maxIterations; i++)
    {
        zVector.push_back(z);
        if (z.real()*z.real() + z.imag()*z.imag() > _bailout*_bailout)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, _m) + k;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
void JuliaZM::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        _myRender[i].SetParams(_m, _bailout);

    this->SetRendererBounds<JuliaZMRenderer>(_myRender);
}
void JuliaZM::CopyOptionFromPanel()
{
    _m = *_panelOpt.GetDoubleValue(0);
    _bailout = *_panelOpt.GetDoubleValue(1);
}
