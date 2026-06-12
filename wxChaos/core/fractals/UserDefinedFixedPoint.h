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
    ~UserDefinedFixedPoint() override;
    wxString GetName() const override { return wxT("User Defined Fixed Point"); }

    void Render() override;
    void SetFormula(FormulaOptions formula) override;        ///< Sets user formula.
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
    void PostRender() override;
};
