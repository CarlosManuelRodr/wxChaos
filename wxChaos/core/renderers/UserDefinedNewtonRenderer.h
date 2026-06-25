#pragma once

#include <complex>
#include <string>
#include <wx/string.h>
#include "../NewtonRootRegistry.h"
#include "../Renderer.h"

class UserDefinedNewtonRenderer : public Renderer
{
    std::string _functionExpression;
    std::string _derivativeExpression;
    wxString _errorInfo;
    NewtonRootRegistry _rootRegistry;

    double _convergenceEpsilon;
    double _functionEpsilon;
    double _derivativeEpsilon;
    double _rootTolerance;
    double _escapeRadius;

    struct IterationResult
    {
        bool converged = false;
        unsigned int iterations = 0;
        std::complex<double> finalZ;
        double finalStep = 0.0;
        double finalResidual = 0.0;
    };

    [[nodiscard]] bool HasCompiledFormula() const;
    static bool IsFinite(const std::complex<double>& value);
    [[nodiscard]] double ColorValue(unsigned int rootId, const IterationResult& result) const;
    void FillFailedRegion();

public:
    UserDefinedNewtonRenderer();

    void SetCompiledFormula(const std::string& functionExpression, const std::string& derivativeExpression);
    void SetFormulaError(const wxString& errorInfo);
    void SetParams(double convergenceEpsilon, double functionEpsilon, double derivativeEpsilon,
                   double rootTolerance, double escapeRadius);
    void Render() override;
    void ClearErrorInfo();
    [[nodiscard]] wxString GetErrorInfo() const;
    [[nodiscard]] bool IsThereError() const;
};
