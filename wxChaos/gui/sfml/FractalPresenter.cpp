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
    _mouseWheelZoomScale = 0.75;
    _zoomAnimationDurationSeconds = 0.18;
    _zoomAnimationActive = false;
    _zoomAnimationElapsed = 0.0;
    _zoomAnimationStartPosition = {0.0F, 0.0F};
    _zoomAnimationTargetPosition = {0.0F, 0.0F};
    _zoomAnimationStartScale = {1.0F, 1.0F};
    _zoomAnimationTargetScale = {1.0F, 1.0F};
    _interactiveZoomActive = false;
    _interactiveZoomAnchorX = 0;
    _interactiveZoomAnchorY = 0;
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

void FractalPresenter::SetZoomOptions(const int zoomStepPercent, const int inertiaMilliseconds)
{
    const int clampedStep = std::clamp(zoomStepPercent, 1, 95);
    const int clampedInertia = std::clamp(inertiaMilliseconds, 0, 1000);
    _mouseWheelZoomScale = 1.0 - static_cast<double>(clampedStep) / 100.0;
    _zoomAnimationDurationSeconds = static_cast<double>(clampedInertia) / 1000.0;
}

double FractalPresenter::GetMouseWheelZoomScale() const
{
    return _mouseWheelZoomScale;
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
    _tempSprite.setPosition(0, 0);
    _tempSprite.setScale(1.0F, 1.0F);

    _geomTexture.loadFromImage(_geomImage);
    _outGeom.setTexture(_geomTexture);
}

