#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

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
    FPUserDefined(sf::RenderWindow* Window);
    FPUserDefined(int width, int height);
    ~FPUserDefined();

    void Render();
    void SetFormula(FormulaOpt formula);        ///< Sets user formula.
    void DrawOrbit();
    void CopyOptFromPanel();
    void PostRender();
};
