#pragma once
#include "wx/common/wxGradient.h"
#include "SFML/Graphics.hpp"
#include "wx/common/PanelOptions.h"
#include "coloring/PaletteMapping.h"
#include "types/RenderingAlgorithmType.h"
#include "types/RenderingPrecisionMode.h"
#include "types/FractalType.h"
#include "numeric/PreciseRect.h"

struct Options
{
    Options();

    double minX;
    double maxX;
    double minY;
    double maxY;
    double xFactor;
    double yFactor;
    unsigned maxIter;
    unsigned int colorVariationOffset;
    wxGradient gradient;
    RenderingAlgorithmType alg;
    RenderingPrecisionMode renderingPrecisionMode;
    FractalType type;
    unsigned int paletteSize;
    unsigned int gradPaletteSize;
    double colorCycleLength;
    double colorRotationSpeed;
    PaletteMappingMode paletteMappingMode;
    double paletteMappingExponent;
    PanelOptions panelOpt;

    double kReal;
    double kImaginary;

    bool orbitTrapMode;
    bool colorSet;
    bool colorMode;
    bool buddhaMode;
    bool smoothRender;
    bool relativeColor;
    sf::Color fSetColor;

    unsigned int screenHeight;
    unsigned int screenWidth;

    PreciseRect preciseView;
    HighPrecisionReal preciseXFactor;
    HighPrecisionReal preciseYFactor;
    bool hasPreciseView;
    bool useHighPrecision;
    unsigned int highPrecisionBits;
};

inline Options::Options()
{
    minX = maxX = minY = maxY = 0.0;
    xFactor = yFactor = 0.0;
    maxIter = 0;
    colorVariationOffset = 0;
    alg = RenderingAlgorithmType::Other;
    renderingPrecisionMode = RenderingPrecisionMode::Adaptative;
    type = FractalType::Undefined;
    paletteSize = gradPaletteSize = 0;
    colorCycleLength = 0.0;
    colorRotationSpeed = 120.0;
    paletteMappingMode = PaletteMappingMode::Linear;
    paletteMappingExponent = 1.5;
    kReal = kImaginary = 0.0;
    orbitTrapMode = false;
    colorSet = false;
    colorMode = false;
    buddhaMode = false;
    smoothRender = false;
    relativeColor = false;
    fSetColor = sf::Color::Black;
    screenHeight = screenWidth = 0;
    preciseXFactor = 0;
    preciseYFactor = 0;
    hasPreciseView = false;
    useHighPrecision = false;
    highPrecisionBits = 0;
}
