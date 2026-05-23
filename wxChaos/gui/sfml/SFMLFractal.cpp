#include "SFMLFractal.h"

#include <string>

#include "Fractal.h"
#include "Filesystem.h"

namespace
{
template<class M> void MoveMatrix(M** matrix, const unsigned int matrixWidth, const unsigned int matrixHeight, const int moveX, const int moveY)
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
}

SFMLFractal::SFMLFractal() : fractal(nullptr) {}

SFMLFractal::SFMLFractal(Fractal* fractal) : fractal(fractal)
{
    EnsureFontLoaded();
}

void SFMLFractal::SetFractal(Fractal* newFractal)
{
    fractal = newFractal;
    EnsureFontLoaded();
}

Fractal* SFMLFractal::GetFractal() const
{
    return fractal;
}

void SFMLFractal::EnsureFontLoaded()
{
    if (!font.getInfo().family.empty())
        return;

    font.loadFromFile(GetAbsPath({ "Resources", "PublicSans-Regular.otf" }));
    text.setFont(font);
}

void SFMLFractal::HandleEvent(const sf::Event& event)
{
    if (fractal == nullptr)
        return;

    if (!fractal->IsRendering() && event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::L:
            fractal->MoreIter();
            break;
        case sf::Keyboard::K:
            fractal->LessIter();
            break;
        default:
            break;
        }
    }

    if (!fractal->onWxCtrl && event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Right && fractal->xVel == 0 && fractal->yVel == 0)
    {
        fractal->ZoomBack();
    }
}

void SFMLFractal::Resize(sf::RenderWindow* window)
{
    if (fractal == nullptr || window == nullptr)
        return;

    fractal->StopRender();
    fractal->paused = false;

    for (int i = 0; i < fractal->backScreenWidth; i++)
    {
        delete[] fractal->setMap[i];
        delete[] fractal->colorMap[i];
        delete[] fractal->auxMap[i];
        fractal->setMap[i] = nullptr;
        fractal->colorMap[i] = nullptr;
        fractal->auxMap[i] = nullptr;
    }
    delete[] fractal->setMap;
    delete[] fractal->colorMap;
    delete[] fractal->auxMap;
    fractal->setMap = nullptr;
    fractal->colorMap = nullptr;
    fractal->auxMap = nullptr;

    fractal->screenHeight = window->getSize().y;
    fractal->backScreenWidth = fractal->screenWidth = window->getSize().x;
    fractal->dontDrawTempImage = true;
    fractal->tempSprite.setTextureRect(sf::IntRect(0, 0, fractal->screenWidth, fractal->screenHeight));

    fractal->setMap = new bool* [fractal->screenWidth];
    fractal->colorMap = new int* [fractal->screenWidth];
    fractal->auxMap = new unsigned int* [fractal->screenWidth];
    for (int i = 0; i < fractal->screenWidth; i++)
    {
        fractal->setMap[i] = new bool[fractal->screenHeight];
        fractal->colorMap[i] = new int[fractal->screenHeight];
        fractal->auxMap[i] = new unsigned int[fractal->screenHeight];
    }

    for (int i = 0; i < fractal->screenWidth; i++)
    {
        for (int j = 0; j < fractal->screenHeight; j++)
        {
            fractal->setMap[i][j] = false;
            fractal->colorMap[i][j] = -1;
            fractal->auxMap[i][j] = 0;
        }
    }

    fractal->maxY = fractal->minY + (fractal->maxX - fractal->minX) * (double)fractal->screenHeight / fractal->screenWidth;
    fractal->xFactor = (fractal->maxX - fractal->minX) / (fractal->screenWidth - 1);
    fractal->yFactor = (fractal->maxY - fractal->minY) / (fractal->screenHeight - 1);
    fractal->SetOutermostZoom();

    fractal->rendered = false;
    fractal->rendering = false;
    fractal->geomImage.create(fractal->screenWidth, fractal->screenHeight, sf::Color(255, 255, 255, 0));
    fractal->geomTexture.loadFromImage(fractal->geomImage);
    fractal->imgVector.clear();
    fractal->imgInVector = false;
    fractal->orbitDrawn = false;

    for (unsigned int i = 0; i < fractal->zoom[3].size(); i++)
    {
        fractal->zoom[3][i] = fractal->zoom[2][i] + (fractal->zoom[1][i] - fractal->zoom[0][i]) * (double)fractal->screenHeight / fractal->screenWidth;
    }

    sf::IntRect size;
    size.width = fractal->screenWidth;
    size.height = fractal->screenHeight;
    fractal->output.setTextureRect(size);
    fractal->outGeom.setTextureRect(size);
}

