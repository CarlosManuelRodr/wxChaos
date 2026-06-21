#include "SFMLFractal.h"

constexpr int stdSpeed = 1;

SFMLFractal::SFMLFractal(Fractal* fractal) : _committedPanOffset(Vector2Int::Zero())
{
    _imgInVector = false;
    _usingRenderImage = false;
    _zoomingBack = false;
    _dontDrawTempImage = false;
    _setHandleRightClickZoomBack = true;
    _fractal = fractal;
    _xVel = 0.0f;
    _yVel = 0.0f;
    _posX = 0;
    _posY = 0;
    _hasCommittedPanOffset = false;
    ResetMovement();
    ResetZoomHistory();
    ResetDisplayImages();
}

void SFMLFractal::SetFractal(Fractal* fractal)
{
    _fractal = fractal;
    _dontDrawTempImage = true;
    ResetMovement();
    ResetZoomHistory();
    ClearImageCache();
    ResetDisplayImages();
}

Fractal* SFMLFractal::GetFractal() const
{
    return _fractal;
}
void SFMLFractal::SetHandleRightClickZoomBack(const bool mode)
{
    _setHandleRightClickZoomBack = mode;
}

void SFMLFractal::ResetDisplayImages()
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

void SFMLFractal::ClearImageCache()
{
    const bool usingCachedRenderImage = _usingRenderImage;

    _imgCache.clear();
    _imgInVector = false;
    _usingRenderImage = false;
    _zoomingBack = false;

    if (usingCachedRenderImage)
    {
        _fractal->MarkRenderDirty();
        _dontDrawTempImage = true;
    }
}

void SFMLFractal::ResetMovement()
{
    for (bool& direction : _movement)
        direction = false;

    _xVel = 0;
    _yVel = 0;
    _posX = 0;
    _posY = 0;
    _committedPanOffset = {0, 0};
    _hasCommittedPanOffset = false;
}

PreciseRect SFMLFractal::CaptureCurrentView() const
{
    return _fractal->GetPreciseView();
}

void SFMLFractal::ApplyView(const PreciseRect& view) const
{
    _fractal->SetPreciseView(view);
}

void SFMLFractal::SaveZoom()
{
    _zoomHistory.push_back(CaptureCurrentView());
}

void SFMLFractal::ResetZoomHistory()
{
    _zoomHistory.clear();
    _outermostZoom = CaptureCurrentView();
}

void SFMLFractal::ExpandCurrentView()
{
    ApplyView(_fractal->GetPreciseExpandedView());
    _outermostZoom = CaptureCurrentView();
}

