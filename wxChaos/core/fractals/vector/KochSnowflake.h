#pragma once

#include "../../Translation.h"
#include "../../vector/VectorFractal.h"

/**
 * @class KochSnowflake
 * @brief Vector rendering of the Koch snowflake boundary.
 */
class KochSnowflake : public VectorFractal
{
public:
    KochSnowflake(unsigned int width, unsigned int height);
    wxString GetName() const override { return WXCHAOS_TRANSLATE_NOOP("Koch Snowflake"); }
    CoordinateSystem GetCoordinateSystem() const override { return {_("x"), _("y")}; }
    void Render() override;
    void PreDrawMaps() override;
};
