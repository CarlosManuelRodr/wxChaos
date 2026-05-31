#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "ColorPalettes.h"
#include "geometry/Rect.h"
#include "geometry/Vector2Int.h"
#include "types/Direction.h"
#include "types/RenderingAlgorithmType.h"
#include "wx/wxGradient.h"

class Fractal;

/**
* @class SFMLFractal
* @brief Owns the SFML presentation state for a Fractal.
*
* SFMLFractal is the bridge between the pure fractal renderer and SFML drawing.
* It keeps textures, sprites, temporary zoom images, cached zoom-back images,
* geometry overlays, and SFML event handling out of the Fractal base class.
*/
class SFMLFractal
{
    Fractal* _fractal;                   ///< Fractal model currently being displayed.
    sf::Font _font;                      ///< Font used by SFML text overlays.
    sf::Text _iterationsText;            ///< Iteration-count overlay text.
    sf::Image _image;                    ///< Current rendered fractal image.
    sf::Texture _texture;                ///< Texture backing the rendered fractal sprite.
    sf::Sprite _output;                  ///< Sprite used to draw the rendered fractal image.
    std::vector<sf::Image> _imgCache;    ///< Cached rendered images used when zooming back.
    sf::Image _tempImage;                ///< Temporary image shown while a new zoom render is running.
    sf::Texture _tempTexture;            ///< Texture backing the temporary zoom image.
    sf::Sprite _tempSprite;              ///< Sprite used to draw the temporary zoom image.
    sf::Image _geomImage;                ///< Image layer for orbit and geometry overlays.
    sf::Texture _geomTexture;            ///< Texture backing the geometry overlay.
    sf::Sprite _outGeom;                 ///< Sprite used to draw the geometry overlay.
    sf::RectangleShape _iterationsOverlay; ///< Background shape for the iteration-count overlay.
    bool _movement[4]{};                 ///< Active keyboard movement state.
    int _xVel;
    int _yVel;
    int _posX;
    int _posY;
    Vector2Int _committedPanOffset;      ///< Settled pan offset waiting for map reuse.
    bool _hasCommittedPanOffset;
    bool _changeFractalIter;
    bool _imgInVector;                   ///< True when there are cached images available for zoom-back.
    bool _usingRenderImage;              ///< True when the current frame came from a cached zoom-back image.
    bool _zoomingBack;                   ///< True while the view is being redrawn after zooming back.
    bool _dontDrawTempImage;             ///< Suppresses drawing the temporary image layer when it would be stale.

    ///@brief Draws fractal maps into the SFML image and then draws the output sprite.
    ///@param window Target window.
    void DrawMaps(sf::RenderWindow* window);

    ///@brief Draws orbit lines, normal lines, and circles over the fractal.
    ///@param window Target window.
    void DrawGeometry(sf::RenderWindow* window) const;

    ///@brief Loads the overlay font if it has not been loaded yet.
    void EnsureFontLoaded();

    ///@brief Recreates all SFML image, texture, and sprite objects for the current fractal size.
    void ResetDisplayImages();

    ///@brief Clears cached images used for zoom-back.
    void ClearImageCache();

    ///@brief Updates the iteration-count text and background from the measured rendered text bounds.
    void UpdateIterationsOverlay();

    ///@brief Resets panning input, velocity, and pending render offset.
    void ResetMovement();

    ///@brief Moves matrix elements and fills the exposed area with zero values.
    template<class M>
    void MoveMatrix(M** matrix, unsigned int matrixWidth, unsigned int matrixHeight, int moveX, int moveY);

    ///@brief Shifts the rendered maps after panning settles.
    void MoveMaps();

    ///@brief Returns the fractal's current world-coordinate view.
    Rect CaptureCurrentView() const;

    ///@brief Applies a world-coordinate view to the fractal.
    void ApplyView(const Rect& view);

    ///@brief Saves the current view for zoom-back.
    void SaveZoom();

    ///@brief Clears zoom-back state and captures the current view as the outermost zoom.
    void ResetZoomHistory();

    ///@brief Expands the current view when zoom-back has no saved view.
    void ExpandCurrentView();

public:
    ///@brief Constructs an SFML fractal presenter bound to a fractal.
    ///@param fractal Fractal model to present.
    explicit SFMLFractal(Fractal* fractal);

