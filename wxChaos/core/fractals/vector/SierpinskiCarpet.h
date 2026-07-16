#pragma once

#include "../../vector/VectorFractal.h"

/** @brief Vector construction of the Sierpinski carpet using filled and cleared rectangles. */
class SierpinskiCarpet : public VectorFractal
{
public:
    /** @brief Creates a carpet with a centered initial view and recursive renderer. */
    SierpinskiCarpet(unsigned int width, unsigned int height);
    /** @brief Returns the user-visible fractal name. */
    wxString GetName() const override { return "Sierpinski Carpet"; }
    /** @brief Returns the Cartesian coordinate labels used by the canvas. */
    CoordinateSystem GetCoordinateSystem() const override { return {_("x"), _("y")}; }
    /** @brief Configures and starts the asynchronous vector render. */
    void Render() override;
    /** @brief Applies the current interior color to retained rectangles before drawing. */
    void PreDrawMaps() override;
};
