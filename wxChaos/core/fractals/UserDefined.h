#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class UserDefined
* @brief Handles the RenderUserDefined threads.
*/
class UserDefined : public Fractal
{
private:
    RenderUserDefined* myRender;
public:
    UserDefined(sf::RenderWindow* Window);
    UserDefined(int width, int height);
    ~UserDefined();

    void Render();
    void SetFormula(FormulaOpt formula);        ///< Sets user formula.
    void DrawOrbit();
    void PostRender();
};
