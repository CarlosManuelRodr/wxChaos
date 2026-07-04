#include "CoordinateSelector.h"
#include "Fractal.h"

int CoordinateSelector::ClampCoordinate(const int value, const unsigned int size)
{
    if (size == 0)
        return 0;
    if (value < 0)
        return 0;
    if (const int maxValue = static_cast<int>(size - 1); value > maxValue)
        return maxValue;
    return value;
}

CoordinateSelector::CoordinateSelector(const sf::RenderWindow* window)
{
    _x = 0;
    _y = 0;
    _screenWidth = window->getSize().x;
    _screenHeight = window->getSize().y;
    this->SetPosition(static_cast<int>(window->getSize().x / 2), static_cast<int>(window->getSize().y / 2));
    _lines = sf::VertexArray(sf::Lines, 12);

    this->Render();
    _rendered = true;
    _inSelection = false;
}

void CoordinateSelector::Show(sf::RenderWindow* window)
{
    if (!_rendered)
        this->Render();
    window->draw(_lines);
}

void CoordinateSelector::Render()
{
    this->SetPosition(_x, _y);

    const float x = static_cast<float>(_x);
    const float y = static_cast<float>(_y);
    const float width = static_cast<float>(_screenWidth);
    const float height = static_cast<float>(_screenHeight);
    const sf::Color outlineColor(0, 0, 0);
    const sf::Color centerColor(255, 255, 255);

    this->SetLine(0, x - 1.f, 0.f, x - 1.f, height, outlineColor);
    this->SetLine(1, x + 1.f, 0.f, x + 1.f, height, outlineColor);
    this->SetLine(2, 0.f, y - 1.f, width, y - 1.f, outlineColor);
    this->SetLine(3, 0.f, y + 1.f, width, y + 1.f, outlineColor);
    this->SetLine(4, x, 0.f, x, height, centerColor);
    this->SetLine(5, 0.f, y, width, y, centerColor);

    _rendered = true;
}

void CoordinateSelector::SetLine(const unsigned int lineIndex, const float startX, const float startY,
                                 const float endX, const float endY, const sf::Color& color)
{
    const unsigned int vertexIndex = lineIndex * 2;
    _lines[vertexIndex].position = sf::Vector2f(startX, startY);
    _lines[vertexIndex].color = color;
    _lines[vertexIndex + 1].position = sf::Vector2f(endX, endY);
    _lines[vertexIndex + 1].color = color;
}

void CoordinateSelector::SetPosition(const int newX, const int newY)
{
    _x = ClampCoordinate(newX, _screenWidth);
    _y = ClampCoordinate(newY, _screenHeight);
}

bool CoordinateSelector::HandleEvents(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            this->SetPosition(event.mouseButton.x, event.mouseButton.y);
            _rendered = false;
            _inSelection = true;
        }
    }

    if (event.type == sf::Event::MouseMoved && _inSelection)
    {
        this->SetPosition(event.mouseMove.x, event.mouseMove.y);
        _rendered = false;
        return true;
    }

    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            _inSelection = false;
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
        _rendered = false;
        _inSelection = true;
        return true;
    }
    return false;
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void CoordinateSelector::ReleaseClickEvent(wxMouseEvent& event)
{
    if (event.ButtonUp(wxMOUSE_BTN_LEFT))
        _inSelection = false;
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool CoordinateSelector::MoveEvent(wxMouseEvent& event)
{
    if (_inSelection)
    {
        const int newX = ClampCoordinate(event.GetPosition().x, _screenWidth);
        const int newY = ClampCoordinate(event.GetPosition().y, _screenHeight);
        if (_x != newX || _y != newY)
        {
            this->SetPosition(newX, newY);
            _rendered = false;
            return true;
        }
    }
    return false;
}

void CoordinateSelector::Resize(const sf::RenderWindow* window)
{
    _screenWidth = window->getSize().x;
    _screenHeight = window->getSize().y;
    this->SetPosition(static_cast<int>(_screenWidth / 2), static_cast<int>(_screenHeight / 2));
    this->Render();
    _inSelection = false;
}

double CoordinateSelector::GetX(const Fractal* target) const
{
    return target->GetX(_x);
}

double CoordinateSelector::GetY(const Fractal* target) const
{
    return target->GetY(_y);
}

void CoordinateSelector::AdjustPosition(const Fractal* target, const double numX, const double numY)
{
    this->SetPosition(target->GetPixelX(numX), target->GetPixelY(numY));
}
