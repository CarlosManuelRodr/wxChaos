/**
* @file FractalTypes.cpp
* @brief Implements definitions of each type of fractal.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/19/2012
*/

#include "FractalTypes.h"
#include <cmath>
#include <ctime>
#include <fstream>
#include <sstream>
#include "StringFuncs.h"
#include "ConfigParser.h"
#include "global.h"
using namespace std;

const int trapFactor = 1;

inline double minVal(double a, double b)
{
    return (a > b ? b : a);
}

inline double gaussianIntDist(double x, double y)
{
  double gint_x = floor(x*trapFactor+0.5)/trapFactor;
  double gint_y = floor(y*trapFactor+0.5)/trapFactor;
  return sqrt((x - gint_x)*(x-gint_x) + (y-gint_y)*(y-gint_y));
}

inline double TIA(double z_re, double z_im, double c_re, double c_im, double tia_prev_x, double tia_prev_y)
{
    double z_mag = sqrt(tia_prev_x*tia_prev_x + tia_prev_y*tia_prev_y);
    double c_mag = sqrt(c_re*c_re + c_im*c_im);

    double mn = abs(z_mag - c_mag);
    double Mn = z_mag + c_mag;

    double num = sqrt(z_re*z_re + z_im*z_im) - mn;
    double den = Mn - mn;
    return num/den;
}

// RenderMandelbrot
RenderMandelbrot::RenderMandelbrot()
{
    buddhaRandomP = 0;
    bd = 0;
}
void RenderMandelbrot::Render()
{
    double c_im;
    double c_re;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    // Creates fractal.
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        unsigned n;
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }

                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }

                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = Z_im = 0;
                c_re = minX + x*xFactor;
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(Z_re, Z_im));
                }

                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::Buddhabrot)
    {
        sf::Mutex mutex;
        srand(static_cast<unsigned int>(time(nullptr)));

        complex<double> z, c;
        complex<double> *cmpArray;
        cmpArray = new complex<double>[static_cast<unsigned int>(maxIter)];
        int topIter;

        for(int i=0; i<maxIter; i++) cmpArray[i] = complex<double>(0, 0);

        for(bd=0; bd<buddhaRandomP; bd++)
        {
            bool out = false;

            // By default uses LGC which isn't very good.
            z = c = complex<double>(((double(rand()) / double(RAND_MAX)) * (maxX - minX)) + minX,
                                    ((double(rand()) / double(RAND_MAX)) * (maxY - minY)) + minY);
            if
            (
               (z.real() >  -1.2 && z.real() <=  -1.1 && z.imag() >  -0.1 && z.imag() < 0.1)
                || (z.real() >  -1.1 && z.real() <=  -0.9 && z.imag() >  -0.2 && z.imag() < 0.2)
                || (z.real() >  -0.9 && z.real() <=  -0.8 && z.imag() >  -0.1 && z.imag() < 0.1)
                || (z.real() > -0.69 && z.real() <= -0.61 && z.imag() >  -0.2 && z.imag() < 0.2)
                || (z.real() > -0.61 && z.real() <=  -0.5 && z.imag() > -0.37 && z.imag() < 0.37)
                || (z.real() >  -0.5 && z.real() <= -0.39 && z.imag() > -0.48 && z.imag() < 0.48)
                || (z.real() > -0.39 && z.real() <=  0.14 && z.imag() > -0.55 && z.imag() < 0.55)
                || (z.real() >  0.14 && z.real() <   0.29 && z.imag() > -0.42 && z.imag() < -0.07)
                || (z.real() >  0.14 && z.real() <   0.29 && z.imag() >  0.07 && z.imag() < 0.42)
            ) continue; // "if" taken from Wikipedia description.


            for(int i=0; i<maxIter; i++)
            {
                if(z.real()*z.real() + z.imag()*z.imag() > 6)
                {
                    out = true;
                    topIter = i;
                    break;
                }
                cmpArray[i] = z;
                z = pow(z, 2) + c;
            }
            if(out)
            {
                for(int i=0; i<=topIter; i++)
                {
                    int indexI = static_cast<int>((cmpArray[i].real()-minX)/xFactor);
                    int indexJ = static_cast<int>((maxY-cmpArray[i].imag())/yFactor);
                    if((indexI >= 0 && indexI < myOpt.screenWidth) && (indexJ >=0 && indexJ < myOpt.screenHeight))
                    {
                        mutex.Lock();
                        auxMap[indexI][indexJ]++;
                        mutex.Unlock();
                    }
                }

                // Takes advantage of the simmetry.
                z = c = complex<double>(c.real(), -c.imag());
                for(int i=0; i<maxIter; i++)
                {
                    z = pow(z,2) + c;
                    int indexI = static_cast<int>((z.real()-minX)/xFactor);
                    int indexJ = static_cast<int>((maxY-z.imag())/yFactor);
                    if((indexI >= 0 && indexI < myOpt.screenWidth) && (indexJ >=0 && indexJ < myOpt.screenHeight))
                    {
                        mutex.Lock();
                        auxMap[indexI][indexJ]++;
                        mutex.Unlock();
                    }
                }
            }
        }
        delete[] cmpArray;
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        unsigned n;
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }

                if(insideSet)
                    setMap[x][y] = true;

                if(Z_re > 0 && Z_im > 0)
                    colorMap[x][y] = n + color1;
                else if(Z_re <= 0 && Z_im > 0)
                    colorMap[x][y] = n + color2;
                else if(Z_re <= 0 && Z_im < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::TriangleInequality)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double tia_prev_x, tia_prev_y;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = Z_im = 0;
                c_re = minX + x*xFactor;
                insideSet = true;
                distance = 0;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im;
                    Z_re = Z_re2 - Z_im2;

                    tia_prev_x = Z_re;
                    tia_prev_y = Z_im;

                    Z_re += c_re;
                    Z_im += c_im;

                    distance1 = distance;
                    if(n > 0) distance += TIA(Z_re, Z_im, c_re, c_im, tia_prev_x, tia_prev_y);
                }

                if(insideSet)
                    setMap[x][y] = true;

                distance1 = distance1/(n-1);
                distance = distance/n;
                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*700)));
            }
        }
    }
}
void RenderMandelbrot::SpecialRender()
{
    double c_im;
    double c_re;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    if(myOpt.orbitTrapMode)
    {
        // Creates fractal.
        double distX, distY;
        int iterations;
        double log2 = log(2.0);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;

                distX = abs(Z_re);
                distY = abs(Z_im);

                insideSet = true;
                iterations = 0;

                for(unsigned n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        if(Z_re2 + Z_im2 > 16)
                        {
                            if(abs(Z_im) < distY) distY = abs(Z_im);
                            if(abs(Z_re) < distX) distX = abs(Z_re);
                            break;
                        }
                    }
                    else iterations = n;

                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;

                    if(abs(Z_im) < distY) distY = abs(Z_im);
                    if(abs(Z_re) < distX) distX = abs(Z_re);
                }
                if(distX == 0) distX = 0.000001;
                if(distY == 0) distY = 0.000001;

                if(insideSet)
                    setMap[x][y] = true;

                if(myOpt.smoothRender)
                {
                    if(!insideSet)
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(iterations + log(1/distX) + log(1/distY));
            }
        }
    }
    if(myOpt.smoothRender && !(myOpt.orbitTrapMode))
    {
        unsigned n;
        double log2 = log(2.0);
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }

                if(insideSet) setMap[x][y] = true;
                colorMap[x][y] = static_cast<unsigned int>(abs(4.0*(n -  log(log(Z_re2+Z_im2))/log2)));
            }
        }
    }
}
void RenderMandelbrot::SetBuddhaRandomP(int n)
{
    buddhaRandomP = n;
}
int RenderMandelbrot::AskProgress()
{
    if(!stopped)
    {
        if(myOpt.alg == RenderingAlgorithm::Buddhabrot)
            threadProgress = static_cast<int>(100.0*(double)(bd+1)/(double)buddhaRandomP);
        else
            threadProgress = static_cast<int>(100.0*((double)(y+1-oldHo)/(double)(hf-oldHo)));
    }
    return threadProgress;
}

// Mandelbrot
Mandelbrot::Mandelbrot(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -2.45296;
    maxX = 1.1624;
    minY = -1.169;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::Mandelbrot;
    hasOrbit = true;
    hasOrbitTrap = true;
    hasSmoothRender = true;
    smoothRender = true;
    colorPaletteMode = ColorMode::Gradient;
    myRender = new RenderMandelbrot[threadNumber];
    SetWatchdog<RenderMandelbrot>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(false);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT(buddhaRandPTxt), &buddhaRandomP, wxT("1000000"));
    buddhaRandomP = 1000000;

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::Buddhabrot);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
    availableAlg.push_back(RenderingAlgorithm::TriangleInequality);
}
Mandelbrot::Mandelbrot(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.89;
    maxX = 0.55;
    minY = -1.12;
    maxY = minY+(maxX-minX);
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(false);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT(buddhaRandPTxt), &buddhaRandomP, wxT("1000000"));
    buddhaRandomP = 1000000;

    alg = RenderingAlgorithm::EscapeTime;
    hasSmoothRender = true;
    hasOrbitTrap = true;
    type = FractalType::Mandelbrot;
    myRender = new RenderMandelbrot[threadNumber];
    SetWatchdog<RenderMandelbrot>(myRender, &watchdog, threadNumber);
}
Mandelbrot::~Mandelbrot()
{
    this->StopRender();
    delete[] myRender;
}
void Mandelbrot::Render()
{
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetBuddhaRandomP(buddhaRandomP);

    this->TRender<RenderMandelbrot>(myRender);
}
void Mandelbrot::DrawOrbit()
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
        z = pow(z, 2) + c;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void Mandelbrot::CopyOptFromPanel()
{
    buddhaRandomP = *panelOpt.GetIntElement(0);
}
void Mandelbrot::PreRender()
{
    // If the algorithm is buddhabrot forces to redraw all.
    if(alg == RenderingAlgorithm::Buddhabrot)
    {
        xMoved = 0;
        yMoved = 0;
    }
}
void Mandelbrot::PreDrawMaps()
{
    // If buddhabrot is active, creates colorMap.
    if(alg == RenderingAlgorithm::Buddhabrot)
    {
        // Search for color maximum.
        unsigned int maxColorVal = 0;
        for(int i=0; i<screenWidth; i++)
        {
            for (int j = 0; j < screenHeight; j++)
            {
                if (auxMap[i][j] > maxColorVal)
                    maxColorVal = auxMap[i][j];
            }
        }
        for(int i=0; i<screenWidth; i++)
        {
            for(int j=0; j<screenHeight; j++)
            {
                colorMap[i][j] %= 60;
                colorMap[i][j] /= 4;
                colorMap[i][j] += static_cast<unsigned int>(paletteSize*(((double)auxMap[i][j])/((double)maxColorVal)));
            }
        }
    }
}

// RenderMandelbrotZN
RenderMandelbrotZN::RenderMandelbrotZN()
{
    n = 0;
    bailout = 0;
}
void RenderMandelbrotZN::Render()
{
    double c_im;
    bool insideSet;

    // Creates fractal.
    unsigned i;
    complex<double> z, c;
    double squaredBail = bailout*bailout;
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + c;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = i;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double zNorm;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(0,0);
                c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(i=0; i<maxIter && insideSet; i++)
                {
                    zNorm = z.real()*z.real() + z.imag()*z.imag();
                    if(zNorm > squaredBail)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        insideSet = false;
                    }
                    z = pow(z,n) + c;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + c;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = i + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = i + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = i + color3;
                else
                    colorMap[x][y] = i + color4;
            }
        }
    }
}
void RenderMandelbrotZN::SpecialRender()
{
    double c_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;
    double squaredBail = bailout*bailout;
    double log2 = log(2.0);
    unsigned i;

    if(myOpt.orbitTrapMode)
    {
        // Creates fractal.
        complex<double> z, c;
        double distX, distY;
        int iterations;
        double bailFourPower = squaredBail*squaredBail;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                distX = abs(z.real());
                distY = abs(z.imag());

                insideSet = true;
                iterations = 0;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z, n) + c;
                    Z_re2 = z.real()*z.real();
                    Z_im2 = z.imag()*z.imag();
                    if(Z_re2 + Z_im2 > squaredBail)
                    {
                        insideSet = false;
                        if(Z_re2 + Z_im2 > bailFourPower)
                        {
                            if(abs(z.imag()) < distY) distY = abs(z.imag());
                            if(abs(z.real()) < distX) distX = abs(z.real());
                            break;
                        }
                    }
                    else iterations = i;

                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }
                if(distX == 0) distX = 0.000001;
                if(distY == 0) distY = 0.000001;

                if(insideSet)
                    setMap[x][y] = true;

                if(myOpt.smoothRender)
                {
                    if(!insideSet)
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(abs(iterations + log(1/distX) + log(1/distY)));
            }
        }
    }
    else if(myOpt.smoothRender)
    {
        // Creates fractal.
        complex<double> z, c;
        double squaredBail = bailout*bailout;
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + c;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<int>(abs(4.0*(i -  log(log(z.real()*z.real()+z.imag()*z.imag()))/log2)));
            }
        }
    }
}
void RenderMandelbrotZN::SetParams(int _n, double _bailout)
{
    n = _n;
    bailout = _bailout;
}

