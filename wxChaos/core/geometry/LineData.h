#pragma once
#include <SFML/Graphics.hpp>

struct LineData
{
    double x1, y1, x2, y2;
    sf::Color color;
    bool screenSpace = false; ///< Coordinates are pixels rather than world coordinates when true.
};
