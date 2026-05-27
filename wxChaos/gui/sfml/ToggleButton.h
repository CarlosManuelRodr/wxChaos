#pragma once
#include "Button.h"

/**
 * @class ToggleButton
 * @brief Simple SFML toggle button with two states.
 */
class ToggleButton : public Button {
    sf::Image _textureImage2;
    sf::Texture _texture2;
public:
    ToggleButton(const std::string& path1, const std::string& path2, int posX, int posY, const sf::RenderWindow* window);
    bool HandleEvents(sf::Event event) override;
    bool ClickEvent(wxMouseEvent& event) override;
    void Reset();
};