// MandelbrotZN
MandelbrotZN::MandelbrotZN(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.87078;
    maxX = 1.74458;
    minY = -1.169;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Ajusta opciones.
    type = FractalType::MandelbrotZN;
    hasOrbit = true;
    hasOrbitTrap = true;
    hasSmoothRender = true;
    smoothRender = false;
    colorPaletteMode = ColorMode::Gaussian;
    myRender = new RenderMandelbrotZN[threadNumber];
    SetWatchdog<RenderMandelbrotZN>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("3"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    n = 3;
    bailout = 2;
}
MandelbrotZN::MandelbrotZN(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.5;
    maxX = 1.25;
    minY = -1.3;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("3"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    n = 3;
    bailout = 2;

    alg = RenderingAlgorithm::EscapeTime;
    hasSmoothRender = false;
    hasOrbitTrap = true;
    type = FractalType::MandelbrotZN;
    myRender = new RenderMandelbrotZN[threadNumber];
    SetWatchdog<RenderMandelbrotZN>(myRender, &watchdog, threadNumber);
}
MandelbrotZN::~MandelbrotZN()
{
    this->StopRender();
    delete[] myRender;
}
void MandelbrotZN::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(n, bailout);
    this->TRender<RenderMandelbrotZN>(myRender);
}
void MandelbrotZN::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> c = z;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned i=0; i<maxIter; i++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > 4)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, n) + c;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void MandelbrotZN::CopyOptFromPanel()
{
    n = *panelOpt.GetIntElement(0);
    bailout = *panelOpt.GetDoubleElement(0);
}

// RenderJulia
RenderJulia::RenderJulia() {}
void RenderJulia::Render()
{
    double c_im;
    double c_re;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    // Creates fractal.
    unsigned n;
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = minX + x*xFactor;
                Z_im = c_im;

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = 2*Z_re*Z_im + kImaginary;
                    Z_re = Z_re2 - Z_im2 + kReal;
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double temp_im;

        for(y=ho; y<hf; y++)
        {
            temp_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = minX + x*xFactor;
                Z_im = temp_im;

                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im + kImaginary;
                    Z_re = Z_re2 - Z_im2 + kReal;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(Z_re, Z_im));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;
        complex<double> z, k;
        k = complex<double>(kReal, kImaginary);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    z = pow(z,2) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = n + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = n + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::TriangleInequality)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double tia_prev_x, tia_prev_y;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                c_re = Z_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;
                distance = 0;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im;
                    Z_re = Z_re2 - Z_im2;

                    tia_prev_x = Z_re;
                    tia_prev_y = Z_im;

                    Z_re += kReal;
                    Z_im += kImaginary;

                    distance1 = distance;
                    if(n > 0) distance += TIA(Z_re, Z_im, c_re, c_im, tia_prev_x, tia_prev_y);
                }
                if(insideSet)
                    setMap[x][y] = true;

                distance1 = distance1/(n-1);
                distance = distance/n;
                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*700)));
            }
        }
    }
}
void RenderJulia::SpecialRender()
{
    double c_im;
    double c_re;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    double log2 = log(2.0);
    if(myOpt.orbitTrapMode)
    {
        // Creates fractal.
        complex<double> z, temp;
        complex<double> constant(kReal, kImaginary);
        double distX, distY;
        double re, im;
        bool broken;
        int out;

        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                re = minX + x*xFactor;
                im = maxY - y*yFactor;
                z = complex<double>(re, im);

                broken = false;
                distX = abs(re);
                distY = abs(im);
                insideSet = true;
                int iterations = 0;

                for(unsigned n=0; n<maxIter; n++)
                {
                    z = pow(z, 2) + constant;
                    if(z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        if(!broken) temp = z;
                        insideSet = false;
                        broken = true;
                    }
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());

                    if(!broken) iterations = n;
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
                if(distX == 0) distX = 0.000001;
                if(distY == 0) distY = 0.000001;
                if(myOpt.smoothRender)
                {
                    if(!insideSet)
                    {
                        out = static_cast<int>(abs(4.0*(iterations -  log(log(pow(temp.real(),2)+pow(temp.imag(),2)))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                        if(out < 0) out = 0;
                        colorMap[x][y] = out;
                    }
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(abs(iterations + log(1/distX) + log(1/distY)));
            }
        }
    }
    if(myOpt.smoothRender && !(myOpt.orbitTrapMode))
    {
        unsigned n;
        int out;
        c_re = kReal;
        c_im = kImaginary;
        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                Z_re = minX + x*xFactor;
                Z_im = maxY - y*yFactor;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if(insideSet) setMap[x][y] = true;
                out = static_cast<int>(abs(4.0*(n -  log(log(Z_re2+Z_im2))/log2)));
                if(out < 0) out = 0;
                colorMap[x][y] = out;
            }
        }
    }
}

// Julia
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

// RenderJuliaZN
RenderJuliaZN::RenderJuliaZN()
{
    n = 0;
    bailout = 0;
}
void RenderJuliaZN::Render()
{
    double c_im;
    bool insideSet;
    unsigned i;
    double squaredBail = bailout*bailout;

    // Creates fractal.
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        complex<double> z, k;
        k = complex<double>(kReal, kImaginary);
        double squaredBail = bailout*bailout;
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = i;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        complex<double> z, k;
        k = complex<double>(kReal, kImaginary);
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double zNorm;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(i=0; i<maxIter && insideSet; i++)
                {
                    zNorm = z.real()*z.real() + z.imag()*z.imag();
                    if(zNorm > squaredBail)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if(i > 0) insideSet = false;
                    }
                    z = pow(z,n) + k;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;
        complex<double> z, k;
        k = complex<double>(kReal, kImaginary);
        double squaredBail = bailout*bailout;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = n + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = n + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}
void RenderJuliaZN::SpecialRender()
{
    double Z_re2;
    double Z_im2;
    double c_im;
    bool insideSet;
    complex<double> z, k;
    k = complex<double>(kReal, kImaginary);
    double squaredBail = bailout*bailout;
    double log2 = log(2.0);
    unsigned i;
    int out;

    if(myOpt.orbitTrapMode)
    {
        double distX, distY;
        int iterations;
        double bailFourPower = squaredBail*squaredBail;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                distX = abs(z.real());
                distY = abs(z.imag());

                insideSet = true;
                iterations = 0;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z, n) + k;
                    Z_re2 = z.real()*z.real();
                    Z_im2 = z.imag()*z.imag();
                    if(Z_re2 + Z_im2 > squaredBail)
                    {
                        insideSet = false;
                        if(Z_re2 + Z_im2 > bailFourPower)
                        {
                            if(abs(z.imag()) < distY) distY = abs(z.imag());
                            if(abs(z.real()) < distX) distX = abs(z.real());
                            break;
                        }
                    }
                    else iterations = i;

                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }
                if(distX == 0) distX = 0.000001;
                if(distY == 0) distY = 0.000001;

                if(insideSet)
                    setMap[x][y] = true;
                if(myOpt.smoothRender)
                {
                    out = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                    if(out < 0) out = 0;
                    if(!insideSet)
                        colorMap[x][y] = out;
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(abs(iterations + log(1/distX) + log(1/distY)));
            }
        }
    }
    else if(myOpt.smoothRender)
    {
        double squaredBail = bailout*bailout;
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                out = static_cast<int>(abs(4.0*(i -  log(log(z.real()*z.real()+z.imag()*z.imag()))/log2)));
                if(out < 0) out = 0;
                colorMap[x][y] = out;
            }
        }
    }
}
void RenderJuliaZN::SetParams(int _n, double _bailout)
{
    n = _n;
    bailout = _bailout;
}

// JuliaZN
JuliaZN::JuliaZN(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.77437;
    maxX = 1.6912;
    minY = -1.06769;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::JuliaZN;
    kReal = -0.754696;
    kImaginary = -0.0524231;
    hasOrbit = true;
    juliaVariety = true;
    hasOrbitTrap = true;
    hasSmoothRender = true;
    smoothRender = false;
    colorPaletteMode = ColorMode::Gaussian;
    myRender = new RenderJuliaZN[threadNumber];
    SetWatchdog<RenderJuliaZN>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("2"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    n = 2;
    bailout = 2;
}
JuliaZN::JuliaZN(int width, int height) : Fractal(width, height)
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

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkInt(PanelOptionType::Spin, wxT("n: "), &n, wxT("2"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    n = 2;
    bailout = 2;

    hasOrbitTrap = true;
    hasSmoothRender = true;
    juliaVariety = true;
    type = FractalType::JuliaZN;
    myRender = new RenderJuliaZN[threadNumber];
    SetWatchdog<RenderJuliaZN>(myRender, &watchdog, threadNumber);
}
JuliaZN::~JuliaZN()
{
    this->StopRender();
    delete[] myRender;
}
void JuliaZN::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> k(kReal, kImaginary);
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned i=0; i<maxIter; i++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > bailout*bailout)
        {
            outOfSet = true;
            break;
        }
        z = pow(z, n) + k;
    }

    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void JuliaZN::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(n, bailout);
    this->TRender<RenderJuliaZN>(myRender);
}
void JuliaZN::CopyOptFromPanel()
{
    n = *panelOpt.GetIntElement(0);
    bailout = *panelOpt.GetDoubleElement(0);
}

// RenderNewton
RenderNewton::RenderNewton()
{

}
void RenderNewton::Render()
{
    double re, im;
    double tmp;
    double d;
    double re2, im2;
    double prev_re, prev_im;

    // Creates fractal.
    unsigned n;
    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            prev_re = re = minX + x*xFactor;
            prev_im = im = maxY - y*yFactor;

            for(n=0; n<maxIter; n++)
            {
                re2 = re*re;
                im2 = im*im;
                d = 3.0*((re2 - im2)*(re2 - im2) + 4.0*re2*im2);
                if(d == 0.0)
                    d = 0.000001;

                tmp = re;
                re = (2.0/3.0)*re + (re2 - im2)/d;
                im = (2.0/3.0)*im - 2.0*tmp*im/d;

                if((prev_re - minStep < re && prev_re + minStep > re) && (prev_im - minStep < im && prev_im + minStep > im))
                {
                    break;
                }
                else
                {
                    prev_re = re;
                    prev_im = im;
                }
            }
            if(re <= 0 && im >= 0)
                colorMap[x][y] = 1 + n;
            else if(re <= 0 && im < 0)
                colorMap[x][y] = 17 + n;
            else
                colorMap[x][y] = 37 + n;
        }
    }
}
void RenderNewton::SpecialRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z, z_prev;
    double distX, distY;
    double re, im;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            re = minX + x*xFactor;
            im = maxY - y*yFactor;
            z_prev = z = complex<double>(re, im);

            distX = abs(re);
            distY = abs(im);

            for(n=0; n<maxIter; n++)
            {
                z = z - (pow(z, 3) - complex<double>(1, 0))/(complex<double>(2, 0)*pow(z,2));

                if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
                    && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                    break;
                else
                    z_prev = z;

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }
            }
            if(myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(n + log(1/distX) + log(1/distY));
            else
                colorMap[x][y] = n;
        }
    }
}
void RenderNewton::SetParams(double _minStep)
{
    minStep = _minStep;
}

