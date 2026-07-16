#pragma once
#include "../../raster/RasterFractal.h"
#include "../../renderers/raster/DoublePendulumRenderer.h"

/**
 * @class DoublePendulum
 * @brief Parameter-space map of a double pendulum simulation.
 *
 * Each pixel supplies the initial angles theta1 and theta2, with initial
 * angular velocities set to 0. The renderer integrates the standard double
 * pendulum equations using Euler integration or the optional Runge-Kutta mode.
 * A point is considered outside when theta1 or theta2 crosses the configured
 * absolute or initial-angle-relative bailout.
 */
class DoublePendulum : public RasterFractal
{
    bool th1Bailout, th2Bailout;
    double th1NumBailout, th2NumBailout;
    double m1, m2, l, g;
    double dt;
    bool referenced;
    bool rungeKutta;
    DoublePendulumRenderer* myRender;
public:
    DoublePendulum(unsigned int width, unsigned int height);
    ~DoublePendulum() override;
    wxString GetName() const override { return "Double Pendulum"; }
    CoordinateSystem GetCoordinateSystem() const override
    {
        return {wxString::FromUTF8(u8"\u03B82"), wxString::FromUTF8(u8"\u03B81")};
    }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptionFromPanel() override;
};
