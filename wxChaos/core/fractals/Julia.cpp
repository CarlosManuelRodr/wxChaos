#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
Julia::Julia(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.77437;
    maxX = 1.6912;
    minY = -1.06769;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::Julia;
    kReal = -0.754696;
    kImaginary = -0.0524231;
    hasOrbit = true;
    juliaVariety = true;
    hasOrbitTrap = true;
    hasSmoothRender = true;
    smoothRender = true;
    colorPaletteMode = ColorMode::Gradient;
    myRender = new RenderJulia[threadNumber];
    SetWatchdog<RenderJulia>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
    availableAlg.push_back(RenderingAlgorithm::TriangleInequality);
}
Julia::Julia(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.77437;
    maxX = 1.6912;
    minY = -1.06769;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    alg = RenderingAlgorithm::EscapeTime;
    hasOrbitTrap = true;
    hasSmoothRender = true;
    juliaVariety = true;
    type = FractalType::Julia;
    myRender = new RenderJulia[threadNumber];
    SetWatchdog<RenderJulia>(myRender, &watchdog, threadNumber);
}
Julia::~Julia()
{
    this->StopRender();
    delete[] myRender;
}
void Julia::DrawOrbit()
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
        z = pow(z, 2) + k;
    }

    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void Julia::Render()
{
    this->TRender<RenderJulia>(myRender);
}

