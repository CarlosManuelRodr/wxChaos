#pragma once
#include "../Fractal.h"
#include "../renderers/UserDefinedRenderer.h"

/*
* @class UserDefined
* @brief Handles the RenderUserDefined threads.
*/
class UserDefined : public Fractal
{
private:
    UserDefinedRenderer* myRender;
public:
    UserDefined(const sf::RenderWindow* window);
    UserDefined(int width, int height);
    ~UserDefined();

    void Render();
    void SetFormula(FormulaOpt formula);        ///< Sets user formula.
    void DrawOrbit();
    void PostRender();
};
