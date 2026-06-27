#include <algorithm>
#include <cmath>
#include "FractalPresenter.h"

constexpr double OldMovementFrameRate = 31.0;
constexpr double MovementAcceleration = OldMovementFrameRate * OldMovementFrameRate;
constexpr double MaximumMovementStepSeconds = 0.05;

FractalPresenter::FractalPresenter(Fractal* fractal) : _committedPanOffset(Vector2Int::Zero())
{
    _usingRenderImage = false;
    _zoomingBack = false;
    _dontDrawTempImage = false;
    _setHandleRightClickZoomBack = true;
    _mousePanning = false;
    _automaticIterations = false;
    _fractal = fractal;
    _automaticIterationBase = fractal->GetIterations();
    _xVel = 0.0;
    _yVel = 0.0;
    _panRemainderX = 0.0;
    _panRemainderY = 0.0;
    _posX = 0;
    _posY = 0;
    _hasCommittedPanOffset = false;
    ResetMovement();
    ResetZoomHistory();
    ResetDisplayImages();
}

void FractalPresenter::SetFractal(Fractal* fractal)
{
    _fractal = fractal;
    _dontDrawTempImage = true;
    ResetMovement();
    ResetZoomHistory();
    ApplyAutomaticIterations();
    ClearImageCache();
    ResetDisplayImages();
}

Fractal* FractalPresenter::GetFractal() const
{
    return _fractal;
}
void FractalPresenter::SetHandleRightClickZoomBack(const bool mode)
{
    _setHandleRightClickZoomBack = mode;
}

void FractalPresenter::ResetDisplayImages()
{
    const sf::Color white(255, 255, 255);
    const sf::Color transparent(255, 255, 255, 0);
    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    _image.create(screenSize.x, screenSize.y, white);
    _tempImage = _image;
    _geomImage.create(screenSize.x, screenSize.y, transparent);

    _texture.loadFromImage(_image);
    _output.setTexture(_texture);

    _tempTexture.loadFromImage(_tempImage);
    _tempSprite.setTexture(_tempTexture);

    _geomTexture.loadFromImage(_geomImage);
    _outGeom.setTexture(_geomTexture);
}

void FractalPresenter::ClearImageCache()
{
    const bool usingCachedRenderImage = _usingRenderImage;

    for (auto& [view, image, iterations] : _zoomHistory)
        image.reset();

    _usingRenderImage = false;
    _zoomingBack = false;

    if (usingCachedRenderImage)
    {
        _fractal->MarkRenderDirty();
        _dontDrawTempImage = true;
    }
}

void FractalPresenter::ResetMovement()
{
    for (bool& direction : _movement)
        direction = false;

    _mousePanning = false;
    _xVel = 0.0;
    _yVel = 0.0;
    _panRemainderX = 0.0;
    _panRemainderY = 0.0;
    _posX = 0;
    _posY = 0;
    _committedPanOffset = {0, 0};
    _hasCommittedPanOffset = false;
}

PreciseRect FractalPresenter::CaptureCurrentView() const
{
    return _fractal->GetPreciseView();
}

void FractalPresenter::ApplyView(const PreciseRect& view)
{
    _fractal->SetPreciseView(view);
    ApplyAutomaticIterations();
}

unsigned int FractalPresenter::CalculateAutomaticIterations() const
{
    const PreciseRect view = CaptureCurrentView();
    const double width = ToDouble(RealAbs(view.right - view.left));
    if (!std::isfinite(width) || width <= 0.0)
        return _automaticIterationBase;

    constexpr double referenceWidth = 3.5;
    constexpr double iterationsPerZoomDoubling = 18.0;
    constexpr unsigned int maximumAutomaticIterations = 20000000;

    const double zoomDepth = std::max(0.0, std::log2(referenceWidth / width));
    const double wantedExtraIterations = std::ceil(zoomDepth * iterationsPerZoomDoubling);
    if (!std::isfinite(wantedExtraIterations))
        return maximumAutomaticIterations;

    const auto maximumExtraIterations = static_cast<double>(maximumAutomaticIterations - std::min(_automaticIterationBase, maximumAutomaticIterations));
    const auto extraIterations = static_cast<unsigned int>(std::clamp(wantedExtraIterations, 0.0, maximumExtraIterations));
    return std::min(_automaticIterationBase + extraIterations, maximumAutomaticIterations);
}

