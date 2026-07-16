#pragma once

#include "../../Translation.h"
#include "../../vector/VectorFractal.h"

/**
 * @class VectorSierpinskiTriangle
 * @brief Vector construction of the Sierpinski triangle and its recursively removed holes.
 */
class VectorSierpinskiTriangle : public VectorFractal
{
public:
    VectorSierpinskiTriangle(unsigned int width, unsigned int height);
    wxString GetName() const override { return WXCHAOS_TRANSLATE_NOOP("Sierpinski Triangle (Vector)"); }
    CoordinateSystem GetCoordinateSystem() const override { return {_("x"), _("y")}; }
    void Render() override;
    void PreDrawMaps() override;
};
