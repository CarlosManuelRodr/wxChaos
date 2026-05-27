#pragma once
#include "../Fractal.h"
#include "../renderers/DoublePendulumRenderer.h"

/*
* @class DPendulum
* @brief Handles the RenderDPendulum threads.
*/
class DoublePendulum : public Fractal
{
    bool th1Bailout, th2Bailout;
    double th1NumBailout, th2NumBailout;
    double m1, m2, l, g;
    double dt;
    bool referenced;
    bool rungeKutta;
    DoublePendulumRenderer* myRender;
public:
    explicit DoublePendulum(const sf::RenderWindow* window);
    DoublePendulum(int width, int height);
    ~DoublePendulum() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
