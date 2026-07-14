#pragma once

#include "../../vector/VectorFractal.h"

class KochSnowflakeRenderer;

/**
 * @class KochSnowflake
 * @brief Vector rendering of the Koch snowflake boundary.
 */
class KochSnowflake : public VectorFractal
{
    KochSnowflakeRenderer* _renderer{};

public:
    KochSnowflake(unsigned int width, unsigned int height);
    wxString GetName() const override { return "Koch Snowflake"; }
    CoordinateSystem GetCoordinateSystem() const override { return {_("x"), _("y")}; }
    void Render() override;
    void PreDrawMaps() override;
};
