#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include "Fractal.h"
#include "SFMLFractal.h"
#include "Filesystem.h"

/**
* @brief Moves matrix elements and fills with zeros.
*
* When the fractal image is moved, it needs to move the elements in the maps so the program doesn't have to redraw the whole screen.
* @param matrix Matrix to move.
* @param matrixWidth The total number of elements in the X axis.
* @param matrixHeight The total number of elements in the Y axis.
* @param moveX Elements to move in the X axis.
* @param moveY Elements to move in the Y axis.
*/
template<class M> void MoveMatrix(M** matrix, const unsigned int matrixWidth, const unsigned int matrixHeight,
                                  const int moveX, const int moveY)
{
    if (matrix == nullptr || matrixWidth == 0 || matrixHeight == 0)
        return;

    if (std::abs(moveX) >= static_cast<int>(matrixWidth) || std::abs(moveY) >= static_cast<int>(matrixHeight))
    {
        for (unsigned int i = 0; i < matrixHeight; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, M{});

        return;
    }

    if (moveX > 0)
    {
        const auto displacement = static_cast<unsigned int>(moveX);
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            std::move_backward(matrix[i], matrix[i] + matrixWidth - displacement, matrix[i] + matrixWidth);
            std::fill(matrix[i], matrix[i] + displacement, M{});
        }
    }
    else if (moveX < 0)
    {
        const auto displacement = static_cast<unsigned int>(-moveX);
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            std::move(matrix[i] + displacement, matrix[i] + matrixWidth, matrix[i]);
            std::fill(matrix[i] + matrixWidth - displacement, matrix[i] + matrixWidth, M{});
        }
    }

    if (moveY > 0)
    {
        const auto displacement = static_cast<unsigned int>(moveY);
        std::rotate(matrix, matrix + matrixHeight - displacement, matrix + matrixHeight);
        for (unsigned int i = 0; i < displacement; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, M{});
    }
    else if (moveY < 0)
    {
        const auto displacement = static_cast<unsigned int>(-moveY);
        std::rotate(matrix, matrix + displacement, matrix + matrixHeight);
        for (unsigned int i = matrixHeight - displacement; i < matrixHeight; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, M{});
    }
}

SFMLFractal::SFMLFractal()
{
    _fractal = nullptr;
    _changeFractalIter = true;
    _imgInVector = false;
    _usingRenderImage = false;
    _zoomingBack = false;
    _dontDrawTempImage = false;
}

SFMLFractal::SFMLFractal(Fractal* fractal)
{
    _changeFractalIter = true;
    _imgInVector = false;
    _usingRenderImage = false;
    _zoomingBack = false;
    _dontDrawTempImage = false;
    _fractal = fractal;
    EnsureFontLoaded();
    ResetDisplayImages();
}

void SFMLFractal::SetFractal(Fractal* fractal)
{
    _fractal = fractal;
    _changeFractalIter = true;
    _dontDrawTempImage = true;
    EnsureFontLoaded();
    ClearImageCache();
    ResetDisplayImages();
}

Fractal* SFMLFractal::GetFractal() const
{
    return _fractal;
}

void SFMLFractal::EnsureFontLoaded()
{
    if (!_font.getInfo().family.empty())
        return;

    _font.loadFromFile(GetAbsPath({ "Resources", "PublicSans-Regular.otf" }));
    _iterationsText.setFont(_font);
}

