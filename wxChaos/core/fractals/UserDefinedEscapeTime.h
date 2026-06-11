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
    ~UserDefinedEscapeTime() override;

    void Render() override;
    void SetFormula(FormulaOptions formula) override;        ///< Sets user formula.
    void DrawOrbit() override;
    void PostRender() override;
};
