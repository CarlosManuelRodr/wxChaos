#pragma once
#include "../Fractal.h"
#include "../renderers/UserDefinedEscapeTimeRenderer.h"

/*
* @class UserDefinedEscapeTime
* @brief Handles the RenderUserDefined threads.
*/
class UserDefinedEscapeTime : public Fractal
{
    UserDefinedEscapeTimeRenderer* _myRender;
public:
    UserDefinedEscapeTime(unsigned int width, unsigned int height);
    ~UserDefinedEscapeTime();

    void Render();
    void SetFormula(FormulaOptions formula);        ///< Sets user formula.
    void DrawOrbit();
    void PostRender();
};
