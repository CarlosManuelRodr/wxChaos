#include "CoordinateSelector.h"
#include "Fractal.h"

int CoordinateSelector::ClampCoordinate(const int value, const unsigned int size)
{
    if (size == 0)
        return 0;
    if (value < 0)
        return 0;
    const int maxValue = static_cast<int>(size - 1);
    if (value > maxValue)
        return maxValue;
    return value;
}

CoordinateSelector::CoordinateSelector(const sf::RenderWindow* window)
{
    screenWidth = window->getSize().x;
    screenHeight = window->getSize().y;
    this->SetPosition(static_cast<int>(window->getSize().x / 2), static_cast<int>(window->getSize().y / 2));

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
    this->SetPosition(x, y);
    for (unsigned int i = 0; i < screenWidth; i++)
        textureImage.setPixel(i, y, color);
    for (unsigned int j = 0; j < screenHeight; j++)
        textureImage.setPixel(x, j, color);
    texture.loadFromImage(textureImage);
    rendered = true;
}

void CoordinateSelector::SetPosition(const int newX, const int newY)
{
    x = ClampCoordinate(newX, screenWidth);
    y = ClampCoordinate(newY, screenHeight);
}

bool CoordinateSelector::HandleEvents(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            this->SetPosition(event.mouseButton.x, event.mouseButton.y);
            rendered = false;
            inSelection = true;
        }
    }

    if (event.type == sf::Event::MouseMoved && inSelection)
    {
        this->SetPosition(event.mouseMove.x, event.mouseMove.y);
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
        this->SetPosition(event.GetPosition().x, event.GetPosition().y);
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
        const int newX = ClampCoordinate(event.GetPosition().x, screenWidth);
        const int newY = ClampCoordinate(event.GetPosition().y, screenHeight);
        if (x != newX || y != newY)
        {
            this->SetPosition(newX, newY);
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
    this->SetPosition(static_cast<int>(screenWidth / 2), static_cast<int>(screenHeight / 2));
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
    this->SetPosition(target->GetPixelX(numX), target->GetPixelY(numY));
}
