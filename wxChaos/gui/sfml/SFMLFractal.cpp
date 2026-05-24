#include "SFMLFractal.h"

#include <string>

#include "Fractal.h"
#include "Filesystem.h"

template<class M> void MoveMatrix(M** matrix, const unsigned int matrixWidth, const unsigned int matrixHeight,
                                  const int moveX, const int moveY)
{
    if (moveX > 0)
    {
        const int displacement = moveX;
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            int iterations = matrixWidth - displacement;
            for (int j = matrixWidth - 1; j >= 0; j--)
            {
                if (iterations > 0)
                {
                    matrix[i][j] = matrix[i][j - displacement];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }
    else if (moveX < 0)
    {
        const int displacement = -moveX;
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            int iterations = matrixWidth - displacement;
            for (unsigned int j = 0; j < matrixWidth; j++)
            {
                if (iterations > 0)
                {
                    matrix[i][j] = matrix[i][j + displacement];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }

    if (moveY > 0)
    {
        const int displacement = moveY;
        for (unsigned int j = 0; j < matrixWidth; j++)
        {
            int iterations = matrixHeight - displacement;
            for (int i = matrixHeight - 1; i >= 0; i--)
            {
                if (iterations > 0)
                {
                    matrix[i][j] = matrix[i - displacement][j];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }
    else if (moveY < 0)
    {
        const int displacement = -moveY;
        for (unsigned int j = 0; j < matrixWidth; j++)
        {
            int iterations = matrixHeight - displacement;
            for (unsigned int i = 0; i < matrixHeight; i++)
            {
                if (iterations > 0)
                {
                    matrix[i][j] = matrix[i + displacement][j];
                    iterations--;
                }
                else
                    matrix[i][j] = 0;
            }
        }
    }
}

SFMLFractal::SFMLFractal() : _fractal(nullptr) {}

SFMLFractal::SFMLFractal(Fractal* fractal) : _fractal(fractal)
{
    EnsureFontLoaded();
    ResetDisplayImages();
}

void SFMLFractal::SetFractal(Fractal* fractal)
{
    _fractal = fractal;
    EnsureFontLoaded();
    _imgVector.clear();
    _imgInVector = false;
    _usingRenderImage = false;
    _zoomingBack = false;
    _dontDrawTempImage = true;
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
    _text.setFont(_font);
}

void SFMLFractal::ResetDisplayImages()
{
    if (_fractal == nullptr)
        return;

    const sf::Color white(255, 255, 255);
    const sf::Color transparent(255, 255, 255, 0);
    _image.create(_fractal->screenWidth, _fractal->screenHeight, white);
    _tempImage = _image;
    _geomImage.create(_fractal->screenWidth, _fractal->screenHeight, transparent);

    _texture.loadFromImage(_image);
    _output.setTexture(_texture);

    _tempTexture.loadFromImage(_tempImage);
    _tempSprite.setTexture(_tempTexture);

    _geomTexture.loadFromImage(_geomImage);
    _outGeom.setTexture(_geomTexture);
}

void SFMLFractal::HandleEvent(const sf::Event& event) const
{
    if (_fractal == nullptr)
        return;

    if (!_fractal->IsRendering() && event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::L:
            _fractal->MoreIter();
            break;
        case sf::Keyboard::K:
            _fractal->LessIter();
            break;
        default:
            break;
        }
    }

    if (!_fractal->onWxCtrl && event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Right && _fractal->xVel == 0 && _fractal->yVel == 0)
    {
        _fractal->ZoomBack();
    }
}

void SFMLFractal::Resize(const sf::RenderWindow* window)
{
    if (_fractal == nullptr || window == nullptr)
        return;

    _fractal->StopRender();
    _fractal->paused = false;

    for (int i = 0; i < _fractal->backScreenWidth; i++)
    {
        delete[] _fractal->setMap[i];
        delete[] _fractal->colorMap[i];
        delete[] _fractal->auxMap[i];
        _fractal->setMap[i] = nullptr;
        _fractal->colorMap[i] = nullptr;
        _fractal->auxMap[i] = nullptr;
    }
    delete[] _fractal->setMap;
    delete[] _fractal->colorMap;
    delete[] _fractal->auxMap;
    _fractal->setMap = nullptr;
    _fractal->colorMap = nullptr;
    _fractal->auxMap = nullptr;

    _fractal->screenHeight = window->getSize().y;
    _fractal->backScreenWidth = _fractal->screenWidth = window->getSize().x;
    _dontDrawTempImage = true;

    _fractal->setMap = new bool* [_fractal->screenWidth];
    _fractal->colorMap = new int* [_fractal->screenWidth];
    _fractal->auxMap = new unsigned int* [_fractal->screenWidth];
    for (int i = 0; i < _fractal->screenWidth; i++)
    {
        _fractal->setMap[i] = new bool[_fractal->screenHeight];
        _fractal->colorMap[i] = new int[_fractal->screenHeight];
        _fractal->auxMap[i] = new unsigned int[_fractal->screenHeight];
    }

    for (int i = 0; i < _fractal->screenWidth; i++)
    {
        for (int j = 0; j < _fractal->screenHeight; j++)
        {
            _fractal->setMap[i][j] = false;
            _fractal->colorMap[i][j] = -1;
            _fractal->auxMap[i][j] = 0;
        }
    }

    _fractal->maxY = _fractal->minY + (_fractal->maxX - _fractal->minX) * (double)_fractal->screenHeight / _fractal->screenWidth;
    _fractal->xFactor = (_fractal->maxX - _fractal->minX) / (_fractal->screenWidth - 1);
    _fractal->yFactor = (_fractal->maxY - _fractal->minY) / (_fractal->screenHeight - 1);
    _fractal->SetOutermostZoom();

    _fractal->rendered = false;
    _fractal->rendering = false;
    _imgVector.clear();
    _imgInVector = false;
    _usingRenderImage = false;
    _zoomingBack = false;
    _fractal->orbitDrawn = false;

    for (unsigned int i = 0; i < _fractal->zoom[3].size(); i++)
    {
        _fractal->zoom[3][i] = _fractal->zoom[2][i] + (_fractal->zoom[1][i] - _fractal->zoom[0][i]) * (double)_fractal->screenHeight / _fractal->screenWidth;
    }

    ResetDisplayImages();
    _tempSprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(_fractal->screenWidth), static_cast<int>(_fractal->screenHeight)));
    sf::IntRect size;
    size.width = static_cast<int>(_fractal->screenWidth);
    size.height = static_cast<int>(_fractal->screenHeight);
    _output.setTextureRect(size);
    _outGeom.setTextureRect(size);
}

void SFMLFractal::SetAreaOfView(const sf::Rect<int>& pixelCoordinates)
{
    if (_fractal == nullptr)
        return;

    if (_fractal->paused)
    {
        _imgVector.clear();
        _imgInVector = false;
        _dontDrawTempImage = true;
    }
    else
    {
        _imgVector.push_back(_image);
        _imgInVector = true;
        _dontDrawTempImage = false;
    }

    _fractal->SetAreaOfView(pixelCoordinates);
    _tempImage = _image;
    _tempTexture.loadFromImage(_tempImage);
    _tempSprite.setTexture(_tempTexture);
    _tempSprite.setTextureRect(pixelCoordinates);
    _tempSprite.setPosition(0, 0);

    const float scaleX = static_cast<float>(_fractal->screenWidth) / static_cast<float>(pixelCoordinates.width);
    const float scaleY = static_cast<float>(_fractal->screenHeight) / static_cast<float>(pixelCoordinates.height);
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

    if (_imgInVector && !_fractal->varGradient && !_imgVector.empty() && !stillRendering)
    {
        _image = _imgVector.back();
        _texture.loadFromImage(_image);
        _imgVector.pop_back();
        _usingRenderImage = true;
        _fractal->rendering = false;
        _fractal->SetRendered(true);
        _zoomingBack = false;
        _dontDrawTempImage = true;
    }
    else
    {
        if (stillRendering && !_imgVector.empty())
            _imgVector.pop_back();
        _fractal->SetRendered(false);
        _zoomingBack = true;
    }
}

void SFMLFractal::Redraw()
{
    if (_fractal == nullptr)
        return;

    if (_fractal->colorMode)
    {
        _tempImage = _image;
        _tempTexture.loadFromImage(_tempImage);
        _tempSprite.setOrigin(0, 0);
        _tempSprite.setScale((float)_fractal->screenWidth / _tempImage.getSize().x, (float)_fractal->screenHeight / _tempImage.getSize().y);
    }

    _fractal->Redraw();
    _imgVector.clear();
    _imgInVector = false;
    _usingRenderImage = false;
    _dontDrawTempImage = true;
}

void SFMLFractal::DrawMaps(sf::RenderWindow* window)
{
    _fractal->PreDrawMaps();

    if (_zoomingBack || _dontDrawTempImage || !_fractal->colorMode)
        _image.create(_fractal->screenWidth, _fractal->screenHeight, sf::Color(255, 255, 255));
    else
    {
        _image.create(_fractal->screenWidth, _fractal->screenHeight, sf::Color(255, 255, 255, 0));
        window->draw(_tempSprite);
    }

    _output.setPosition(0, 0);
    if (_fractal->relativeColor)
    {
        _fractal->maxColorMapVal = 0;
        for (int i = 0; i < _fractal->screenWidth; i++)
        {
            for (int j = 0; j < _fractal->screenHeight; j++)
            {
                if (_fractal->colorMap[i][j] > _fractal->maxColorMapVal)
                    _fractal->maxColorMapVal = _fractal->colorMap[i][j];
            }
        }

        if (_fractal->maxColorMapVal == 0)
            _fractal->maxColorMapVal = 1;

        for (int i = 0; i < _fractal->screenWidth; i++)
        {
            for (int j = 0; j < _fractal->screenHeight; j++)
            {
                if (_fractal->setMap[i][j] == true && _fractal->colorSet)
                    _image.setPixel(i, j, _fractal->GetSetColor());
                else if (_fractal->colorMode)
                {
                    if (_fractal->colorMap[i][j] >= 0)
                    {
                        const sf::Color color = _fractal->CalcColor(((double)_fractal->colorMap[i][j] / (double)_fractal->maxColorMapVal) * _fractal->paletteSize + _fractal->changeGradient);
                        _image.setPixel(i, j, color);
                    }
                    else if (_zoomingBack || _dontDrawTempImage)
                    {
                        _image.setPixel(i, j, _fractal->CalcColor(_fractal->changeGradient));
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < _fractal->screenWidth; i++)
        {
            for (int j = 0; j < _fractal->screenHeight; j++)
            {
                if (_fractal->setMap[i][j] == true && _fractal->colorSet)
                    _image.setPixel(i, j, _fractal->GetSetColor());
                else if (_fractal->colorMode)
                {
                    if (_fractal->colorMap[i][j] >= 0)
                        _image.setPixel(i, j, _fractal->CalcColor(_fractal->colorMap[i][j] + _fractal->changeGradient));
                    else if (_zoomingBack || _dontDrawTempImage)
                        _image.setPixel(i, j, _fractal->CalcColor(_fractal->changeGradient));
                }
            }
        }
    }

    _texture.loadFromImage(_image);
    window->draw(_output);
}

void SFMLFractal::DrawGeometry(sf::RenderWindow* window)
{
    for (unsigned int i = 0; i < _fractal->lines.size(); i++)
    {
        const float x1 = (_fractal->lines[i].x1 - _fractal->minX) / _fractal->xFactor;
        const float y1 = (_fractal->maxY - _fractal->lines[i].y1) / _fractal->yFactor;
        const float x2 = (_fractal->lines[i].x2 - _fractal->minX) / _fractal->xFactor;
        const float y2 = (_fractal->maxY - _fractal->lines[i].y2) / _fractal->yFactor;
        sf::Vertex line[] = { sf::Vertex(sf::Vector2f(x1, y1), _fractal->lines[i].color), sf::Vertex(sf::Vector2f(x2, y2), _fractal->lines[i].color) };
        window->draw(line, 2, sf::Lines);
    }

    for (unsigned int i = 0; i < _fractal->orbitLines.size(); i++)
    {
        const float x1 = (_fractal->orbitLines[i].x1 - _fractal->minX) / _fractal->xFactor;
        const float y1 = (_fractal->maxY - _fractal->orbitLines[i].y1) / _fractal->yFactor;
        const float x2 = (_fractal->orbitLines[i].x2 - _fractal->minX) / _fractal->xFactor;
        const float y2 = (_fractal->maxY - _fractal->orbitLines[i].y2) / _fractal->yFactor;
        sf::Vertex line[] = { sf::Vertex(sf::Vector2f(x1, y1), _fractal->orbitLines[i].color), sf::Vertex(sf::Vector2f(x2, y2), _fractal->orbitLines[i].color) };
        window->draw(line, 2, sf::Lines);
    }

    for (unsigned int i = 0; i < _fractal->circles.size(); i++)
    {
        const float x0 = (_fractal->circles[i].x_center - _fractal->minX) / _fractal->xFactor;
        const float y0 = (_fractal->maxY - _fractal->circles[i].y_center) / _fractal->yFactor;
        const float right = (_fractal->circles[i].x_center + _fractal->circles[i].radius - _fractal->minX) / _fractal->xFactor;
        const float r = right - x0;
        sf::CircleShape circle(r);
        circle.setPosition(x0 - r, y0 - r);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(_fractal->circles[i].color);
        circle.setOutlineThickness(2);
        window->draw(circle);
    }
}

void SFMLFractal::Show(sf::RenderWindow* window)
{
    if (_fractal == nullptr || window == nullptr)
        return;

    if (_fractal->xVel != 0 || _fractal->yVel != 0)
    {
        // While panning, only draw the shifted render output. Showing the
        // cached preview sprite here leaves stale pixels in the newly exposed area.
        _dontDrawTempImage = true;
        _output.setPosition(static_cast<float>(_fractal->posX), static_cast<float>(_fractal->posY));
    }
    else
    {
        if (_fractal->moving)
        {
            MoveMatrix<bool>(_fractal->setMap, _fractal->screenHeight, _fractal->screenWidth, _fractal->yMoved, _fractal->xMoved);
            MoveMatrix<int>(_fractal->colorMap, _fractal->screenHeight, _fractal->screenWidth, _fractal->yMoved, _fractal->xMoved);
            MoveMatrix<unsigned int>(_fractal->auxMap, _fractal->screenHeight, _fractal->screenWidth, _fractal->yMoved, _fractal->xMoved);
            _fractal->moving = false;
            _fractal->rendered = false;
        }

        if (!_fractal->rendered)
        {
            if (_usingRenderImage)
            {
                _fractal->moving = false;
                _usingRenderImage = false;
                _fractal->xMoved = 0;
                _fractal->yMoved = 0;
            }

            if (!_fractal->rendering)
            {
                _fractal->rendering = true;
                _fractal->PrepareRender();
                _fractal->Render();
            }

            _fractal->xMoved = 0;
            _fractal->yMoved = 0;

            DrawMaps(window);

            if (!_fractal->IsRendering())
            {
                _fractal->rendered = true;
                _fractal->rendering = false;
                _dontDrawTempImage = false;
                _zoomingBack = false;
                _fractal->PostRender();
                DrawMaps(window);
                _fractal->refreshImage = false;
            }
        }

        if (_fractal->pausing)
        {
            DrawMaps(window);
            _fractal->pausing = false;
            _fractal->refreshImage = false;
        }

        if (_fractal->refreshImage)
        {
            DrawMaps(window);
            _fractal->refreshImage = false;
        }

        if (_fractal->varGradient || _fractal->varGradChange)
        {
            if (_fractal->changeGradient < _fractal->paletteSize)
                _fractal->changeGradient += _fractal->varGradientStep;
            else
                _fractal->changeGradient = 0;

            if (_fractal->rendered)
            {
                const double coef = (double)_fractal->paletteSize / (double)_fractal->maxColorMapVal;
                for (int i = 0; i < _fractal->screenWidth; i++)
                {
                    for (int j = 0; j < _fractal->screenHeight; j++)
                    {
                        if (_fractal->setMap[i][j] == false || !_fractal->colorSet)
                        {
                            sf::Color color;
                            if (_fractal->relativeColor)
                                color = _fractal->CalcColor(_fractal->colorMap[i][j] * coef + _fractal->changeGradient);
                            else
                                color = _fractal->CalcColor(_fractal->colorMap[i][j] + _fractal->changeGradient);

                            _image.setPixel(i, j, color);
                        }
                    }
                }
                _texture.loadFromImage(_image);
            }
            _fractal->varGradChange = false;
        }
    }

    if (!_dontDrawTempImage && _fractal->colorMode)
        window->draw(_tempSprite);

    window->draw(_output);

    if (_fractal->orbitMode && !_fractal->IsRendering())
    {
        if (!_fractal->orbitDrawn)
        {
            _fractal->orbitLines.clear();
            _geomImage.create(_fractal->screenWidth, _fractal->screenHeight, sf::Color(255, 255, 255, 0));
            _fractal->DrawOrbit();
            _geomTexture.loadFromImage(_geomImage);
        }
        window->draw(_outGeom);
    }
    if (_fractal->geomFigure && !_fractal->IsRendering())
        DrawGeometry(window);

    if (!_fractal->onSnapshot && !_fractal->rendering)
    {
        static sf::Image base;
        static sf::Texture baseTexture;
        static sf::Sprite baseSprite;
        if (_fractal->changeFractalIter)
        {
            int number = _fractal->maxIter;
            int digits = 1;
            while (number >= 10)
            {
                number /= 10;
                digits++;
            }

            base.create(148 + (12 * digits), 35, sf::Color(0, 0, 0, 100));
            baseTexture.loadFromImage(base);
            baseSprite.setTexture(baseTexture);
            baseSprite.setPosition(0, 0);

            _text.setString(" Iterations: " + std::to_string(_fractal->maxIter));
            _text.setCharacterSize(25);
            _text.setPosition(0, 0);
            _fractal->changeFractalIter = false;
        }
        window->draw(baseSprite);
        window->draw(_text);
    }
}