void FractalPresenter::ClearImageCache()
{
    const bool usingCachedRenderImage = _usingRenderImage;

    for (auto& [view, image, iterations, imageComplete] : _zoomHistory)
        image.reset();

    _usingRenderImage = false;
    _zoomingBack = false;
    StopZoomAnimation();

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

sf::Rect<int> FractalPresenter::GetPixelZoomRect(const int pixelX, const int pixelY, const double scale) const
{
    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    const auto screenWidth = static_cast<int>(screenSize.x);
    const auto screenHeight = static_cast<int>(screenSize.y);

    if (screenWidth <= 0 || screenHeight <= 0)
        return {0, 0, 1, 1};

    const int clampedX = std::clamp(pixelX, 0, screenWidth - 1);
    const int clampedY = std::clamp(pixelY, 0, screenHeight - 1);
    const double clampedScale = std::clamp(scale, 0.01, 100.0);
    const int zoomWidth = std::max(1, static_cast<int>(std::round(static_cast<double>(screenWidth) * clampedScale)));
    const int zoomHeight = std::max(1, static_cast<int>(std::round(static_cast<double>(screenHeight) * clampedScale)));
    const int left = static_cast<int>(std::round(static_cast<double>(clampedX) * (1.0 - clampedScale)));
    const int top = static_cast<int>(std::round(static_cast<double>(clampedY) * (1.0 - clampedScale)));

    return {left, top, zoomWidth, zoomHeight};
}

PreciseRect FractalPresenter::GetPixelZoomView(const int pixelX, const int pixelY, const double scale) const
{
    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    const auto screenWidth = static_cast<int>(screenSize.x);
    const auto screenHeight = static_cast<int>(screenSize.y);
    PreciseRect currentView = CaptureCurrentView();

    if (screenWidth <= 0 || screenHeight <= 0)
        return currentView;

    const int clampedX = std::clamp(pixelX, 0, screenWidth - 1);
    const int clampedY = std::clamp(pixelY, 0, screenHeight - 1);
    const HighPrecisionReal width = currentView.right - currentView.left;
    const HighPrecisionReal height = currentView.top - currentView.bottom;
    const HighPrecisionReal targetWidth = width * HighPrecisionReal(std::clamp(scale, 0.01, 100.0));
    const HighPrecisionReal targetHeight = height * HighPrecisionReal(std::clamp(scale, 0.01, 100.0));
    const HighPrecisionReal xDivisor = HighPrecisionReal(screenWidth > 1 ? screenWidth - 1 : 1);
    const HighPrecisionReal yDivisor = HighPrecisionReal(screenHeight > 1 ? screenHeight - 1 : 1);
    const HighPrecisionReal xRatio = HighPrecisionReal(clampedX) / xDivisor;
    const HighPrecisionReal yRatio = HighPrecisionReal(clampedY) / yDivisor;
    const HighPrecisionReal mouseRe = currentView.left + xRatio * width;
    const HighPrecisionReal mouseIm = currentView.top - yRatio * height;

    PreciseRect targetView;
    targetView.left = mouseRe - xRatio * targetWidth;
    targetView.right = targetView.left + targetWidth;
    targetView.top = mouseIm + yRatio * targetHeight;
    targetView.bottom = targetView.top - targetHeight;
    return targetView;
}

sf::Rect<int> FractalPresenter::GetViewRectInsideView(const PreciseRect& innerView, const PreciseRect& outerView) const
{
    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    const auto screenWidth = static_cast<int>(screenSize.x);
    const auto screenHeight = static_cast<int>(screenSize.y);

    if (screenWidth <= 0 || screenHeight <= 0)
        return {0, 0, 1, 1};

    const HighPrecisionReal outerWidth = outerView.right - outerView.left;
    const HighPrecisionReal outerHeight = outerView.top - outerView.bottom;
    if (outerWidth == 0 || outerHeight == 0)
        return {0, 0, screenWidth, screenHeight};

    const auto left = static_cast<int>(std::round(ToDouble((innerView.left - outerView.left) / outerWidth) * screenWidth));
    const auto top = static_cast<int>(std::round(ToDouble((outerView.top - innerView.top) / outerHeight) * screenHeight));
    const auto width = static_cast<int>(std::round(ToDouble((innerView.right - innerView.left) / outerWidth) * screenWidth));
    const auto height = static_cast<int>(std::round(ToDouble((innerView.top - innerView.bottom) / outerHeight) * screenHeight));

    const int clampedLeft = std::clamp(left, 0, screenWidth - 1);
    const int clampedTop = std::clamp(top, 0, screenHeight - 1);
    const int clampedWidth = std::clamp(width, 1, screenWidth - clampedLeft);
    const int clampedHeight = std::clamp(height, 1, screenHeight - clampedTop);
    return {clampedLeft, clampedTop, clampedWidth, clampedHeight};
}

void FractalPresenter::ApplyZoomPreviewTransform(const int pixelX, const int pixelY, const double scale)
{
    const sf::Rect<int> pixelCoordinates = GetPixelZoomRect(pixelX, pixelY, scale);
    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    const float scaleX = static_cast<float>(screenSize.x) / static_cast<float>(pixelCoordinates.width);
    const float scaleY = static_cast<float>(screenSize.y) / static_cast<float>(pixelCoordinates.height);

    _tempSprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y)));
    _tempSprite.setPosition(
        -static_cast<float>(pixelCoordinates.left) * scaleX,
        -static_cast<float>(pixelCoordinates.top) * scaleY);
    _tempSprite.setScale(scaleX, scaleY);
}

sf::Image FractalPresenter::CapturePreviewImage() const
{
    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    sf::Image preview;
    preview.create(screenSize.x, screenSize.y, sf::Color::White);

    sf::RenderTexture renderTexture;
    if (renderTexture.create(screenSize.x, screenSize.y))
    {
        renderTexture.clear(sf::Color::White);
        const bool hasTemporaryPreview = !_dontDrawTempImage &&
            _tempImage.getSize().x == screenSize.x &&
            _tempImage.getSize().y == screenSize.y;

        if (hasTemporaryPreview)
            renderTexture.draw(_tempSprite);

        // ReSharper disable once CppDFAConstantConditions
        if (!_zoomAnimationActive && _image.getSize().x == screenSize.x && _image.getSize().y == screenSize.y)
            renderTexture.draw(_output);

        renderTexture.display();
        preview = renderTexture.getTexture().copyToImage();
    }
    else if (!_dontDrawTempImage && _tempImage.getSize().x == screenSize.x && _tempImage.getSize().y == screenSize.y)
        preview = _tempImage;
    else if (_image.getSize().x == screenSize.x && _image.getSize().y == screenSize.y)
        preview = _image;

    _fractal->PrepareDisplayColorLookup();
    for (unsigned int x = 0; x < screenSize.x; x++)
    {
        for (unsigned int y = 0; y < screenSize.y; y++)
        {
            if (_fractal->HasDisplayPixelColor(x, y))
                preview.setPixel(x, y, _fractal->GetRenderedPixelColor(x, y));
        }
    }

    return preview;
}

