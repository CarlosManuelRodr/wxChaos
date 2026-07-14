#pragma once

#include "../../vector/VectorFractal.h"

/**
 * @class ApollonianGasket
 * @brief Vector rendering of an Apollonian packing of mutually tangent circles.
 */
class ApollonianGasket : public VectorFractal
{
public:
    ApollonianGasket(unsigned int width, unsigned int height);
    wxString GetName() const override { return "Apollonian Gasket"; }
    CoordinateSystem GetCoordinateSystem() const override { return {_("x"), _("y")}; }
    void Render() override;
    void PreDrawMaps() override;
};
