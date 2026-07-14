#include "KochSnowflake.h"
#include <memory>
#include "renderers/KochSnowflakeRenderer.h"

KochSnowflake::KochSnowflake(const unsigned int width, const unsigned int height) : VectorFractal(width, height)
{
    _minX = -2.56;
    _maxX = 2.55;
    _minY = -1.6;
    _maxY = _minY + (_maxX - _minX) * static_cast<double>(_screenHeight) / _screenWidth;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);
    _type = FractalType::KochSnowflake;

    ConfigureIterationDefaults(7, 1);
    SetVectorRenderWorker(std::make_unique<KochSnowflakeRenderer>());
}

void KochSnowflake::Render()
{
    const sf::Vector2u screenSize = GetScreenSize();
    auto& renderer = GetVectorRenderWorker<KochSnowflakeRenderer>();
    renderer.Configure(_maxIterations, GetView(), screenSize.x, screenSize.y, GetSetColor());
    StartVectorRender();
}

void KochSnowflake::PreDrawMaps()
{
    const sf::Color color = GetSetColor();
    for (LineData& line : _lines)
        line.color = color;
}
