#pragma once
#include "../Fractal.h"
#include "../renderers/FPUserDefinedRenderer.h"

/*
* @class FPUserDefined
* @brief Handles the RenderUserDefined threads.
*/
class FPUserDefined : public Fractal
{
private:
    FPUserDefinedRenderer* myRender;
    double minStep;
public:
    FPUserDefined(int width, int height);
    ~FPUserDefined();

    void Render();
    void SetFormula(FormulaOpt formula);        ///< Sets user formula.
    void DrawOrbit();
    void CopyOptFromPanel();
    void PostRender();
};
