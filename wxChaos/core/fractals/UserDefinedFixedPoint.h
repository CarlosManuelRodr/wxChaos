#pragma once
#include "../Fractal.h"
#include "../renderers/UserDefinedFixedPointRenderer.h"

/*
* @class UserDefinedFixedPoint
* @brief Handles the RenderUserDefined threads.
*/
class UserDefinedFixedPoint : public Fractal
{
    UserDefinedFixedPointRenderer* _myRender;
    double _minStep;
public:
    UserDefinedFixedPoint(unsigned int width, unsigned int height);
    ~UserDefinedFixedPoint();

    void Render();
    void SetFormula(FormulaOptions formula);        ///< Sets user formula.
    void DrawOrbit();
    void CopyOptFromPanel();
    void PostRender();
};
