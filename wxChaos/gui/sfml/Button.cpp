#include "Button.h"

Button::Button(const std::string& path, const int posX, const int posY, const sf::RenderWindow* window)
{
    const double screenWidth = window->getSize().x;
    const double screenHeight = window->getSize().y;
    _textureImage.loadFromFile(path);
    _texture.loadFromImage(_textureImage);
    _sprite.setTexture(_texture);
    _sprite.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    _width = _texture.getSize().x;
    _height = _texture.getSize().y;
    _area = _sprite.getGlobalBounds();
    _pressed = false;
    _fy = screenHeight > 0 ? _area.top/screenHeight : 0;
    _fx = screenWidth > 0 ? _area.left/screenWidth : 0;
    _leftMargin = _area.left;
    _topMargin = _area.top;
    _rightMargin = screenWidth - _area.left - _area.width;
    _bottomMargin = screenHeight - _area.top - _area.height;
    _anchorType = 0;
}

void Button::Resize(const sf::RenderWindow* window)
{
    if (_anchorType == 0)
    {
        _sprite.setPosition(static_cast<float>(_fx * window->getSize().x),
                            static_cast<float>(_fy * window->getSize().y));
    }
    else
    {
        if (window->getSize().x == 0 || window->getSize().y == 0)
        {
            _area = _sprite.getGlobalBounds();
            return;
        }

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
    {
        _anchorType = 1;
        _leftMargin = _area.left;
        _topMargin = _area.top;
    }
    else if (bottom && left)
    {
        _anchorType = 2;
        _leftMargin = _area.left;
        _bottomMargin = _area.top;
    }
    else if (bottom && right)
    {
        _anchorType = 3;
        _rightMargin = _area.left;
        _bottomMargin = _area.top;
    }
    else if (top && right)
    {
        _anchorType = 4;
        _rightMargin = _area.left;
        _topMargin = _area.top;
    }
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