// Newton
Newton::Newton(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.5721;
    maxX = 1.4086;
    minY = -1;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::NewtonRaphsonMethod;
    hasOrbit = true;
    hasOrbitTrap = true;
    myRender = new RenderNewton[threadNumber];
    SetWatchdog<RenderNewton>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
Newton::Newton(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.5721;
    maxX = 1.4086;
    minY = -1;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    type = FractalType::NewtonRaphsonMethod;
    myRender = new RenderNewton[threadNumber];
    SetWatchdog<RenderNewton>(myRender, &watchdog, threadNumber);
}
Newton::~Newton()
{
    this->StopRender();
    delete[] myRender;
}
void Newton::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> anterior;
    const double minStep = 0.001;

    if(orbitX != 0 && orbitY != 0)
    {
        for(unsigned n=0; n<maxIter; n++)
        {
            anterior = z;
            z = z - (pow(z, 3) - complex<double>(1, 0))/(complex<double>(2, 0)*pow(z,2));

            this->DrawLine(anterior.real(), anterior.imag(), z.real(), z.imag(), sf::Color(0,255,0), true);

            if((anterior.real() - minStep < z.real() && anterior.real() + minStep > z.real())
                && (anterior.imag() - minStep < z.imag() && anterior.imag() + minStep > z.imag()))
                break;
        }
    }

    orbitDrawn = true;
}
void Newton::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderNewton>(myRender);
}
void Newton::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

// RenderSinoidal
RenderSinoidal::RenderSinoidal()
{

}
void RenderSinoidal::Render()
{
    // Creates fractal.
    unsigned n;
    complex<double> z;
    complex<double> k(kReal, kImaginary);
    bool insideSet;
    double c_im;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = k*sin(z);
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double zNorm;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    zNorm = z.real()*z.real() + z.imag()*z.imag();
                    if(zNorm > maxIter)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    z = k*sin(z);

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    z = k*sin(z);
                    if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = n + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = n + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}
void RenderSinoidal::SpecialRender()
{
    // Creates fractal.
    complex<double> z;
    complex<double> constant(kReal, kImaginary);
    double distX, distY;
    double re, im;
    bool broken;
    bool insideSet;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            re = minX + x*xFactor;
            im = maxY - y*yFactor;
            z = complex<double>(re, im);
            broken = false;

            distX = abs(re);
            distY = abs(im);

            insideSet = true;
            int iterations = 0;

            for(unsigned n=0; n<maxIter; n++)
            {
                z = constant*sin(z);
                if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
                {
                    insideSet = false;
                    broken = true;
                }

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }

                if(!broken) iterations = n;
            }
            if(insideSet)
                setMap[x][y] = true;
            if(myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(abs(iterations + log(1/distX) + log(1/distY)));
            else
                colorMap[x][y] = iterations;
        }
    }
}

// Sinoidal
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

// RenderMagnet
RenderMagnet::RenderMagnet()
{

}
void RenderMagnet::Render()
{
    // Creates fractal.
    complex<double> z;
    complex<double> c;
    unsigned n;
    bool insideSet;
    double c_im;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(0, 0);
                c = complex<double>(minX + x*xFactor, c_im);

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {

                    if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow((pow(z, 2) + c - complex<double>(1.0, 0.0))/(complex<double>(2.0, 0.0)*z + c - complex<double>(2.0, 0.0)), 2.0);
                }
                if(insideSet) setMap[x][y] = true;
                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(0, 0);
                c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow((pow(z, 2) + c - complex<double>(1.0, 0.0))/(complex<double>(2.0, 0.0)*z + c - complex<double>(2.0, 0.0)), 2.0);
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = n + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = n + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}

// Magnet
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

// RenderMedusa
RenderMedusa::RenderMedusa()
{

}
void RenderMedusa::Render()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    complex<double> z, k;
    k = complex<double>(kReal, kImaginary);
    double c_im;
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow(z, 1.5) + k;
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double zNorm;
        double c_im;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    zNorm = z.real()*z.real() + z.imag()*z.imag();
                    if(zNorm > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    z = pow(z, 1.5) + k;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow(z, 1.5) + k;
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
                if(z.real() > 0 && z.imag() > 0)
                {
                    colorMap[x][y] = n + color1;
                }
                else if(z.real() <= 0 && z.imag() > 0)
                {
                    colorMap[x][y] = n + color2;
                }
                else if(z.real() <= 0 && z.imag() < 0)
                {
                    colorMap[x][y] = n + color3;
                }
                else
                {
                    colorMap[x][y] = n + color4;
                }
            }
        }
    }
}
void RenderMedusa::SpecialRender()
{
    // Creates fractal.
    complex<double> z;
    complex<double> constant(kReal, kImaginary);
    double distX, distY;
    double re, im;
    bool broken;
    bool insideSet;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            re = minX + x*xFactor;
            im = maxY - y*yFactor;
            z = complex<double>(re, im);
            broken = false;

            distX = abs(re);
            distY = abs(im);

            insideSet = true;
            int iterations = 0;

            for(unsigned n=0; n<maxIter; n++)
            {
                z = pow(z, 1.5) + constant;
                if(z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    broken = true;
                }

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }

                if(!broken) iterations = n;
            }
            if(insideSet)
            {
                setMap[x][y] = true;
            }
            if(myOpt.orbitTrapMode)
            {
                colorMap[x][y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            }
            else
            {
                colorMap[x][y] = iterations;
            }
        }
    }
}

// Medusa
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

// RenderManowar
RenderManowar::RenderManowar()
{

}
void RenderManowar::Render()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    double z_re, z_im, z_re2, z_im2, man_re, man_im;
    double c_re, c_im, temp_re, temp_im;
    double z_y_init;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if(z_re2 + z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + c_im + man_im;
                    z_re = z_re2 - z_im2 + c_re + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;

                    if(z_re2 + z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(z_re2 + z_im2))))/log2 + 1;
                        insideSet = false;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + c_im + man_im;
                    z_re = z_re2 - z_im2 + c_re + man_re;
                    man_re = temp_re;
                    man_im = temp_im;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z_re, z_im));
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if(z_re2 + z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + c_im + man_im;
                    z_re = z_re2 - z_im2 + c_re + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z_re > 0 && z_im > 0)
                    colorMap[x][y] = n + color1;
                else if(z_re <= 0 && z_im > 0)
                    colorMap[x][y] = n + color2;
                else if(z_re <= 0 && z_im < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}
void RenderManowar::SpecialRender()
{
    // Creates fractal.
    bool insideSet;
    double z_re, z_im, z_re2, z_im2, man_re, man_im;
    double c_re, c_im, temp_re, temp_im;

    double distX, distY;
    bool broken;

    for(y=ho; y<hf; y++)
    {
        c_im = maxY - y*yFactor;
        for(x=wo; x<wf; x++)
        {
            man_re = z_re = c_re = minX + x*xFactor;
            man_im = z_im = c_im;
            broken = false;

            distX = abs(c_re);
            distY = abs(c_im);

            insideSet = true;
            int iterations = 0;

            for(unsigned n=0; n<maxIter; n++)
            {
                z_re2 = z_re*z_re;
                z_im2 = z_im*z_im;
                if(z_re2 + z_im2 > 4)
                {
                    insideSet = false;
                    broken = true;
                }
                temp_re = z_re;
                temp_im = z_im;
                z_im = 2*z_re*z_im + c_im + man_im;
                z_re = z_re2 - z_im2 + c_re + man_re;
                man_re = temp_re;
                man_im = temp_im;

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z_im) < distY) distY = abs(z_im);
                    if(abs(z_re) < distX) distX = abs(z_re);
                }

                if(!broken) iterations = n;
            }
            if(distX == 0) distX = 0.000001;
            if(distY == 0) distY = 0.000001;

            if(insideSet)
                setMap[x][y] = 1;

            if(myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            else
                colorMap[x][y] = iterations;
        }
    }
}

// Manowar
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

// RenderManowarJulia
RenderManowarJulia::RenderManowarJulia()
{

}
void RenderManowarJulia::Render()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    double z_re, z_im, z_re2, z_im2, man_re, man_im;
    double temp_re, temp_im;
    double z_y_init;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = minX + x*xFactor;
                man_im = z_im = z_y_init;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if(z_re2 + z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + kImaginary + man_im;
                    z_re = z_re2 - z_im2 + kReal + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double temp_im;

        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = minX + x*xFactor;
                man_im = z_im = z_y_init;

                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;

                    if(z_re2 + z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(z_re2 + z_im2))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + kImaginary + man_im;
                    z_re = z_re2 - z_im2 + kReal + man_re;
                    man_re = temp_re;
                    man_im = temp_im;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z_re, z_im));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = minX + x*xFactor;
                man_im = z_im = z_y_init;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if(z_re2 + z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + kImaginary + man_im;
                    z_re = z_re2 - z_im2 + kReal + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z_re > 0 && z_im > 0)
                    colorMap[x][y] = n + color1;
                else if(z_re <= 0 && z_im > 0)
                    colorMap[x][y] = n + color2;
                else if(z_re <= 0 && z_im < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}
void RenderManowarJulia::SpecialRender()
{
    // Creates fractal.
    complex<double> z;
    complex<double> man;
    complex<double> temp;
    complex<double> constant;
    bool insideSet;
    int iterations;

    double distX, distY;
    double re, im;
    bool broken;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            re = minX + x*xFactor;
            im = maxY - y*yFactor;
            man = z = complex<double>(re, im);
            constant = complex<double>(kReal, kImaginary);
            broken = false;

            distX = abs(re);
            distY = abs(im);

            insideSet = true;
            iterations = 0;

            for(unsigned n=0; n<maxIter; n++)
            {
                if(z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    broken = true;
                }

                temp = z;
                z = pow(z, 2) + man + constant;
                man = temp;

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }

                if(!broken) iterations = n;
            }
            if(insideSet)
                setMap[x][y] = true;

            if(distX == 0) distX = 0.000001;
            if(distY == 0) distY = 0.000001;

            if(myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            else
                colorMap[x][y] = iterations;
        }
    }
}

// ManowarJulia
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

// RenderSierpTriangle
RenderSierpTriangle::RenderSierpTriangle()
{

}
void RenderSierpTriangle::Render()
{
    complex<double> z;
    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            z = complex<double>(minX + x*xFactor, maxY - y*yFactor);

            bool insideSet = true;
            int iterations = 0;
            for(unsigned n=0; n<maxIter; n++)
            {
                if(z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    break;
                }

                if(z.imag() > 0.5)
                    z = complex<double>(2, 0)*z - complex<double>(0, 1);
                else if(z.real() > 0.5)
                    z = complex<double>(2, 0)*z - complex<double>(1, 0);
                else
                    z = complex<double>(2, 0)*z;

                iterations = n;
            }
            if(insideSet)
                setMap[x][y] = true;

            colorMap[x][y] = iterations;
        }
    }
}

// SierpTriangle
SierpTriangle::SierpTriangle(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = -0.5;
    maxX = 1.5;
    minY = -0.2;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    myRender = new RenderSierpTriangle[threadNumber];
    SetWatchdog<RenderSierpTriangle>(myRender, &watchdog, threadNumber);

    type = FractalType::SierpinskyTriangle;
}
SierpTriangle::SierpTriangle(int width, int height) : Fractal(width, height)
{
    minX = -0.5;
    maxX = 1.5;
    minY = -0.2;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    renderJobComp = false;
    type = FractalType::SierpinskyTriangle;
    myRender = new RenderSierpTriangle[threadNumber];
    SetWatchdog<RenderSierpTriangle>(myRender, &watchdog, threadNumber);
}
void SierpTriangle::Render()
{
    this->TRender<RenderSierpTriangle>(myRender);
}

// RenderFixedPoint1
RenderFixedPoint1::RenderFixedPoint1()
{
    minStep = 0.01;
}
void RenderFixedPoint1::Render()
{
    complex<double> z;
    complex<double> z_prev;
    unsigned n;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            z_prev = z = complex<double>(minX + x*xFactor, maxY - y*yFactor);

            for(n=0; n<maxIter; n++)
            {
                z = sin(z);

                if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
                    && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                    break;
                else
                    z_prev = z;
            }
            if(z.real() > 0) colorMap[x][y] = 1 + n;
            else colorMap[x][y] = 30 + n;
        }
    }
}
void RenderFixedPoint1::SetParams(double _minStep)
{
    minStep = _minStep;
}

// FixedPoint1
FixedPoint1::FixedPoint1(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -6.15385;
    maxX = 5.84615;
    minY = -4;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    hasOrbit = true;

    type = FractalType::FixedPoint1;
    myRender = new RenderFixedPoint1[threadNumber];
    SetWatchdog<RenderFixedPoint1>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FixedPoint1::FixedPoint1(int width, int height) : Fractal(width, height)
{
    minStep = 0.01;
    minX = -6.15385;
    maxX = 5.84615;
    minY = -4;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::FixedPoint1;
    myRender = new RenderFixedPoint1[threadNumber];
    SetWatchdog<RenderFixedPoint1>(myRender, &watchdog, threadNumber);
}
FixedPoint1::~FixedPoint1()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint1::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderFixedPoint1>(myRender);
}
void FixedPoint1::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> z_prev;

    vector< complex<double> > zVector;
    bool outOfSet = false;

    for (unsigned n = 0; n < maxIter; n++)
    {
        zVector.push_back(z);
        z = sin(z);

        if ((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
            && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
            break;
        else
            z_prev = z;
    }

    sf::Color color;
    if (outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for (unsigned int i = 0; i < zVector.size() - 1; i++)
    {
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i + 1].real(), zVector[i + 1].imag(), color, true);
    }
    orbitDrawn = true;
}
void FixedPoint1::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

