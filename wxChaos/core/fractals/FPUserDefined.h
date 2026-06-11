#pragma once
#include "../Fractal.h"
#include "../renderers/UserDefinedFixedPointRenderer.h"

/*
* @class FPUserDefined
* @brief Handles the RenderUserDefined threads.
*/
class FPUserDefined : public Fractal
{
private:
    UserDefinedFixedPointRenderer* myRender;
    double minStep;
public:
    FPUserDefined(unsigned int width, unsigned int height);
    ~FPUserDefined();

    void Render();
    void SetFormula(FormulaOptions formula);        ///< Sets user formula.
    void DrawOrbit();
    void CopyOptFromPanel();
    void PostRender();
};
