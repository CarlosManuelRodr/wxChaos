#pragma once
#include "../Fractal.h"
#include "../UserFormulaSymbolicCompiler.h"
#include "../renderers/UserDefinedNewtonRenderer.h"

class UserDefinedNewton : public Fractal
{
    UserDefinedNewtonRenderer* _myRender;
    CompiledUserFormula _compiledFormula;
    wxString _errorInfo;

    double _convergenceEpsilon;
    double _functionEpsilon;
    double _derivativeEpsilon;
    double _rootTolerance;
    double _escapeRadius;

    struct OrbitResult
    {
        bool converged = false;
        bool failed = false;
    };

    [[nodiscard]] bool HasCompiledFormula() const;
    OrbitResult DrawNewtonOrbit();
    void ApplyRendererState();

public:
    UserDefinedNewton(unsigned int width, unsigned int height);
    ~UserDefinedNewton() override;
    wxString GetName() const override { return wxT("User Defined Newton-Raphson"); }

    void Render() override;
    void SetFormula(FormulaOptions formula) override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
    void PostRender() override;
};
