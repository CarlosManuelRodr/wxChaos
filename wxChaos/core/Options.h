#pragma once
#include "wx/gradient.h"
#include "SFML/Graphics.hpp"
#include "wx/PanelOptions.h"
#include "types/ColorMode.h"
#include "types/RenderingAlgorithm.h"
#include "types/FractalType.h"

struct Options
{
    double minX;
    double maxX;
    double minY;
    double maxY;
    double xFactor;
    double yFactor;
    unsigned maxIter;
    int changeGradient;
    wxGradient gradient;
    ColorMode colorPaletteMode;
    RenderingAlgorithm alg;
    FractalType type;
    int paletteSize;
    int gradPaletteSize;
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

    int screenHeight;
    int screenWidth;
};