void FractalPresenter::StartZoomAnimation(const sf::Rect<int>& pixelCoordinates)
{
    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    const float scaleX = static_cast<float>(screenSize.x) / static_cast<float>(pixelCoordinates.width);
    const float scaleY = static_cast<float>(screenSize.y) / static_cast<float>(pixelCoordinates.height);

    _zoomAnimationActive = true;
    _zoomAnimationElapsed = 0.0;
    _zoomAnimationStartPosition = {
        0.0F,
        0.0F
    };
    _zoomAnimationTargetPosition = {
        -static_cast<float>(pixelCoordinates.left) * scaleX,
        -static_cast<float>(pixelCoordinates.top) * scaleY
    };
    _zoomAnimationStartScale = {1.0F, 1.0F};
    _zoomAnimationTargetScale = {scaleX, scaleY};

    _tempSprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y)));
    _tempSprite.setPosition(_zoomAnimationStartPosition);
    _tempSprite.setScale(_zoomAnimationStartScale);
}

void FractalPresenter::StartZoomBackAnimation(const sf::Rect<int>& targetCoordinates)
{
    const sf::Vector2u screenSize = _fractal->GetScreenSize();
    const float scaleX = static_cast<float>(targetCoordinates.width) / static_cast<float>(screenSize.x);
    const float scaleY = static_cast<float>(targetCoordinates.height) / static_cast<float>(screenSize.y);

    _zoomAnimationActive = true;
    _zoomAnimationElapsed = 0.0;
    _zoomAnimationStartPosition = {
        0.0F,
        0.0F
    };
    _zoomAnimationTargetPosition = {
        static_cast<float>(targetCoordinates.left),
        static_cast<float>(targetCoordinates.top)
    };
    _zoomAnimationStartScale = {1.0F, 1.0F};
    _zoomAnimationTargetScale = {scaleX, scaleY};

    _tempSprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y)));
    _tempSprite.setPosition(_zoomAnimationStartPosition);
    _tempSprite.setScale(_zoomAnimationStartScale);
}

bool FractalPresenter::UpdateZoomAnimation(const double elapsedSeconds)
{
    if (!_zoomAnimationActive)
        return false;

    _zoomAnimationElapsed += std::clamp(elapsedSeconds, 0.0, MaximumMovementStepSeconds);
    const double progress = _zoomAnimationDurationSeconds <= 0.0
        ? 1.0
        : std::clamp(_zoomAnimationElapsed / _zoomAnimationDurationSeconds, 0.0, 1.0);
    const auto easedProgress = static_cast<float>(1.0 - std::pow(1.0 - progress, 3.0));

    const sf::Vector2f position = {
        _zoomAnimationStartPosition.x + (_zoomAnimationTargetPosition.x - _zoomAnimationStartPosition.x) * easedProgress,
        _zoomAnimationStartPosition.y + (_zoomAnimationTargetPosition.y - _zoomAnimationStartPosition.y) * easedProgress
    };
    const sf::Vector2f scale = {
        _zoomAnimationStartScale.x + (_zoomAnimationTargetScale.x - _zoomAnimationStartScale.x) * easedProgress,
        _zoomAnimationStartScale.y + (_zoomAnimationTargetScale.y - _zoomAnimationStartScale.y) * easedProgress
    };

    _tempSprite.setPosition(position);
    _tempSprite.setScale(scale);

    if (progress < 1.0)
        return false;

    _tempSprite.setPosition(_zoomAnimationTargetPosition);
    _tempSprite.setScale(_zoomAnimationTargetScale);
    _zoomAnimationActive = false;
    return true;
}

