#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Sinoidal::Sinoidal(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -8;
    maxX = 4;
    minY = -4.12;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::Sinoidal;
    kReal = 1;
    kImaginary = 0.25;
    hasOrbit = true;
    juliaVariety = true;

    hasOrbitTrap = true;

    myRender = new RenderSinoidal[threadNumber];
    SetWatchdog<RenderSinoidal>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
Sinoidal::Sinoidal(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -6;
    maxX = 4;
    minY = -4.5;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    alg = RenderingAlgorithm::EscapeTime;
    kReal = 1;
    kImaginary = 0.25;
    juliaVariety = true;

    type = FractalType::Sinoidal;
    myRender = new RenderSinoidal[threadNumber];
    SetWatchdog<RenderSinoidal>(myRender, &watchdog, threadNumber);
}
Sinoidal::~Sinoidal()
{
    this->StopRender();
    delete[] myRender;
}
void Sinoidal::Render()
{
    this->TRender<RenderSinoidal>(myRender);
}
void Sinoidal::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> k(kReal, kImaginary);
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
        {
            outOfSet = true;
            break;
        }
        z = k*sin(z);
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

