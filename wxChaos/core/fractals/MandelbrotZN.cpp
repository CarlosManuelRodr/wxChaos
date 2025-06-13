#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
MandelbrotZN::MandelbrotZN(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.87078;
    maxX = 1.74458;
    minY = -1.169;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Ajusta opciones.
    type = FractalType::MandelbrotZN;
    hasOrbit = true;
    hasOrbitTrap = true;
    hasSmoothRender = true;
    smoothRender = false;
    colorPaletteMode = ColorMode::Gaussian;
    myRender = new RenderMandelbrotZN[threadNumber];
    SetWatchdog<RenderMandelbrotZN>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("3"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    n = 3;
    bailout = 2;
}
MandelbrotZN::MandelbrotZN(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.5;
    maxX = 1.25;
    minY = -1.3;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("3"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    n = 3;
    bailout = 2;

    alg = RenderingAlgorithm::EscapeTime;
    hasSmoothRender = false;
    hasOrbitTrap = true;
    type = FractalType::MandelbrotZN;
    myRender = new RenderMandelbrotZN[threadNumber];
    SetWatchdog<RenderMandelbrotZN>(myRender, &watchdog, threadNumber);
}
MandelbrotZN::~MandelbrotZN()
{
    this->StopRender();
    delete[] myRender;
}
void MandelbrotZN::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(n, bailout);
    this->TRender<RenderMandelbrotZN>(myRender);
}
void MandelbrotZN::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> c = z;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned i=0; i<maxIter; i++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > 4)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, n) + c;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void MandelbrotZN::CopyOptFromPanel()
{
    n = *panelOpt.GetIntElement(0);
    bailout = *panelOpt.GetDoubleElement(0);
}

