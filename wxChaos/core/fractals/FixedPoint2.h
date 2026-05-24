#pragma once
#include "../Fractal.h"
#include "../renderers/RenderFixedPoint2.h"

/*
* @class FixedPoint2
* @brief Handles the RenderFixedPoint2 threads.
*/
class FixedPoint2 : public Fractal
{
    RenderFixedPoint2* myRender;
    double minStep;
public:
    explicit FixedPoint2(sf::RenderWindow* Window);
    FixedPoint2(int width, int height);
    ~FixedPoint2() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
