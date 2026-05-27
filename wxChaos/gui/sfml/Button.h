#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

class Button {
protected:
    sf::Image _textureImage;
    sf::Texture _texture;
    sf::Sprite _sprite;
    sf::Font font;
    sf::Text _buttonText;
    sf::FloatRect _area;
    bool _pressed;
    double _screenWidth;
    double _screenHeight;
    double _fx, _fy;
    bool _anchorage;
    bool _thereIsText;
    int _anchorType;

    int _width;
    int _height;

public:
    virtual ~Button() = default;
    void Resize(const sf::RenderWindow* window);
    Button(const std::string& path, int posX, int posY, sf::RenderWindow* window);
    Button(int posX, int posY, sf::RenderWindow* window, std::string text);
    void Show(sf::RenderWindow* window);
    void SetAnchorage(bool top, bool left, bool bottom, bool right);
    void ChangeState();
    virtual bool HandleEvents(sf::Event Event);
    virtual bool ClickEvent(wxMouseEvent& event);
};
