#include "docs/DocumentationLinkAction.h"

std::optional<DocumentationLinkAction::Location> DocumentationLinkAction::FindLocation(const wxString& fractal,
                                                                                       const wxString& location)
{
    static const Location mandelbrotLocations[] = {
        {"mandelbrot", "seahorse-valley", -0.7453, 0.1127, 0.04},
        {"mandelbrot", "elephant-valley", 0.275, 0.0, 0.075},
        {"mandelbrot", "miniature", -1.749, 0.0, 0.035}
    };

    if (fractal != "mandelbrot")
        return std::nullopt;

    for (const Location& landmark : mandelbrotLocations)
    {
        if (landmark.id == location)
            return landmark;
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
        if (remainder == "mandelbrot" || remainder == "script-editor")
        {
            action._type = Type::OpenFractal;
            action._target = remainder;
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

const DocumentationLinkAction::Location& DocumentationLinkAction::GetLocation() const
{
    return _location;
}