void FractalPresenter::StopZoomAnimation()
{
    _zoomAnimationActive = false;
    _zoomAnimationElapsed = 0.0;
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
    constexpr unsigned int maximumAutomaticIterations = 20000000;
    const unsigned int base = std::min(_automaticIterationBase, maximumAutomaticIterations);
    return std::min(base + CalculateAutomaticIterationExtra(), maximumAutomaticIterations);
}

unsigned int FractalPresenter::CalculateAutomaticIterationExtra() const
{
    const PreciseRect view = CaptureCurrentView();
    const double width = ToDouble(RealAbs(view.right - view.left));
    if (!std::isfinite(width) || width <= 0.0)
        return 0;

    constexpr double referenceWidth = 3.5;
    constexpr double iterationsPerZoomDoubling = 18.0;

    const double zoomDepth = std::max(0.0, std::log2(referenceWidth / width));
    const double wantedExtraIterations = std::ceil(zoomDepth * iterationsPerZoomDoubling);
    if (!std::isfinite(wantedExtraIterations))
        return 20000000;

    return static_cast<unsigned int>(std::clamp(wantedExtraIterations, 0.0, 20000000.0));
}

void FractalPresenter::SetAutomaticIterationBaseForCurrentIterations(const unsigned int iterations)
{
    const unsigned int automaticExtra = CalculateAutomaticIterationExtra();
    _automaticIterationBase = iterations > automaticExtra ? iterations - automaticExtra : 0;
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

void FractalPresenter::SaveZoom(std::optional<sf::Image> image, const bool imageComplete)
{
    _zoomHistory.push_back({CaptureCurrentView(), std::move(image), _fractal->GetIterations(), imageComplete});
}

void FractalPresenter::SetTemporaryPreviewImage(const sf::Image& image, const bool drawPreview)
{
    const sf::Vector2u screenSize = _fractal->GetScreenSize();

    _tempImage = image;
    _tempTexture.loadFromImage(_tempImage);
    _tempSprite.setTexture(_tempTexture);
    _tempSprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y)));
    _tempSprite.setOrigin(0, 0);
    _tempSprite.setPosition(0, 0);
    _tempSprite.setScale(1.0F, 1.0F);
    _dontDrawTempImage = !drawPreview;
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

bool FractalPresenter::IsZoomPreviewActive() const
{
    return _zoomAnimationActive || _interactiveZoomActive;
}

void FractalPresenter::SetMovement(const Direction direction)
{
    if (_zoomAnimationActive)
        return;

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
    if (_zoomAnimationActive)
        return;

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
    const sf::Vector2u windowSize = window->getSize();
    if (windowSize.x == 0 || windowSize.y == 0)
        return;

    _dontDrawTempImage = true;
    ResetMovement();
    _fractal->Resize(windowSize.x, windowSize.y);
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
    SetAreaOfView(pixelCoordinates, _fractal->GetPreciseViewForPixelRect(pixelCoordinates));
}

void FractalPresenter::SetAreaOfView(const sf::Rect<int>& pixelCoordinates, const PreciseRect& targetView)
{
    if (_zoomAnimationActive)
        return;

    ResetMovement();
    const bool wasPaused = _fractal->IsPausedForPresentation();
    const bool wasRendering = _fractal->StopRender();
    std::optional<sf::Image> zoomHistoryImage;
    bool zoomHistoryImageComplete = false;
    std::optional<sf::Image> previewImage;

    if (wasPaused)
    {
        ClearImageCache();
        _dontDrawTempImage = true;
    }
    else
    {
        if (_fractal->IsRendered())
        {
            zoomHistoryImage = _image;
            zoomHistoryImageComplete = true;
            previewImage = _image;
        }
        else
        {
            _dontDrawTempImage = false;
            previewImage = CapturePreviewImage();
            zoomHistoryImage = *previewImage;
        }
        _dontDrawTempImage = false;
    }

    if (!previewImage.has_value())
        previewImage = CapturePreviewImage();

    SaveZoom(std::move(zoomHistoryImage), zoomHistoryImageComplete);

    ApplyView(targetView);
    if (wasPaused || wasRendering)
        _fractal->MarkRenderInterrupted();

    _fractal->MarkOrbitDirty();
    SetTemporaryPreviewImage(*previewImage, true);
    StartZoomAnimation(pixelCoordinates);
    _usingRenderImage = false;
    _zoomingBack = false;
}

