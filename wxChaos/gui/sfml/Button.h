#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

class Button {
protected:
    sf::Image _textureImage;
    sf::Texture _texture;
    sf::Sprite _sprite;
    sf::FloatRect _area;
    bool _pressed;
    double _screenWidth;
    double _screenHeight;
    double _fx, _fy;
    bool _anchorage;
    int _anchorType;

    int _width;
    int _height;

public:
    virtual ~Button() = default;
    void Resize(const sf::RenderWindow* window);
    Button(const std::string& path, int posX, int posY, const sf::RenderWindow* window);
    void Show(sf::RenderWindow* window) const;
    void SetAnchorage(bool top, bool left, bool bottom, bool right);
    void ChangeState();
    virtual bool HandleEvents(sf::Event Event);
    virtual bool ClickEvent(wxMouseEvent& event);
};
