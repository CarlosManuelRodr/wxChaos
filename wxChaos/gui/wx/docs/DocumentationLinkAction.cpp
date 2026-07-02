#include "docs/DocumentationLinkAction.h"

std::optional<DocumentationLinkAction::FractalReference> DocumentationLinkAction::FindFractal(const wxString& fractal)
{
    static const FractalReference fractals[] = {
        {"mandelbrot", FractalType::Mandelbrot, true},
        {"mandelbrot-zn", FractalType::MandelbrotZN, true},
        {"julia", FractalType::Julia, false},
        {"julia-zn", FractalType::JuliaZN, false},
        {"newton", FractalType::NewtonRaphsonMethod, false},
        {"sinusoidal", FractalType::Sinusoidal, false},
        {"magnet", FractalType::Magnetic, false},
        {"jellyfish", FractalType::Jellyfish, false},
        {"manowar", FractalType::Manowar, true},
        {"manowar-julia", FractalType::ManowarJulia, false},
        {"sierpinski-triangle", FractalType::SierpinskiTriangle, false},
        {"fixed-point-sin", FractalType::FixedPoint1, false},
        {"fixed-point-cos", FractalType::FixedPoint2, false},
        {"fixed-point-tan", FractalType::FixedPoint3, false},
        {"fixed-point-z2", FractalType::FixedPoint4, false},
        {"tricorn", FractalType::Tricorn, false},
        {"burning-ship", FractalType::BurningShip, true},
        {"burning-ship-julia", FractalType::BurningShipJulia, false},
        {"fractory", FractalType::Fractory, false},
        {"cell", FractalType::Cell, false},
        {"henon-map", FractalType::HenonMap, false},
        {"double-pendulum", FractalType::DoublePendulum, false},
        {"user-defined-escape-time", FractalType::UserDefinedEscapeTime, false},
        {"user-defined-fixed-point", FractalType::UserDefinedFixedPoint, false},
        {"user-defined-newton", FractalType::UserDefinedNewtonRaphson, false}
    };

    for (const FractalReference& candidate : fractals)
    {
        if (candidate.slug == fractal)
            return candidate;
    }

    return std::nullopt;
}

std::optional<DocumentationLinkAction::Location> DocumentationLinkAction::FindLocation(const wxString& fractal,
                                                                                       const wxString& location)
{
    const std::optional<FractalReference> fractalReference = FindFractal(fractal);
    if (!fractalReference.has_value())
        return std::nullopt;

    static const Location mandelbrotLocations[] = {
        {"mandelbrot", FractalType::Mandelbrot, true, "seahorse-valley", -0.7453, 0.1127, 0.04},
        {"mandelbrot", FractalType::Mandelbrot, true, "elephant-valley", 0.275, 0.0, 0.075},
        {"mandelbrot", FractalType::Mandelbrot, true, "miniature", -1.749, 0.0, 0.035}
    };

    for (const Location& landmark : mandelbrotLocations)
    {
        if (landmark.fractal == fractalReference->slug && landmark.id == location)
            return landmark;
    }

    return std::nullopt;
}

std::optional<DocumentationLinkAction::RenderingMethod> DocumentationLinkAction::FindRenderingMethod(const wxString& method)
{
    static const RenderingMethod methods[] = {
        {wxEmptyString, FractalType::Undefined, false, "escape-time", RenderingAlgorithmType::EscapeTime, false, false},
        {wxEmptyString, FractalType::Undefined, false, "smooth-coloring", RenderingAlgorithmType::EscapeTime, true, false},
        {wxEmptyString, FractalType::Undefined, false, "gaussian-integer", RenderingAlgorithmType::GaussianInt, false, false},
        {wxEmptyString, FractalType::Undefined, false, "escape-angle", RenderingAlgorithmType::EscapeAngle, false, false},
        {wxEmptyString, FractalType::Undefined, false, "triangle-inequality", RenderingAlgorithmType::TriangleInequality, false, false},
        {wxEmptyString, FractalType::Undefined, false, "orbit-traps", RenderingAlgorithmType::EscapeTime, false, true},
        {wxEmptyString, FractalType::Undefined, false, "buddhabrot", RenderingAlgorithmType::Buddhabrot, false, false}
    };

    for (const RenderingMethod& renderingMethod : methods)
    {
        if (renderingMethod.id == method)
            return renderingMethod;
    }

    return std::nullopt;
}

