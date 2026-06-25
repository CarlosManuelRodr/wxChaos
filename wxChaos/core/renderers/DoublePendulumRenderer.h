#pragma once
#include "../Renderer.h"

/**
* @class DoublePendulumRenderer
* @brief Renders double-pendulum parameter-space samples for one pixel region.
*
* Each pixel is interpreted as a pair of initial pendulum angles, and the
* renderer records whether the simulated trajectory crosses its escape
* threshold within the configured iteration count.
*/
class DoublePendulumRenderer : public Renderer
{
    bool _th1Bailout, _th2Bailout;
    double _th1NumBailout, _th2NumBailout;
    double _m1, _m2, _l, _g;
    double _dt;
    bool _referenced, _rungeKutta;

    void EscapeTimeRender();
    void EscapeAngleRender();

public:
    DoublePendulumRenderer();
    void Render() override;
    void SetParams(bool th1B, bool th2B, double th1NB, double th2NB,
                   double dt, double m1, double m2, double l, double g, bool ref, bool rungeKutta);
};
