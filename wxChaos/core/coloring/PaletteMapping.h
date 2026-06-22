#pragma once

enum class PaletteMappingMode
{
    Linear,
    Exponential
};

class PaletteMapping
{
public:
    static double Map(double value, double minValue, double maxValue, unsigned int paletteSize,
                      double cycleLength, PaletteMappingMode mode, double exponent, bool relativeColor);
};
