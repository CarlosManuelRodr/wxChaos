#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Fractal;

class SFMLFractal
{
    Fractal* fractal;
    sf::Font font;
    sf::Text text;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite output;
    std::vector<sf::Image> imgVector;
    sf::Image tempImage;
    sf::Texture tempTexture;
    sf::Sprite tempSprite;
    sf::Image geomImage;
    sf::Texture geomTexture;
    sf::Sprite outGeom;
    bool imgInVector{};
    bool usingRenderImage{};
    bool zoomingBack{};
    bool dontDrawTempImage{};

    void DrawMaps(sf::RenderWindow* window);
    void DrawGeometry(sf::RenderWindow* window);
    void EnsureFontLoaded();
    void ResetDisplayImages();

public:
    SFMLFractal();
    explicit SFMLFractal(Fractal* fractal);

    void SetFractal(Fractal* fractal);
    Fractal* GetFractal() const;

    void HandleEvent(const sf::Event& event);
    void Resize(sf::RenderWindow* window);
    void Show(sf::RenderWindow* window);
    void SetAreaOfView(const sf::Rect<int>& pixelCoordinates);
    void ZoomBack();
    void Redraw();
};
