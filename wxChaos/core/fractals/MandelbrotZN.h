#pragma once
#include "../Fractal.h"
#include "../renderers/RenderMandelbrotZN.h"

/*
* @class MandelbrotZN
* @brief Handles the RenderMandelbrotZN threads.
*/
class MandelbrotZN : public Fractal
{
private:
    int n;
    double bailout;
    RenderMandelbrotZN* myRender;
public:
    explicit MandelbrotZN(sf::RenderWindow* window);
    MandelbrotZN(int width, int height);
    ~MandelbrotZN() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
