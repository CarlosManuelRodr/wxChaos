#pragma once
#include <SFML/Graphics.hpp>

struct CircleData
{
    double xCenter, yCenter, radius;
    sf::Color color;
    bool filled = false;
};
