#pragma once
#include "../RenderFractal.h"

/*
* @class RenderDPendulum
* @brief Threaded DPendulum rendering routines.
*/
class RenderDPendulum : public RenderFractal
{
    bool _th1Bailout, _th2Bailout;
    double _th1NumBailout, _th2NumBailout;
    double _m1, _m2, _l, _g;
    double _dt;
    bool _referenced, _rungeKutta;
public:
    RenderDPendulum();
    void Render() override;
    void SetParams(bool th1B, bool th2B, double th1NB, double th2NB,
                   double dt, double m1, double m2, double l, double g, bool ref, bool rungeKutta);
};
