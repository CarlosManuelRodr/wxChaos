#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Newton::Newton(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.5721;
    maxX = 1.4086;
    minY = -1;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::NewtonRaphsonMethod;
    hasOrbit = true;
    hasOrbitTrap = true;
    myRender = new RenderNewton[threadNumber];
    SetWatchdog<RenderNewton>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
Newton::Newton(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.5721;
    maxX = 1.4086;
    minY = -1;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    type = FractalType::NewtonRaphsonMethod;
    myRender = new RenderNewton[threadNumber];
    SetWatchdog<RenderNewton>(myRender, &watchdog, threadNumber);
}
Newton::~Newton()
{
    this->StopRender();
    delete[] myRender;
}
void Newton::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> anterior;
    const double minStep = 0.001;

    if(orbitX != 0 && orbitY != 0)
    {
        for(unsigned n=0; n<maxIter; n++)
        {
            anterior = z;
            z = z - (pow(z, 3) - complex<double>(1, 0))/(complex<double>(2, 0)*pow(z,2));

            this->DrawLine(anterior.real(), anterior.imag(), z.real(), z.imag(), sf::Color(0,255,0), true);

            if((anterior.real() - minStep < z.real() && anterior.real() + minStep > z.real())
                && (anterior.imag() - minStep < z.imag() && anterior.imag() + minStep > z.imag()))
                break;
        }
    }

    orbitDrawn = true;
}
void Newton::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderNewton>(myRender);
}
void Newton::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

