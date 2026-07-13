#include <cmath>
#include "KochSnowflake.h"

KochSnowflake::KochSnowflake(const unsigned int width, const unsigned int height) : VectorFractal(width, height)
{
    _minX = -1.25;
    _maxX = 1.25;
    _minY = -0.9;
    _maxY = _minY + (_maxX - _minX) * static_cast<double>(_screenHeight) / _screenWidth;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);

    ConfigureIterationDefaults(4, 1);
    _type = FractalType::KochSnowflake;
}

void KochSnowflake::AppendKochSegment(const double x1, const double y1, const double x2, const double y2,
                                      const unsigned int iterations, const sf::Color& color)
{
    if (iterations == 0)
    {
        DrawLine(x1, y1, x2, y2, color);
        return;
    }

    constexpr double cosine60 = 0.5;
    constexpr double sine60 = 0.86602540378443864676;
    const double deltaX = (x2 - x1) / 3.0;
    const double deltaY = (y2 - y1) / 3.0;
    const double firstX = x1 + deltaX;
    const double firstY = y1 + deltaY;
    const double secondX = x1 + 2.0 * deltaX;
    const double secondY = y1 + 2.0 * deltaY;
    const double peakX = firstX + deltaX * cosine60 - deltaY * sine60;
    const double peakY = firstY + deltaX * sine60 + deltaY * cosine60;
    const unsigned int remainingIterations = iterations - 1;

    AppendKochSegment(x1, y1, firstX, firstY, remainingIterations, color);
    AppendKochSegment(firstX, firstY, peakX, peakY, remainingIterations, color);
    AppendKochSegment(peakX, peakY, secondX, secondY, remainingIterations, color);
    AppendKochSegment(secondX, secondY, x2, y2, remainingIterations, color);
}

void KochSnowflake::Render()
{
    constexpr double lowerY = -0.57735026918962576451;
    constexpr double upperY = 1.1547005383792515290;
    const sf::Color color = GetSetColor();

    AppendKochSegment(-1.0, lowerY, 0.0, upperY, _maxIterations, color);
    AppendKochSegment(0.0, upperY, 1.0, lowerY, _maxIterations, color);
    AppendKochSegment(1.0, lowerY, -1.0, lowerY, _maxIterations, color);
}

void KochSnowflake::PreDrawMaps()
{
    const sf::Color color = GetSetColor();
    for (LineData& line : _lines)
        line.color = color;
}
