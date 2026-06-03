#pragma once
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

/**
* @class SelectionRect
* @brief A selection rectangle used to zoom into the fractal.
*/
class SelectionRect {
    int _x, _y;
    int _xSelect, _ySelect;
    bool _inSelection;
    sf::IntRect _pos;
    sf::IntRect _select;
    sf::RectangleShape _output;
    sf::Color _color;

public:
    explicit SelectionRect();
    void Show(sf::RenderWindow* window);
    bool HandleEvents(const sf::Event& event);
    void ClickEvent(wxMouseEvent& event);
    bool UnClickEvent(wxMouseEvent& event);
    void MoveEvent(wxMouseEvent& event);
    sf::IntRect GetSelection() const;
};