void FractalPresenter::ApplyAutomaticIterations()
{
    if (!_automaticIterations)
        return;

    const unsigned int iterations = CalculateAutomaticIterations();
    if (_fractal->GetIterations() != iterations)
    {
        const bool usingCachedRenderImage = _usingRenderImage;
        _usingRenderImage = false;
        _zoomingBack = false;
        _fractal->SetIterations(iterations);
        if (usingCachedRenderImage)
            _dontDrawTempImage = true;
    }
}

void FractalPresenter::SaveZoom(std::optional<sf::Image> image)
{
    _zoomHistory.push_back({CaptureCurrentView(), std::move(image), _fractal->GetIterations()});
}

void FractalPresenter::ResetZoomHistory()
{
    _zoomHistory.clear();
    _outermostZoom = CaptureCurrentView();
}

void FractalPresenter::ExpandCurrentView()
{
    ApplyView(_fractal->GetPreciseExpandedView());
    _outermostZoom = CaptureCurrentView();
}

void FractalPresenter::Move(const double elapsedSeconds)
{
    if (!_fractal->IsRendered())
        return;

    if (_mousePanning)
        return;

    const double frameSeconds = std::clamp(elapsedSeconds, 0.0, MaximumMovementStepSeconds);
    const double velocityStep = MovementAcceleration * frameSeconds;

    if (_movement[Left])
        _xVel += velocityStep;
    if (_movement[Right])
        _xVel -= velocityStep;
    if (_movement[Up])
        _yVel += velocityStep;
    if (_movement[Down])
        _yVel -= velocityStep;

    if (!_movement[Left] && !_movement[Right] && !_movement[Up] && !_movement[Down])
    {
        if (_xVel > 0) _xVel = std::max(0.0, _xVel - velocityStep);
        if (_xVel < 0) _xVel = std::min(0.0, _xVel + velocityStep);
        if (_yVel > 0) _yVel = std::max(0.0, _yVel - velocityStep);
        if (_yVel < 0) _yVel = std::min(0.0, _yVel + velocityStep);
    }

    if (_xVel != 0 || _yVel != 0)
    {
        const double xMovement = _xVel * frameSeconds + _panRemainderX;
        const double yMovement = _yVel * frameSeconds + _panRemainderY;
        const auto pixelDeltaX = static_cast<int>(std::trunc(xMovement));
        const auto pixelDeltaY = static_cast<int>(std::trunc(yMovement));
        _panRemainderX = xMovement - pixelDeltaX;
        _panRemainderY = yMovement - pixelDeltaY;

        if (pixelDeltaX != 0 || pixelDeltaY != 0)
        {
            _fractal->PanViewByPixels(pixelDeltaX, pixelDeltaY);
            _posX += pixelDeltaX;
            _posY += pixelDeltaY;
        }
    }
    else if (_posX != 0 || _posY != 0)
    {
        if (_fractal->ShouldResumeFromPausedPan())
        {
            _fractal->ResumeFromPausedPan();
            ResetMovement();
        }
        else
        {
            _committedPanOffset = {_posX, _posY};
            _hasCommittedPanOffset = true;
            _posX = 0;
            _posY = 0;
        }
    }
}

bool FractalPresenter::IsMoving() const
{
    return _mousePanning || _xVel != 0 || _yVel != 0 || _movement[Up] || _movement[Down] || _movement[Left] || _movement[Right];
}

void FractalPresenter::SetMovement(const Direction direction)
{
    ClearImageCache();
    ResetZoomHistory();

    switch (direction)
    {
        case Up:
            _movement[Up] = true;
            break;
        case Down:
            _movement[Down] = true;
            break;
        case Left:
            _movement[Left] = true;
            break;
        case Right:
            _movement[Right] = true;
            break;
        default:
            break;
    }
}

