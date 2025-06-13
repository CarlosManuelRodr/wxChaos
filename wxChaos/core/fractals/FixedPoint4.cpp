#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
FixedPoint4::FixedPoint4(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.8713;
    maxX = 1.82101;
    minY = -1.22781;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    hasOrbit = true;

    type = FractalType::FixedPoint4;
    myRender = new RenderFixedPoint4[threadNumber];
    SetWatchdog<RenderFixedPoint4>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FixedPoint4::FixedPoint4(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.8713;
    maxX = 1.82101;
    minY = -1.22781;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::FixedPoint4;
    myRender = new RenderFixedPoint4[threadNumber];
    SetWatchdog<RenderFixedPoint4>(myRender, &watchdog, threadNumber);
}
FixedPoint4::~FixedPoint4()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint4::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderFixedPoint4>(myRender);
}
void FixedPoint4::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> z_ant;
    double minStep = 0.001;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        z = pow(z,2);

        if((z_ant.real() - minStep < z.real() && z_ant.real() + minStep > z.real())
            && (z_ant.imag() - minStep < z.imag() && z_ant.imag() + minStep > z.imag()))
            break;
        else
            z_ant = z;
    }

    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

void FixedPoint4::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