void FractalPresenter::ZoomAtPixel(const int pixelX, const int pixelY)
{
    ZoomAtPixel(pixelX, pixelY, _mouseWheelZoomScale);
}

void FractalPresenter::ZoomAtPixel(const int pixelX, const int pixelY, const double scale)
{
    if (std::abs(scale - 1.0) < 0.001)
        return;

    SetAreaOfView(GetPixelZoomRect(pixelX, pixelY, scale), GetPixelZoomView(pixelX, pixelY, scale));
}

bool FractalPresenter::BeginInteractiveZoomAtPixel(const int pixelX, const int pixelY)
{
    if (_zoomAnimationActive || _interactiveZoomActive)
        return false;

    ResetMovement();
    SetTemporaryPreviewImage(_fractal->IsRendered() ? _image : CapturePreviewImage(), true);
    _interactiveZoomActive = true;
    _interactiveZoomAnchorX = pixelX;
    _interactiveZoomAnchorY = pixelY;
    ApplyZoomPreviewTransform(_interactiveZoomAnchorX, _interactiveZoomAnchorY, 1.0);
    return true;
}

void FractalPresenter::UpdateInteractiveZoom(const double scale)
{
    if (!_interactiveZoomActive)
        return;

    ApplyZoomPreviewTransform(_interactiveZoomAnchorX, _interactiveZoomAnchorY, scale);
}

void FractalPresenter::CommitInteractiveZoom(const double scale)
{
    if (!_interactiveZoomActive)
        return;

    if (std::abs(scale - 1.0) < 0.001)
    {
        CancelInteractiveZoom();
        return;
    }

    _interactiveZoomActive = false;
    ResetMovement();
    const bool wasPaused = _fractal->IsPausedForPresentation();
    const bool wasRendering = _fractal->StopRender();
    std::optional<sf::Image> zoomHistoryImage;
    bool zoomHistoryImageComplete = false;

    if (wasPaused)
    {
        ClearImageCache();
        _dontDrawTempImage = true;
    }
    else if (_fractal->IsRendered())
    {
        zoomHistoryImage = _image;
        zoomHistoryImageComplete = true;
    }
    else
        zoomHistoryImage = _tempImage;

    SaveZoom(std::move(zoomHistoryImage), zoomHistoryImageComplete);

    ApplyView(GetPixelZoomView(_interactiveZoomAnchorX, _interactiveZoomAnchorY, scale));
    if (wasPaused || wasRendering)
        _fractal->MarkRenderInterrupted();

    _fractal->MarkOrbitDirty();
    _dontDrawTempImage = false;
    _usingRenderImage = false;
    _zoomingBack = false;
    ApplyZoomPreviewTransform(_interactiveZoomAnchorX, _interactiveZoomAnchorY, scale);
}

void FractalPresenter::CancelInteractiveZoom()
{
    if (!_interactiveZoomActive)
        return;

    _interactiveZoomActive = false;
    SetTemporaryPreviewImage(_tempImage, false);
}

