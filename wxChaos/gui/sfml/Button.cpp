#include "Button.h"

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
    _leftMargin = _area.left;
    _topMargin = _area.top;
    _rightMargin = _screenWidth - _area.left - _area.width;
    _bottomMargin = _screenHeight - _area.top - _area.height;
    _anchorType = 0;
}

void Button::Resize(const sf::RenderWindow* window)
{
    if (!_anchorage)
    {
        _sprite.setPosition(static_cast<float>(_fx * window->getSize().x),
                            static_cast<float>(_fy * window->getSize().y));
    }
    else
    {
        float x = static_cast<float>(_leftMargin);
        float y = static_cast<float>(_topMargin);

        if (_anchorType == 2 || _anchorType == 3)
            y = static_cast<float>(window->getSize().y - _bottomMargin - _height);

        if (_anchorType == 3 || _anchorType == 4)
            x = static_cast<float>(window->getSize().x - _rightMargin - _width);

        _sprite.setPosition(x, y);
    }

    _area = _sprite.getGlobalBounds();
}

void Button::Show(sf::RenderWindow* window) const
{
    window->draw(_sprite);
}

void Button::SetAnchorage(const bool top, const bool left, const bool bottom, const bool right)
{
    _anchorType = 0;

    if (top && left)
        _anchorType = 1;
    else if (bottom && left)
        _anchorType = 2;
    else if (bottom && right)
        _anchorType = 3;
    else if (top && right)
        _anchorType = 4;

    _anchorage = _anchorType != 0;
}

void Button::ChangeState()
{
    _pressed = !_pressed;
}

bool Button::HandleEvents(const sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            if (event.mouseButton.x >= _area.left && event.mouseButton.x <= _area.left + _area.width)
            {
                if (event.mouseButton.y >= _area.top && event.mouseButton.y <= _area.top + _area.height)
                {
                    _pressed = !_pressed;
                    _sprite.setColor(_pressed ? sf::Color(0, 255, 0, 100) : sf::Color(255, 255, 255, 100));
                    return true;
                }
            }
        }
    }
    return false;
}

bool Button::ClickEvent(wxMouseEvent& event)
{
    return false;
}
