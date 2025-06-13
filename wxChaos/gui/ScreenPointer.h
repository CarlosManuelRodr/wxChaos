#pragma once
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

class Fractal; // forward declaration

/**
 * @class ScreenPointer
 * @brief A selection cross to point to a number. Used in Slider, JuliaMode, and OrbitMode.
 */
class ScreenPointer
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
    explicit ScreenPointer(sf::RenderWindow* Window);
    void Show(sf::RenderWindow* Window);
    void Resize(sf::RenderWindow* Window);
    bool HandleEvents(sf::Event Event);

    // WX events
    bool ClickEvent(wxMouseEvent& event);
    void UnClickEvent(wxMouseEvent& event);
    bool MoveEvent(wxMouseEvent& event);

    double GetX(Fractal* target);
    double GetY(Fractal* target);
    void AdjustPosition(Fractal* target, double numX, double numY);
};
