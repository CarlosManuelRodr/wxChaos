#pragma once
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

class SelectRect {
    sf::IntRect pos;
    sf::IntRect select;
    sf::RectangleShape output;
    int x, y;
    int xSelect, ySelect;
    sf::Color color;
    bool inSelection;
public:
    SelectRect(sf::RenderWindow* Window);
    void Show(sf::RenderWindow* Window);
    bool HandleEvents(sf::Event Event);
    void ClickEvent(wxMouseEvent& event);
    bool UnClickEvent(wxMouseEvent& event);
    void MoveEvent(wxMouseEvent& event);
    sf::IntRect GetSeleccion();
};
