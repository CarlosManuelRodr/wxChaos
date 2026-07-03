/**
* @file FractalDocumentation.h
* @brief Maps fractal types to bundled documentation files.
*/

#pragma once

#include <wx/string.h>
#include "ScriptData.h"
#include "types/FractalType.h"

/**
* @class FractalDocumentation
* @brief Resolves the local documentation page for a fractal type.
*/
class FractalDocumentation
{
    static wxString GetDocumentFilename(FractalType type);
    static wxString ResolveScriptDocumentFile(const std::string& documentationPath);

public:
    /// @brief Checks whether a fractal type has bundled documentation.
    /// @param type Fractal type to inspect.
    /// @return true when an information page exists.
    static bool HasDocumentation(FractalType type);

    /// @brief Gets the bundled documentation file for a fractal type.
    /// @param type Fractal type to inspect.
    /// @return Full local file path, or an empty string when unavailable.
    static wxString GetDocumentFile(FractalType type);

    /// @brief Checks whether script metadata declares a documentation page.
    /// @param scriptData Script configuration captured from Configure().
    /// @return true when the script provides a documentation path.
    static bool HasDocumentation(const ScriptData& scriptData);

    /// @brief Gets the documentation file declared by a script.
    /// @param scriptData Script configuration captured from Configure().
    /// @return Full local file path, or an empty string when unavailable.
    static wxString GetDocumentFile(const ScriptData& scriptData);
};