    ///@brief Changes the fractal model shown by this presenter.
    ///@param fractal New fractal model.
    void SetFractal(Fractal* fractal);

    ///@brief Gets the fractal model currently shown by this presenter.
    ///@return Pointer to the current fractal model.
    Fractal* GetFractal() const;

    ///@brief Handles SFML input events that affect the fractal view.
    ///@param event SFML event to process.
    void HandleEvent(const sf::Event& event);

    ///@brief Updates inertial panning state for one frame.
    void Move();

    ///@brief Returns true while the fractal is actively panning.
    bool IsMoving() const;

    ///@brief Starts movement in the given direction.
    ///@param direction Direction to activate.
    void SetMovement(Direction direction);

    ///@brief Stops movement in the given direction.
    ///@param direction Direction to deactivate.
    void ReleaseMovement(Direction direction);

    ///@brief Resizes the fractal maps and SFML presentation layers to match the window.
    ///@param window Window whose size will be copied.
    void Resize(const sf::RenderWindow* window);

    ///@brief Updates render state and draws the fractal presentation to the window.
    ///@param window Target window.
    void Show(sf::RenderWindow* window);

    ///@brief Zooms into the selected pixel rectangle and prepares the temporary zoom preview.
    ///@param pixelCoordinates Selection rectangle in pixel coordinates.
    void SetAreaOfView(const sf::Rect<int>& pixelCoordinates);

    ///@brief Restores the previous zoom level, using a cached image when possible.
    void ZoomBack();

    ///@brief Return the farthest zoom viewed by the user.
    Rect GetOutermostZoom() const;

    ///@brief Return the current zoom rect.
    Rect GetCurrentZoom() const;

    ///@brief Returns true when the current view differs from the outermost zoom.
    bool HasZoomed() const;

    ///@brief Forces a redrawing and clears cached zoom-back images.
    void Redraw();

    ///@brief Increases the iteration count and invalidates cached images.
    void IncreaseIterations();

    ///@brief Decreases the iteration count and invalidates cached images.
    void DecreaseIterations();

    ///@brief Sets the iteration count and invalidates cached images.
    ///@param iterations New maximum iteration count.
    void ChangeIterations(int iterations);

    ///@brief Sets the Julia constant and invalidates cached images.
    ///@param real Real component.
    ///@param imaginary Imaginary component.
    void SetK(double real, double imaginary);

    ///@brief Sets the gradient and invalidates cached images.
    ///@param gradient New gradient.
    void SetGradient(const wxGradient& gradient);

    ///@brief Sets the gradient palette size and invalidates cached images.
    ///@param size New palette size.
    void SetGradientSize(unsigned int size);

    ///@brief Sets the color palette style.
    ///@param palette New palette style.
    void SetColorPalette(ColorPalettes palette);

    ///@brief Sets whether exterior colors are drawn and invalidates cached images.
    ///@param mode Exterior color mode.
    void SetExteriorColorMode(bool mode);

    ///@brief Sets whether set colors are drawn and invalidates cached images.
    ///@param mode Set color mode.
    void SetFractalSetColorMode(bool mode);

    ///@brief Sets the fractal set color and invalidates cached images.
    ///@param color New set color.
    void SetFractalSetColor(sf::Color color);

    ///@brief Sets relative color mode and invalidates cached images.
    ///@param mode Relative color mode.
    void SetRelativeColor(bool mode);

    ///@brief Toggles animated gradient variation and invalidates cached images.
    void ChangeVarGradient();

    ///@brief Sets the color variation offset and invalidates cached images.
    ///@param offset Color variation offset.
    void SetVarGradient(int offset);

    ///@brief Sets the rendering algorithm and invalidates cached images.
    ///@param algorithm New rendering algorithm.
    void SetAlgorithm(RenderingAlgorithmType algorithm);

    ///@brief Sets orbit trap mode and invalidates cached images.
    ///@param mode Orbit trap mode.
    void SetOrbitTrapMode(bool mode);

    ///@brief Sets smooth render mode and invalidates cached images.
    ///@param mode Smooth render mode.
    void SetSmoothRender(bool mode);
};