void FractalPresenter::ReleaseMovement(const Direction direction)
{
    switch (direction)
    {
        case Up:
            _movement[Up] = false;
            break;
        case Down:
            _movement[Down] = false;
            break;
        case Left:
            _movement[Left] = false;
            break;
        case Right:
            _movement[Right] = false;
            break;
        default:
            break;
    }
}

void FractalPresenter::BeginMousePan()
{
    if (!_fractal->IsRendered())
        return;

    ClearImageCache();
    ResetZoomHistory();
    _mousePanning = true;
    _xVel = 0;
    _yVel = 0;

    for (bool& direction : _movement)
        direction = false;
}

void FractalPresenter::PanByMousePixels(const int pixelDeltaX, const int pixelDeltaY)
{
    if (!_mousePanning || !_fractal->IsRendered())
        return;

    _fractal->PanViewByPixels(pixelDeltaX, pixelDeltaY);
    _posX += pixelDeltaX;
    _posY += pixelDeltaY;
}

void FractalPresenter::EndMousePan()
{
    _mousePanning = false;
}

void FractalPresenter::HandleEvent(const sf::Event& event)
{
    if (!_fractal->IsRendering() && event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::L:
                IncreaseIterations();
                break;
            case sf::Keyboard::K:
                DecreaseIterations();
                break;
            default:
                break;
        }
    }

    if (_setHandleRightClickZoomBack && event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Right && !IsMoving())
    {
        ZoomBack();
    }
}

void FractalPresenter::Resize(const sf::RenderWindow* window)
{
    _dontDrawTempImage = true;
    ResetMovement();
    _fractal->Resize(window->getSize().x, window->getSize().y);
    _outermostZoom = CaptureCurrentView();
    ApplyAutomaticIterations();
    const sf::Vector2u screenSize = _fractal->GetScreenSize();

    for (ZoomHistoryEntry& entry : _zoomHistory)
    {
        PreciseRect& view = entry.view;
        view.top = view.bottom + (view.right - view.left) * HighPrecisionReal(screenSize.y) / HighPrecisionReal(screenSize.x);
    }

    _fractal->MarkRenderDirty();
    _fractal->MarkOrbitDirty();
    ClearImageCache();
    ResetDisplayImages();

    _tempSprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y)));
    sf::IntRect size;
    size.width = static_cast<int>(screenSize.x);
    size.height = static_cast<int>(screenSize.y);
    _output.setTextureRect(size);
    _outGeom.setTextureRect(size);
}

void FractalPresenter::SetAreaOfView(const sf::Rect<int>& pixelCoordinates)
{
    ResetMovement();
    const bool wasPaused = _fractal->IsPausedForPresentation();
    std::optional<sf::Image> renderedImage;

    if (wasPaused)
    {
        ClearImageCache();
        _dontDrawTempImage = true;
    }
    else
    {
        if (_fractal->IsRendered())
            renderedImage = _image;
        _dontDrawTempImage = false;
    }

    SaveZoom(std::move(renderedImage));

    ApplyView(_fractal->GetPreciseViewForPixelRect(pixelCoordinates));
    if (wasPaused)
        _fractal->MarkRenderInterrupted();

    _fractal->MarkOrbitDirty();
    _tempImage = _image;
    _tempTexture.loadFromImage(_tempImage);
    _tempSprite.setTexture(_tempTexture);
    _tempSprite.setTextureRect(pixelCoordinates);
    _tempSprite.setPosition(0, 0);

    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    const float scaleX = static_cast<float>(screenSize.x) / static_cast<float>(pixelCoordinates.width);
    const float scaleY = static_cast<float>(screenSize.y) / static_cast<float>(pixelCoordinates.height);
    _tempSprite.setScale(scaleX, scaleY);
    _usingRenderImage = false;
    _zoomingBack = false;
}

