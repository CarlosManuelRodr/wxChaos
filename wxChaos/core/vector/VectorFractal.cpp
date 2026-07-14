#include "VectorFractal.h"

VectorFractal::VectorFractal(const unsigned int width, const unsigned int height) : Fractal(width, height) {}

VectorFractal::~VectorFractal()
{
    if (_vectorRenderWorker != nullptr)
        _vectorRenderWorker->Stop();
}

void VectorFractal::Resize(const unsigned int width, const unsigned int height)
{
    if (_vectorRenderWorker != nullptr)
        _vectorRenderWorker->Stop();

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
    _geomFigure = !_lines.empty() || !_circles.empty();
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