void FractalPresenter::ZoomBack()
{
    if (_zoomAnimationActive)
        return;

    StopZoomAnimation();
    ResetMovement();
    const bool wasRendering = _fractal->StopRender();
    const PreciseRect sourceView = CaptureCurrentView();
    const sf::Image sourceImage = _fractal->IsRendered() ? _image : CapturePreviewImage();
    std::optional<sf::Image> cachedImage;
    unsigned int cachedImageIterations = 0;
    bool cachedImageComplete = false;
    PreciseRect targetView;

    if (!_zoomHistory.empty())
    {
        ZoomHistoryEntry entry = std::move(_zoomHistory.back());
        _zoomHistory.pop_back();
        targetView = entry.view;
        ApplyView(targetView);
        cachedImage = std::move(entry.image);
        cachedImageIterations = entry.iterations;
        cachedImageComplete = entry.imageComplete;
    }
    else
    {
        targetView = _fractal->GetPreciseExpandedView();
        ApplyView(targetView);
        _outermostZoom = CaptureCurrentView();
    }

    if (wasRendering)
        _fractal->MarkRenderInterrupted();
    _fractal->MarkOrbitDirty();
    SetTemporaryPreviewImage(sourceImage, true);
    StartZoomBackAnimation(GetViewRectInsideView(sourceView, targetView));

    if (cachedImage.has_value())
    {
        _image = *cachedImage;
        _texture.loadFromImage(_image);
        _usingRenderImage = true;

        if (cachedImageComplete && cachedImageIterations == _fractal->GetIterations() && !_fractal->IsGradientAnimating())
        {
            _fractal->MarkRenderComplete();
            _dontDrawTempImage = true;
        }
        else
        {
            _fractal->MarkRenderDirty();
            _dontDrawTempImage = false;
        }

        _zoomingBack = false;
    }
    else
    {
        _fractal->MarkRenderDirty();
        _usingRenderImage = false;
        _zoomingBack = false;
        _dontDrawTempImage = false;
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
    ClearImageCache();
    const unsigned change = _fractal->GetIterations() + _fractal->GetIterationStep();
    if (_automaticIterations)
        SetAutomaticIterationBaseForCurrentIterations(change);
    _fractal->SetIterations(change);
}

void FractalPresenter::DecreaseIterations()
{
    ClearImageCache();

    const unsigned int iterationStep = _fractal->GetIterationStep();
    if (_fractal->GetIterations() > iterationStep)
    {
        const unsigned int change = _fractal->GetIterations() - iterationStep;
        if (_automaticIterations)
            SetAutomaticIterationBaseForCurrentIterations(change);
        _fractal->SetIterations(change);
    }
}

void FractalPresenter::ChangeIterations(const unsigned int iterations)
{
    if (_automaticIterations)
        SetAutomaticIterationBaseForCurrentIterations(iterations);
    else
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

void FractalPresenter::SetColorVariationOffset(const unsigned int offset)
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

    if (_zoomingBack || _dontDrawTempImage || !_fractal->IsExteriorColorEnabled() || _zoomAnimationActive)
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
    const bool zoomAnimationFinished = UpdateZoomAnimation(elapsedSeconds);

    if (zoomAnimationFinished && _usingRenderImage && !_fractal->IsRendered())
    {
        SetTemporaryPreviewImage(_image, true);
        _usingRenderImage = false;
    }

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
            if (_usingRenderImage && !_zoomAnimationActive)
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

            if (!_zoomAnimationActive)
                DrawMaps(window);

            if (!_fractal->IsRendering())
            {
                _fractal->MarkRenderComplete();
                _dontDrawTempImage = false;
                _zoomingBack = false;
                _fractal->PostRender();
                if (!_zoomAnimationActive)
                    DrawMaps(window);
                _fractal->ConsumeImageRefreshRequest();
            }
        }

        if (_fractal->ConsumePausePresentationRefresh() && !_zoomAnimationActive)
            DrawMaps(window);

        if (_fractal->ConsumeImageRefreshRequest() && !_zoomAnimationActive)
            DrawMaps(window);

        if (zoomAnimationFinished && _fractal->IsRendered() && !_usingRenderImage)
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

    if (_interactiveZoomActive)
    {
        window->draw(_tempSprite);
        return;
    }

    if (_zoomAnimationActive)
    {
        if (_usingRenderImage)
            window->draw(_output);
        window->draw(_tempSprite);
        return;
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
