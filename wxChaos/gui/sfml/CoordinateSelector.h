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
    int _x, _y;
    unsigned int _screenWidth;
    unsigned int _screenHeight;
    sf::Image _textureImage;
    sf::Texture _texture;
    sf::Sprite _output;
    sf::Color _color;
    bool _rendered;
    bool _inSelection;

    void Render();
    void SetPosition(int newX, int newY);
    static int ClampCoordinate(int value, unsigned int size);

public:
    explicit CoordinateSelector(const sf::RenderWindow* window);
    void Show(sf::RenderWindow* window);
    void Resize(const sf::RenderWindow* window);
    bool HandleEvents(const sf::Event& event);

    // WX events
    bool ClickEvent(wxMouseEvent& event);
    void ReleaseClickEvent(wxMouseEvent& event);
    bool MoveEvent(wxMouseEvent& event);

    double GetX(const Fractal* target) const;
    double GetY(const Fractal* target) const;
    void AdjustPosition(const Fractal* target, double numX, double numY);
};
