#include "VectorFractal.h"

VectorFractal::VectorFractal(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    _antiAliasingScale = 1;
}

bool**& VectorFractal::SetMapStorage()
{
    static bool** unused = nullptr;
    return unused;
}

bool** VectorFractal::SetMapStorage() const
{
    return nullptr;
}

double**& VectorFractal::ColorMapStorage()
{
    static double** unused = nullptr;
    return unused;
}

double** VectorFractal::ColorMapStorage() const
{
    return nullptr;
}

RenderThreadPool& VectorFractal::RenderPoolStorage()
{
    static RenderThreadPool unused;
    return unused;
}

const RenderThreadPool& VectorFractal::RenderPoolStorage() const
{
    static const RenderThreadPool unused;
    return unused;
}

std::vector<LineData>& VectorFractal::OrbitLinesStorage()
{
    static std::vector<LineData> unused;
    return unused;
}

const std::vector<LineData>& VectorFractal::OrbitLinesStorage() const
{
    static const std::vector<LineData> unused;
    return unused;
}

void VectorFractal::Resize(const unsigned int width, const unsigned int height)
{
    _screenWidth = width;
    _screenHeight = height;
    UpdateRenderDimensions();

    EnsurePreciseViewInitialized();
    _preciseView.top = _preciseView.bottom + (_preciseView.right - _preciseView.left) *
        HighPrecisionReal(_screenHeight) / HighPrecisionReal(_screenWidth);
    UpdatePreciseFactors();
    SyncDoubleViewFromPrecise();
    MarkRenderDirty();
}

void VectorFractal::PrepareRender(const Vector2Int)
{
    ClearGeometryFigures();
    PreRender();
}

const std::vector<LineData>& VectorFractal::GetOrbitLines() const
{
    static const std::vector<LineData> noOrbitLines;
    return noOrbitLines;
}

void VectorFractal::DrawPrimitives(sf::RenderTarget& target) const
{
    for (const LineData& line : _lines)
    {
        const sf::Vertex vertices[] = {
            sf::Vertex(sf::Vector2f(static_cast<float>(GetPixelX(line.x1)), static_cast<float>(GetPixelY(line.y1))), line.color),
            sf::Vertex(sf::Vector2f(static_cast<float>(GetPixelX(line.x2)), static_cast<float>(GetPixelY(line.y2))), line.color)
        };
        target.draw(vertices, 2, sf::Lines);
    }

    for (const CircleData& circleData : _circles)
    {
        const auto centerX = static_cast<float>(GetPixelX(circleData.xCenter));
        const auto centerY = static_cast<float>(GetPixelY(circleData.yCenter));
        const float radius = static_cast<float>(GetPixelX(circleData.xCenter + circleData.radius)) - centerX;
        sf::CircleShape circle(radius);
        circle.setPosition(centerX - radius, centerY - radius);
        circle.setFillColor(circleData.filled ? circleData.color : sf::Color::Transparent);
        circle.setOutlineColor(circleData.color);
        circle.setOutlineThickness(2.0F);
        target.draw(circle);
    }
}

sf::Image VectorFractal::GetRenderedImage()
{
    if (!_rendered && !_rendering)
    {
        PrepareRender();
        Render();
        MarkRenderComplete();
    }

    sf::RenderTexture target;
    if (!target.create(_screenWidth, _screenHeight))
    {
        sf::Image image;
        image.create(_screenWidth, _screenHeight, sf::Color::White);
        return image;
    }

    target.clear(sf::Color::White);
    DrawPrimitives(target);
    target.display();
    return target.getTexture().copyToImage();
}

bool VectorFractal::SaveBmp(const std::string& filename)
{
    return GetRenderedImage().saveToFile(filename);
}