void SFMLFractal::DrawMaps(sf::RenderWindow* window)
{
    fractal->PreDrawMaps();

    if (fractal->zoomingBack || fractal->dontDrawTempImage || !fractal->colorMode)
        fractal->image.create(fractal->screenWidth, fractal->screenHeight, fractal->white);
    else
    {
        fractal->image.create(fractal->screenWidth, fractal->screenHeight, sf::Color(255, 255, 255, 0));
        window->draw(fractal->tempSprite);
    }

    fractal->output.setPosition(0, 0);
    if (fractal->relativeColor)
    {
        fractal->maxColorMapVal = 0;
        for (int i = 0; i < fractal->screenWidth; i++)
        {
            for (int j = 0; j < fractal->screenHeight; j++)
            {
                if (fractal->colorMap[i][j] > fractal->maxColorMapVal)
                    fractal->maxColorMapVal = fractal->colorMap[i][j];
            }
        }

        if (fractal->maxColorMapVal == 0)
            fractal->maxColorMapVal = 1;

        for (int i = 0; i < fractal->screenWidth; i++)
        {
            for (int j = 0; j < fractal->screenHeight; j++)
            {
                if (fractal->setMap[i][j] == true && fractal->colorSet)
                    fractal->image.setPixel(i, j, fractal->fSetColor);
                else if (fractal->colorMode)
                {
                    if (fractal->colorMap[i][j] >= 0)
                    {
                        const sf::Color color = fractal->CalcColor(((double)fractal->colorMap[i][j] / (double)fractal->maxColorMapVal) * fractal->paletteSize + fractal->changeGradient);
                        fractal->image.setPixel(i, j, color);
                    }
                    else if (fractal->zoomingBack || fractal->dontDrawTempImage)
                    {
                        fractal->image.setPixel(i, j, fractal->CalcColor(fractal->changeGradient));
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < fractal->screenWidth; i++)
        {
            for (int j = 0; j < fractal->screenHeight; j++)
            {
                if (fractal->setMap[i][j] == true && fractal->colorSet)
                    fractal->image.setPixel(i, j, fractal->fSetColor);
                else if (fractal->colorMode)
                {
                    if (fractal->colorMap[i][j] >= 0)
                        fractal->image.setPixel(i, j, fractal->CalcColor(fractal->colorMap[i][j] + fractal->changeGradient));
                    else if (fractal->zoomingBack || fractal->dontDrawTempImage)
                        fractal->image.setPixel(i, j, fractal->CalcColor(fractal->changeGradient));
                }
            }
        }
    }

    fractal->texture.loadFromImage(fractal->image);
    window->draw(fractal->output);
}

void SFMLFractal::DrawGeometry(sf::RenderWindow* window)
{
    for (unsigned int i = 0; i < fractal->lines.size(); i++)
    {
        const float x1 = (fractal->lines[i].x1 - fractal->minX) / fractal->xFactor;
        const float y1 = (fractal->maxY - fractal->lines[i].y1) / fractal->yFactor;
        const float x2 = (fractal->lines[i].x2 - fractal->minX) / fractal->xFactor;
        const float y2 = (fractal->maxY - fractal->lines[i].y2) / fractal->yFactor;
        sf::Vertex line[] = { sf::Vertex(sf::Vector2f(x1, y1), fractal->lines[i].color), sf::Vertex(sf::Vector2f(x2, y2), fractal->lines[i].color) };
        window->draw(line, 2, sf::Lines);
    }

    for (unsigned int i = 0; i < fractal->orbitLines.size(); i++)
    {
        const float x1 = (fractal->orbitLines[i].x1 - fractal->minX) / fractal->xFactor;
        const float y1 = (fractal->maxY - fractal->orbitLines[i].y1) / fractal->yFactor;
        const float x2 = (fractal->orbitLines[i].x2 - fractal->minX) / fractal->xFactor;
        const float y2 = (fractal->maxY - fractal->orbitLines[i].y2) / fractal->yFactor;
        sf::Vertex line[] = { sf::Vertex(sf::Vector2f(x1, y1), fractal->orbitLines[i].color), sf::Vertex(sf::Vector2f(x2, y2), fractal->orbitLines[i].color) };
        window->draw(line, 2, sf::Lines);
    }

    for (unsigned int i = 0; i < fractal->circles.size(); i++)
    {
        const float x0 = (fractal->circles[i].x_center - fractal->minX) / fractal->xFactor;
        const float y0 = (fractal->maxY - fractal->circles[i].y_center) / fractal->yFactor;
        const float right = (fractal->circles[i].x_center + fractal->circles[i].radius - fractal->minX) / fractal->xFactor;
        const float r = right - x0;
        sf::CircleShape circle(r);
        circle.setPosition(x0 - r, y0 - r);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(fractal->circles[i].color);
        circle.setOutlineThickness(2);
        window->draw(circle);
    }
}

void SFMLFractal::Show(sf::RenderWindow* window)
{
    if (fractal == nullptr || window == nullptr)
        return;

    if (fractal->xVel != 0 || fractal->yVel != 0)
        fractal->output.setPosition(static_cast<float>(fractal->posX), static_cast<float>(fractal->posY));
    else
    {
        if (fractal->moving)
        {
            MoveMatrix<bool>(fractal->setMap, fractal->screenHeight, fractal->screenWidth, fractal->yMoved, fractal->xMoved);
            MoveMatrix<int>(fractal->colorMap, fractal->screenHeight, fractal->screenWidth, fractal->yMoved, fractal->xMoved);
            MoveMatrix<unsigned int>(fractal->auxMap, fractal->screenHeight, fractal->screenWidth, fractal->yMoved, fractal->xMoved);
            fractal->moving = false;
            fractal->rendered = false;
        }

        if (!fractal->rendered)
        {
            if (fractal->usingRenderImage)
            {
                fractal->moving = false;
                fractal->usingRenderImage = false;
                fractal->xMoved = 0;
                fractal->yMoved = 0;
            }

            if (!fractal->rendering)
            {
                fractal->rendering = true;
                fractal->PrepareRender();
                fractal->Render();
            }

            fractal->xMoved = 0;
            fractal->yMoved = 0;

            DrawMaps(window);

            if (!fractal->IsRendering())
            {
                fractal->rendered = true;
                fractal->rendering = false;
                fractal->dontDrawTempImage = false;
                fractal->zoomingBack = false;
                fractal->PostRender();
                DrawMaps(window);
            }
        }

        if (fractal->pausing)
        {
            DrawMaps(window);
            fractal->pausing = false;
        }

        if (fractal->varGradient || fractal->varGradChange)
        {
            if (fractal->changeGradient < fractal->paletteSize)
                fractal->changeGradient += fractal->varGradientStep;
            else
                fractal->changeGradient = 0;

            if (fractal->rendered)
            {
                const double coef = (double)fractal->paletteSize / (double)fractal->maxColorMapVal;
                for (int i = 0; i < fractal->screenWidth; i++)
                {
                    for (int j = 0; j < fractal->screenHeight; j++)
                    {
                        if (fractal->setMap[i][j] == false || !fractal->colorSet)
                        {
                            sf::Color color;
                            if (fractal->relativeColor)
                                color = fractal->CalcColor(fractal->colorMap[i][j] * coef + fractal->changeGradient);
                            else
                                color = fractal->CalcColor(fractal->colorMap[i][j] + fractal->changeGradient);

                            fractal->image.setPixel(i, j, color);
                        }
                    }
                }
                fractal->texture.loadFromImage(fractal->image);
            }
            fractal->varGradChange = false;
        }
    }

    if (!fractal->dontDrawTempImage && fractal->colorMode)
        window->draw(fractal->tempSprite);

    window->draw(fractal->output);

    if (fractal->orbitMode && !fractal->IsRendering())
    {
        if (!fractal->orbitDrawn)
        {
            fractal->orbitLines.clear();
            fractal->geomImage.create(fractal->screenWidth, fractal->screenHeight, sf::Color(255, 255, 255, 0));
            fractal->DrawOrbit();
            fractal->geomTexture.loadFromImage(fractal->geomImage);
        }
        window->draw(fractal->outGeom);
    }
    if (fractal->geomFigure && !fractal->IsRendering())
        DrawGeometry(window);

    if (!fractal->onSnapshot && !fractal->rendering)
    {
        static sf::Image base;
        static sf::Texture baseTexture;
        static sf::Sprite baseSprite;
        if (fractal->changeFractalIter)
        {
            int number = fractal->maxIter;
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

            text.setString(" Iterations: " + std::to_string(fractal->maxIter));
            text.setCharacterSize(25);
            text.setPosition(0, 0);
            fractal->changeFractalIter = false;
        }
        window->draw(baseSprite);
        window->draw(text);
    }
}
