#pragma once

#include <SFML/Graphics.hpp>

/** @brief World-coordinate filled rectangle used by vector fractals. */
struct RectangleData
{
    double left{};              ///< Minimum horizontal coordinate.
    double right{};             ///< Maximum horizontal coordinate.
    double bottom{};            ///< Minimum vertical coordinate.
    double top{};               ///< Maximum vertical coordinate.
    sf::Color color;            ///< Display color used to fill the rectangle.
    bool belongsToSet = true;   ///< Whether the rectangle fills or clears box-count occupancy.
    bool screenSpace = false;   ///< Coordinates are pixels rather than world coordinates when true.
};
