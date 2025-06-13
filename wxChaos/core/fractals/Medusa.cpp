#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Medusa::Medusa(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = -1.1342;
    maxX = 1.7251;
    minY = -0.90215;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::Medusa;
    kReal = -0.2;
    kImaginary = 0;
    juliaVariety = true;
    hasOrbit = true;
    hasOrbitTrap = true;

    myRender = new RenderMedusa[threadNumber];
    SetWatchdog<RenderMedusa>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
Medusa::Medusa(int width, int height) : Fractal(width, height)
{
    minX = -1.1342;
    maxX = 1.7251;
    minY = -0.90215;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    kReal = -0.2;
    kImaginary = 0;
    juliaVariety = true;
    alg = RenderingAlgorithm::EscapeTime;

    type = FractalType::Medusa;
    myRender = new RenderMedusa[threadNumber];
    SetWatchdog<RenderMedusa>(myRender, &watchdog, threadNumber);
}
Medusa::~Medusa()
{
    this->StopRender();
    delete[] myRender;
}
void Medusa::Render()
{
    this->TRender<RenderMedusa>(myRender);
}
void Medusa::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> k(kReal, kImaginary);
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
        z = pow(z, 1.5) + k;
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