void FractalPresenter::ZoomBack()
{
    _fractal->StopRender();
    ResetMovement();
    std::optional<sf::Image> cachedImage;
    unsigned int cachedImageIterations = 0;

    if (!_zoomHistory.empty())
    {
        ZoomHistoryEntry entry = std::move(_zoomHistory.back());
        _zoomHistory.pop_back();
        ApplyView(entry.view);
        cachedImage = std::move(entry.image);
        cachedImageIterations = entry.iterations;
    }
    else
        ExpandCurrentView();

    _fractal->MarkRenderInterrupted();
    _fractal->MarkOrbitDirty();

    if (cachedImage.has_value() && cachedImageIterations == _fractal->GetIterations() && !_fractal->IsGradientAnimating())
    {
        _image = *cachedImage;
        _texture.loadFromImage(_image);
        _usingRenderImage = true;
        _fractal->MarkRenderComplete();
        _zoomingBack = false;
        _dontDrawTempImage = true;
    }
    else
    {
        _fractal->MarkRenderDirty();
        _zoomingBack = true;
    }
}

Rect FractalPresenter::GetOutermostZoom() const
{
    return _outermostZoom.ToRect();
}

Rect FractalPresenter::GetCurrentZoom() const
{
    return CaptureCurrentView().ToRect();
}

bool FractalPresenter::HasZoomed() const
{
    const PreciseRect currentZoom = CaptureCurrentView();
    return _outermostZoom.left != currentZoom.left ||
        _outermostZoom.right != currentZoom.right ||
        _outermostZoom.bottom != currentZoom.bottom ||
        _outermostZoom.top != currentZoom.top;
}

void FractalPresenter::Redraw()
{
    ResetMovement();

    if (_fractal->IsExteriorColorEnabled())
    {
        _tempImage = _image;
        _tempTexture.loadFromImage(_tempImage);
        _tempSprite.setOrigin(0, 0);

        const sf::Vector2u screenSize = _fractal->GetScreenSize();
        const float xFactor = static_cast<float>(screenSize.x) / static_cast<float>(_tempImage.getSize().x);
        const float yFactor = static_cast<float>(screenSize.y) / static_cast<float>(_tempImage.getSize().y);
        _tempSprite.setScale(xFactor, yFactor);
    }

    _fractal->Redraw();
    ClearImageCache();
    _dontDrawTempImage = true;
}

void FractalPresenter::SetView(const Rect& view)
{
    _fractal->StopRender();
    ResetMovement();
    ApplyView(PreciseRect(view));
    _fractal->MarkOrbitDirty();
    ClearImageCache();
    ResetZoomHistory();
    _dontDrawTempImage = true;
}

void FractalPresenter::IncreaseIterations()
{
    _automaticIterations = false;
    ClearImageCache();
    const unsigned change = _fractal->GetIterations() + 100;
    _fractal->SetIterations(change);
}

void FractalPresenter::DecreaseIterations()
{
    _automaticIterations = false;
    ClearImageCache();

    if (_fractal->GetIterations() > 100)
    {
        const unsigned int change = _fractal->GetIterations() - 100;
        _fractal->SetIterations(change);
    }
}

void FractalPresenter::ChangeIterations(const unsigned int iterations)
{
    _automaticIterations = false;
    _automaticIterationBase = iterations;
    ClearImageCache();
    _fractal->SetIterations(iterations);
}

void FractalPresenter::SetAutomaticIterations(const bool mode)
{
    _automaticIterations = mode;
    _automaticIterationBase = _fractal->GetIterations();
    ApplyAutomaticIterations();
    if (mode)
    {
        _fractal->MarkRenderDirty();
        _fractal->MarkOrbitDirty();
    }
}

bool FractalPresenter::AutomaticIterationsEnabled() const
{
    return _automaticIterations;
}

void FractalPresenter::SetK(const double real, const double imaginary)
{
    ClearImageCache();
    _fractal->SetK(real, imaginary);
}

void FractalPresenter::SetGradient(const wxGradient& gradient)
{
    ClearImageCache();
    _fractal->SetGradient(gradient);
}

void FractalPresenter::SetGradientSize(const unsigned int size)
{
    ClearImageCache();
    _fractal->SetGradientSize(size);
}

void FractalPresenter::SetColorCycleLength(const double cycleLength)
{
    ClearImageCache();
    _fractal->SetColorCycleLength(cycleLength);
}

void FractalPresenter::SetColorRotationSpeed(const double speed) const
{
    _fractal->SetColorRotationSpeed(speed);
}

void FractalPresenter::SetPaletteMappingMode(const PaletteMappingMode mode)
{
    ClearImageCache();
    _fractal->SetPaletteMappingMode(mode);
}

