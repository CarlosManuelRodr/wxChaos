#include "CoordinateSelector.h"
#include "Fractal.h"

CoordinateSelector::CoordinateSelector(const sf::RenderWindow* window)
{
    screenWidth = window->getSize().x;
    screenHeight = window->getSize().y;
    x = window->getSize().x / 2;
    y = window->getSize().y / 2;

    color = sf::Color(0, 0, 0);
    textureImage.create(screenWidth, screenHeight, sf::Color(255, 255, 255, 0));
    texture.loadFromImage(textureImage);
    output.setTexture(texture);

    this->Render();
    rendered = true;
    inSelection = false;
}

void CoordinateSelector::Show(sf::RenderWindow* window)
{
    if (!rendered)
        this->Render();
    window->draw(output);
}

void CoordinateSelector::Render()
{
    textureImage.create(screenWidth, screenHeight, sf::Color(255, 255, 255, 0));
    for (unsigned int i = 0; i < screenWidth; i++)
        textureImage.setPixel(i, y, color);
    for (unsigned int j = 0; j < screenHeight; j++)
        textureImage.setPixel(x, j, color);
    texture.loadFromImage(textureImage);
    rendered = true;
}

bool CoordinateSelector::HandleEvents(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            y = event.mouseButton.y;
            x = event.mouseButton.x;
            rendered = false;
            inSelection = true;
        }
    }

    if (event.type == sf::Event::MouseMoved && inSelection)
    {
        x = event.mouseMove.x;
        y = event.mouseMove.y;
        rendered = false;
        return true;
    }

    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            inSelection = false;
            return true;
        }
    }
    return false;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool CoordinateSelector::ClickEvent(wxMouseEvent& event)
{
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        y = event.GetPosition().y;
        x = event.GetPosition().x;
        rendered = false;
        inSelection = true;
        return true;
    }
    return false;
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void CoordinateSelector::UnClickEvent(wxMouseEvent& event)
{
    if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        inSelection = false;
    }
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool CoordinateSelector::MoveEvent(wxMouseEvent& event)
{
    if (inSelection)
    {
        if (x != event.GetPosition().x || y != event.GetPosition().y)
        {
            y = event.GetPosition().y;
            x = event.GetPosition().x;
            rendered = false;
            return true;
        }
    }
    return false;
}

void CoordinateSelector::Resize(const sf::RenderWindow* window)
{
    screenWidth = window->getSize().x;
    screenHeight = window->getSize().y;
    x = screenWidth / 2;
    y = screenHeight / 2;
    this->Render();
    inSelection = false;

    sf::IntRect Size;
    Size.width = screenWidth;
    Size.height = screenHeight;
    output.setTextureRect(Size);
}

double CoordinateSelector::GetX(const Fractal* target) const
{
    return target->GetX(x);
}

double CoordinateSelector::GetY(const Fractal* target) const
{
    return target->GetY(y);
}

void CoordinateSelector::AdjustPosition(const Fractal* target, const double numX, const double numY)
{
    x = target->GetPixelX(numX);
    y = target->GetPixelY(numY);
}
