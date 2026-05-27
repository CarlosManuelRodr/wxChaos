#pragma once
#include "../Fractal.h"
#include "../renderers/RenderFixedPoint3.h"

/*
* @class FixedPoint3
* @brief Handles the RenderFixedPoint3 threads.
*/
class FixedPoint3 : public Fractal
{
    RenderFixedPoint3* myRender;
    double minStep;
public:
    explicit FixedPoint3(const sf::RenderWindow* window);
    FixedPoint3(int width, int height);
    ~FixedPoint3() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
