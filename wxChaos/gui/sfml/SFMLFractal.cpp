#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include "AppPaths.h"
#include "Fractal.h"
#include "SFMLFractal.h"

constexpr int stdSpeed = 1;

/**
* @brief Moves matrix elements and fills with a default value.
*
* When the fractal image is moved, it needs to move the elements in the maps so the program doesn't have to redraw the whole screen.
* @param matrix Matrix to move.
* @param matrixWidth The total number of elements in the X axis.
* @param matrixHeight The total number of elements in the Y axis.
* @param moveX Elements to move in the X axis.
* @param moveY Elements to move in the Y axis.
* @param fillValue The value to fill the matrix with when moving elements.
*/
template<class M> void SFMLFractal::MoveMatrix(M** matrix, const unsigned int matrixWidth, const unsigned int matrixHeight,
                                               const int moveX, const int moveY, const M fillValue)
{
    if (matrix == nullptr || matrixWidth == 0 || matrixHeight == 0)
        return;

    if (std::abs(moveX) >= static_cast<int>(matrixWidth) || std::abs(moveY) >= static_cast<int>(matrixHeight))
    {
        for (unsigned int i = 0; i < matrixHeight; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);

        return;
    }

    if (moveX > 0)
    {
        const auto displacement = static_cast<unsigned int>(moveX);
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            std::move_backward(matrix[i], matrix[i] + matrixWidth - displacement, matrix[i] + matrixWidth);
            std::fill(matrix[i], matrix[i] + displacement, fillValue);
        }
    }
    else if (moveX < 0)
    {
        const auto displacement = static_cast<unsigned int>(-moveX);
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            std::move(matrix[i] + displacement, matrix[i] + matrixWidth, matrix[i]);
            std::fill(matrix[i] + matrixWidth - displacement, matrix[i] + matrixWidth, fillValue);
        }
    }

    if (moveY > 0)
    {
        const auto displacement = static_cast<unsigned int>(moveY);
        std::rotate(matrix, matrix + matrixHeight - displacement, matrix + matrixHeight);
        for (unsigned int i = 0; i < displacement; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);
    }
    else if (moveY < 0)
    {
        const auto displacement = static_cast<unsigned int>(-moveY);
        std::rotate(matrix, matrix + displacement, matrix + matrixHeight);
        for (unsigned int i = matrixHeight - displacement; i < matrixHeight; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);
    }
}

SFMLFractal::SFMLFractal(Fractal* fractal) : _committedPanOffset(Vector2Int::Zero())
{
    _changeFractalIter = true;
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
    EnsureFontLoaded();
    ResetDisplayImages();
}

