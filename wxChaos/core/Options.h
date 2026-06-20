#pragma once
#include "wx/wxGradient.h"
#include "SFML/Graphics.hpp"
#include "wx/PanelOptions.h"
#include "types/RenderingAlgorithmType.h"
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
    unsigned int changeGradient;
    wxGradient gradient;
    RenderingAlgorithmType alg;
    FractalType type;
    unsigned int paletteSize;
    unsigned int gradPaletteSize;
    PanelOptions panelOpt;

    double kReal;
    double kImaginary;

    bool orbitTrapMode;
    bool colorSet;
    bool colorMode;
    bool buddhaMode;
    bool smoothRender;
    bool justLaunchThreads;
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
    changeGradient = 0;
    alg = RenderingAlgorithmType::Other;
    type = FractalType::Undefined;
    paletteSize = gradPaletteSize = 0;
    kReal = kImaginary = 0.0;
    orbitTrapMode = false;
    colorSet = false;
    colorMode = false;
    buddhaMode = false;
    smoothRender = false;
    justLaunchThreads = false;
    relativeColor = false;
    fSetColor = sf::Color::Black;
    screenHeight = screenWidth = 0;
    preciseXFactor = 0;
    preciseYFactor = 0;
    hasPreciseView = false;
    useHighPrecision = false;
    highPrecisionBits = 0;
}
