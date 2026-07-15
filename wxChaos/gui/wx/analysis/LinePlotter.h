#pragma once

#include <mathplot.h>

/**
 * @struct LineParams
 * @brief Slope-intercept coefficients for a fitted line.
 */
struct LineParams
{
    double m;   ///< Line slope.
    double b;   ///< Vertical intercept.
};

/**
 * @class LinePlotter
 * @brief MathPlot function layer that evaluates a fitted line as y = mx + b.
 */
class LinePlotter : public mpFX
{
    LineParams _params;   ///< Coefficients evaluated by GetY().

public:
    /** @brief Creates a function layer for the supplied line coefficients. */
    explicit LinePlotter(LineParams params);

    /**
     * @brief Evaluates the fitted line at an x coordinate.
     * @return The value m * x + b.
     */
    double GetY(double x) override;
};
