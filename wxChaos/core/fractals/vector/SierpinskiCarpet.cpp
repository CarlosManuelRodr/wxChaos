#include "SierpinskiCarpet.h"
#include <memory>
#include "renderers/vector/SierpinskiCarpetRenderer.h"

SierpinskiCarpet::SierpinskiCarpet(const unsigned int width, const unsigned int height)
    : VectorFractal(width, height)
{
    _minX = -1.3;
    _maxX = 1.3;
    _minY = -1.3;
    _maxY = _minY + (_maxX - _minX) * static_cast<double>(_screenHeight) / _screenWidth;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);
    _type = FractalType::SierpinskiCarpet;

    ConfigureIterationDefaults(7, 1);
    SetVectorRenderWorker(std::make_unique<SierpinskiCarpetRenderer>());
}

void SierpinskiCarpet::Render()
{
    auto& renderer = GetVectorRenderWorker<SierpinskiCarpetRenderer>();
    renderer.Configure(_maxIterations, GetOptions(), GetSetColor());
    StartVectorRender();
}

void SierpinskiCarpet::PreDrawMaps()
{
    const sf::Color setColor = GetSetColor();
    for (RectangleData& rectangle : _rectangles)
        rectangle.color = rectangle.belongsToSet ? setColor : sf::Color::White;
}
