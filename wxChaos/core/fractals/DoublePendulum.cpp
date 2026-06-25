#include "DoublePendulum.h"
using namespace std;

DoublePendulum::DoublePendulum(unsigned int width, unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -3.5;
    _maxX = 3.5;
    _minY = -3.5;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);
    _maxIter = 4000;

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(" θ1 Bailout"), &th1Bailout, wxT("true"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Value: "), &th1NumBailout, wxT("3.14159"));
    _panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(" θ2 Bailout"), &th2Bailout, wxT("true"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Value: "), &th2NumBailout, wxT("3.14159"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("DeltaT: "), &dt, wxT("0.005"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("m1: "), &m1, wxT("10"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("m2: "), &m2, wxT("10"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Longitude: "), &l, wxT("20"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Gravity: "), &g, wxT("9.81"));
    _panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(" Relative angles"), &referenced, wxT("false"));
    _panelOpt.LinkBool(PanelOptionType::CheckBox, wxT(" Runge-Kutta"), &rungeKutta, wxT("false"));
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

    _algorithm = RenderingAlgorithmType::EscapeTime;
    _type = FractalType::DoublePendulum;
    myRender = new DoublePendulumRenderer[_threadNumber];
}
DoublePendulum::~DoublePendulum()
{
    this->StopRender();
    delete[] myRender;
}
void DoublePendulum::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(th1Bailout, th2Bailout, th1NumBailout, th2NumBailout, dt, m1, m2, l, g, referenced, rungeKutta);
    this->SetRendererBounds<DoublePendulumRenderer>(myRender);
}
void DoublePendulum::CopyOptFromPanel()
{
    th1Bailout = *_panelOpt.GetBoolElement(0);
    th1NumBailout = *_panelOpt.GetDoubleElement(0);
    th2Bailout = *_panelOpt.GetBoolElement(1);
    th2NumBailout = *_panelOpt.GetDoubleElement(1);
    dt = *_panelOpt.GetDoubleElement(2);
    m1 = *_panelOpt.GetDoubleElement(3);
    m2 = *_panelOpt.GetDoubleElement(4);
    l = *_panelOpt.GetDoubleElement(5);
    g = *_panelOpt.GetDoubleElement(6);
    referenced = *_panelOpt.GetBoolElement(2);
}
void DoublePendulum::DrawOrbit()
{
    double part0 = m1 + m2;
    double part1 = -g * (2 * m1 + m2);
    double part2 = 2 * m2;
    double part3 = m2 * g;
    double part4 = g * (m1 + m2);
    double part5 = l * 2 * m1 + m2;
    double part6 = l * m2;
    vector<double> th1Vector, th2Vector;
    double th1 = _orbitX;
    double th2 = _orbitY;
    double vth1 = 0;
    double vth2 = 0;
    bool insideSet = true;
    const double th1_init = th1;
    const double th2_init = th2;

    for (unsigned n=0; n<_maxIter; n++)
    {
        th1Vector.push_back(th1);
        th2Vector.push_back(th2);
        const double denominator = part5 - part6 * cos(2 * (th1 - th2));
        double ecMov1 = part1 * sin(th1) - part3 * sin(th1 - 2 * th2) - part2 * sin(th1 - th2) * (l * (pow(vth2, 2) + pow(vth1, 2) * cos(th1 - th2)));
        ecMov1 /= denominator;
        double ecMov2 = 2 * sin(th1 - th2) * (part4 * cos(th1) + l * (pow(vth2, 2) * m2 * cos(th1 - th2) + pow(vth1, 2) * (part0)));
        ecMov2 /= denominator;
        vth1 += dt*ecMov1;
        vth2 += dt*ecMov2;
        th1 += dt*vth1;
        th2 += dt*vth2;

        if (referenced)
        {
            if ((th1Bailout && (abs(th1-th1_init) > th1NumBailout)) || (th2Bailout && (abs(th2-th2_init) > th2NumBailout)))
            {
                insideSet = false;
                break;
            }
        }
        else
        {
            if ((th1Bailout && (abs(th1) > th1NumBailout)) || (th2Bailout && (abs(th2) > th2NumBailout)))
            {
                insideSet = false;
                break;
            }
        }
    }

    const auto color = insideSet ? sf::Color(0, 255, 0) : sf::Color(255, 0, 0);
    for (unsigned int i=0; i<th1Vector.size()-1; i++)
        this->DrawLine(th1Vector[i], th2Vector[i], th1Vector[i+1], th2Vector[i+1], color, true);

    _orbitDrawn = true;
}
