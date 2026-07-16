#include "VectorFractal.h"
#include <algorithm>
#include <cmath>

VectorFractal::VectorFractal(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    _hasHighPrecisionRender = true;
}

VectorFractal::~VectorFractal()
{
    if (_vectorRenderWorker != nullptr)
        _vectorRenderWorker->Stop();
}

void VectorFractal::Resize(const unsigned int width, const unsigned int height)
{
    if (_vectorRenderWorker != nullptr)
        _vectorRenderWorker->Stop();
    MarkRenderInterrupted();

    _screenWidth = width;
    _screenHeight = height;

    EnsurePreciseViewInitialized();
    _preciseView.top = _preciseView.bottom + (_preciseView.right - _preciseView.left) *
        HighPrecisionReal(_screenHeight) / HighPrecisionReal(_screenWidth);
    UpdatePreciseFactors();
    SyncDoubleViewFromPrecise();
    MarkRenderDirty();
}

void VectorFractal::PrepareRender(const Vector2Int)
{
    if (_vectorRenderWorker != nullptr)
        _vectorRenderWorker->Stop();

    ClearGeometryFigures();
    PreRender();
}

void VectorFractal::SetVectorRenderWorker(std::unique_ptr<VectorRenderWorker> worker)
{
    if (_vectorRenderWorker != nullptr)
        _vectorRenderWorker->Stop();

    _vectorRenderWorker = std::move(worker);
}

void VectorFractal::StartVectorRender()
{
    if (_vectorRenderWorker == nullptr)
        return;

    _vectorRenderWorker->Start();
    if (_waitRoutine)
    {
        _vectorRenderWorker->Wait();
        PublishCompletedGeometry();
    }
}

void VectorFractal::PublishCompletedGeometry()
{
    if (_vectorRenderWorker == nullptr)
        return;

    VectorRenderWorker::Geometry geometry;
    if (!_vectorRenderWorker->TakeCompletedGeometry(geometry))
        return;

    _lines = std::move(geometry.lines);
    _circles = std::move(geometry.circles);
    _rectangles = std::move(geometry.rectangles);
    _geomFigure = !_lines.empty() || !_circles.empty() || !_rectangles.empty();
    _refreshImage = true;
}

int VectorFractal::GetRenderProgress() const
{
    return _vectorRenderWorker != nullptr ? _vectorRenderWorker->GetProgress() : 100;
}

void VectorFractal::PauseContinue()
{
    if (_vectorRenderWorker == nullptr || !_vectorRenderWorker->IsRunning())
        return;

    _paused = !_vectorRenderWorker->IsPaused();
    _vectorRenderWorker->SetPaused(_paused);
}

bool VectorFractal::StopRender()
{
    if (_vectorRenderWorker == nullptr || !_vectorRenderWorker->IsRunning())
        return false;

    _vectorRenderWorker->Stop();
    _rendering = false;
    _paused = false;
    return true;
}

bool VectorFractal::IsRendering()
{
    if (_waitRoutine || _vectorRenderWorker == nullptr)
        return false;

    PublishCompletedGeometry();
    return _vectorRenderWorker->IsRunning();
}

const std::vector<LineData>& VectorFractal::GetOrbitLines() const
{
    static const std::vector<LineData> noOrbitLines;
    return noOrbitLines;
}

void VectorFractal::DrawPrimitives(sf::RenderTarget& target) const
{
    sf::VertexArray rectangles(sf::Quads);
    for (const RectangleData& rectangle : _rectangles)
    {
        if (rectangle.screenSpace)
        {
            rectangles.append(sf::Vertex({static_cast<float>(rectangle.left), static_cast<float>(rectangle.top)},
                                         rectangle.color));
            rectangles.append(sf::Vertex({static_cast<float>(rectangle.right), static_cast<float>(rectangle.top)},
                                         rectangle.color));
            rectangles.append(sf::Vertex({static_cast<float>(rectangle.right), static_cast<float>(rectangle.bottom)},
                                         rectangle.color));
            rectangles.append(sf::Vertex({static_cast<float>(rectangle.left), static_cast<float>(rectangle.bottom)},
                                         rectangle.color));
            continue;
        }

        const double clippedLeft = std::max(rectangle.left, _minX);
        const double clippedRight = std::min(rectangle.right, _maxX);
        const double clippedBottom = std::max(rectangle.bottom, _minY);
        const double clippedTop = std::min(rectangle.top, _maxY);
        if (clippedLeft >= clippedRight || clippedBottom >= clippedTop)
            continue;

        const auto left = static_cast<float>(GetPixelX(clippedLeft));
        const auto right = static_cast<float>(GetPixelX(clippedRight));
        const auto top = static_cast<float>(GetPixelY(clippedTop));
        const auto bottom = static_cast<float>(GetPixelY(clippedBottom));
        rectangles.append(sf::Vertex({left, top}, rectangle.color));
        rectangles.append(sf::Vertex({right, top}, rectangle.color));
        rectangles.append(sf::Vertex({right, bottom}, rectangle.color));
        rectangles.append(sf::Vertex({left, bottom}, rectangle.color));
    }
    if (rectangles.getVertexCount() != 0)
        target.draw(rectangles);

    for (const LineData& line : _lines)
    {
        if (line.screenSpace)
        {
            const sf::Vertex vertices[] = {
                sf::Vertex({static_cast<float>(line.x1), static_cast<float>(line.y1)}, line.color),
                sf::Vertex({static_cast<float>(line.x2), static_cast<float>(line.y2)}, line.color)
            };
            target.draw(vertices, 2, sf::Lines);
            continue;
        }

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
        const float radius = std::abs(static_cast<float>(GetPixelX(circleData.xCenter + circleData.radius)) - centerX);
        sf::CircleShape circle(radius, CalculateCirclePointCount(radius));
        circle.setPosition(centerX - radius, centerY - radius);
        circle.setFillColor(circleData.filled ? circleData.color : sf::Color::Transparent);
        circle.setOutlineColor(circleData.color);
        circle.setOutlineThickness(2.0F);
        target.draw(circle);
    }
}

std::size_t VectorFractal::CalculateCirclePointCount(const float pixelRadius)
{
    constexpr std::size_t minimumPointCount = 32;
    constexpr std::size_t maximumPointCount = 8192;
    constexpr double maximumDeviationPixels = 0.2;
    constexpr double pi = 3.14159265358979323846;

    if (pixelRadius <= maximumDeviationPixels)
        return minimumPointCount;

    const double cosine = std::clamp(1.0 - maximumDeviationPixels / pixelRadius, -1.0, 1.0);
    const double halfSegmentAngle = std::acos(cosine);
    if (halfSegmentAngle <= 0.0)
        return maximumPointCount;

    const auto pointCount = static_cast<std::size_t>(std::ceil(pi / halfSegmentAngle));
    return std::clamp(pointCount, minimumPointCount, maximumPointCount);
}

sf::Image VectorFractal::GetRenderedImage()
{
    PublishCompletedGeometry();

    if (!_rendered && !_rendering)
    {
        RenderBlocking();
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
