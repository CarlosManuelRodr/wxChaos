#include "Button.h"
#include <sstream>

Button::Button(const std::string& path, const int posX, const int posY, sf::RenderWindow* window)
{
    _textureImage.loadFromFile(path);
    _texture.loadFromImage(_textureImage);
    _sprite.setTexture(_texture);
    _sprite.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    _area = _sprite.getGlobalBounds();
    _pressed = false;
    _thereIsText = false;
}

Button::Button(const int posX, const int posY, sf::RenderWindow* window, std::string text)
{
    font.loadFromFile("arial.ttf");
    _buttonText.setFont(font);
    _buttonText.setString(text);
    _buttonText.setCharacterSize(14);
    _buttonText.setFillColor(sf::Color::White);
    _sprite.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    _area = _sprite.getGlobalBounds();
    _pressed = false;
    _thereIsText = true;
}

void Button::Resize(const sf::RenderWindow* window)
{
    if (_fx != 0.0)
        _area.left = window->getSize().x - _fx;
    if (_fy != 0.0)
        _area.top = window->getSize().y - _fy;
}

void Button::Show(sf::RenderWindow* window)
{
    window->draw(_sprite);
    if (_thereIsText)
    {
        _buttonText.setPosition(_sprite.getPosition());
        window->draw(_buttonText);
    }
}

void Button::SetAnchorage(bool top, bool left, bool bottom, bool right)
{
    // Implementation placeholder
}

void Button::ChangeState()
{
    _pressed = !_pressed;
}

bool Button::HandleEvents(sf::Event Event)
{
    return false;
}

bool Button::ClickEvent(wxMouseEvent& event)
{
    return false;
}
