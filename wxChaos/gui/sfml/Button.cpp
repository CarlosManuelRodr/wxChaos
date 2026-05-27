#include "Button.h"
#include <sstream>

Button::Button(const std::string& path, const int posX, const int posY, const sf::RenderWindow* window)
{
    _textureImage.loadFromFile(path);
    _texture.loadFromImage(_textureImage);
    _sprite.setTexture(_texture);
    _sprite.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    _width = _texture.getSize().x;
    _height = _texture.getSize().y;
    _area = _sprite.getGlobalBounds();
    _pressed = false;
    _anchorage = false;
    _screenWidth = window->getSize().x;
    _screenHeight = window->getSize().y;
    _fy = _area.top/_screenHeight;
    _fx = _area.left/_screenWidth;
    _anchorType = 0;
}

void Button::Resize(const sf::RenderWindow* window)
{
    if (!_anchorage)
    {
        _area.top = _fy * window->getSize().y;
        _area.height = _texture.getSize().y;
        _area.left = _fx * window->getSize().x;
        _area.width = _texture.getSize().x + _fx * _texture.getSize().x;
        _sprite.setPosition(_area.left, _area.top);
    }
    else
    {
        // TODO: If necessary define more anchor types.
        if (_anchorType == 2)
        {
            _area.top = window->getSize().y - _texture.getSize().y;
            _area.height = window->getSize().y;
            _sprite.setPosition(0.0, _area.top);
        }
        if (_anchorType == 3)
        {
            _area.width = window->getSize().x - _area.left;
            _area.left = _area.left + _area.width - _width;
            _area.top = window->getSize().y - static_cast<unsigned>(_height);
            _area.height = _area.top - window->getSize().y;
            _sprite.setPosition(_area.left, _area.top);
        }
    }
}

void Button::Show(sf::RenderWindow* window) const
{
    window->draw(_sprite);
}

void Button::SetAnchorage(const bool top, const bool left, const bool bottom, const bool right)
{
    if (top && left) _anchorType = 1;
    if (bottom && left) _anchorType = 2;
    if (bottom && right) _anchorType = 3;
    if (top && right) _anchorType = 4;
    if (_anchorType != 0) _anchorType = true;
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
