#pragma once

/**
* @enum PaletteMappingMode
* @brief Selects how continuous renderer values are transformed before palette lookup.
*/
enum class PaletteMappingMode
{
    ///< Uses the existing linear palette-cycle mapping.
    Linear,

    ///< Normalizes the renderer value, raises it to an exponent, then maps it through the palette.
    Exponential
};

/**
* @class PaletteMapping
* @brief Transforms continuous renderer values into palette indices.
*
* PaletteMapping keeps color-index math independent from wxWidgets and from the
* rendering loops. The linear mode preserves the historical cycle-length based
* lookup, while exponential mode follows the normalized power mapping used by
* exponentially mapped cyclic coloring.
*/
class PaletteMapping
{
public:
    ///@brief Maps a renderer value to a continuous palette index.
    ///@param value Renderer value produced for the pixel.
    ///@param minValue Lower normalization bound, used by relative/exponential mappings.
    ///@param maxValue Upper normalization bound, or maximum iteration span for non-relative exponential mapping.
    ///@param paletteSize Number of colors in the active palette.
    ///@param cycleLength Renderer-value length corresponding to one palette cycle in linear mode.
    ///@param mode Palette mapping mode to apply.
    ///@param exponent Power used by exponential mapping.
    ///@param relativeColor Whether values should be normalized through the current relative color range.
    ///@return Continuous palette index ready for cyclic palette sampling.
    static double Map(double value, double minValue, double maxValue, unsigned int paletteSize,
                      double cycleLength, PaletteMappingMode mode, double exponent, bool relativeColor);
};
