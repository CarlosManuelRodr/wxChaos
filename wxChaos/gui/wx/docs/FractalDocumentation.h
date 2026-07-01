/**
* @file FractalDocumentation.h
* @brief Maps fractal types to bundled documentation files.
*/

#pragma once

#include <wx/string.h>
#include "types/FractalType.h"

/**
* @class FractalDocumentation
* @brief Resolves the local documentation page for a fractal type.
*/
class FractalDocumentation
{
    static wxString GetDocumentFilename(FractalType type);

public:
    /// @brief Checks whether a fractal type has bundled documentation.
    /// @param type Fractal type to inspect.
    /// @return true when an information page exists.
    static bool HasDocumentation(FractalType type);

    /// @brief Gets the bundled documentation file for a fractal type.
    /// @param type Fractal type to inspect.
    /// @return Full local file path, or an empty string when unavailable.
    static wxString GetDocumentFile(FractalType type);
};
