#pragma once
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

class Fractal; // forward declaration

/**
 * @class CoordinateSelector
 * @brief A selection cross to point to a coordinate on the rendering plane.
 */
class CoordinateSelector
{
    int x, y;
    unsigned int screenWidth;
    unsigned int screenHeight;
    sf::Image textureImage;
    sf::Texture texture;
    sf::Sprite output;
    sf::Color color;
    bool rendered;
    bool inSelection;

    void Render();

public:
    explicit CoordinateSelector(const sf::RenderWindow* window);
    void Show(sf::RenderWindow* window);
    void Resize(const sf::RenderWindow* window);
    bool HandleEvents(const sf::Event& event);

    // WX events
    bool ClickEvent(wxMouseEvent& event);
    void UnClickEvent(wxMouseEvent& event);
    bool MoveEvent(wxMouseEvent& event);

    double GetX(const Fractal* target) const;
    double GetY(const Fractal* target) const;
    void AdjustPosition(const Fractal* target, double numX, double numY);
};
