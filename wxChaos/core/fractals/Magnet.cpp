#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Magnet::Magnet(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -1.8;
    maxX = 4.4;
    minY = -2.2;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::Magnetic;
    hasOrbit = true;

    myRender = new RenderMagnet[threadNumber];
    SetWatchdog<RenderMagnet>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
Magnet::Magnet(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.05;
    maxX = 3.35;
    minY = -2.2;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    alg = RenderingAlgorithm::EscapeTime;
    type = FractalType::Magnetic;
    myRender = new RenderMagnet[threadNumber];
    SetWatchdog<RenderMagnet>(myRender, &watchdog, threadNumber);
}
Magnet::~Magnet()
{
    this->StopRender();
    delete[] myRender;
}
void Magnet::Render()
{
    this->TRender<RenderMagnet>(myRender);
}
void Magnet::DrawOrbit()
{
    complex<double> z(0, 0);
    complex<double> constant(orbitX, orbitY);
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
        z = pow((pow(z, 2) + constant - complex<double>(1, 0))/(complex<double>(2, 0)*z + constant - complex<double>(2,0)), 2);
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

