#include "KochSnowflake.h"
#include <memory>
#include "renderers/vector/KochSnowflakeRenderer.h"

KochSnowflake::KochSnowflake(const unsigned int width, const unsigned int height) : VectorFractal(width, height)
{
    _minX = -1.95;
    _maxX = 1.95;
    _minY = -1.23;
    _maxY = _minY + (_maxX - _minX) * static_cast<double>(_screenHeight) / _screenWidth;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);
    _type = FractalType::KochSnowflake;

    ConfigureIterationDefaults(7, 1);
    SetVectorRenderWorker(std::make_unique<KochSnowflakeRenderer>());
}

void KochSnowflake::Render()
{
    auto& renderer = GetVectorRenderWorker<KochSnowflakeRenderer>();
    renderer.Configure(_maxIterations, GetOptions(), GetSetColor());
    StartVectorRender();
}

void KochSnowflake::PreDrawMaps()
{
    const sf::Color color = GetSetColor();
    for (LineData& line : _lines)
        line.color = color;
}