void SFMLFractal::Move()
{
    if (!_fractal->IsRendered())
        return;

    if (_movement[Left])
        _xVel += stdSpeed;
    if (_movement[Right])
        _xVel -= stdSpeed;
    if (_movement[Up])
        _yVel += stdSpeed;
    if (_movement[Down])
        _yVel -= stdSpeed;

    if (!_movement[Left] && !_movement[Right] && !_movement[Up] && !_movement[Down])
    {
        if (_xVel > 0) _xVel -= stdSpeed;
        if (_xVel < 0) _xVel += stdSpeed;
        if (_yVel > 0) _yVel -= stdSpeed;
        if (_yVel < 0) _yVel += stdSpeed;
    }

    if (_xVel != 0 || _yVel != 0)
    {
        _fractal->PanViewByPixels(_xVel, _yVel);
        _posX += _xVel;
        _posY += _yVel;
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

bool SFMLFractal::IsMoving() const
{
    return _xVel != 0 || _yVel != 0 || _movement[Up] || _movement[Down] || _movement[Left] || _movement[Right];
}

void SFMLFractal::SetMovement(const Direction direction)
{
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

void SFMLFractal::ReleaseMovement(const Direction direction)
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

void SFMLFractal::HandleEvent(const sf::Event& event)
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

void SFMLFractal::Resize(const sf::RenderWindow* window)
{
    _dontDrawTempImage = true;
    ResetMovement();
    _fractal->Resize(window->getSize().x, window->getSize().y);
    _outermostZoom = CaptureCurrentView();
    const sf::Vector2u screenSize = _fractal->GetScreenSize();

    for (PreciseRect& view : _zoomHistory)
    {
        view.top = view.bottom + (view.right - view.left) *
            HighPrecisionReal(screenSize.y) / HighPrecisionReal(screenSize.x);
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

void SFMLFractal::SetAreaOfView(const sf::Rect<int>& pixelCoordinates)
{
    ResetMovement();
    const bool wasPaused = _fractal->IsPausedForPresentation();

    if (wasPaused)
    {
        ClearImageCache();
        _dontDrawTempImage = true;
    }
    else
    {
        _imgCache.push_back(_image);
        _imgInVector = true;
        _dontDrawTempImage = false;
    }

    SaveZoom();

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

void SFMLFractal::ZoomBack()
{
    const bool stillRendering = _fractal->IsRendering();
    _fractal->StopRender();
    ResetMovement();

    if (!_zoomHistory.empty())
    {
        ApplyView(_zoomHistory.back());
        _zoomHistory.pop_back();
    }
    else
    {
        ExpandCurrentView();
    }

    _fractal->MarkRenderInterrupted();
    _fractal->MarkOrbitDirty();

    if (_imgInVector && !_fractal->IsGradientAnimating() && !_imgCache.empty() && !stillRendering)
    {
        _image = _imgCache.back();
        _texture.loadFromImage(_image);
        _imgCache.pop_back();
        _usingRenderImage = true;
        _fractal->MarkRenderComplete();
        _zoomingBack = false;
        _dontDrawTempImage = true;
    }
    else
    {
        if (stillRendering && !_imgCache.empty())
            _imgCache.pop_back();
        _fractal->MarkRenderDirty();
        _zoomingBack = true;
    }
}

Rect SFMLFractal::GetOutermostZoom() const
{
    return _outermostZoom.ToRect();
}

Rect SFMLFractal::GetCurrentZoom() const
{
    return CaptureCurrentView().ToRect();
}

bool SFMLFractal::HasZoomed() const
{
    const PreciseRect currentZoom = CaptureCurrentView();
    return _outermostZoom.left != currentZoom.left ||
        _outermostZoom.right != currentZoom.right ||
        _outermostZoom.bottom != currentZoom.bottom ||
        _outermostZoom.top != currentZoom.top;
}

void SFMLFractal::Redraw()
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

void SFMLFractal::SetView(const Rect& view)
{
    _fractal->StopRender();
    ResetMovement();
    ApplyView(PreciseRect(view));
    _fractal->MarkOrbitDirty();
    ClearImageCache();
    ResetZoomHistory();
    _dontDrawTempImage = true;
}

void SFMLFractal::IncreaseIterations()
{
    ClearImageCache();
    const unsigned change = _fractal->GetIterations() + 100;
    _fractal->SetIterations(change);
}

void SFMLFractal::DecreaseIterations()
{
    ClearImageCache();

    if (_fractal->GetIterations() > 100)
    {
        const unsigned int change = _fractal->GetIterations() - 100;
        _fractal->SetIterations(change);
    }
}

void SFMLFractal::ChangeIterations(const unsigned int iterations)
{
    ClearImageCache();
    _fractal->SetIterations(iterations);
}

void SFMLFractal::SetK(const double real, const double imaginary)
{
    ClearImageCache();
    _fractal->SetK(real, imaginary);
}

void SFMLFractal::SetGradient(const wxGradient& gradient)
{
    ClearImageCache();
    _fractal->SetGradient(gradient);
}

void SFMLFractal::SetGradientSize(const unsigned int size)
{
    ClearImageCache();
    _fractal->SetGradientSize(size);
}

void SFMLFractal::SetColorCycleLength(const double cycleLength)
{
    ClearImageCache();
    _fractal->SetColorCycleLength(cycleLength);
}

void SFMLFractal::SetColorPalette(const ColorPaletteTypes palette)
{
    ClearImageCache();
    _fractal->SetColorPalette(palette);
}

void SFMLFractal::SetExteriorColorMode(const bool mode)
{
    ClearImageCache();
    _fractal->SetExteriorColorMode(mode);
}

void SFMLFractal::SetFractalSetColorMode(const bool mode)
{
    ClearImageCache();
    _fractal->SetFractalSetColorMode(mode);
}

void SFMLFractal::SetFractalSetColor(const sf::Color color)
{
    ClearImageCache();
    _fractal->SetFractalSetColor(color);
}

void SFMLFractal::SetRelativeColor(const bool mode)
{
    ClearImageCache();
    _fractal->SetRelativeColor(mode);
}

void SFMLFractal::ChangeVarGradient()
{
    ClearImageCache();
    _fractal->ChangeVarGradient();
}

void SFMLFractal::SetVarGradient(const int offset)
{
    ClearImageCache();
    _fractal->SetVarGradient(offset);
}

void SFMLFractal::SetAlgorithm(const RenderingAlgorithmType algorithm)
{
    ClearImageCache();
    _fractal->SetAlgorithm(algorithm);
}

void SFMLFractal::SetOrbitTrapMode(const bool mode)
{
    ClearImageCache();
    _fractal->SetOrbitTrapMode(mode);
}

void SFMLFractal::SetSmoothRender(const bool mode)
{
    ClearImageCache();
    _fractal->SetSmoothRender(mode);
}

void SFMLFractal::DrawMaps(sf::RenderWindow* window)
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

void SFMLFractal::DrawGeometry(sf::RenderWindow* window) const
{
    for (const LineData& line : _fractal->GetLines())
    {
        const auto x1 = static_cast<float>(_fractal->GetPixelX(line.x1));
        const auto y1 = static_cast<float>(_fractal->GetPixelY(line.y1));
        const auto x2 = static_cast<float>(_fractal->GetPixelX(line.x2));
        const auto y2 = static_cast<float>(_fractal->GetPixelY(line.y2));
        sf::Vertex vertices[] = { sf::Vertex(sf::Vector2f(x1, y1), line.color), sf::Vertex(sf::Vector2f(x2, y2), line.color) };
        window->draw(vertices, 2, sf::Lines);
    }

    for (const LineData& orbitLine : _fractal->GetOrbitLines())
    {
        const auto x1 = static_cast<float>(_fractal->GetPixelX(orbitLine.x1));
        const auto y1 = static_cast<float>(_fractal->GetPixelY(orbitLine.y1));
        const auto x2 = static_cast<float>(_fractal->GetPixelX(orbitLine.x2));
        const auto y2 = static_cast<float>(_fractal->GetPixelY(orbitLine.y2));
        sf::Vertex vertices[] = { sf::Vertex(sf::Vector2f(x1, y1), orbitLine.color), sf::Vertex(sf::Vector2f(x2, y2), orbitLine.color) };
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

void SFMLFractal::Show(sf::RenderWindow* window)
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
        {
            DrawMaps(window);
        }

        if (_fractal->ConsumeImageRefreshRequest())
        {
            DrawMaps(window);
        }

        const bool gradientChanged = _fractal->ConsumeGradientChangeRequest();
        if (_fractal->IsGradientAnimating() || gradientChanged)
        {
            _fractal->AdvanceGradientOffset();

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
