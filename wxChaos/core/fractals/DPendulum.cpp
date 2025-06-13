#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
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

