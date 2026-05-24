#pragma once
#include "../Fractal.h"
#include "../renderers/RenderFixedPoint1.h"

/*
* @class FixedPoint1
* @brief Handles the RenderFixedPoint1 threads.
*/
class FixedPoint1 : public Fractal
{
    RenderFixedPoint1* myRender;
    double minStep;
public:
    explicit FixedPoint1(sf::RenderWindow* Window);
    FixedPoint1(int width, int height);
    ~FixedPoint1() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