void FractalPresenter::SetPaletteMappingExponent(const double exponent)
{
    ClearImageCache();
    _fractal->SetPaletteMappingExponent(exponent);
}

void FractalPresenter::SetColorPalette(const ColorPaletteTypes palette)
{
    ClearImageCache();
    _fractal->SetColorPalette(palette);
}

void FractalPresenter::SetExteriorColorMode(const bool mode)
{
    ClearImageCache();
    _fractal->SetExteriorColorMode(mode);
}

void FractalPresenter::SetFractalSetColorMode(const bool mode)
{
    ClearImageCache();
    _fractal->SetFractalSetColorMode(mode);
}

void FractalPresenter::SetFractalSetColor(const sf::Color color)
{
    ClearImageCache();
    _fractal->SetFractalSetColor(color);
}

void FractalPresenter::SetRelativeColor(const bool mode)
{
    ClearImageCache();
    _fractal->SetRelativeColor(mode);
}

void FractalPresenter::ToggleColorRotation()
{
    ClearImageCache();
    _fractal->ToggleColorRotation();
}

void FractalPresenter::SetColorVariationOffset(const int offset)
{
    ClearImageCache();
    _fractal->SetVarGradient(offset);
}

void FractalPresenter::SetAlgorithm(const RenderingAlgorithmType algorithm)
{
    ClearImageCache();
    _fractal->SetAlgorithm(algorithm);
}

void FractalPresenter::SetRenderingPrecisionMode(const RenderingPrecisionMode mode)
{
    ClearImageCache();
    _fractal->SetRenderingPrecisionMode(mode);
}

void FractalPresenter::SetOrbitTrapMode(const bool mode)
{
    ClearImageCache();
    _fractal->SetOrbitTrapMode(mode);
}

void FractalPresenter::SetSmoothRender(const bool mode)
{
    ClearImageCache();
    _fractal->SetSmoothRender(mode);
}

void FractalPresenter::DrawMaps(sf::RenderWindow* window)
{
    _fractal->PreDrawMaps();
    const sf::Vector2u screenSize = _fractal->GetScreenSize();

    if (_zoomingBack || _dontDrawTempImage || !_fractal->IsExteriorColorEnabled())
        _image.create(screenSize.x, screenSize.y, sf::Color(255, 255, 255));
    else
    {
        _image.create(screenSize.x, screenSize.y, sf::Color(255, 255, 255, 0));
        window->draw(_tempSprite);
    }

    _output.setPosition(0, 0);
    _fractal->PrepareDisplayColorLookup();

    for (unsigned int i = 0; i < screenSize.x; i++)
    {
        for (unsigned int j = 0; j < screenSize.y; j++)
        {
            if (_fractal->HasDisplayPixelColor(i, j))
            {
                _image.setPixel(i, j, _fractal->GetRenderedPixelColor(i, j));
            }
            else if (_fractal->IsExteriorColorEnabled() && (_zoomingBack || _dontDrawTempImage))
            {
                _image.setPixel(i, j, _fractal->GetInvalidPixelColor());
            }
        }
    }

    _texture.loadFromImage(_image);
    window->draw(_output);
}

