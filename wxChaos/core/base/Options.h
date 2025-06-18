#pragma once
#include <wx/gradient.h>
#include <SFML/Graphics.hpp>
#include "../gui/wx/PanelOptions.h"
#include "ColorMode.h"
#include "RenderingAlgorithm.h"
#include "FractalType.h"

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
    int gaussianPaletteSize;
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
    int redInt, greenInt, blueInt;
    double redMean, greenMean, blueMean;
    double redStdDev, greenStdDev, blueStdDev;
    sf::Color fSetColor;

    int screenHeight;
    int screenWidth;
};