DocumentationLinkAction DocumentationLinkAction::Parse(const wxString& url)
{
    DocumentationLinkAction action;
    constexpr auto scheme = "wxchaos://";

    wxString path = url;
    if (!path.Lower().StartsWith(scheme))
        return action;

    path = path.Mid(wxString(scheme).length());
    path = path.BeforeFirst('#').BeforeFirst('?');
    path.MakeLower();

    const wxString kind = path.BeforeFirst('/');
    const wxString remainder = path.AfterFirst('/');

    if (kind == "fractal")
    {
        if (remainder == "script-editor")
        {
            action._type = Type::OpenFractal;
            action._target = remainder;
            return action;
        }

        const std::optional<FractalReference> fractal = FindFractal(remainder);
        if (fractal.has_value())
        {
            action._type = Type::OpenFractal;
            action._target = fractal->slug;
            action._targetFractalType = fractal->type;
            action._targetFractalEnablesJulia = fractal->enableJulia;
        }
        return action;
    }

    if (kind == "julia")
    {
        const wxString fractalSlug = remainder.BeforeFirst('/');
        const std::optional<FractalReference> fractal = FindFractal(fractalSlug);
        if (fractal.has_value() && fractal->enableJulia)
        {
            action._type = Type::OpenJuliaMode;
            action._target = fractal->slug;
            action._targetFractalType = fractal->type;
            action._targetFractalEnablesJulia = fractal->enableJulia;
        }
        return action;
    }

    if (kind == "tool")
    {
        const wxString tool = remainder.BeforeFirst('/');
        if (tool == "orbit")
        {
            action._type = Type::EnableTool;
            action._target = tool;
        }
        return action;
    }

    if (kind == "location")
    {
        const wxString fractal = remainder.BeforeFirst('/');
        const wxString location = remainder.AfterFirst('/');
        const std::optional<Location> parsedLocation = FindLocation(fractal, location);
        if (parsedLocation.has_value())
        {
            action._type = Type::OpenLocation;
            action._location = *parsedLocation;
        }
    }

    if (kind == "rendering")
    {
        const wxString fractal = remainder.BeforeFirst('/');
        const wxString method = remainder.AfterFirst('/');
        const std::optional<FractalReference> fractalReference = FindFractal(fractal);
        const std::optional<RenderingMethod> parsedMethod = FindRenderingMethod(method);
        if (fractalReference.has_value() && parsedMethod.has_value())
        {
            action._type = Type::SetRendering;
            action._renderingMethod = *parsedMethod;
            action._renderingMethod.fractal = fractalReference->slug;
            action._renderingMethod.fractalType = fractalReference->type;
            action._renderingMethod.enableJulia = fractalReference->enableJulia;
        }
    }

    return action;
}

DocumentationLinkAction::Type DocumentationLinkAction::GetType() const
{
    return _type;
}

const wxString& DocumentationLinkAction::GetTarget() const
{
    return _target;
}

FractalType DocumentationLinkAction::GetTargetFractalType() const
{
    return _targetFractalType;
}

bool DocumentationLinkAction::TargetFractalEnablesJulia() const
{
    return _targetFractalEnablesJulia;
}

const DocumentationLinkAction::Location& DocumentationLinkAction::GetLocation() const
{
    return _location;
}

const DocumentationLinkAction::RenderingMethod& DocumentationLinkAction::GetRenderingMethod() const
{
    return _renderingMethod;
}
