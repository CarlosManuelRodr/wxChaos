#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Fractory::Fractory(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = 0.837154;
    maxX = 1.14419;
    minY = -0.102209;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    hasOrbit = true;
    type = FractalType::Fractory;
    myRender = new RenderFractory[threadNumber];
    SetWatchdog<RenderFractory>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
Fractory::Fractory(int width, int height) : Fractal(width, height)
{
    minX = 0.837154;
    maxX = 1.14419;
    minY = -0.102209;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    alg = RenderingAlgorithm::EscapeTime;
    hasOrbit = true;

    type = FractalType::Fractory;
    myRender = new RenderFractory[threadNumber];
    SetWatchdog<RenderFractory>(myRender, &watchdog, threadNumber);
}
Fractory::~Fractory()
{
    this->StopRender();
    delete[] myRender;
}
void Fractory::Render()
{
    this->TRender<RenderFractory>(myRender);
}
void Fractory::DrawOrbit()
{
    complex<double> c(orbitX, orbitY);
    complex<double> b, z;
    z = c;
    b = c - sin(c);

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
        b = c + b/c - z;
        z = z*c + b/z;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

