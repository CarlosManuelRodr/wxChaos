#pragma once
#include "Button.h"

/**
 * @class ToggleButton
 * @brief Simple SFML toggle button with two states.
 */
class ToggleButton : public Button {
    sf::Image _toggledTextureImage;
    sf::Texture _toggledTexture;
public:
    ToggleButton(const std::string& textureImagePath, const std::string& toggledTextureImagePath, int positionX, int positionY, const sf::RenderWindow* window);
    bool HandleEvents(sf::Event event) override;
    bool ClickEvent(wxMouseEvent& event) override;
    void Reset();
};
