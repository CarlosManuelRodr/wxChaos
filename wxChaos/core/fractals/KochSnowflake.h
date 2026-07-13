#pragma once

#include "../VectorFractal.h"

/**
 * @class KochSnowflake
 * @brief Vector rendering of the Koch snowflake boundary.
 */
class KochSnowflake : public VectorFractal
{
    void AppendKochSegment(double x1, double y1, double x2, double y2, unsigned int iterations,
                           const sf::Color& color);

public:
    KochSnowflake(unsigned int width, unsigned int height);
    wxString GetName() const override { return "Koch Snowflake"; }
    CoordinateSystem GetCoordinateSystem() const override { return {_("x"), _("y")}; }
    void Render() override;
    void PreDrawMaps() override;
};
