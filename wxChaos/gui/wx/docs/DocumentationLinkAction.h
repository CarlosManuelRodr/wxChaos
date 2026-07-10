/**
* @file DocumentationLinkAction.h
* @brief Parses wxChaos documentation links into application actions.
*/

#pragma once

#include <optional>
#include <wx/string.h>
#include "types/FractalType.h"
#include "types/FormulaType.h"
#include "types/RenderingAlgorithmType.h"

/**
* @class DocumentationLinkAction
* @brief Value object describing an app action requested by a documentation link.
*
* Documentation pages use wxchaos:// links for museum-style interactions. This
* class keeps the URL grammar and fractal-specific named locations out of the
* frame classes that execute the actions.
*/
class DocumentationLinkAction
{
public:
    enum class Type
    {
        Unknown,
        OpenFractal,
        OpenJuliaMode,
        OpenLocation,
        ToggleTool,
        SetRendering,
        OpenFormulaDialog,
        OpenFractalOptions
    };

    struct Location
    {
        wxString fractal;
        FractalType fractalType{FractalType::Undefined};
        bool enableJulia{};
        wxString id;
        double centerX{};
        double centerY{};
        double radius{};
    };

    struct RenderingMethod
    {
        wxString fractal;
        FractalType fractalType{FractalType::Undefined};
        bool enableJulia{};
        wxString id;
        RenderingAlgorithmType algorithm{RenderingAlgorithmType::Other};
        bool smoothRender{};
        bool orbitTrap{};
    };

    struct FormulaReference
    {
        wxString slug;
        FractalType fractalType{FractalType::Undefined};
        FormulaType formulaType{FormulaType::Complex};
    };

private:
    struct FractalReference
    {
        wxString slug;
        FractalType type{FractalType::Undefined};
        bool enableJulia{};
    };

    Type _type{Type::Unknown};
    wxString _target;
    FractalType _targetFractalType{FractalType::Undefined};
    FormulaType _targetFormulaType{FormulaType::Complex};
    bool _targetFractalEnablesJulia{};
    Location _location;
    RenderingMethod _renderingMethod;

    static std::optional<FractalReference> FindFractal(const wxString& fractal);
    static std::optional<FormulaReference> FindFormula(const wxString& formula);
    static std::optional<Location> FindLocation(const wxString& fractal, const wxString& location);
    static std::optional<RenderingMethod> FindRenderingMethod(const wxString& method);

public:
    /// @brief Parses a wxchaos:// URL into a documentation action.
    static DocumentationLinkAction Parse(const wxString& url);

    /// @return The parsed action type.
    [[nodiscard]] Type GetType() const;

    /// @return Target fractal/tool identifier for non-location actions.
    [[nodiscard]] const wxString& GetTarget() const;

    /// @return Fractal type requested by OpenFractal actions.
    [[nodiscard]] FractalType GetTargetFractalType() const;

    /// @return Formula type requested by OpenFormulaDialog actions.
    [[nodiscard]] FormulaType GetTargetFormulaType() const;

    /// @return True when the target fractal can open a Julia preview from the main view.
    [[nodiscard]] bool TargetFractalEnablesJulia() const;

    /// @return Named location data for OpenLocation actions.
    [[nodiscard]] const Location& GetLocation() const;

    /// @return Rendering method data for SetRendering actions.
    [[nodiscard]] const RenderingMethod& GetRenderingMethod() const;
};
