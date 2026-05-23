#pragma once

#include <SFML/Graphics.hpp>

class Fractal;

class SFMLFractal
{
public:
    SFMLFractal();
    explicit SFMLFractal(Fractal* fractal);

    void SetFractal(Fractal* fractal);
    Fractal* GetFractal() const;

    void HandleEvent(const sf::Event& event);
    void Resize(sf::RenderWindow* window);
    void Show(sf::RenderWindow* window);

private:
    Fractal* fractal;
    sf::Font font;
    sf::Text text;

    void DrawMaps(sf::RenderWindow* window);
    void DrawGeometry(sf::RenderWindow* window);
    void EnsureFontLoaded();
};