void SFMLFractal::SetFractal(Fractal* fractal)
{
    _fractal = fractal;
    _changeFractalIter = true;
    _dontDrawTempImage = true;
    ResetMovement();
    ResetZoomHistory();
    EnsureFontLoaded();
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

void SFMLFractal::EnsureFontLoaded()
{
    if (!_font.getInfo().family.empty())
        return;

    _font.loadFromFile(AppPaths::ResourceFileStd({wxT("PublicSans-Regular.otf")}));
    _iterationsText.setFont(_font);
}

void SFMLFractal::ResetDisplayImages()
{
    const sf::Color white(255, 255, 255);
    const sf::Color transparent(255, 255, 255, 0);
    _image.create(_fractal->_screenWidth, _fractal->_screenHeight, white);
    _tempImage = _image;
    _geomImage.create(_fractal->_screenWidth, _fractal->_screenHeight, transparent);

    _texture.loadFromImage(_image);
    _output.setTexture(_texture);

    _tempTexture.loadFromImage(_tempImage);
    _tempSprite.setTexture(_tempTexture);

    _geomTexture.loadFromImage(_geomImage);
    _outGeom.setTexture(_geomTexture);
}

void SFMLFractal::ClearImageCache()
{
    _imgCache.clear();
    _imgInVector = false;
    _usingRenderImage = false;
    _zoomingBack = false;
}

void SFMLFractal::UpdateIterationsOverlay()
{
    constexpr float horizontalPadding = 8.0F;
    constexpr float verticalPadding = 4.0F;

    _iterationsText.setCharacterSize(25);
    _iterationsText.setString("Iterations: " + std::to_string(_fractal->_maxIter));

    const sf::FloatRect textBounds = _iterationsText.getLocalBounds();
    const auto overlayWidth = static_cast<unsigned int>(std::ceil(textBounds.width + horizontalPadding * 2.0F));
    const auto overlayHeight = static_cast<unsigned int>(std::ceil(textBounds.height + verticalPadding * 2.0F));

    _iterationsOverlay.setFillColor(sf::Color(0, 0, 0, 100));
    _iterationsOverlay.setSize(sf::Vector2f(static_cast<float>(overlayWidth), static_cast<float>(overlayHeight)));
    _iterationsOverlay.setPosition(0.0F, 0.0F);

    _iterationsText.setPosition(horizontalPadding - textBounds.left, verticalPadding - textBounds.top);
    _changeFractalIter = false;
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

void SFMLFractal::MoveMaps()
{
    MoveMatrix<bool>(_fractal->_setMap, _fractal->_screenHeight, _fractal->_screenWidth,
        _committedPanOffset.y, _committedPanOffset.x);
    MoveMatrix<unsigned int>(_fractal->_colorMap, _fractal->_screenHeight, _fractal->_screenWidth,
        _committedPanOffset.y, _committedPanOffset.x, Fractal::InvalidColor);
    MoveMatrix<unsigned int>(_fractal->_auxMap, _fractal->_screenHeight, _fractal->_screenWidth,
        _committedPanOffset.y, _committedPanOffset.x);
}

Rect SFMLFractal::CaptureCurrentView() const
{
    return {_fractal->_minX, _fractal->_minY, _fractal->_maxX, _fractal->_maxY};
}

void SFMLFractal::ApplyView(const Rect& view) const
{
    _fractal->SetView(view);
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
    Rect view = CaptureCurrentView();
    const double scaleX = std::abs(view._right - view._left);
    const double scaleY = std::abs(view._top - view._bottom);

    view._left -= scaleX;
    view._right += scaleX;
    view._bottom -= scaleY;
    view._top = view._bottom + (view._right - view._left) *
        static_cast<double>(_fractal->_screenHeight) / _fractal->_screenWidth;

    ApplyView(view);
    _outermostZoom = CaptureCurrentView();
}

void SFMLFractal::Move()
{
    if (!_fractal->_rendered)
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
        const double fx = (_fractal->_maxX - _fractal->_minX) / _fractal->_screenWidth;
        const double fy = (_fractal->_maxY - _fractal->_minY) / _fractal->_screenHeight;

        _fractal->_minX -= _xVel * fx;
        _fractal->_maxX -= _xVel * fx;
        _fractal->_minY += _yVel * fy;
        _fractal->_maxY += _yVel * fy;

        _posX += _xVel;
        _posY += _yVel;
    }
    else if (_posX != 0 || _posY != 0)
    {
        if (_fractal->_paused && !_fractal->_pausing)
        {
            _fractal->_rendering = false;
            _fractal->_rendered = false;
            _fractal->_paused = false;
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

    for (Rect& view : _zoomHistory)
    {
        view._top = view._bottom + (view._right - view._left) *
            static_cast<double>(_fractal->_screenHeight) / _fractal->_screenWidth;
    }

    _fractal->_rendered = false;
    _fractal->_rendering = false;
    _fractal->_orbitDrawn = false;
    ClearImageCache();
    ResetDisplayImages();

    _tempSprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(_fractal->_screenWidth), static_cast<int>(_fractal->_screenHeight)));
    sf::IntRect size;
    size.width = static_cast<int>(_fractal->_screenWidth);
    size.height = static_cast<int>(_fractal->_screenHeight);
    _output.setTextureRect(size);
    _outGeom.setTextureRect(size);
}

void SFMLFractal::SetAreaOfView(const sf::Rect<int>& pixelCoordinates)
{
    ResetMovement();

    if (_fractal->_paused)
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

    if (_fractal->_paused)
        _fractal->_paused = false;

    SaveZoom();

    const double xFactor = (_fractal->_maxX - _fractal->_minX) / _fractal->_screenWidth;
    const double yFactor = (_fractal->_maxY - _fractal->_minY) / _fractal->_screenHeight;

    Rect view;
    view._right = _fractal->_minX + (pixelCoordinates.left + pixelCoordinates.width) * xFactor;
    view._left = _fractal->_minX + pixelCoordinates.left * xFactor;
    view._bottom = _fractal->_maxY - (pixelCoordinates.top + pixelCoordinates.height) * yFactor;
    view._top = view._bottom + (view._right - view._left) *
        static_cast<double>(_fractal->_screenHeight) / _fractal->_screenWidth;

    ApplyView(view);
    _fractal->_orbitDrawn = false;
    _tempImage = _image;
    _tempTexture.loadFromImage(_tempImage);
    _tempSprite.setTexture(_tempTexture);
    _tempSprite.setTextureRect(pixelCoordinates);
    _tempSprite.setPosition(0, 0);

    const float scaleX = static_cast<float>(_fractal->_screenWidth) / static_cast<float>(pixelCoordinates.width);
    const float scaleY = static_cast<float>(_fractal->_screenHeight) / static_cast<float>(pixelCoordinates.height);
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

    _fractal->_rendered = false;
    _fractal->_rendering = false;
    _fractal->_paused = false;
    _fractal->_orbitDrawn = false;

    if (_imgInVector && !_fractal->_varGradient && !_imgCache.empty() && !stillRendering)
    {
        _image = _imgCache.back();
        _texture.loadFromImage(_image);
        _imgCache.pop_back();
        _usingRenderImage = true;
        _fractal->_rendering = false;
        _fractal->SetRendered(true);
        _zoomingBack = false;
        _dontDrawTempImage = true;
    }
    else
    {
        if (stillRendering && !_imgCache.empty())
            _imgCache.pop_back();
        _fractal->SetRendered(false);
        _zoomingBack = true;
    }
}

Rect SFMLFractal::GetOutermostZoom() const
{
    return _outermostZoom;
}

Rect SFMLFractal::GetCurrentZoom() const
{
    return CaptureCurrentView();
}

bool SFMLFractal::HasZoomed() const
{
    const Rect currentZoom = GetCurrentZoom();
    const Rect outermostZoom = GetOutermostZoom();
    return outermostZoom._left != currentZoom._left ||
        outermostZoom._right != currentZoom._right ||
        outermostZoom._bottom != currentZoom._bottom ||
        outermostZoom._top != currentZoom._top;
}

void SFMLFractal::Redraw()
{
    ResetMovement();

    if (_fractal->_colorMode)
    {
        _tempImage = _image;
        _tempTexture.loadFromImage(_tempImage);
        _tempSprite.setOrigin(0, 0);

        const float xFactor = static_cast<float>(_fractal->_screenWidth) / static_cast<float>(_tempImage.getSize().x);
        const float yFactor = static_cast<float>(_fractal->_screenHeight) / static_cast<float>(_tempImage.getSize().y);
        _tempSprite.setScale(xFactor, yFactor);
    }

    _fractal->Redraw();
    ClearImageCache();
    _dontDrawTempImage = true;
}

void SFMLFractal::IncreaseIterations()
{
    _changeFractalIter = true;

    ClearImageCache();
    const unsigned change = _fractal->GetIterations() + 100;
    _fractal->SetIterations(change);
}

void SFMLFractal::DecreaseIterations()
{
    _changeFractalIter = true;

    ClearImageCache();

    if (_fractal->GetIterations() > 100)
    {
        const unsigned int change = _fractal->GetIterations() - 100;
        _fractal->SetIterations(change);
    }
}

void SFMLFractal::ChangeIterations(const int iterations)
{
    _changeFractalIter = true;

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

void SFMLFractal::SetColorPalette(const ColorPalettes palette)
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

    if (_zoomingBack || _dontDrawTempImage || !_fractal->_colorMode)
        _image.create(_fractal->_screenWidth, _fractal->_screenHeight, sf::Color(255, 255, 255));
    else
    {
        _image.create(_fractal->_screenWidth, _fractal->_screenHeight, sf::Color(255, 255, 255, 0));
        window->draw(_tempSprite);
    }

    _output.setPosition(0, 0);
    if (_fractal->_relativeColor)
    {
        _fractal->UpdateMaxColorMapValue();

        for (int i = 0; i < _fractal->_screenWidth; i++)
        {
            for (int j = 0; j < _fractal->_screenHeight; j++)
            {
                if (_fractal->_setMap[i][j] == true && _fractal->_colorSet)
                    _image.setPixel(i, j, _fractal->GetSetColor());
                else if (_fractal->_colorMode)
                {
                    if (_fractal->_colorMap[i][j] != Fractal::InvalidColor)
                    {
                        double calculatedColor = (static_cast<double>(_fractal->_colorMap[i][j]) / static_cast<double>(_fractal->_maxColorMapVal)) * _fractal->_paletteSize + _fractal->_changeGradient;
                        const sf::Color color = _fractal->GetColorFromPalette(static_cast<int>(calculatedColor));
                        _image.setPixel(i, j, color);
                    }
                    else if (_zoomingBack || _dontDrawTempImage)
                    {
                        _image.setPixel(i, j, _fractal->GetColorFromPalette(_fractal->_changeGradient));
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < _fractal->_screenWidth; i++)
        {
            for (int j = 0; j < _fractal->_screenHeight; j++)
            {
                if (_fractal->_setMap[i][j] == true && _fractal->_colorSet)
                    _image.setPixel(i, j, _fractal->GetSetColor());
                else if (_fractal->_colorMode)
                {
                    if (_fractal->_colorMap[i][j] != Fractal::InvalidColor)
                        _image.setPixel(i, j, _fractal->GetColorFromPalette(_fractal->_colorMap[i][j] + _fractal->_changeGradient));
                    else if (_zoomingBack || _dontDrawTempImage)
                        _image.setPixel(i, j, _fractal->GetColorFromPalette(_fractal->_changeGradient));
                }
            }
        }
    }

    _texture.loadFromImage(_image);
    window->draw(_output);
}

void SFMLFractal::DrawGeometry(sf::RenderWindow* window) const
{
    for (auto & _line : _fractal->_lines)
    {
        const auto x1 = static_cast<float>((_line.x1 - _fractal->_minX) / _fractal->_xFactor);
        const auto y1 = static_cast<float>((_fractal->_maxY - _line.y1) / _fractal->_yFactor);
        const auto x2 = static_cast<float>((_line.x2 - _fractal->_minX) / _fractal->_xFactor);
        const auto y2 = static_cast<float>((_fractal->_maxY - _line.y2) / _fractal->_yFactor);
        sf::Vertex line[] = { sf::Vertex(sf::Vector2f(x1, y1), _line.color), sf::Vertex(sf::Vector2f(x2, y2), _line.color) };
        window->draw(line, 2, sf::Lines);
    }

    for (auto & _orbitLine : _fractal->_orbitLines)
    {
        const auto x1 = static_cast<float>((_orbitLine.x1 - _fractal->_minX) / _fractal->_xFactor);
        const auto y1 = static_cast<float>((_fractal->_maxY - _orbitLine.y1) / _fractal->_yFactor);
        const auto x2 = static_cast<float>((_orbitLine.x2 - _fractal->_minX) / _fractal->_xFactor);
        const auto y2 = static_cast<float>((_fractal->_maxY - _orbitLine.y2) / _fractal->_yFactor);
        sf::Vertex line[] = { sf::Vertex(sf::Vector2f(x1, y1), _orbitLine.color), sf::Vertex(sf::Vector2f(x2, y2), _orbitLine.color) };
        window->draw(line, 2, sf::Lines);
    }

    for (auto & _circle : _fractal->_circles)
    {
        const auto x0 = static_cast<float>((_circle.xCenter - _fractal->_minX) / _fractal->_xFactor);
        const auto y0 = static_cast<float>((_fractal->_maxY - _circle.yCenter) / _fractal->_yFactor);
        const auto right = static_cast<float>((_circle.xCenter + _circle.radius - _fractal->_minX) / _fractal->_xFactor);
        const float r = right - x0;
        sf::CircleShape circle(r);
        circle.setPosition(x0 - r, y0 - r);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(_circle.color);
        circle.setOutlineThickness(2);
        window->draw(circle);
    }
}

void SFMLFractal::Show(sf::RenderWindow* window)
{
    if (_fractal->_rendered && IsMoving())
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
            MoveMaps();
            _hasCommittedPanOffset = false;
            _fractal->_rendered = false;
        }

        if (!_fractal->_rendered)
        {
            if (_usingRenderImage)
            {
                _usingRenderImage = false;
                renderOffset = {0, 0};
                _committedPanOffset = {0, 0};
                _hasCommittedPanOffset = false;
            }

            if (!_fractal->_rendering)
            {
                _fractal->_rendering = true;
                _fractal->PrepareRender(renderOffset);
                _fractal->Render();
            }

            _committedPanOffset = {0, 0};

            DrawMaps(window);

            if (!_fractal->IsRendering())
            {
                _fractal->_rendered = true;
                _fractal->_rendering = false;
                _dontDrawTempImage = false;
                _zoomingBack = false;
                _fractal->PostRender();
                DrawMaps(window);
                _fractal->_refreshImage = false;
            }
        }

        if (_fractal->_pausing)
        {
            DrawMaps(window);
            _fractal->_pausing = false;
            _fractal->_refreshImage = false;
        }

        if (_fractal->_refreshImage)
        {
            DrawMaps(window);
            _fractal->_refreshImage = false;
        }

        if (_fractal->_varGradient || _fractal->_varGradChange)
        {
            if (_fractal->_changeGradient < _fractal->_paletteSize)
                _fractal->_changeGradient += _fractal->_varGradientStep;
            else
                _fractal->_changeGradient = 0;

            if (_fractal->_rendered)
            {
                _fractal->UpdateMaxColorMapValue();
                const double coef = static_cast<double>(_fractal->_paletteSize) / static_cast<double>(_fractal->_maxColorMapVal);
                for (int i = 0; i < _fractal->_screenWidth; i++)
                {
                    for (int j = 0; j < _fractal->_screenHeight; j++)
                    {
                        if ((_fractal->_setMap[i][j] == false || !_fractal->_colorSet) &&
                            _fractal->_colorMap[i][j] != Fractal::InvalidColor)
                        {
                            sf::Color color;
                            if (_fractal->_relativeColor)
                                color = _fractal->GetColorFromPalette(static_cast<int>(_fractal->_colorMap[i][j] * coef + _fractal->_changeGradient));
                            else
                                color = _fractal->GetColorFromPalette(_fractal->_colorMap[i][j] + _fractal->_changeGradient);

                            _image.setPixel(i, j, color);
                        }
                    }
                }
                _texture.loadFromImage(_image);
            }
            _fractal->_varGradChange = false;
        }
    }

    if (!_dontDrawTempImage && _fractal->_colorMode)
        window->draw(_tempSprite);

    window->draw(_output);

    if (_fractal->_orbitMode && !_fractal->IsRendering())
    {
        if (!_fractal->_orbitDrawn)
        {
            _fractal->_orbitLines.clear();
            _geomImage.create(_fractal->_screenWidth, _fractal->_screenHeight, sf::Color(255, 255, 255, 0));
            _fractal->DrawOrbit();
            _geomTexture.loadFromImage(_geomImage);
        }
        window->draw(_outGeom);
    }
    if (_fractal->_geomFigure && !_fractal->IsRendering())
        DrawGeometry(window);

    if (!_fractal->_onSnapshot && !_fractal->_rendering)
    {
        if (_changeFractalIter)
            UpdateIterationsOverlay();

        window->draw(_iterationsOverlay);
        window->draw(_iterationsText);
    }
}
