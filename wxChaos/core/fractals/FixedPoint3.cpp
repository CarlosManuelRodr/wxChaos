#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
FixedPoint3::FixedPoint3(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -3.76339;
    maxX = 3.59018;
    minY = -2.39204;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    hasOrbit = true;

    type = FractalType::FixedPoint3;
    myRender = new RenderFixedPoint3[threadNumber];
    SetWatchdog<RenderFixedPoint3>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FixedPoint3::FixedPoint3(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -3.76339;
    maxX = 3.59018;
    minY = -2.39204;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::FixedPoint3;
    myRender = new RenderFixedPoint3[threadNumber];
    SetWatchdog<RenderFixedPoint3>(myRender, &watchdog, threadNumber);
}
FixedPoint3::~FixedPoint3()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint3::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderFixedPoint3>(myRender);
}
void FixedPoint3::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> z_ant;
    double minStep = 0.001;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        z = tan(z);

        if((z_ant.real() - minStep < z.real() && z_ant.real() + minStep > z.real())
            && (z_ant.imag() - minStep < z.imag() && z_ant.imag() + minStep > z.imag()))
        {
            break;
        }
        else
        {
            z_ant = z;
        }
    }

    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
    {
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);
    }
    orbitDrawn = true;
}
void FixedPoint3::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

