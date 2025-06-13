#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
BurningShipJulia::BurningShipJulia(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = -2.77051;
    maxX = 2.77682;
    minY = -1.75939;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    juliaVariety = true;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    hasOrbit = true;
    type = FractalType::BurningShip;
    myRender = new RenderBurningShipJulia[threadNumber];
    SetWatchdog<RenderBurningShipJulia>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
BurningShipJulia::BurningShipJulia(int width, int height) : Fractal(width, height)
{
    minX = -2.77051;
    maxX = 2.77682;
    minY = -1.75939;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();
    juliaVariety = true;
    hasOrbit = true;

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    alg = RenderingAlgorithm::EscapeTime;
    type = FractalType::BurningShip;
    myRender = new RenderBurningShipJulia[threadNumber];
    SetWatchdog<RenderBurningShipJulia>(myRender, &watchdog, threadNumber);
}
BurningShipJulia::~BurningShipJulia()
{
    this->StopRender();
    delete[] myRender;
}
void BurningShipJulia::Render()
{
    this->TRender<RenderBurningShipJulia>(myRender);
}
void BurningShipJulia::DrawOrbit()
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
        z = pow( complex< double >(abs(z.real()), abs(z.imag())), 2 ) + k;
    }

    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

