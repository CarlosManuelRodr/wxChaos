#include "ApollonianGasket.h"
#include <memory>
#include "../../renderers/vector/ApollonianGasketRenderer.h"

ApollonianGasket::ApollonianGasket(const unsigned int width, const unsigned int height) : VectorFractal(width, height)
{
    _minX = -1.95;
    _maxX = 1.95;
    _minY = -1.27;
    _maxY = _minY + (_maxX - _minX) * static_cast<double>(_screenHeight) / _screenWidth;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);
    _type = FractalType::ApollonianGasket;

    ConfigureIterationDefaults(6, 1);
    SetVectorRenderWorker(std::make_unique<ApollonianGasketRenderer>());
}

void ApollonianGasket::Render()
{
    const sf::Vector2u screenSize = GetScreenSize();
    auto& renderer = GetVectorRenderWorker<ApollonianGasketRenderer>();
    renderer.Configure(_maxIterations, GetView(), screenSize.x, screenSize.y, GetSetColor());
    StartVectorRender();
}

void ApollonianGasket::PreDrawMaps()
{
    const sf::Color color = GetSetColor();
    for (CircleData& circle : _circles)
        circle.color = color;
}
