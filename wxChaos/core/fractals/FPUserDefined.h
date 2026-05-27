#pragma once
#include "../Fractal.h"
#include "../renderers/RenderFPUserDefined.h"

/*
* @class FPUserDefined
* @brief Handles the RenderUserDefined threads.
*/
class FPUserDefined : public Fractal
{
private:
    RenderFPUserDefined* myRender;
    double minStep;
public:
    FPUserDefined(const sf::RenderWindow* window);
    FPUserDefined(int width, int height);
    ~FPUserDefined();

    void Render();
    void SetFormula(FormulaOpt formula);        ///< Sets user formula.
    void DrawOrbit();
    void CopyOptFromPanel();
    void PostRender();
};
