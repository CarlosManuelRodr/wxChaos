#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class DPendulum
* @brief Handles the RenderDPendulum threads.
*/
class DPendulum : public Fractal
{
private:
    bool th1Bailout, th2Bailout;
    double th1NumBailout, th2NumBailout;
    double m1, m2, l, g;
    double dt;
    bool referenced;
    bool rungeKutta;
    RenderDPendulum* myRender;
public:
    DPendulum(sf::RenderWindow* Window);
    DPendulum(int width, int height);
    ~DPendulum();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
    void MoreIter();
    void LessIter();
};