void SFMLFractal::ResetDisplayImages()
{
    if (_fractal == nullptr)
        return;

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

void SFMLFractal::HandleEvent(const sf::Event& event)
{
    if (_fractal == nullptr)
        return;

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

    if (!_fractal->_onWxCtrl && event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Right && _fractal->_xVel == 0 && _fractal->_yVel == 0)
    {
        ZoomBack();
    }
}

void SFMLFractal::Resize(const sf::RenderWindow* window)
{
    if (_fractal == nullptr || window == nullptr)
        return;

    _fractal->StopRender();
    _fractal->_paused = false;

    for (int i = 0; i < _fractal->_backScreenWidth; i++)
    {
        delete[] _fractal->_setMap[i];
        delete[] _fractal->_colorMap[i];
        delete[] _fractal->_auxMap[i];
        _fractal->_setMap[i] = nullptr;
        _fractal->_colorMap[i] = nullptr;
        _fractal->_auxMap[i] = nullptr;
    }
    delete[] _fractal->_setMap;
    delete[] _fractal->_colorMap;
    delete[] _fractal->_auxMap;
    _fractal->_setMap = nullptr;
    _fractal->_colorMap = nullptr;
    _fractal->_auxMap = nullptr;

    _fractal->_screenHeight = window->getSize().y;
    _fractal->_backScreenWidth = _fractal->_screenWidth = window->getSize().x;
    _dontDrawTempImage = true;

    _fractal->_setMap = new bool* [_fractal->_screenWidth];
    _fractal->_colorMap = new int* [_fractal->_screenWidth];
    _fractal->_auxMap = new unsigned int* [_fractal->_screenWidth];
    for (int i = 0; i < _fractal->_screenWidth; i++)
    {
        _fractal->_setMap[i] = new bool[_fractal->_screenHeight];
        _fractal->_colorMap[i] = new int[_fractal->_screenHeight];
        _fractal->_auxMap[i] = new unsigned int[_fractal->_screenHeight];
    }

    for (int i = 0; i < _fractal->_screenWidth; i++)
    {
        for (int j = 0; j < _fractal->_screenHeight; j++)
        {
            _fractal->_setMap[i][j] = false;
            _fractal->_colorMap[i][j] = -1;
            _fractal->_auxMap[i][j] = 0;
        }
    }

    _fractal->_maxY = _fractal->_minY + (_fractal->_maxX - _fractal->_minX) * (double)_fractal->_screenHeight / _fractal->_screenWidth;
    _fractal->_xFactor = (_fractal->_maxX - _fractal->_minX) / (_fractal->_screenWidth - 1);
    _fractal->_yFactor = (_fractal->_maxY - _fractal->_minY) / (_fractal->_screenHeight - 1);
    _fractal->SetOutermostZoom();

    _fractal->_rendered = false;
    _fractal->_rendering = false;
    ClearImageCache();
    _fractal->_orbitDrawn = false;

    for (unsigned int i = 0; i < _fractal->_zoom[3].size(); i++)
    {
        _fractal->_zoom[3][i] = _fractal->_zoom[2][i] + (_fractal->_zoom[1][i] - _fractal->_zoom[0][i]) * (double)_fractal->_screenHeight / _fractal->_screenWidth;
    }

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
    if (_fractal == nullptr)
        return;

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

    _fractal->SetAreaOfView(pixelCoordinates);
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
    if (_fractal == nullptr)
        return;

    const bool stillRendering = _fractal->IsRendering();
    _fractal->ZoomBack();

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

void SFMLFractal::Redraw()
{
    if (_fractal == nullptr)
        return;

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

    if (_fractal == nullptr)
        return;

    ClearImageCache();
    const unsigned change = _fractal->GetIterations() + 100;
    _fractal->SetIterations(change);
}

void SFMLFractal::DecreaseIterations()
{
    _changeFractalIter = true;

    if (_fractal == nullptr)
        return;

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

    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetIterations(iterations);
}

void SFMLFractal::SetK(const double real, const double imaginary)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetK(real, imaginary);
}

void SFMLFractal::SetGradient(const wxGradient& gradient)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetGradient(gradient);
}

void SFMLFractal::SetGradientSize(const unsigned int size)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetGradientSize(size);
}

void SFMLFractal::SetColorPalette(const ColorPalettes palette)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetColorPalette(palette);
}

void SFMLFractal::SetExteriorColorMode(const bool mode)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetExtColorMode(mode);
}

