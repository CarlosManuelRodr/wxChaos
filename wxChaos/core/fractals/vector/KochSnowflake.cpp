#include "KochSnowflake.h"
#include <memory>
#include "renderers/KochSnowflakeRenderer.h"

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

    auto renderer = std::make_unique<KochSnowflakeRenderer>();
    _renderer = renderer.get();
    SetVectorRenderWorker(std::move(renderer));
}

void KochSnowflake::Render()
{
    const sf::Vector2u screenSize = GetScreenSize();
    _renderer->Configure(_maxIterations, GetView(), screenSize.x, screenSize.y, GetSetColor());
    StartVectorRender();
}

void KochSnowflake::PreDrawMaps()
{
    const sf::Color color = GetSetColor();
    for (LineData& line : _lines)
        line.color = color;
}
