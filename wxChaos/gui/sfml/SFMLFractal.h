#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Fractal;

class SFMLFractal
{
    Fractal* _fractal;
    sf::Font _font;
    sf::Text _text;
    sf::Image _image;
    sf::Texture _texture;
    sf::Sprite _output;
    std::vector<sf::Image> _imgVector;
    sf::Image _tempImage;
    sf::Texture _tempTexture;
    sf::Sprite _tempSprite;
    sf::Image _geomImage;
    sf::Texture _geomTexture;
    sf::Sprite _outGeom;
    bool _imgInVector{};
    bool _usingRenderImage{};
    bool _zoomingBack{};
    bool _dontDrawTempImage{};

    void DrawMaps(sf::RenderWindow* window);
    void DrawGeometry(sf::RenderWindow* window);
    void EnsureFontLoaded();
    void ResetDisplayImages();

public:
    SFMLFractal();
    explicit SFMLFractal(Fractal* fractal);

    void SetFractal(Fractal* fractal);
    Fractal* GetFractal() const;

    void HandleEvent(const sf::Event& event) const;
    void Resize(const sf::RenderWindow* window);
    void Show(sf::RenderWindow* window);
    void SetAreaOfView(const sf::Rect<int>& pixelCoordinates);
    void ZoomBack();
    void Redraw();
};
