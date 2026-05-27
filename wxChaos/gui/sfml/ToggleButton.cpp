#include "ToggleButton.h"

ToggleButton::ToggleButton(const std::string& path1, const std::string& path2, const int posX, const int posY,
                           sf::RenderWindow* window) : Button(path1, posX, posY, window)
{
    textureImage2.loadFromFile(path2);
    texture2.loadFromImage(textureImage2);
}

bool ToggleButton::HandleEvents(const sf::Event event)
{
    return Button::HandleEvents(event);
}

bool ToggleButton::ClickEvent(wxMouseEvent& event)
{
    return Button::ClickEvent(event);
}

void ToggleButton::Reset()
{
    _texture.loadFromImage(_textureImage);
}
