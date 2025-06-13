#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
ManowarJulia::ManowarJulia(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = -2.0;
    maxX = 1.0;
    minY = -1.2;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    kReal = 0.0272873;
    kImaginary = -0.0432547;
    type = FractalType::ManowarJulia;
    juliaVariety = true;
    hasOrbit = true;
    hasOrbitTrap = true;
    myRender = new RenderManowarJulia[threadNumber];
    SetWatchdog<RenderManowarJulia>(myRender, &watchdog, threadNumber);

    // Especify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
ManowarJulia::ManowarJulia(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -2.0;
    maxX = 1.0;
    minY = -1.2;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    this->SetOutermostZoom();

    kReal = 0.0272873;
    kImaginary = -0.0432547;

    juliaVariety = true;
    alg = RenderingAlgorithm::EscapeTime;

    type = FractalType::ManowarJulia;
    myRender = new RenderManowarJulia[threadNumber];
    SetWatchdog<RenderManowarJulia>(myRender, &watchdog, threadNumber);
}
ManowarJulia::~ManowarJulia()
{
    this->StopRender();
    delete[] myRender;
}
void ManowarJulia::Render()
{
    this->TRender<RenderManowarJulia>(myRender);
}
void ManowarJulia::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> k(kReal, kImaginary);
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
        z = pow(z, 2) + man + k;
        man = temp;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

