#pragma once
#include "../Fractal.h"
#include "../renderers/UserDefinedEscapeTimeRenderer.h"

/*
* @class UserDefined
* @brief Handles the RenderUserDefined threads.
*/
class UserDefined : public Fractal
{
private:
    UserDefinedEscapeTimeRenderer* myRender;
public:
    UserDefined(unsigned int width, unsigned int height);
    ~UserDefined();

    void Render();
    void SetFormula(FormulaOptions formula);        ///< Sets user formula.
    void DrawOrbit();
    void PostRender();
};
