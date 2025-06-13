#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
FixedPoint1::FixedPoint1(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -6.15385;
    maxX = 5.84615;
    minY = -4;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    hasOrbit = true;

    type = FractalType::FixedPoint1;
    myRender = new RenderFixedPoint1[threadNumber];
    SetWatchdog<RenderFixedPoint1>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FixedPoint1::FixedPoint1(int width, int height) : Fractal(width, height)
{
    minStep = 0.01;
    minX = -6.15385;
    maxX = 5.84615;
    minY = -4;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::FixedPoint1;
    myRender = new RenderFixedPoint1[threadNumber];
    SetWatchdog<RenderFixedPoint1>(myRender, &watchdog, threadNumber);
}
FixedPoint1::~FixedPoint1()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint1::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderFixedPoint1>(myRender);
}
void FixedPoint1::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> z_prev;

    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned n = 0; n < maxIter; n++)
    {
        zVector.push_back(z);
        z = sin(z);

        if ((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
            && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
            break;
        else
            z_prev = z;
    }

    sf::Color color;
    if (outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for (unsigned int i = 0; i < zVector.size() - 1; i++)
    {
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i + 1].real(), zVector[i + 1].imag(), color, true);
    }
    orbitDrawn = true;
}
void FixedPoint1::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

