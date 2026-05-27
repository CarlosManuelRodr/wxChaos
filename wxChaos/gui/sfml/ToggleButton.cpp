#include "ToggleButton.h"

ToggleButton::ToggleButton(const std::string& textureImagePath, const std::string& toggledTextureImagePath,
                           const int positionX, const int positionY, const sf::RenderWindow* window)
                           : Button(textureImagePath, positionX, positionY, window)
{
    _toggledTextureImage.loadFromFile(toggledTextureImagePath);
    _toggledTexture.loadFromImage(_toggledTextureImage);
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
                    _sprite.setTexture(_pressed ? _toggledTexture : _texture);
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