void FractalPresenter::DrawGeometry(sf::RenderWindow* window) const
{
    for (const LineData& line : _fractal->GetLines())
    {
        const auto x1 = static_cast<float>(_fractal->GetPixelX(line.x1));
        const auto y1 = static_cast<float>(_fractal->GetPixelY(line.y1));
        const auto x2 = static_cast<float>(_fractal->GetPixelX(line.x2));
        const auto y2 = static_cast<float>(_fractal->GetPixelY(line.y2));
        const sf::Vertex vertices[] = { sf::Vertex(sf::Vector2f(x1, y1), line.color), sf::Vertex(sf::Vector2f(x2, y2), line.color) };
        window->draw(vertices, 2, sf::Lines);
    }

    for (const LineData& orbitLine : _fractal->GetOrbitLines())
    {
        const auto x1 = static_cast<float>(_fractal->GetPixelX(orbitLine.x1));
        const auto y1 = static_cast<float>(_fractal->GetPixelY(orbitLine.y1));
        const auto x2 = static_cast<float>(_fractal->GetPixelX(orbitLine.x2));
        const auto y2 = static_cast<float>(_fractal->GetPixelY(orbitLine.y2));
        const sf::Vertex vertices[] = { sf::Vertex(sf::Vector2f(x1, y1), orbitLine.color), sf::Vertex(sf::Vector2f(x2, y2), orbitLine.color) };
        window->draw(vertices, 2, sf::Lines);
    }

    for (const CircleData& circleData : _fractal->GetCircles())
    {
        const auto x0 = static_cast<float>(_fractal->GetPixelX(circleData.xCenter));
        const auto y0 = static_cast<float>(_fractal->GetPixelY(circleData.yCenter));
        const auto right = static_cast<float>(_fractal->GetPixelX(circleData.xCenter + circleData.radius));
        const float r = right - x0;
        sf::CircleShape circle(r);
        circle.setPosition(x0 - r, y0 - r);
        circle.setFillColor(circleData.filled ? circleData.color : sf::Color::Transparent);
        circle.setOutlineColor(circleData.color);
        circle.setOutlineThickness(2);
        window->draw(circle);
    }
}

void FractalPresenter::Show(sf::RenderWindow* window, const double elapsedSeconds)
{
    if (_fractal->IsRendered() && IsMoving())
    {
        // While panning, only draw the shifted render output. Showing the
        // cached preview sprite here leaves stale pixels in the newly exposed area.
        _dontDrawTempImage = true;
        _output.setPosition(static_cast<float>(_posX), static_cast<float>(_posY));
    }
    else
    {
        Vector2Int renderOffset = {0, 0};

        if (_hasCommittedPanOffset)
        {
            renderOffset = _committedPanOffset;
            _fractal->ReuseRenderedMaps(_committedPanOffset);
            _hasCommittedPanOffset = false;
            _fractal->MarkRenderDirty();
        }

        if (!_fractal->IsRendered())
        {
            if (_usingRenderImage)
            {
                _usingRenderImage = false;
                renderOffset = {0, 0};
                _committedPanOffset = {0, 0};
                _hasCommittedPanOffset = false;
            }

            if (!_fractal->IsRenderStarted())
            {
                _fractal->MarkRenderStarted();
                _fractal->PrepareRender(renderOffset);
                _fractal->Render();
            }

            _committedPanOffset = {0, 0};

            DrawMaps(window);

            if (!_fractal->IsRendering())
            {
                _fractal->MarkRenderComplete();
                _dontDrawTempImage = false;
                _zoomingBack = false;
                _fractal->PostRender();
                DrawMaps(window);
                _fractal->ConsumeImageRefreshRequest();
            }
        }

        if (_fractal->ConsumePausePresentationRefresh())
            DrawMaps(window);

        if (_fractal->ConsumeImageRefreshRequest())
            DrawMaps(window);

        const bool gradientChanged = _fractal->ConsumeGradientChangeRequest();
        if (_fractal->IsGradientAnimating() || gradientChanged)
        {
            if (_fractal->IsGradientAnimating())
                _fractal->AdvanceGradientOffset(elapsedSeconds);

            if (_fractal->IsRendered())
            {
                _fractal->RefreshAnimatedColors(_image);
                _texture.loadFromImage(_image);
            }
        }
    }

    if (!_dontDrawTempImage && _fractal->IsExteriorColorEnabled())
        window->draw(_tempSprite);

    window->draw(_output);

    if (_fractal->ShouldDrawOrbit() && !_fractal->IsRendering())
    {
        if (!_fractal->IsOrbitDrawn())
        {
            const sf::Vector2u screenSize = _fractal->GetScreenSize();
            _fractal->ClearOrbitLines();
            _geomImage.create(screenSize.x, screenSize.y, sf::Color(255, 255, 255, 0));
            _fractal->DrawOrbit();
            _geomTexture.loadFromImage(_geomImage);
        }
        window->draw(_outGeom);
    }
    if (_fractal->HasGeometryFigures() && !_fractal->IsRendering())
        DrawGeometry(window);

}