void SFMLFractal::SetFractalSetColorMode(const bool mode)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetFractalSetColorMode(mode);
}

void SFMLFractal::SetFractalSetColor(const sf::Color color)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetFractalSetColor(color);
}

void SFMLFractal::SetRelativeColor(const bool mode)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetRelativeColor(mode);
}

void SFMLFractal::ChangeVarGradient()
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->ChangeVarGradient();
}

void SFMLFractal::SetVarGradient(const int offset)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetVarGradient(offset);
}

void SFMLFractal::SetAlgorithm(const RenderingAlgorithmType algorithm)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetAlgorithm(algorithm);
}

void SFMLFractal::SetOrbitTrapMode(const bool mode)
{
    if (_fractal == nullptr)
        return;

    ClearImageCache();
    _fractal->SetOrbitTrapMode(mode);
}

void SFMLFractal::SetSmoothRender(const bool mode)
{
    if (_fractal == nullptr)
        return;

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
        _fractal->_maxColorMapVal = 0;
        for (int i = 0; i < _fractal->_screenWidth; i++)
        {
            for (int j = 0; j < _fractal->_screenHeight; j++)
            {
                if (_fractal->_colorMap[i][j] > _fractal->_maxColorMapVal)
                    _fractal->_maxColorMapVal = _fractal->_colorMap[i][j];
            }
        }

        if (_fractal->_maxColorMapVal == 0)
            _fractal->_maxColorMapVal = 1;

        for (int i = 0; i < _fractal->_screenWidth; i++)
        {
            for (int j = 0; j < _fractal->_screenHeight; j++)
            {
                if (_fractal->_setMap[i][j] == true && _fractal->_colorSet)
                    _image.setPixel(i, j, _fractal->GetSetColor());
                else if (_fractal->_colorMode)
                {
                    if (_fractal->_colorMap[i][j] >= 0)
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
                    if (_fractal->_colorMap[i][j] >= 0)
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
    if (_fractal == nullptr || window == nullptr)
        return;

    if (_fractal->_rendered && (_fractal->_xVel != 0 || _fractal->_yVel != 0))
    {
        // While panning, only draw the shifted render output. Showing the
        // cached preview sprite here leaves stale pixels in the newly exposed area.
        _dontDrawTempImage = true;
        _output.setPosition(static_cast<float>(_fractal->_posX), static_cast<float>(_fractal->_posY));
    }
    else
    {
        if (_fractal->_moving)
        {
            MoveMatrix<bool>(_fractal->_setMap, _fractal->_screenHeight, _fractal->_screenWidth, _fractal->_yMoved, _fractal->_xMoved);
            MoveMatrix<int>(_fractal->_colorMap, _fractal->_screenHeight, _fractal->_screenWidth, _fractal->_yMoved, _fractal->_xMoved);
            MoveMatrix<unsigned int>(_fractal->_auxMap, _fractal->_screenHeight, _fractal->_screenWidth, _fractal->_yMoved, _fractal->_xMoved);
            _fractal->_moving = false;
            _fractal->_rendered = false;
        }

        if (!_fractal->_rendered)
        {
            if (_usingRenderImage)
            {
                _fractal->_moving = false;
                _usingRenderImage = false;
                _fractal->_xMoved = 0;
                _fractal->_yMoved = 0;
            }

            if (!_fractal->_rendering)
            {
                _fractal->_rendering = true;
                _fractal->PrepareRender();
                _fractal->Render();
            }

            _fractal->_xMoved = 0;
            _fractal->_yMoved = 0;

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
                const double coef = static_cast<double>(_fractal->_paletteSize) / static_cast<double>(_fractal->_maxColorMapVal);
                for (int i = 0; i < _fractal->_screenWidth; i++)
                {
                    for (int j = 0; j < _fractal->_screenHeight; j++)
                    {
                        if (_fractal->_setMap[i][j] == false || !_fractal->_colorSet)
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
