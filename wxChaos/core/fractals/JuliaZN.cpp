#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
JuliaZN::JuliaZN(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.77437;
    maxX = 1.6912;
    minY = -1.06769;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::JuliaZN;
    kReal = -0.754696;
    kImaginary = -0.0524231;
    hasOrbit = true;
    juliaVariety = true;
    hasOrbitTrap = true;
    hasSmoothRender = true;
    smoothRender = false;
    colorPaletteMode = ColorMode::Gaussian;
    myRender = new RenderJuliaZN[threadNumber];
    SetWatchdog<RenderJuliaZN>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("2"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    n = 2;
    bailout = 2;
}
JuliaZN::JuliaZN(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.77437;
    maxX = 1.6912;
    minY = -1.06769;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    alg = RenderingAlgorithm::EscapeTime;

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("2"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    n = 2;
    bailout = 2;

    hasOrbitTrap = true;
    hasSmoothRender = true;
    juliaVariety = true;
    type = FractalType::JuliaZN;
    myRender = new RenderJuliaZN[threadNumber];
    SetWatchdog<RenderJuliaZN>(myRender, &watchdog, threadNumber);
}
JuliaZN::~JuliaZN()
{
    this->StopRender();
    delete[] myRender;
}
void JuliaZN::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> k(kReal, kImaginary);
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned i=0; i<maxIter; i++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > bailout*bailout)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, n) + k;
    }

    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void JuliaZN::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(n, bailout);
    this->TRender<RenderJuliaZN>(myRender);
}
void JuliaZN::CopyOptFromPanel()
{
    n = *panelOpt.GetIntElement(0);
    bailout = *panelOpt.GetDoubleElement(0);
}

