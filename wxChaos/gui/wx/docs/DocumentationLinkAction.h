/**
* @file DocumentationLinkAction.h
* @brief Parses wxChaos documentation links into application actions.
*/

#pragma once

#include <optional>
#include <wx/string.h>

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
        OpenLocation,
        EnableTool
    };

    struct Location
    {
        wxString fractal;
        wxString id;
        double centerX{};
        double centerY{};
        double radius{};
    };

private:
    Type _type{Type::Unknown};
    wxString _target;
    Location _location;

    static std::optional<Location> FindLocation(const wxString& fractal, const wxString& location);

public:
    /// @brief Parses a wxchaos:// URL into a documentation action.
    static DocumentationLinkAction Parse(const wxString& url);

    /// @return The parsed action type.
    [[nodiscard]] Type GetType() const;

    /// @return Target fractal/tool identifier for non-location actions.
    [[nodiscard]] const wxString& GetTarget() const;

    /// @return Named location data for OpenLocation actions.
    [[nodiscard]] const Location& GetLocation() const;
};
