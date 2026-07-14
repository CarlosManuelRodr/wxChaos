#include "VectorSierpinskiTriangle.h"
#include <memory>
#include "renderers/VectorSierpinskiTriangleRenderer.h"

VectorSierpinskiTriangle::VectorSierpinskiTriangle(const unsigned int width, const unsigned int height)
    : VectorFractal(width, height)
{
    _minX = -1.76;
    _maxX = 1.76;
    _minY = -0.9;
    _maxY = _minY + (_maxX - _minX) * static_cast<double>(_screenHeight) / _screenWidth;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);
    _type = FractalType::VectorSierpinskiTriangle;

    ConfigureIterationDefaults(7, 1);
    SetVectorRenderWorker(std::make_unique<VectorSierpinskiTriangleRenderer>());
}

void VectorSierpinskiTriangle::Render()
{
    const sf::Vector2u screenSize = GetScreenSize();
    auto& renderer = GetVectorRenderWorker<VectorSierpinskiTriangleRenderer>();
    renderer.Configure(_maxIterations, GetView(), screenSize.x, screenSize.y, GetSetColor());
    StartVectorRender();
}

void VectorSierpinskiTriangle::PreDrawMaps()
{
    const sf::Color color = GetSetColor();
    for (LineData& line : _lines)
        line.color = color;
}
