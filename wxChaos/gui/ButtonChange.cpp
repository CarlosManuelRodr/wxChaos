#include "ButtonChange.h"

ButtonChange::ButtonChange(std::string Path1, std::string Path2, int posX, int posY, sf::RenderWindow* Window)
    : Button(Path1, posX, posY, Window)
{
    textureImage2.loadFromFile(Path2);
    texture2.loadFromImage(textureImage2);
}

bool ButtonChange::HandleEvents(sf::Event Event)
{
    return Button::HandleEvents(Event);
}

bool ButtonChange::ClickEvent(wxMouseEvent& event)
{
    return Button::ClickEvent(event);
}

void ButtonChange::Reset()
{
    texture.loadFromImage(textureImage);
}
