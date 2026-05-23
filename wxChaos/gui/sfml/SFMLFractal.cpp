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

SFMLFractal::SFMLFractal()
    : fractal(nullptr), imgInVector(false), usingRenderImage(false), zoomingBack(false), dontDrawTempImage(false) {}

SFMLFractal::SFMLFractal(Fractal* fractal) : fractal(fractal)
{
    EnsureFontLoaded();
    ResetDisplayImages();
}

void SFMLFractal::SetFractal(Fractal* newFractal)
{
    fractal = newFractal;
    EnsureFontLoaded();
    imgVector.clear();
    imgInVector = false;
    usingRenderImage = false;
    zoomingBack = false;
    dontDrawTempImage = true;
    ResetDisplayImages();
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

void SFMLFractal::ResetDisplayImages()
{
    if (fractal == nullptr)
        return;

    const sf::Color white(255, 255, 255);
    const sf::Color transparent(255, 255, 255, 0);
    image.create(fractal->screenWidth, fractal->screenHeight, white);
    tempImage = image;
    geomImage.create(fractal->screenWidth, fractal->screenHeight, transparent);

    texture.loadFromImage(image);
    output.setTexture(texture);

    tempTexture.loadFromImage(tempImage);
    tempSprite.setTexture(tempTexture);

    geomTexture.loadFromImage(geomImage);
    outGeom.setTexture(geomTexture);
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
    dontDrawTempImage = true;

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
    imgVector.clear();
    imgInVector = false;
    usingRenderImage = false;
    zoomingBack = false;
    fractal->orbitDrawn = false;

    for (unsigned int i = 0; i < fractal->zoom[3].size(); i++)
    {
        fractal->zoom[3][i] = fractal->zoom[2][i] + (fractal->zoom[1][i] - fractal->zoom[0][i]) * (double)fractal->screenHeight / fractal->screenWidth;
    }

    ResetDisplayImages();
    tempSprite.setTextureRect(sf::IntRect(0, 0, fractal->screenWidth, fractal->screenHeight));
    sf::IntRect size;
    size.width = fractal->screenWidth;
    size.height = fractal->screenHeight;
    output.setTextureRect(size);
    outGeom.setTextureRect(size);
}

void SFMLFractal::SetAreaOfView(const sf::Rect<int>& pixelCoordinates)
{
    if (fractal == nullptr)
        return;

    if (fractal->paused)
    {
        imgVector.clear();
        imgInVector = false;
        dontDrawTempImage = true;
    }
    else
    {
        imgVector.push_back(image);
        imgInVector = true;
        dontDrawTempImage = false;
    }

    fractal->SetAreaOfView(pixelCoordinates);
    tempImage = image;
    tempTexture.loadFromImage(tempImage);
    tempSprite.setTexture(tempTexture);
    tempSprite.setTextureRect(pixelCoordinates);
    tempSprite.setPosition(0, 0);
    tempSprite.setScale((float)fractal->screenWidth / pixelCoordinates.width, (float)fractal->screenHeight / pixelCoordinates.height);
    usingRenderImage = false;
    zoomingBack = false;
}

void SFMLFractal::ZoomBack()
{
    if (fractal == nullptr)
        return;

    const bool stillRendering = fractal->IsRendering();
    fractal->ZoomBack();

    if (imgInVector && !fractal->varGradient && !imgVector.empty() && !stillRendering)
    {
        image = imgVector.back();
        texture.loadFromImage(image);
        imgVector.pop_back();
        usingRenderImage = true;
        fractal->rendering = false;
        fractal->SetRendered(true);
        zoomingBack = false;
        dontDrawTempImage = true;
    }
    else
    {
        if (stillRendering && !imgVector.empty())
            imgVector.pop_back();
        fractal->SetRendered(false);
        zoomingBack = true;
    }
}

void SFMLFractal::Redraw()
{
    if (fractal == nullptr)
        return;

    if (fractal->colorMode)
    {
        tempImage = image;
        tempTexture.loadFromImage(tempImage);
        tempSprite.setOrigin(0, 0);
        tempSprite.setScale((float)fractal->screenWidth / tempImage.getSize().x, (float)fractal->screenHeight / tempImage.getSize().y);
    }

    fractal->Redraw();
    imgVector.clear();
    imgInVector = false;
    usingRenderImage = false;
    dontDrawTempImage = true;
}

void SFMLFractal::DrawMaps(sf::RenderWindow* window)
{
    fractal->PreDrawMaps();

    if (zoomingBack || dontDrawTempImage || !fractal->colorMode)
        image.create(fractal->screenWidth, fractal->screenHeight, sf::Color(255, 255, 255));
    else
    {
        image.create(fractal->screenWidth, fractal->screenHeight, sf::Color(255, 255, 255, 0));
        window->draw(tempSprite);
    }

    output.setPosition(0, 0);
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
                    image.setPixel(i, j, fractal->GetSetColor());
                else if (fractal->colorMode)
                {
                    if (fractal->colorMap[i][j] >= 0)
                    {
                        const sf::Color color = fractal->CalcColor(((double)fractal->colorMap[i][j] / (double)fractal->maxColorMapVal) * fractal->paletteSize + fractal->changeGradient);
                        image.setPixel(i, j, color);
                    }
                    else if (zoomingBack || dontDrawTempImage)
                    {
                        image.setPixel(i, j, fractal->CalcColor(fractal->changeGradient));
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
                    image.setPixel(i, j, fractal->GetSetColor());
                else if (fractal->colorMode)
                {
                    if (fractal->colorMap[i][j] >= 0)
                        image.setPixel(i, j, fractal->CalcColor(fractal->colorMap[i][j] + fractal->changeGradient));
                    else if (zoomingBack || dontDrawTempImage)
                        image.setPixel(i, j, fractal->CalcColor(fractal->changeGradient));
                }
            }
        }
    }

    texture.loadFromImage(image);
    window->draw(output);
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
    {
        // While panning, only draw the shifted render output. Showing the
        // cached preview sprite here leaves stale pixels in the newly exposed area.
        dontDrawTempImage = true;
        output.setPosition(static_cast<float>(fractal->posX), static_cast<float>(fractal->posY));
    }
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
            if (usingRenderImage)
            {
                fractal->moving = false;
                usingRenderImage = false;
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
                dontDrawTempImage = false;
                zoomingBack = false;
                fractal->PostRender();
                DrawMaps(window);
                fractal->refreshImage = false;
            }
        }

        if (fractal->pausing)
        {
            DrawMaps(window);
            fractal->pausing = false;
            fractal->refreshImage = false;
        }

        if (fractal->refreshImage)
        {
            DrawMaps(window);
            fractal->refreshImage = false;
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

                            image.setPixel(i, j, color);
                        }
                    }
                }
                texture.loadFromImage(image);
            }
            fractal->varGradChange = false;
        }
    }

    if (!dontDrawTempImage && fractal->colorMode)
        window->draw(tempSprite);

    window->draw(output);

    if (fractal->orbitMode && !fractal->IsRendering())
    {
        if (!fractal->orbitDrawn)
        {
            fractal->orbitLines.clear();
            geomImage.create(fractal->screenWidth, fractal->screenHeight, sf::Color(255, 255, 255, 0));
            fractal->DrawOrbit();
            geomTexture.loadFromImage(geomImage);
        }
        window->draw(outGeom);
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
