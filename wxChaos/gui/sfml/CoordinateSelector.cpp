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

    _color = sf::Color(0, 0, 0);
    _textureImage.create(_screenWidth, _screenHeight, sf::Color(255, 255, 255, 0));
    _texture.loadFromImage(_textureImage);
    _output.setTexture(_texture);

    this->Render();
    _rendered = true;
    _inSelection = false;
}

void CoordinateSelector::Show(sf::RenderWindow* window)
{
    if (!_rendered)
        this->Render();
    window->draw(_output);
}

void CoordinateSelector::Render()
{
    _textureImage.create(_screenWidth, _screenHeight, sf::Color(255, 255, 255, 0));
    this->SetPosition(_x, _y);
    for (unsigned int i = 0; i < _screenWidth; i++)
        _textureImage.setPixel(i, _y, _color);
    for (unsigned int j = 0; j < _screenHeight; j++)
        _textureImage.setPixel(_x, j, _color);
    _texture.loadFromImage(_textureImage);
    _rendered = true;
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

    sf::IntRect size;
    size.width = static_cast<int>(_screenWidth);
    size.height = static_cast<int>(_screenHeight);
    _output.setTextureRect(size);
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
