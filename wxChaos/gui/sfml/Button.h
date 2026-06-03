#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

enum AnchorType
{
    None,
    TopLeft,
    BottomLeft,
    BottomRight,
    TopRight
};

/**
 * @class Button
 * @brief Simple clickable SFML sprite used as an overlay control.
 *
 * Button owns a single texture-backed sprite, tracks its clickable area, and
 * can keep the sprite anchored to one corner of a render window while it is
 * resized.
 */
class Button {
protected:
    /// Width of the button texture in pixels.
    unsigned int _width;

    /// Height of the button texture in pixels.
    unsigned int _height;

    /// Current pressed/toggled state.
    bool _pressed;

    /// Relative X/Y position used by non-anchored buttons during resize.
    double _fx, _fy;

    /// Distance from the left edge used by left-anchored buttons.
    double _leftMargin;

    /// Distance from the top edge used by top-anchored buttons.
    double _topMargin;

    /// Distance from the right edge used by right-anchored buttons.
    double _rightMargin;

    /// Distance from the bottom edge used by bottom-anchored buttons.
    double _bottomMargin;

    /// Anchor corner: 0 none, 1 top-left, 2 bottom-left, 3 bottom-right, 4 top-right.
    AnchorType _anchorType;

    /// Source image loaded from disk.
    sf::Image _textureImage;

    /// Texture used by the sprite.
    sf::Texture _texture;

    /// Sprite drawn to the render window.
    sf::Sprite _sprite;

    /// Current clickable bounds of the sprite.
    sf::FloatRect _area;

public:
    /// @brief Default virtual destructor.
    virtual ~Button() = default;

    /// @brief Updates the sprite position and clickable area after a window resize.
    /// @param window Render window that owns the button.
    void Resize(const sf::RenderWindow* window);

    /// @brief Creates a button from an image file.
    /// @param textureImagePath Image path for the button texture.
    /// @param positionX Initial X position, or horizontal margin when anchored.
    /// @param positionY Initial Y position, or vertical margin when anchored.
    /// @param window Render window used to calculate initial placement.
    Button(const std::string& textureImagePath, int positionX, int positionY, const sf::RenderWindow* window);

    /// @brief Draws the button sprite.
    /// @param window Target render window.
    void Show(sf::RenderWindow* window) const;

    /// @brief Anchors the button to one render-window corner.
    /// @param top True when anchoring to the top edge.
    /// @param left True when anchoring to the left edge.
    /// @param bottom True when anchoring to the bottom edge.
    /// @param right True when anchoring to the right edge.
    void SetAnchor(bool top, bool left, bool bottom, bool right);

    /// @brief Toggles the pressed state.
    void ChangeState();

    /// @brief Handles SFML mouse events for the button.
    /// @param event SFML event to process.
    /// @return True if the event clicked the button.
    virtual bool HandleEvents(sf::Event event);

    /// @brief Handles wxWidgets mouse events for compatibility with wx-backed canvases.
    /// @param event wxWidgets mouse event to process.
    /// @return True if the event clicked the button.
    virtual bool ClickEvent(wxMouseEvent& event);
};