// RenderFixedPoint2
RenderFixedPoint2::RenderFixedPoint2()
{
    minStep = 0.01;
}
void RenderFixedPoint2::Render()
{
    complex<double> z;
    complex<double> z_prev;
    unsigned n;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            z_prev = z = complex<double>(minX + x*xFactor, maxY - y*yFactor);

            for(n=0; n<maxIter; n++)
            {
                z = cos(z);

                if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
                    && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                    break;
                else
                    z_prev = z;
            }
            if(z.real() > 0) colorMap[x][y] = 1 + n;
            else colorMap[x][y] = 30 + n;
        }
    }
}
void RenderFixedPoint2::SetParams(double _minStep)
{
    minStep = _minStep;
}

// FixedPoint2
FixedPoint2::FixedPoint2(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -5.88462;
    maxX = 6.11538;
    minY = -4;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    hasOrbit = true;
    type = FractalType::FixedPoint2;
    myRender = new RenderFixedPoint2[threadNumber];
    SetWatchdog<RenderFixedPoint2>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FixedPoint2::FixedPoint2(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -5.88462;
    maxX = 6.11538;
    minY = -4;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::FixedPoint2;
    myRender = new RenderFixedPoint2[threadNumber];
    SetWatchdog<RenderFixedPoint2>(myRender, &watchdog, threadNumber);
}
FixedPoint2::~FixedPoint2()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint2::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderFixedPoint2>(myRender);
}
void FixedPoint2::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> z_prev;
    double minStep = 0.001;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        z = cos(z);

        if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
            && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
            break;
        else
            z_prev = z;
    }

    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void FixedPoint2::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

// RenderFixedPoint3
RenderFixedPoint3::RenderFixedPoint3()
{
    minStep = 0.01;
}
void RenderFixedPoint3::Render()
{
    complex<double> z;
    complex<double> z_prev;
    unsigned n;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            z_prev = z = complex<double>(minX + x*xFactor, maxY - y*yFactor);

            for(n=0; n<maxIter; n++)
            {
                z = tan(z);

                if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
                    && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                    break;
                else
                    z_prev = z;
            }
            if(z.real() > 0) colorMap[x][y] = 1 + n;
            else colorMap[x][y] = 30 + n;
        }
    }
}
void RenderFixedPoint3::SetParams(double _minStep)
{
    minStep = _minStep;
}

// FixedPoint3
FixedPoint3::FixedPoint3(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -3.76339;
    maxX = 3.59018;
    minY = -2.39204;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    hasOrbit = true;

    type = FractalType::FixedPoint3;
    myRender = new RenderFixedPoint3[threadNumber];
    SetWatchdog<RenderFixedPoint3>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FixedPoint3::FixedPoint3(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -3.76339;
    maxX = 3.59018;
    minY = -2.39204;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::FixedPoint3;
    myRender = new RenderFixedPoint3[threadNumber];
    SetWatchdog<RenderFixedPoint3>(myRender, &watchdog, threadNumber);
}
FixedPoint3::~FixedPoint3()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint3::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderFixedPoint3>(myRender);
}
void FixedPoint3::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> z_ant;
    double minStep = 0.001;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        z = tan(z);

        if((z_ant.real() - minStep < z.real() && z_ant.real() + minStep > z.real())
            && (z_ant.imag() - minStep < z.imag() && z_ant.imag() + minStep > z.imag()))
        {
            break;
        }
        else
        {
            z_ant = z;
        }
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
void FixedPoint3::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

// RenderFixedPoint4
RenderFixedPoint4::RenderFixedPoint4()
{
    minStep = 0.01;
}
void RenderFixedPoint4::Render()
{
    complex<double> z;
    complex<double> z_prev;
    unsigned n;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            z_prev = z = complex<double>(minX + x*xFactor, maxY - y*yFactor);

            for(n=0; n<maxIter; n++)
            {
                z = pow(z,2);

                if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
                    && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                    break;
                else
                    z_prev = z;
            }
            if(z.real() > 0) colorMap[x][y] = 1 + n;
            else colorMap[x][y] = 30 + n;
        }
    }
}
void RenderFixedPoint4::SetParams(double _minStep)
{
    minStep = _minStep;
}

// FixedPoint4
FixedPoint4::FixedPoint4(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -1.8713;
    maxX = 1.82101;
    minY = -1.22781;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    hasOrbit = true;

    type = FractalType::FixedPoint4;
    myRender = new RenderFixedPoint4[threadNumber];
    SetWatchdog<RenderFixedPoint4>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(minStepTxt), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FixedPoint4::FixedPoint4(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.8713;
    maxX = 1.82101;
    minY = -1.22781;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::FixedPoint4;
    myRender = new RenderFixedPoint4[threadNumber];
    SetWatchdog<RenderFixedPoint4>(myRender, &watchdog, threadNumber);
}
FixedPoint4::~FixedPoint4()
{
    this->StopRender();
    delete[] myRender;
}
void FixedPoint4::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(minStep);
    this->TRender<RenderFixedPoint4>(myRender);
}
void FixedPoint4::DrawOrbit()
{
    complex<double> z(orbitX, orbitY);
    complex<double> z_ant;
    double minStep = 0.001;
    vector< complex<double> > zVector;
    bool outOfSet = false;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        z = pow(z,2);

        if((z_ant.real() - minStep < z.real() && z_ant.real() + minStep > z.real())
            && (z_ant.imag() - minStep < z.imag() && z_ant.imag() + minStep > z.imag()))
            break;
        else
            z_ant = z;
    }

    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}

void FixedPoint4::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}

// RenderTricorn
RenderTricorn::RenderTricorn()
{

}
void RenderTricorn::Render()
{
    unsigned n;
    double Z_re, Z_im, Z_re2, Z_im2;
    double c_re, c_im;
    bool insideSet;

    // Creates fractal.
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = -Z_im;
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = Z_im = 0;
                c_re = minX + x*xFactor;
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        insideSet = false;
                    }
                    Z_im = -Z_im;
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(Z_re, Z_im));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = -Z_im;
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(Z_re > 0 && Z_im > 0)
                    colorMap[x][y] = n + color1;
                else if(Z_re <= 0 && Z_im > 0)
                    colorMap[x][y] = n + color2;
                else if(Z_re <= 0 && Z_im < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}

// Tricorn
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

// RenderBurningShip
RenderBurningShip::RenderBurningShip()
{

}
void RenderBurningShip::Render()
{
    unsigned n;
    double Z_re, Z_im, Z_re2, Z_im2;
    double c_re, c_im;
    bool insideSet;

    // Creates fractal.
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = 0;
                Z_im = 0;
                c_re = minX + x*xFactor;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = abs(Z_im);
                    Z_re = abs(Z_re);
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = Z_im = 0;
                c_re = minX + x*xFactor;
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        insideSet = false;
                    }
                    Z_im = abs(Z_im);
                    Z_re = abs(Z_re);
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(Z_re, Z_im));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = 0;
                Z_im = 0;
                c_re = minX + x*xFactor;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = abs(Z_im);
                    Z_re = abs(Z_re);
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(Z_re > 0 && Z_im > 0)
                    colorMap[x][y] = n + color1;
                else if(Z_re <= 0 && Z_im > 0)
                    colorMap[x][y] = n + color2;
                else if(Z_re <= 0 && Z_im < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}

// BurningShip
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

// RenderBurningShipJulia
RenderBurningShipJulia::RenderBurningShipJulia()
{

}
void RenderBurningShipJulia::Render()
{
    unsigned n;
    double Z_re, Z_im, Z_re2, Z_im2;
    double c_re, c_im;
    bool insideSet;
    c_im = kImaginary;
    c_re = kReal;

    // Creates fractal.
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                Z_re = maxY - y*yFactor;
                Z_im = minX + x*xFactor;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = abs(Z_im);
                    Z_re = abs(Z_re);
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                Z_re = minX + x*xFactor;
                Z_im = maxY - y*yFactor;
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    Z_im = abs(Z_im);
                    Z_re = abs(Z_re);
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(Z_re, Z_im));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                Z_re = minX + x*xFactor;
                Z_im = maxY - y*yFactor;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = abs(Z_im);
                    Z_re = abs(Z_re);
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(Z_re > 0 && Z_im > 0)
                    colorMap[x][y] = n + color1;
                else if(Z_re <= 0 && Z_im > 0)
                    colorMap[x][y] = n + color2;
                else if(Z_re <= 0 && Z_im < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}

// BurningShipJulia
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

