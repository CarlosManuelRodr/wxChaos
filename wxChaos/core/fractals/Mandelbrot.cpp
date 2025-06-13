#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Mandelbrot::Mandelbrot(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -2.45296;
    maxX = 1.1624;
    minY = -1.169;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::Mandelbrot;
    hasOrbit = true;
    hasOrbitTrap = true;
    hasSmoothRender = true;
    smoothRender = true;
    colorPaletteMode = ColorMode::Gradient;
    myRender = new RenderMandelbrot[threadNumber];
    SetWatchdog<RenderMandelbrot>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(false);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT(buddhaRandPTxt), &buddhaRandomP, wxT("1000000"));
    buddhaRandomP = 1000000;

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
    availableAlg.push_back(RenderingAlgorithm::TriangleInequality);
}
Mandelbrot::Mandelbrot(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.89;
    maxX = 0.55;
    minY = -1.12;
    maxY = minY+(maxX-minX);
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(false);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT(buddhaRandPTxt), &buddhaRandomP, wxT("1000000"));
    buddhaRandomP = 1000000;

    alg = RenderingAlgorithm::EscapeTime;
    hasSmoothRender = true;
    hasOrbitTrap = true;
    type = FractalType::Mandelbrot;
    myRender = new RenderMandelbrot[threadNumber];
    SetWatchdog<RenderMandelbrot>(myRender, &watchdog, threadNumber);
}
Mandelbrot::~Mandelbrot()
{
    this->StopRender();
    delete[] myRender;
}
void Mandelbrot::Render()
{
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetBuddhaRandomP(buddhaRandomP);

    this->TRender<RenderMandelbrot>(myRender);
}
void Mandelbrot::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> c = z;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > 4)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, 2) + c;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void Mandelbrot::CopyOptFromPanel()
{
    buddhaRandomP = *panelOpt.GetIntElement(0);
}
void Mandelbrot::PreRender()
{
}
void Mandelbrot::PreDrawMaps()
{
}

