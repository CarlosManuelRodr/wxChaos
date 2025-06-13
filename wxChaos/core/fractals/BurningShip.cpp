#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
BurningShip::BurningShip(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -2.36;
    maxX = 1.79;
    minY = -1.94;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    hasOrbit = true;
    type = FractalType::BurningShip;
    myRender = new RenderBurningShip[threadNumber];
    SetWatchdog<RenderBurningShip>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
BurningShip::BurningShip(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -2.36;
    maxX = 1.79;
    minY = -1.94;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    alg = RenderingAlgorithm::EscapeTime;
    hasOrbit = true;
    type = FractalType::BurningShip;
    myRender = new RenderBurningShip[threadNumber];
    SetWatchdog<RenderBurningShip>(myRender, &watchdog, threadNumber);
}
BurningShip::~BurningShip()
{
    this->StopRender();
    delete[] myRender;
}
void BurningShip::Render()
{
    this->TRender<RenderBurningShip>(myRender);
}
void BurningShip::DrawOrbit()
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
        z = pow( complex< double >(abs(z.real()), abs(z.imag())), 2 ) + c;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