// RenderFractory
RenderFractory::RenderFractory()
{

}
void RenderFractory::Render()
{
    // Creates fractal.
    unsigned n;
    complex<double> z, b, c;
    double c_im;
    bool insideSet;
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                c = complex<double>(minX + x*xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {

                c = complex<double>(minX + x*xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    if(z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(z.real()*z.real()+ z.imag()*z.imag()))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                c = complex<double>(minX + x*xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = n + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = n + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}

// Fractory
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

// RenderCell
RenderCell::RenderCell()
{
    bailout = 1.0;
}
void RenderCell::Render()
{
    // Creates fractal.
    unsigned n;
    double squaredBail = bailout*bailout;
    complex<double> z, b, c;
    double c_im;
    bool insideSet;
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                c = complex<double>(minX + x*xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real()+ z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                    b /= c;
                    z = z*c + b/z;
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        unsigned n;
        double distance, distance1;
        double mu, norm;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                c = complex<double>(minX + x*xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    norm = z.real()*z.real()+ z.imag()*z.imag();
                    if(norm > squaredBail)
                    {
                        mu = (loglog2 - log(log(sqrt(norm))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    b /= c;
                    z = z*c + b/z;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
}
void RenderCell::SetParams(double _bailout)
{
    bailout = _bailout;
}

// Cell
Cell::Cell(sf::RenderWindow* Window) : Fractal(Window)
{
    minX = -1.95533;
    maxX = 1.91967;
    minY = -1.2495;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    hasOrbit = true;
    type = FractalType::Cell;
    myRender = new RenderCell[threadNumber];
    SetWatchdog<RenderCell>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    bailout = 2;

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::GaussianInt);
}
Cell::Cell(int width, int height) : Fractal(width, height)
{
    minX = -1.3;
    maxX = 1.3;
    minY = -1.2495;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(bailoutTxt), &bailout, wxT("2"));
    bailout = 2;

    hasOrbit = true;
    alg = RenderingAlgorithm::EscapeTime;
    type = FractalType::Cell;
    myRender = new RenderCell[threadNumber];
    SetWatchdog<RenderCell>(myRender, &watchdog, threadNumber);
}
Cell::~Cell()
{
    this->StopRender();
    delete[] myRender;
}
void Cell::Render()
{
    for(unsigned int i=0; i<threadNumber; i++) myRender[i].SetParams(bailout);
    this->TRender<RenderCell>(myRender);
}
void Cell::DrawOrbit()
{
    complex<double> c(orbitX, orbitY);
    complex<double> b, z;
    z = c;
    b = c - sin(c);

    vector< complex<double> > zVector;
    bool outOfSet = false;
    double squaredBail = bailout*bailout;

    for(unsigned n=0; n<maxIter; n++)
    {
        zVector.push_back(z);
        if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
        {
            outOfSet = true;
            break;
        }
        b /= c;
        z = z*c + b/z;
    }
    sf::Color color;
    if(outOfSet) color = sf::Color(255, 0, 0);
    else color = sf::Color(0, 255, 0);

    for(unsigned int i=0; i<zVector.size()-1; i++)
        this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

    orbitDrawn = true;
}
void Cell::CopyOptFromPanel()
{
    bailout = *panelOpt.GetDoubleElement(0);
}

// RenderLogisticMap
RenderLogisticMap::RenderLogisticMap()
{
    progress = 0;
    seed = 0.0;
    stabilizePoint = false;
}
void RenderLogisticMap::Render()
{
    double a, x;
    int coordX, coordY;
    progress = 0;

    if(myOpt.alg == RenderingAlgorithm::ChaoticMap)
    {
        for(int i=0; i<myOpt.screenWidth; i++)
        {
            a = minX + i*xFactor;
            x = seed;
            if(stabilizePoint)
            {
                for(unsigned int n=0; n< maxIter; n++)
                {
                    x = a*x*(1-x);
                    progress++;
                }
            }
            for(unsigned int n=0; n<maxIter; n++)
            {
                x = a*x*(1-x);
                coordX = static_cast<int>((a-minX)/xFactor);
                coordY = static_cast<int>((maxY-x)/yFactor);
                if(coordX >= 0 && coordX < myOpt.screenWidth)
                {
                    if(coordY >= 0 && coordY < myOpt.screenHeight)
                        setMap[coordX][coordY] = 1;
                }
                progress++;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::Lyapunov)
    {
        double fPrimeSum;
        for(int i=0; i<myOpt.screenWidth; i++)
        {
            a = minX + i*xFactor;
            x = seed;
            fPrimeSum = 0;

            for(unsigned int n=0; n<maxIter; n++)
            {
                x = a*x*(1-x);
                fPrimeSum += log(abs(a*(1 - 2*x)));
                progress++;
            }
            coordX = static_cast<int>((a-minX)/xFactor);
            coordY = static_cast<int>((maxY-(fPrimeSum/(double)maxIter))/yFactor);
            if(coordX >= 0 && coordX < myOpt.screenWidth)
            {
                if(coordY >= 0 && coordY < myOpt.screenHeight)
                    setMap[coordX][coordY] = 1;
            }
        }
    }
}
void RenderLogisticMap::SetParams(double _seed, bool _stabilizePoint)
{
    seed = _seed;
    stabilizePoint = _stabilizePoint;
}
int RenderLogisticMap::AskProgress()
{
    if(!stopped)
    {
        if(myOpt.alg == RenderingAlgorithm::ChaoticMap && stabilizePoint)
            threadProgress = (100*progress)/(2*maxIter*myOpt.screenWidth);
        else
            threadProgress = (100*progress)/(maxIter*myOpt.screenWidth);
    }
    return threadProgress;
}


// LogisticMap
LogisticMap::LogisticMap(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = 2.8;
    maxX = 4;
    minY = 0.24;
    maxY = 1;
    this->SetOutermostZoom();

    type = FractalType::Logistic;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    this->SetExtColorMode(false);
    redrawAlways = true;
    threadNumber = 1;

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(seedTxt), &logisticSeed, wxT("0.25"));
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(stabilizeTxt), &stabilizePoint, wxT("true"));
    logisticSeed = 0.25;
    stabilizePoint = true;

    myRender = new RenderLogisticMap[1];
    SetWatchdog<RenderLogisticMap>(myRender, &watchdog, 1);

    // Specify algorithms.
    alg = RenderingAlgorithm::ChaoticMap;
    availableAlg.push_back(RenderingAlgorithm::ChaoticMap);
    availableAlg.push_back(RenderingAlgorithm::Lyapunov);
}
LogisticMap::LogisticMap(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = 2.8;
    maxX = 4;
    minY = 0.24;
    maxY = 1;
    this->SetOutermostZoom();
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    threadNumber = 1;

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(seedTxt), &logisticSeed, wxT("0.25"));
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(stabilizeTxt), &stabilizePoint, wxT("true"));
    logisticSeed = 0.25;
    stabilizePoint = true;
    renderJobComp = false;
    alg = RenderingAlgorithm::ChaoticMap;

    type = FractalType::Logistic;
    myRender = new RenderLogisticMap[1];
    SetWatchdog<RenderLogisticMap>(myRender, &watchdog, 1);
}
void LogisticMap::Render()
{
    watchdog.Reset();
    myRender[0].SetParams(logisticSeed, stabilizePoint);
    myRender[0].SetOpt(this->GetOptions());
    myRender[0].SetRenderOut(setMap, colorMap, auxMap);
    myRender[0].Launch();
    watchdog.Launch();
}
void LogisticMap::CopyOptFromPanel()
{
    logisticSeed = *panelOpt.GetDoubleElement(0);
    stabilizePoint = *panelOpt.GetBoolElement(0);
}

// RenderHenonMap
RenderHenonMap::RenderHenonMap()
{
    i = 0;
    alpha = beta = x0 = y0 = 0.0;
}
void RenderHenonMap::Render()
{
    double x = x0;
    double y = y0;
    double tempX;
    int coordX, coordY;

    if(myOpt.alg == RenderingAlgorithm::ChaoticMap)
    {
        for(i=0; i<maxIter; i++)
        {
            coordX = ((x-minX)/xFactor);
            coordY = ((maxY-y)/yFactor);
            if((coordX >= 0 && coordX < myOpt.screenWidth) && (coordY >= 0 && coordY < myOpt.screenHeight))
                setMap[coordX][coordY] = 1;

            tempX = x;
            x = y + 1 - alpha*x*x;
            y = beta*tempX;
        }
    }
}
void RenderHenonMap::SetParams(double _alpha, double _beta, double _x0, double _y0)
{
    alpha = _alpha;
    beta = _beta;
    x0 = _x0;
    y0 = _y0;
}
int RenderHenonMap::AskProgress()
{
    if(!stopped)
        threadProgress = 100*i/maxIter;

    return threadProgress;
}

// HenonMap
HenonMap::HenonMap(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -1.5;
    maxX = 1.5;
    minY = -0.96;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();
    maxIter = 20000;

    type = FractalType::HenonMap;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    this->SetExtColorMode(false);
    redrawAlways = true;
    threadNumber = 1;

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(alphaTxt), &alpha, wxT("1.4"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(betaTxt), &beta, wxT("0.3"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("X0: "), &x0, wxT("0.5"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Y0: "), &y0, wxT("0.2"));
    alpha = 1.4;
    beta = 0.3;
    x0 = 0.5;
    y0 = 0.2;

    myRender = new RenderHenonMap[1];
    SetWatchdog<RenderHenonMap>(myRender, &watchdog, 1);

    // Specify algorithms.
    alg = RenderingAlgorithm::ChaoticMap;
    availableAlg.push_back(RenderingAlgorithm::ChaoticMap);
}
HenonMap::HenonMap(int width, int height) : Fractal(width, height)
{
    type = FractalType::HenonMap;
    alg = RenderingAlgorithm::ChaoticMap;

    threadNumber = 1;
    renderJobComp = false;
    minX = -1.5;
    maxX = 1.5;
    minY = -0.4;
    maxY = 0.4;
    this->SetOutermostZoom();
    maxIter = 20000;
    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(alphaTxt), &alpha, wxT("1.4"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(betaTxt), &beta, wxT("0.3"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("X0: "), &x0, wxT("0.5"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Y0: "), &y0, wxT("0.2"));
    alpha = 1.4;
    beta = 0.3;
    x0 = 0.5;
    y0 = 0.2;

    myRender = new RenderHenonMap[1];
    SetWatchdog<RenderHenonMap>(myRender, &watchdog, 1);
}
HenonMap::~HenonMap()
{
    this->StopRender();
    delete[] myRender;
}
void HenonMap::Render()
{
    watchdog.Reset();
    myRender[0].SetParams(alpha, beta, x0, y0);
    myRender[0].SetOpt(this->GetOptions());
    myRender[0].SetRenderOut(setMap, colorMap, auxMap);
    myRender[0].Launch();
    watchdog.Launch();
}
void HenonMap::CopyOptFromPanel()
{
    alpha = *panelOpt.GetDoubleElement(0);
    beta = *panelOpt.GetDoubleElement(1);
    x0 = *panelOpt.GetDoubleElement(2);
    y0 = *panelOpt.GetDoubleElement(3);
}
void HenonMap::MoreIter()
{
    // Increases 1000 iterations.
    changeFractalIter = true;
    this->DeleteSavedZooms();
    redrawAll = true;
    maxIter += 1000;
}
void HenonMap::LessIter()
{
    // Decreases 1000 iterations.
    changeFractalIter = true;
    this->DeleteSavedZooms();
    redrawAll = true;
    int signedMaxIter = (int)maxIter;
    if(signedMaxIter - 1000 > 0)
        maxIter -= 1000;
}

// RenderDPendulum
RenderDPendulum::RenderDPendulum()
{

}
void RenderDPendulum::Render()
{
    //Crea fractal
    double th1, th2, vth1, vth2;
    double ecMov1, ecMov2;
    bool insideSet;
    unsigned n;
    double den;
    double minTh1 = minX;
    double maxTh2 = maxY;
    double th1Factor = xFactor;
    double th2Factor = yFactor;
    double th1_init, th2_init;

    double part0, part1, part2, part3, part4, part5, part6;
    part0 = m1+m2;
    part1 = -g*(2*m1+m2);
    part2 = 2*m2;
    part3 = m2*g;
    part4 = g*(m1+m2);
    part5 = l*2*m1+m2;
    part6 = l*m2;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        if(rungeKutta)
        {
            double k11, k12, k13, k14;
            double k21, k22, k23, k24;

            for(int y=ho; y<hf; y++)
            {
                for(int x=wo; x<wf; x++)
                {
                    th1 = minTh1 + x*th1Factor;
                    th2 = maxTh2 - y*th2Factor;
                    vth1 = 0;
                    vth2 = 0;
                    th1_init = th1;
                    th2_init = th2;

                    insideSet = true;
                    for(n=0; n<maxIter; n++)
                    {
                        den = part5 - part6*cos(2*(th1-th2));
                        k11 = part1*sin(th1)-part3*sin(th1-2*th2)-part2*sin(th1-th2)*(l*(pow(vth2,2)+pow(vth1,2)*cos(th1-th2)));
                        k11 /= den;
                        k11 *= dt;
                        k21 = 2*sin(th1-th2)*(part4*cos(th1)+l*(pow(vth2,2)*m2*cos(th1-th2)+pow(vth1,2)*part0));
                        k21 /= den;
                        k21 *= dt;

                        den = part5 - part6*cos(2*(th1 + k11/2.0 - (th2 + k21/2.0)));
                        k12 = part1*sin(th1 + k11/2.0)-part3*sin(th1 + k11/2.0-2*(th2 + k21/2.0))-part2*sin(th1 + k11/2.0-(th2 + k21/2.0))*(l*(pow(vth2,2)+pow(vth1,2)*cos(th1 + k11/2.0-(th2 + k21/2.0))));
                        k12 /= den;
                        k12 *= dt;
                        k22 = 2*sin((th1 + k11/2.0)-(th2 + k21/2.0))*(part4*cos((th1 + k11/2.0))+l*(pow(vth2,2)*m2*cos((th1 + k11/2.0)-(th2 + k21/2.0))+pow(vth1,2)*part0));
                        k22 /= den;
                        k22 *= dt;

                        den = part5 - part6*cos(2*(th1 + k12/2.0 - (th2 + k22/2.0)));
                        k13 = part1*sin(th1 + k12/2.0)-part3*sin(th1 + k12/2.0-2*(th2 + k22/2.0))-part2*sin(th1 + k12/2.0-(th2 + k22/2.0))*(l*(pow(vth2,2)+pow(vth1,2)*cos(th1 + k12/2.0-(th2 + k22/2.0))));
                        k13 /= den;
                        k13 *= dt;
                        k23 = 2*sin((th1 + k12/2.0)-(th2 + k22/2.0))*(part4*cos((th1 + k12/2.0))+l*(pow(vth2,2)*m2*cos((th1 + k12/2.0)-(th2 + k22/2.0))+pow(vth1,2)*part0));
                        k23 /= den;
                        k23 *= dt;

                        den = part5 - part6*cos(2*(th1 + k13/2.0 - (th2 + k23/2.0)));
                        k14 = part1*sin(th1 + k13/2.0)-part3*sin(th1 + k13/2.0-2*(th2 + k23/2.0))-part2*sin(th1 + k13/2.0-(th2 + k23/2.0))*(l*(pow(vth2,2)+pow(vth1,2)*cos(th1 + k13/2.0-(th2 + k23/2.0))));
                        k14 /= den;
                        k14 *= dt;
                        k24 = 2*sin((th1 + k13/2.0)-(th2 + k23/2.0))*(part4*cos((th1 + k13/2.0))+l*(pow(vth2,2)*m2*cos((th1 + k13/2.0)-(th2 + k23/2.0))+pow(vth1,2)*part0));
                        k24 /= den;
                        k24 *= dt;

                        vth1 += (k11/6) + (k12/3) + (k13/3) + (k14/6);
                        vth2 += (k21/6) + (k22/3) + (k23/3) + (k24/6);
                        th1 += dt*vth1;
                        th2 += dt*vth2;

                        if(referenced)
                        {
                            if((th1Bailout && (abs(th1-th1_init) > th1NumBailout)) || (th2Bailout && (abs(th2-th2_init) > th2NumBailout)))
                            {
                                insideSet = false;
                                break;
                            }
                        }
                        else
                        {
                            if((th1Bailout && (abs(th1) > th1NumBailout)) || (th2Bailout && (abs(th2) > th2NumBailout)))
                            {
                                insideSet = false;
                                break;
                            }
                        }
                    }
                    if(insideSet)
                        setMap[x][y] = true;
                    else
                        colorMap[x][y] = n;
                }
            }
        }
        else
        {
            double cosTh1MinTh2, sinTh1MinTh2;
            for(y=ho; y<hf && threadRunning; y++)
            {
                for(x=wo; x<wf; x++)
                {
                    th1 = minTh1 + x*th1Factor;
                    th2 = maxTh2 - y*th2Factor;
                    vth1 = 0;
                    vth2 = 0;
                    th1_init = th1;
                    th2_init = th2;

                    insideSet = true;
                    for(n=0; n<maxIter; n++)
                    {
                        cosTh1MinTh2 = cos(th1-th2);
                        sinTh1MinTh2 = sin(th1-th2);

                        den = part5 - part6*cos(2*(th1-th2));
                        ecMov1 = part1*sin(th1)-part3*sin(th1-2*th2)-part2*sinTh1MinTh2*(l*(vth2*vth2 + vth1*vth1*cosTh1MinTh2));
                        ecMov1 /= den;
                        ecMov2 = 2*sinTh1MinTh2*(part4*cos(th1)+l*(vth2*vth2*m2*cosTh1MinTh2 + vth1*vth1*(part0)));
                        ecMov2 /= den;
                        vth1 += dt*ecMov1;
                        vth2 += dt*ecMov2;
                        th1 += dt*vth1;
                        th2 += dt*vth2;

                        if(referenced)
                        {
                            if((th1Bailout && (abs(th1-th1_init) > th1NumBailout)) || (th2Bailout && (abs(th2-th2_init) > th2NumBailout)))
                            {
                                insideSet = false;
                                break;
                            }
                        }
                        else
                        {
                            if((th1Bailout && (abs(th1) > th1NumBailout)) || (th2Bailout && (abs(th2) > th2NumBailout)))
                            {
                                insideSet = false;
                                break;
                            }
                        }
                    }

                    if(!threadRunning) break;

                    if(insideSet)
                        setMap[x][y] = true;

                    colorMap[x][y] = n;
                }
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                th1 = minTh1 + x*th1Factor;
                th2 = maxTh2 - y*th2Factor;
                vth1 = 0;
                vth2 = 0;
                th1_init = th1;
                th2_init = th2;

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    den = part5 - part6*cos(2*(th1-th2));
                    ecMov1 = part1*sin(th1)-part3*sin(th1-2*th2)-part2*sin(th1-th2)*(l*(pow(vth2,2)+pow(vth1,2)*cos(th1-th2)));
                    ecMov1 /= den;
                    ecMov2 = 2*sin(th1-th2)*(part4*cos(th1)+l*(pow(vth2,2)*m2*cos(th1-th2)+pow(vth1,2)*(part0)));
                    ecMov2 /= den;
                    vth1 += dt*ecMov1;
                    vth2 += dt*ecMov2;
                    th1 += dt*vth1;
                    th2 += dt*vth2;

                    if(referenced)
                    {
                        if((th1Bailout && (abs(th1-th1_init) > th1NumBailout)) || (th2Bailout && (abs(th2-th2_init) > th2NumBailout)))
                        {
                            insideSet = false;
                            break;
                        }
                    }
                    else
                    {
                        if((th1Bailout && (abs(th1) > th1NumBailout)) || (th2Bailout && (abs(th2) > th2NumBailout)))
                        {
                            insideSet = false;
                            break;
                        }
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(th1 > 0 && th2 > 0)
                    colorMap[x][y] = color1;
                else if(th1 <= 0 && th2 > 0)
                    colorMap[x][y] = color2;
                else if(th1 <= 0 && th2 < 0)
                    colorMap[x][y] = color3;
                else
                    colorMap[x][y] = color4;
            }
        }
    }
}
void RenderDPendulum::SetParams(bool th1B, bool th2B, double th1NB, double th2NB,
                    double _dt, double _m1, double _m2, double _l, double _g, bool ref, bool _rungeKutta)
{
    th1Bailout = th1B;
    th2Bailout = th2B;
    th1NumBailout = th1NB;
    th2NumBailout = th2NB;
    dt = _dt;
    m1 = _m1;
    m2 = _m2;
    l = _l;
    g = _g;
    referenced = ref;
    rungeKutta = _rungeKutta;
}

// DPendulum
DPendulum::DPendulum(sf::RenderWindow* Window):Fractal(Window)
{
    // Adjust the scale.
    minX = -5.57488;
    maxX = 5.43961;
    minY = -3.57502;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    maxIter = 4000;
    hasOrbit = true;

    type = FractalType::DoublePendulum;
    myRender = new RenderDPendulum[threadNumber];
    SetWatchdog<RenderDPendulum>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(th1BailoutTxt), &th1Bailout, wxT("true"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(valueTxt), &th1NumBailout, wxT("3.14159"));
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(th2BailoutTxt), &th2Bailout, wxT("true"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(valueTxt), &th2NumBailout, wxT("3.14159"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(deltaTTxt), &dt, wxT("0.005"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("m1: "), &m1, wxT("10"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("m2: "), &m2, wxT("10"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(longitudeTxt), &l, wxT("20"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(gravityTxt), &g, wxT("9.81"));
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(relAnglesTxt), &referenced, wxT("false"));
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(" Runge-Kutta"), &rungeKutta, wxT("false"));
    th1Bailout = true;
    th2Bailout = true;
    th1NumBailout = 3.14159;
    th2NumBailout = 3.14159;
    dt = 0.005;
    m1 = 10;
    m2 = 10;
    l = 20;
    g = 9.81;
    referenced = false;
    rungeKutta = false;

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
    availableAlg.push_back(RenderingAlgorithm::EscapeAngle);
}
DPendulum::DPendulum(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -3.5;
    maxX = 3.5;
    minY = -3.5;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);
    maxIter = 4000;

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(th1BailoutTxt), &th1Bailout, wxT("true"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(valueTxt), &th1NumBailout, wxT("3.14159"));
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(th2BailoutTxt), &th2Bailout, wxT("true"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(valueTxt), &th2NumBailout, wxT("3.14159"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(deltaTTxt), &dt, wxT("0.005"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("m1: "), &m1, wxT("10"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("m2: "), &m2, wxT("10"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(longitudeTxt), &l, wxT("20"));
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT(gravityTxt), &g, wxT("9.81"));
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(relAnglesTxt), &referenced, wxT("false"));
    panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(" Runge-Kutta"), &rungeKutta, wxT("false"));
    th1Bailout = true;
    th2Bailout = true;
    th1NumBailout = 3.14159;
    th2NumBailout = 3.14159;
    dt = 0.005;
    m1 = 10;
    m2 = 10;
    l = 20;
    g = 9.81;
    referenced = false;
    rungeKutta = false;

    alg = RenderingAlgorithm::EscapeTime;
    type = FractalType::DoublePendulum;
    myRender = new RenderDPendulum[threadNumber];
    SetWatchdog<RenderDPendulum>(myRender, &watchdog, threadNumber);
}
DPendulum::~DPendulum()
{
    this->StopRender();
    delete[] myRender;
}
void DPendulum::Render()
{
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetParams(th1Bailout, th2Bailout, th1NumBailout, th2NumBailout, dt, m1, m2, l, g, referenced, rungeKutta);
    this->TRender<RenderDPendulum>(myRender);
}
void DPendulum::CopyOptFromPanel()
{
    th1Bailout = *panelOpt.GetBoolElement(0);
    th1NumBailout = *panelOpt.GetDoubleElement(0);
    th2Bailout = *panelOpt.GetBoolElement(1);
    th2NumBailout = *panelOpt.GetDoubleElement(1);
    dt = *panelOpt.GetDoubleElement(2);
    m1 = *panelOpt.GetDoubleElement(3);
    m2 = *panelOpt.GetDoubleElement(4);
    l = *panelOpt.GetDoubleElement(5);
    g = *panelOpt.GetDoubleElement(6);
    referenced = *panelOpt.GetBoolElement(2);
}
void DPendulum::MoreIter()
{
    // Increases 100 iterations.
    changeFractalIter = true;
    if(paused) dontDrawTempImage = true;
    this->DeleteSavedZooms();
    redrawAll = true;
    maxIter += 100;
}
void DPendulum::LessIter()
{
    // Decreases 100 iterations.
    changeFractalIter = true;
    this->DeleteSavedZooms();
    if(paused) dontDrawTempImage = true;
    redrawAll = true;
    int signedMaxIter = (int)maxIter;
    if(signedMaxIter - 100 > 0)
    {
        maxIter -= 100;
    }
}
void DPendulum::DrawOrbit()
{
    double th1, th2, vth1, vth2;
    double ecMov1, ecMov2;
    double part0, part1, part2, part3, part4, part5, part6;
    double den;
    part0 = m1+m2;
    part1 = -g*(2*m1+m2);
    part2 = 2*m2;
    part3 = m2*g;
    part4 = g*(m1+m2);
    part5 = l*2*m1+m2;
    part6 = l*m2;
    vector<double> th1Vector, th2Vector;
    th1 = orbitX;
    th2 = orbitY;
    vth1 = 0;
    vth2 = 0;
    bool insideSet = true;
    double th1_init = th1;
    double th2_init = th2;

    for(unsigned n=0; n<maxIter; n++)
    {
        th1Vector.push_back(th1);
        th2Vector.push_back(th2);
        den = part5 - part6*cos(2*(th1-th2));
        ecMov1 = part1*sin(th1)-part3*sin(th1-2*th2)-part2*sin(th1-th2)*(l*(pow(vth2,2)+pow(vth1,2)*cos(th1-th2)));
        ecMov1 /= den;
        ecMov2 = 2*sin(th1-th2)*(part4*cos(th1)+l*(pow(vth2,2)*m2*cos(th1-th2)+pow(vth1,2)*(part0)));
        ecMov2 /= den;
        vth1 += dt*ecMov1;
        vth2 += dt*ecMov2;
        th1 += dt*vth1;
        th2 += dt*vth2;

        if(referenced)
        {
            if((th1Bailout && (abs(th1-th1_init) > th1NumBailout)) || (th2Bailout && (abs(th2-th2_init) > th2NumBailout)))
            {
                insideSet = false;
                break;
            }
        }
        else
        {
            if((th1Bailout && (abs(th1) > th1NumBailout)) || (th2Bailout && (abs(th2) > th2NumBailout)))
            {
                insideSet = false;
                break;
            }
        }
    }

    sf::Color color;
    if(insideSet) color = sf::Color(0, 255, 0);
    else color = sf::Color(255, 0, 0);

    for(unsigned int i=0; i<th1Vector.size()-1; i++)
        this->DrawLine(th1Vector[i], th2Vector[i], th1Vector[i+1], th2Vector[i+1], color, true);

    orbitDrawn = true;
}

// RenderUserDefined
RenderUserDefined::RenderUserDefined()
{
    bailout = 1;
    julia = false;
}
void RenderUserDefined::SetFormula(FormulaOpt formula)
{
    bailout = formula.bailout;
    julia = formula.julia;
    parserFormula = formula.userFormula;
}
void RenderUserDefined::Render()
{
    mup::ParserX parser;
    parser.SetExpr(parserFormula.wc_str());

    unsigned n;
    int squaredBail = bailout*bailout;
    double z_y;

    // muParserX vars.
    mup::Value zVal;
    mup::Value cVal;
    mup::Value zero = mup::cmplx_type(0, 0);
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("c"),  mup::Variable(&cVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    parser.DefineVar(_T("C"),  mup::Variable(&cVal));
    if(julia) cVal = mup::cmplx_type(kReal, kImaginary);
    bool insideSet;

    // Parser execution.
    try
    {
        for(y=ho; y<hf; y++)
        {
            z_y = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                if(!julia)
                {
                    cVal = mup::cmplx_type(minX + x*xFactor, z_y);
                    zVal = zero;
                }
                else
                    zVal = mup::cmplx_type(minX + x*xFactor, z_y);

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    if(zVal.GetFloat()*zVal.GetFloat() + zVal.GetImag()*zVal.GetImag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                    zVal = parser.Eval();
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    catch(mup::ParserError& e)
    {
        // Reports error and fill screen with null values.
        errorInfo = wxT("Error: ");
        errorInfo += wxString(e.GetMsg());

        for(int y=ho; y<hf; y++)
        {
            for(int x=wo; x<wf; x++)
            {
                setMap[x][y] = false;
                colorMap[x][y] = 0;
            }
        }
    }

    parser.ClearVar();
    parser.ClearFun();
}
wxString RenderUserDefined::GetErrorInfo()
{
    return errorInfo;
}
void RenderUserDefined::ClearErrorInfo()
{
    errorInfo.clear();
}
bool RenderUserDefined::IsThereError()
{
    return !(errorInfo.size() == 0);
}

// UserDefined
UserDefined::UserDefined(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -2.5;
    maxX = 2.5;
    minY = -1.5;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::UserDefined;
    hasOrbit = true;
    myRender = new RenderUserDefined[threadNumber];
    SetWatchdog<RenderUserDefined>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
}
UserDefined::UserDefined(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -2.5;
    maxX = 2.5;
    minY = -1.5;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::UserDefined;
    myRender = new RenderUserDefined[threadNumber];
    SetWatchdog<RenderUserDefined>(myRender, &watchdog, threadNumber);
}
UserDefined::~UserDefined()
{
    this->StopRender();
    delete[] myRender;
}
void UserDefined::Render()
{
    this->TRender<RenderUserDefined>(myRender);
}
void UserDefined::SetFormula(FormulaOpt formula)
{
    userFormula = formula;
    for(unsigned int i = 0; i < threadNumber; i++)
        myRender[i].SetFormula(formula);

    if(formula.julia)
        juliaVariety = true;
}
void UserDefined::DrawOrbit()
{
    bool julia = userFormula.julia;
    vector< complex<double> > zVector;
    mup::ParserX parser;
    parser.SetExpr(userFormula.userFormula.wc_str());

    int bailout = userFormula.bailout;
    mup::Value zVal;
    mup::Value cVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("c"),  mup::Variable(&cVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    parser.DefineVar(_T("C"),  mup::Variable(&cVal));
    if(julia) cVal = mup::cmplx_type(kReal, kImaginary);
    bool outOfSet = false;

    zVal = mup::cmplx_type(orbitX, orbitY);
    if(!julia) cVal = mup::cmplx_type(orbitX, orbitY);

    try
    {
        for(unsigned n=0; n<maxIter; n++)
        {
            zVector.push_back(complex<double>(zVal.GetFloat(),zVal.GetImag()));
            if(zVal.GetFloat()*zVal.GetFloat() + zVal.GetImag()*zVal.GetImag() > bailout*bailout)
            {
                outOfSet = true;
                break;
            }
            zVal = parser.Eval();
        }
        sf::Color color;
        if(outOfSet) color = sf::Color(255, 0, 0);
        else color = sf::Color(0, 255, 0);

        for(unsigned int i=0; i<zVector.size()-1; i++)
            this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

        orbitDrawn = true;
    }
    catch (mup::ParserError&)
    {
        return;
    }
}
void UserDefined::PostRender()
{
    if(myRender[0].IsThereError())
    {
        wxString out = wxString(wxT("Fatal error in formula.\n")) + myRender[0].GetErrorInfo() + wxT("\n");
        myRender[0].ClearErrorInfo();
        wxMessageDialog errorDialog(NULL, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}

// FixedPointUserDefined
RenderFPUserDefined::RenderFPUserDefined()
{
    bailout = 0;
    julia = false;
    minStep = 0.001;
}
void RenderFPUserDefined::SetFormula(FormulaOpt formula)
{
    bailout = formula.bailout;
    julia = formula.julia;
    parserFormula = formula.userFormula;
}
void RenderFPUserDefined::Render()
{
    mup::ParserX  parser;
    parser.SetExpr(parserFormula.wc_str());

    // Creates fractal.
    double z_y;
    mup::Value zVal, z_prevVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));

    try
    {
        unsigned n;

        for(y=ho; y<hf; y++)
        {
            z_y = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z_prevVal = zVal = mup::cmplx_type(minX + x*xFactor, z_y);

                for(n=0; n<maxIter; n++)
                {
                    zVal = parser.Eval();

                    if((z_prevVal.GetFloat() - minStep < zVal.GetFloat() && z_prevVal.GetFloat() + minStep > zVal.GetFloat())
                        && (z_prevVal.GetImag() - minStep < zVal.GetImag() && z_prevVal.GetImag() + minStep > zVal.GetImag()))
                        break;
                    else
                        z_prevVal = zVal;
                }
                if(zVal.GetFloat() > 0) colorMap[x][y] = 1 + n;
                else colorMap[x][y] = 30 + n;
            }
        }
    }
    catch(mup::ParserError& e)
    {
        errorInfo = wxT("Error: ");
        errorInfo += wxString(e.GetMsg());

        for(int y=ho; y<hf; y++)
        {
            for(int x=wo; x<wf; x++)
            {
                setMap[x][y] = false;
                colorMap[x][y] = 0;
            }
        }
    }

    parser.ClearVar();
    parser.ClearFun();
}
void RenderFPUserDefined::SetParams(double _minStep)
{
    minStep = _minStep;
}
wxString RenderFPUserDefined::GetErrorInfo()
{
    return errorInfo;
}
void RenderFPUserDefined::ClearErrorInfo()
{
    errorInfo.clear();
}
bool RenderFPUserDefined::IsThereError()
{
    return !(errorInfo.size() == 0);
}

// FixedPointUserDefined
FPUserDefined::FPUserDefined(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -1.8713;
    maxX = 1.82101;
    minY = -1.22781;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::FixedPointUserDefined;
    hasOrbit = true;
    myRender = new RenderFPUserDefined[threadNumber];
    SetWatchdog<RenderFPUserDefined>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Min step: "), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FPUserDefined::FPUserDefined(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.8713;
    maxX = 1.82101;
    minY = -1.22781;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    minStep = 0.001;
    type = FractalType::FixedPointUserDefined;
    myRender = new RenderFPUserDefined[threadNumber];
    SetWatchdog<RenderFPUserDefined>(myRender, &watchdog, threadNumber);
}
FPUserDefined::~FPUserDefined()
{
    this->StopRender();
    delete[] myRender;
}
void FPUserDefined::Render()
{
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetParams(minStep);

    this->TRender<RenderFPUserDefined>(myRender);
}
void FPUserDefined::SetFormula(FormulaOpt formula)
{
    userFormula = formula;
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetFormula(formula);
}
void FPUserDefined::DrawOrbit()
{
    vector<complex<double>> zVector;
    mup::ParserX parser;
    parser.SetExpr(userFormula.userFormula.wc_str());

    mup::Value zVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    zVal = mup::cmplx_type(orbitX, orbitY);

    try
    {
        for(unsigned n=0; n<maxIter; n++)
        {
            zVector.push_back(complex<double>(zVal.GetFloat(),zVal.GetImag()));
            zVal = parser.Eval();
        }

        sf::Color color = sf::Color(0, 255, 0);
        for(unsigned int i=0; i<zVector.size()-1; i++)
            this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

        orbitDrawn = true;
    }
    catch(mup::ParserError&)
    {
        return;
    }
}
void FPUserDefined::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
}
void FPUserDefined::PostRender()
{
    if(myRender[0].IsThereError())
    {
        wxString out = wxString(wxT("Fatal error in formula.\n")) + myRender[0].GetErrorInfo() + wxT("\n");
        myRender[0].ClearErrorInfo();
        wxMessageDialog errorDialog(NULL, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}

// RenderScriptFractal
RenderScriptFractal::RenderScriptFractal()
{
    renderEngine = nullptr;
    threadIndex = 0;
    hasEnded = true;
}
RenderScriptFractal::~RenderScriptFractal()
{

}
void RenderScriptFractal::SetPath(string scriptPath)
{
    path = scriptPath;
}
void RenderScriptFractal::Render()
{
    hasEnded = false;

    // Creates script engine.
    renderEngine = new AngelscriptRenderEngine();
    if (renderEngine->GetStatus() == EngineStatus::Error)
    {
        errorInfo = renderEngine->GetErrorInfo();
        return;
    }

    // Register global variables
    bool isEngineOk = true;
    isEngineOk &= renderEngine->RegisterGlobalVariable("double minX", &minX);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxX", &maxX);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double minY", &minY);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxY", &maxY);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double xFactor", &xFactor);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double yFactor", &yFactor);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double kReal", &kReal);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double kImaginary", &kImaginary);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int ho", &ho);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int hf", &hf);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int wo", &wo);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int wf", &wf);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxIter", &maxIter);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int threadIndex", &threadIndex);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int screenWidth", &myOpt.screenWidth);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int screenHeight", &myOpt.screenHeight);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int paletteSize", &myOpt.paletteSize);

    if (!isEngineOk)
    {
        errorInfo = renderEngine->GetErrorInfo();
        return;
    }

    // Compile and execute the script code.
    if (!renderEngine->CompileFromPath(path))
    {
        errorInfo = renderEngine->GetErrorInfo();
        return;
    }

    if (!renderEngine->Execute())
    {
        errorInfo = renderEngine->GetErrorInfo();
        return;
    }

    hasEnded = true;

    delete renderEngine;
    renderEngine = nullptr;
}
wxString RenderScriptFractal::GetErrorInfo()
{
    return errorInfo;
}
void RenderScriptFractal::ClearErrorInfo()
{
    errorInfo.clear();
}
bool RenderScriptFractal::IsThereError()
{
    if(errorInfo.size() == 0)
        return false;
    else
        return true;
}
void RenderScriptFractal::PreTerminate()
{
    if(!hasEnded)
        renderEngine->Abort();
}
void RenderScriptFractal::SetParams(int _threadIndex)
{
    threadIndex = _threadIndex;
}

// ScriptFractal
ScriptFractal::ScriptFractal(sf::RenderWindow* Window, ScriptData scriptData) : Fractal(Window)
{
    // Adjust the scale.
    minX = scriptData.minX;
    maxX = scriptData.maxX;
    minY = scriptData.minY;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    maxIter = scriptData.defaultIter;
    juliaVariety = scriptData.juliaVariety;
    redrawAlways = scriptData.redrawAlways;
    this->SetExtColorMode(scriptData.extColor);

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    path = scriptData.file;
    myScriptData = scriptData;
    type = FractalType::ScriptFractal;
    myRender = new RenderScriptFractal[threadNumber];
    for(unsigned int i=0; i<threadNumber; i++)
    {
        myRender[i].SetParams(i);
        myRender[i].SetPath(scriptData.file);
    }
    SetWatchdog<RenderScriptFractal>(myRender, &watchdog, threadNumber);
}
ScriptFractal::ScriptFractal(int width, int height, ScriptData scriptData, int renderThreads) 
    : Fractal(width, height)
{
    if (renderThreads != -1)
        threadNumber = (unsigned) renderThreads;

    // Adjust the scale.
    minX = scriptData.minX;
    maxX = scriptData.maxX;
    minY = scriptData.minY;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    maxIter = scriptData.defaultIter;
    juliaVariety = scriptData.juliaVariety;
    redrawAlways = scriptData.redrawAlways;
    this->SetExtColorMode(scriptData.extColor);

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    path = scriptData.file;
    myScriptData = scriptData;
    type = FractalType::ScriptFractal;
    myRender = new RenderScriptFractal[threadNumber];
    for(unsigned int i=0; i<threadNumber; i++)
    {
        myRender[i].SetParams(i);
        myRender[i].SetPath(scriptData.file);
    }
    SetWatchdog<RenderScriptFractal>(myRender, &watchdog, threadNumber);
}
ScriptFractal::ScriptFractal(int width, int height, string scriptPath) : Fractal(width, height)
{
    path = scriptPath;
    AngelscriptConfigurationEngine configEngine;
    if (configEngine.CompileFromPath(path) && configEngine.Execute())
    {
        ScriptData params = configEngine.GetScriptData();
        minX = params.minX;
        maxX = params.maxX;
        minY = params.minY;
        maxY = minY + (maxX - minX) * screenHeight / screenWidth;
        this->SetOutermostZoom();
    }

    type = FractalType::ScriptFractal;
    myRender = new RenderScriptFractal[threadNumber];
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetPath(scriptPath);
    SetWatchdog<RenderScriptFractal>(myRender, &watchdog, threadNumber);
}
ScriptFractal::~ScriptFractal()
{
    this->StopRender();
    delete[] myRender;
}
void ScriptFractal::Render()
{
    asSetMap = setMap;
    asColorMap = colorMap;
    asPrepareMultithread();
    this->TRender<RenderScriptFractal>(myRender);
    asUnprepareMultithread();
}
void ScriptFractal::PostRender()
{
    wxString errorLog = wxT("");
    for(unsigned int i=0; i<threadNumber; i++)
    {
        if(myRender[i].IsThereError())
        {
            errorLog += wxT("Thread ");
            errorLog += num_to_string((int)i);
            errorLog += wxT(" says:\n");
            errorLog += myRender[i].GetErrorInfo();
            errorLog += wxT("\n");
            myRender[i].ClearErrorInfo();
        }
    }
    if(errorLog.size() != 0)
    {
        wxString out = wxString(wxT("Fatal error in script.\n")) + errorLog;
        wxMessageDialog errorDialog(NULL, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}
void ScriptFractal::PreRestartRender()
{
    // Clears all the maps.
    for (int i = 0; i < screenWidth; i++)
    {
        for (int j = 0; j < screenHeight; j++)
        {
            setMap[i][j] = false;
            colorMap[i][j] = 0;
            auxMap[i][j] = 0;
        }
    }
}
bool ScriptFractal::IsThereError()
{
    return myRender[0].IsThereError();
}
wxString ScriptFractal::GetErrorInfo()
{
    return myRender[0].GetErrorInfo();
}
void ScriptFractal::ClearErrorInfo()
{
    return myRender[0].ClearErrorInfo();
}
string ScriptFractal::GetPath()
{
    return path;
}

// FractalHandler
FractalHandler::FractalHandler()
{
    mandelbrot = nullptr;
    mandelbrotZN = nullptr;
    julia = nullptr;
    juliaZN = nullptr;
    newton = nullptr;
    sinoidal = nullptr;
    magnet = nullptr;
    sierpinskyTriangle = nullptr;
    medusa = nullptr;
    manowar = nullptr;
    manowarJulia = nullptr;
    fixedPoint1 = nullptr;
    fixedPoint2 = nullptr;
    fixedPoint3 = nullptr;
    fixedPoint4 = nullptr;
    tricorn = nullptr;
    burningShip = nullptr;
    burningShipJulia = nullptr;
    fractory = nullptr;
    cell = nullptr;
    logisticMap = nullptr;
    henonMap = nullptr;
    dPendulum = nullptr;
    userDefined = nullptr;
    fpUserDefined = nullptr;
    scriptFractal = nullptr;
    target = nullptr;
    type = FractalType::Undefined;
}
FractalHandler::~FractalHandler()
{
    this->DeleteFractal();
}
void FractalHandler::CreateFractal(FractalType _type, sf::RenderWindow* Window)
{
    this->DeleteFractal();
    type = _type;
    switch(_type)
    {
    case FractalType::Mandelbrot:
        {
            target = mandelbrot = new Mandelbrot(Window);
            break;
        }
    case FractalType::MandelbrotZN:
        {
            target = mandelbrotZN = new MandelbrotZN(Window);
            break;
        }
    case FractalType::Julia:
        {
            target = julia = new Julia(Window);
            break;
        }
    case FractalType::JuliaZN:
        {
            target = juliaZN = new JuliaZN(Window);
            break;
        }
    case FractalType::NewtonRaphsonMethod:
        {
            target = newton = new Newton(Window);
            break;
        }
    case FractalType::Sinoidal:
        {
            target = sinoidal = new Sinoidal(Window);
            break;
        }
    case FractalType::Magnetic:
        {
            target = magnet = new Magnet(Window);
            break;
        }
    case FractalType::Medusa:
        {
            target = medusa = new Medusa(Window);
            break;
        }
    case FractalType::Manowar:
        {
            target = manowar = new Manowar(Window);
            break;
        }
    case FractalType::ManowarJulia:
        {
            target = manowarJulia = new ManowarJulia(Window);
            break;
        }
    case FractalType::SierpinskyTriangle:
        {
            target = sierpinskyTriangle = new SierpTriangle(Window);
            break;
        }
    case FractalType::FixedPoint1:
        {
            target = fixedPoint1 = new FixedPoint1(Window);
            break;
        }
    case FractalType::FixedPoint2:
        {
            target = fixedPoint2 = new FixedPoint2(Window);
            break;
        }
    case FractalType::FixedPoint3:
        {
            target = fixedPoint3 = new FixedPoint3(Window);
            break;
        }
    case FractalType::FixedPoint4:
        {
            target = fixedPoint4 = new FixedPoint4(Window);
            break;
        }
    case FractalType::Tricorn:
        {
            target = tricorn = new Tricorn(Window);
            break;
        }
    case FractalType::BurningShip:
        {
            target = burningShip = new BurningShip(Window);
            break;
        }
    case FractalType::BurningShipJulia:
        {
            target = burningShipJulia = new BurningShipJulia(Window);
            break;
        }
    case FractalType::Fractory:
        {
            target = fractory = new Fractory(Window);
            break;
        }
    case FractalType::Cell:
        {
            target = cell = new Cell(Window);
            break;
        }
    case FractalType::Logistic:
        {
            target = logisticMap = new LogisticMap(Window);
            break;
        }
    case FractalType::HenonMap:
        {
            target = henonMap = new HenonMap(Window);
            break;
        }
    case FractalType::DoublePendulum:
        {
            target = dPendulum = new DPendulum(Window);
            break;
        }
    case FractalType::UserDefined:
        {
            target = userDefined = new UserDefined(Window);
            break;
        }
    case FractalType::FixedPointUserDefined:
        {
            target = fpUserDefined = new FPUserDefined(Window);
            break;
        }
    default: break;
    }
}
void FractalHandler::CreateFractal(FractalType _type, int width, int height)
{
    this->DeleteFractal();
    type = _type;
    switch(_type)
    {
    case FractalType::Mandelbrot:
        {
            target = mandelbrot = new Mandelbrot(width, height);
            break;
        }
    case FractalType::MandelbrotZN:
        {
            target = mandelbrotZN = new MandelbrotZN(width, height);
            break;
        }
    case FractalType::Julia:
        {
            target = julia = new Julia(width, height);
            break;
        }
    case FractalType::JuliaZN:
        {
            target = juliaZN = new JuliaZN(width, height);
            break;
        }
    case FractalType::NewtonRaphsonMethod:
        {
            target = newton = new Newton(width, height);
            break;
        }
    case FractalType::Sinoidal:
        {
            target = sinoidal = new Sinoidal(width, height);
            break;
        }
    case FractalType::Magnetic:
        {
            target = magnet = new Magnet(width, height);
            break;
        }
    case FractalType::Medusa:
        {
            target = medusa = new Medusa(width, height);
            break;
        }
    case FractalType::Manowar:
        {
            target = manowar = new Manowar(width, height);
            break;
        }
    case FractalType::ManowarJulia:
        {
            target = manowarJulia = new ManowarJulia(width, height);
            break;
        }
    case FractalType::SierpinskyTriangle:
        {
            target = sierpinskyTriangle = new SierpTriangle(width, height);
            break;
        }
    case FractalType::FixedPoint1:
        {
            target = fixedPoint1 = new FixedPoint1(width, height);
            break;
        }
    case FractalType::FixedPoint2:
        {
            target = fixedPoint2 = new FixedPoint2(width, height);
            break;
        }
    case FractalType::FixedPoint3:
        {
            target = fixedPoint3 = new FixedPoint3(width, height);
            break;
        }
    case FractalType::FixedPoint4:
        {
            target = fixedPoint4 = new FixedPoint4(width, height);
            break;
        }
    case FractalType::Tricorn:
        {
            target = tricorn = new Tricorn(width, height);
            break;
        }
    case FractalType::BurningShip:
        {
            target = burningShip = new BurningShip(width, height);
            break;
        }
    case FractalType::BurningShipJulia:
        {
            target = burningShipJulia = new BurningShipJulia(width, height);
            break;
        }
    case FractalType::Fractory:
        {
            target = fractory = new Fractory(width, height);
            break;
        }
    case FractalType::Cell:
        {
            target = cell = new Cell(width, height);
            break;
        }
    case FractalType::Logistic:
        {
            target = logisticMap = new LogisticMap(width, height);
            break;
        }
    case FractalType::HenonMap:
        {
            target = henonMap = new HenonMap(width, height);
            break;
        }
    case FractalType::DoublePendulum:
        {
            target = dPendulum = new DPendulum(width, height);
            break;
        }
    case FractalType::UserDefined:
        {
            target = userDefined = new UserDefined(width, height);
            break;
        }
    case FractalType::FixedPointUserDefined:
        {
            target = fpUserDefined = new FPUserDefined(width, height);
            break;
        }
    default: break;
    }
}
void FractalHandler::CreateScriptFractal(sf::RenderWindow* Window, ScriptData scriptData)
{
    target = scriptFractal = new ScriptFractal(Window, scriptData);
}
void FractalHandler::CreateScriptFractal(int width, int height, ScriptData scriptData)
{
    target = scriptFractal = new ScriptFractal(width, height, scriptData);
}
void FractalHandler::CreateScriptFractal(int width, int height, string scriptPath)
{
    target = scriptFractal = new ScriptFractal(width, height, scriptPath);
}
void FractalHandler::SetFormula(FormulaOpt formula)
{
    if(type == FractalType::UserDefined)
    {
        if(formula.type != FormulaType::Complex)
        {
            formula.type = FormulaType::Complex;
            formula.userFormula = wxT("z = z^2 + c");
        }
        userDefined->SetFormula(formula);
    }
    else if(type == FractalType::FixedPointUserDefined)
    {
        if(formula.type != FormulaType::FixedPoint)
        {
            formula.type = FormulaType::FixedPoint;
            formula.userFormula = wxT("z = sin(z)");
        }
        fpUserDefined->SetFormula(formula);
    }
}
void FractalHandler::DeleteFractal()
{
    target = nullptr;
    if(mandelbrot != nullptr)
    {
        delete mandelbrot;
        mandelbrot = nullptr;
    }
    if(mandelbrotZN != nullptr)
    {
        delete mandelbrotZN;
        mandelbrotZN = nullptr;
    }
    if(julia != nullptr)
    {
        delete julia;
        julia = nullptr;
    }
    if(juliaZN != nullptr)
    {
        delete juliaZN;
        juliaZN = nullptr;
    }
    if(newton != nullptr)
    {
        delete newton;
        newton = nullptr;
    }
    if(sinoidal != nullptr)
    {
        delete sinoidal;
        sinoidal = nullptr;
    }
    if(magnet != nullptr)
    {
        delete magnet;
        magnet = nullptr;
    }
    if(medusa != nullptr)
    {
        delete medusa;
        medusa = nullptr;
    }
    if(manowar != nullptr)
    {
        delete manowar;
        manowar = nullptr;
    }
    if(manowarJulia != nullptr)
    {
        delete manowarJulia;
        manowarJulia = nullptr;
    }
    if(sierpinskyTriangle != nullptr)
    {
        delete sierpinskyTriangle;
        sierpinskyTriangle = nullptr;
    }
    if(fixedPoint1 != nullptr)
    {
        delete fixedPoint1;
        fixedPoint1 = nullptr;
    }
    if(fixedPoint2 != nullptr)
    {
        delete fixedPoint2;
        fixedPoint2 = nullptr;
    }
    if(fixedPoint3 != nullptr)
    {
        delete fixedPoint3;
        fixedPoint3 = nullptr;
    }
    if(fixedPoint4 != nullptr)
    {
        delete fixedPoint4;
        fixedPoint4 = nullptr;
    }
    if(tricorn != nullptr)
    {
        delete tricorn;
        tricorn = nullptr;
    }
    if(burningShip != nullptr)
    {
        delete burningShip;
        burningShip = nullptr;
    }
    if(burningShipJulia != nullptr)
    {
        delete burningShipJulia;
        burningShipJulia = nullptr;
    }
    if(fractory != nullptr)
    {
        delete fractory;
        fractory = nullptr;
    }
    if(cell != nullptr)
    {
        delete cell;
        cell = nullptr;
    }
    if(logisticMap != nullptr)
    {
        delete logisticMap;
        logisticMap = nullptr;
    }
    if(henonMap != nullptr)
    {
        delete henonMap;
        henonMap = nullptr;
    }
    if(dPendulum != nullptr)
    {
        delete dPendulum;
        dPendulum = nullptr;
    }
    if(userDefined != nullptr)
    {
        delete userDefined;
        userDefined = nullptr;
    }
    if(fpUserDefined != nullptr)
    {
        delete fpUserDefined;
        fpUserDefined = nullptr;
    }
    if(scriptFractal != nullptr)
    {
        delete scriptFractal;
        scriptFractal = nullptr;
    }
}
Fractal* FractalHandler::GetFractalPtr()
{
    return target;
}
FractalType FractalHandler::GetType()
{
    return type;
}