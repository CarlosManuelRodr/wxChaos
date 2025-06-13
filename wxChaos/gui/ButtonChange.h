#pragma once
#include "Button.h"

class ButtonChange : public Button {
    sf::Image textureImage2;
    sf::Texture texture2;
public:
    ButtonChange(std::string Path1, std::string Path2, int posX, int posY, sf::RenderWindow* Window);
    bool HandleEvents(sf::Event Event);
    bool ClickEvent(wxMouseEvent& event);
    void Reset();
};
