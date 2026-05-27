#pragma once
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

/**
* @class SelectionRect
* @brief A selection rectangle used to zoom into the fractal.
*/
class SelectionRect {
    sf::IntRect pos;
    sf::IntRect select;
    sf::RectangleShape output;
    int x, y;
    int xSelect, ySelect;
    sf::Color color;
    bool inSelection;
public:
    explicit SelectionRect();
    void Show(sf::RenderWindow* window);
    bool HandleEvents(const sf::Event& event);
    void ClickEvent(wxMouseEvent& event);
    bool UnClickEvent(wxMouseEvent& event);
    void MoveEvent(wxMouseEvent& event);
    sf::IntRect GetSelection() const;
};
