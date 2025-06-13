#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Cell::Cell(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = -1.95533;
    maxX = 1.91967;
    minY = -1.2495;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    hasOrbit = true;
    type = FractalType::Cell;
    myRender = new RenderCell[threadNumber];
    SetWatchdog<RenderCell>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    bailout = 2;

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
}
Cell::Cell(int width, int height) : Fractal(width, height)
{
    minX = -1.3;
    maxX = 1.3;
    minY = -1.2495;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    bailout = 2;

    hasOrbit = true;
    alg = RenderingAlgorithm::EscapeTime;
    type = FractalType::Cell;
    myRender = new RenderCell[threadNumber];
    SetWatchdog<RenderCell>(myRender, &watchdog, threadNumber);
}
Cell::~Cell()
{
    this->StopRender();
    delete[] myRender;
}
void Cell::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(bailout);
    this->TRender<RenderCell>(myRender);
}
void Cell::DrawOrbit()
{
    complex<double> c(orbitX, orbitY);
    complex<double> b, z;
    z = c;
    b = c - sin(c);

    vector< complex<double> > zVector;
    bool outOfSet = false;
    double squaredBail = bailout*bailout;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
        {
            outOfSet = true;
            break;
        }
        b /= c;
        z = z*c + b/z;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void Cell::CopyOptFromPanel()
{
    bailout = *panelOpt.GetDoubleElement(0);
}

