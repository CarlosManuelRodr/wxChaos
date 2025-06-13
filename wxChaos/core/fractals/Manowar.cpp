#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Manowar::Manowar(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -0.4795;
    maxX = 0.26108;
    minY = -0.2375;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::Manowar;
    hasOrbit = true;
    hasOrbitTrap = true;
    myRender = new RenderManowar[threadNumber];
    SetWatchdog<RenderManowar>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
Manowar::Manowar(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -0.20;
    maxX = 0.11;
    minY = -0.15;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    alg = RenderingAlgorithm::EscapeTime;

    type = FractalType::Manowar;
    myRender = new RenderManowar[threadNumber];
    SetWatchdog<RenderManowar>(myRender, &watchdog, threadNumber);
}
Manowar::~Manowar()
{
    this->StopRender();
    delete[] myRender;
}
void Manowar::Render()
{
    this->TRender<RenderManowar>(myRender);
}
void Manowar::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> constant(orbitX, orbitY);
    complex<double> man;
    complex<double> temp;
    man = z;
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
        temp = z;
        z = pow(z, 2) + man + constant;
        man = temp;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

