#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderDPendulum
* @brief Threaded DPendulum rendering routines.
*/
class RenderDPendulum : public RenderFractal
{
private:
    bool th1Bailout, th2Bailout;
    double th1NumBailout, th2NumBailout;
    double m1, m2, l, g;
    double dt;
    bool referenced, rungeKutta;
public:
    RenderDPendulum();
    void Render();
    void SetParams(bool th1B, bool th2B, double th1NB, double th2NB,
                   double _dt, double _m1, double _m2, double _l, double _g, bool ref, bool _rungeKutta);
};
