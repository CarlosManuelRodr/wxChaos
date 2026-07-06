#include <complex>
#include "MandelbrotZM.h"
using namespace std;

MandelbrotZM::MandelbrotZM(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.87078;
    _maxX = 1.74458;
    _minY = -1.169;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    // Adjust options.
    _type = FractalType::MandelbrotZN;
    _hasHighPrecisionRender = true;
    _hasOrbit = true;
    _hasOrbitTrap = true;
    _hasSmoothRender = true;
    _smoothRender = true;
    _myRender = new MandelbrotZMRenderer[_threadNumber];

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
    _availableAlg.push_back(RenderingAlgorithmType::GaussianInt);
    _availableAlg.push_back(RenderingAlgorithmType::EscapeAngle);
    _availableAlg.push_back(RenderingAlgorithmType::TriangleInequality);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, "m: ", &_m, "3");
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, "Bailout: ", &_bailout, "2");
    _m = 3;
    _bailout = 2;
}
MandelbrotZM::~MandelbrotZM()
{
    this->StopRender();
    delete[] _myRender;
}
void MandelbrotZM::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        _myRender[i].SetParams(_m, _bailout);

    this->SetRendererBounds<MandelbrotZMRenderer>(_myRender);
}
void MandelbrotZM::DrawOrbit()
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
        z = pow(z, _m) + c;
    }

    const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
    for (unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    _orbitDrawn = true;
}
void MandelbrotZM::CopyOptionFromPanel()
{
    _m = *_panelOpt.GetDoubleValue(0);
    _bailout = *_panelOpt.GetDoubleValue(1);
}
