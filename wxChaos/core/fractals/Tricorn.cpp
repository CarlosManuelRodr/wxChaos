#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Tricorn::Tricorn(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = -3;
    maxX = 3;
    minY = -1.94;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    hasOrbit = true;
    type = FractalType::Tricorn;
    myRender = new RenderTricorn[threadNumber];
    SetWatchdog<RenderTricorn>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
}
Tricorn::Tricorn(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -2.5;
    maxX = 2.5;
    minY = -2.5;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    hasOrbit = true;
    alg = RenderingAlgorithm::EscapeTime;

    type = FractalType::Tricorn;
    myRender = new RenderTricorn[threadNumber];
    SetWatchdog<RenderTricorn>(myRender, &watchdog, threadNumber);
}
Tricorn::~Tricorn()
{
    this->StopRender();
    delete[] myRender;
}
void Tricorn::Render()
{
    this->TRender<RenderTricorn>(myRender);
}
void Tricorn::DrawOrbit()
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

        z = pow(conj(z), 2) + c;
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

