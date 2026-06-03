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
            const auto mousePositionX = static_cast<float>(event.mouseButton.x);
            const auto mousePositionY = static_cast<float>(event.mouseButton.y);
            if (mousePositionX >= _area.left && mousePositionX <= _area.left + _area.width)
            {
                if (mousePositionY >= _area.top && mousePositionY <= _area.top + _area.height)
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
