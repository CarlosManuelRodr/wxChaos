#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

class Button {
protected:
    sf::Image textureImage;
    sf::Texture texture;
    sf::Sprite output;
    sf::Font font;
    sf::Text buttonText;
    sf::FloatRect area;
    bool pressed;
    double screenWidth;
    double screenHeight;
    double FX, FY;
    bool anchorage;
    bool thereIsText;
    int anchorType;

    int buttonWidth;
    int buttonHeight;

public:
    void Resize(sf::RenderWindow* Window);
    Button(std::string Path, int posX, int posY, sf::RenderWindow* Window);
    Button(int posX, int posY, sf::RenderWindow* Window, std::string text);
    void Show(sf::RenderWindow* Window);
    void SetAnchorage(bool Top, bool Left, bool Bottom, bool Right);
    void ChangeState();
    virtual bool HandleEvents(sf::Event Event);
    virtual bool ClickEvent(wxMouseEvent& event);
};
