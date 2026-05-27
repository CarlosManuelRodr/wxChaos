#include "ToggleButton.h"

ToggleButton::ToggleButton(const std::string& path1, const std::string& path2, const int posX, const int posY,
                           const sf::RenderWindow* window) : Button(path1, posX, posY, window)
{
    _textureImage2.loadFromFile(path2);
    _texture2.loadFromImage(_textureImage2);
}

bool ToggleButton::HandleEvents(const sf::Event event)
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
                    _sprite.setTexture(_pressed ? _texture2 : _texture);
                    return true;
                }
            }
        }
    }
    return false;
}

bool ToggleButton::ClickEvent(wxMouseEvent& event)
{
    return Button::ClickEvent(event);
}

void ToggleButton::Reset()
{
    _texture.loadFromImage(_textureImage);
}
