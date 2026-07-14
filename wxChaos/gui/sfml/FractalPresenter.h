#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "coloring/ColorPaletteTypes.h"
#include "geometry/Rect.h"
#include "geometry/Vector2Int.h"
#include "types/Direction.h"
#include "types/RenderingAlgorithmType.h"
#include "wx/common/wxGradient.h"
#include "Fractal.h"

/**
* @class FractalPresenter
* @brief Owns the SFML presentation state for a Fractal.
*
* FractalPresenter is the bridge between the pure fractal renderer and SFML drawing.
* It keeps textures, sprites, temporary zoom images, cached zoom-back images,
* geometry overlays, and SFML event handling out of the Fractal base class.
*/
class FractalPresenter
{
    struct ZoomHistoryEntry
    {
        PreciseRect view;
        std::optional<sf::Image> image;
        unsigned int iterations{};
        bool imageComplete{};
    };

    Fractal* _fractal;                          ///< Fractal model currently being displayed.
    sf::Image _image;                           ///< Current rendered fractal image.
    sf::Texture _texture;                       ///< Texture backing the rendered fractal sprite.
    sf::Sprite _output;                         ///< Sprite used to draw the rendered fractal image.
    sf::Image _tempImage;                       ///< Temporary image shown while a new zoom render is running.
    sf::Texture _tempTexture;                   ///< Texture backing the temporary zoom image.
    sf::Sprite _tempSprite;                     ///< Sprite used to draw the temporary zoom image.
    sf::Image _geomImage;                       ///< Image layer for orbit and geometry overlays.
    sf::Texture _geomTexture;                   ///< Texture backing the geometry overlay.
    sf::Sprite _outGeom;                        ///< Sprite used to draw the geometry overlay.
    std::vector<ZoomHistoryEntry> _zoomHistory; ///< World-coordinate views and optional rendered images for zoom-back.
    PreciseRect _outermostZoom;                 ///< Farthest world-coordinate view reached by zoom-back.
    bool _movement[4]{};                        ///< Active keyboard movement state.
    double _xVel;
    double _yVel;
    double _panRemainderX;
    double _panRemainderY;
    int _posX;
    int _posY;
    Vector2Int _committedPanOffset;             ///< Settled pan offset waiting for map reuse.
    bool _hasCommittedPanOffset;
    bool _usingRenderImage;                     ///< True when the current frame came from a cached zoom-back image.
    bool _zoomingBack;                          ///< True while the view is being redrawn after zooming back.
    bool _dontDrawTempImage;                    ///< Suppresses drawing the temporary image layer when it would be stale.
    bool _setHandleRightClickZoomBack;          ///< True when SFML right-click events should zoom back.
    bool _mousePanning;                         ///< True while a direct mouse pan gesture is active.
    bool _automaticIterations;                  ///< True when iterations follow the current zoom level.
    unsigned int _automaticIterationBase;        ///< Minimum iteration count used by automatic mode.
    double _mouseWheelZoomScale;                ///< Fraction of the current view kept for each wheel zoom.
    double _zoomAnimationDurationSeconds;        ///< Seconds used by the temporary zoom preview easing.
    bool _zoomAnimationActive;                  ///< True while the temporary zoom preview is easing into place.
    double _zoomAnimationElapsed;               ///< Seconds elapsed in the active zoom animation.
    sf::Vector2f _zoomAnimationStartPosition;   ///< Starting sprite position for the zoom preview.
    sf::Vector2f _zoomAnimationTargetPosition;  ///< Final sprite position for the zoom preview.
    sf::Vector2f _zoomAnimationStartScale;      ///< Starting sprite scale for the zoom preview.
    sf::Vector2f _zoomAnimationTargetScale;     ///< Final sprite scale for the zoom preview.
    bool _interactiveZoomActive;                ///< True while the zoom tool is resizing the temporary image.
    int _interactiveZoomAnchorX;                ///< Pixel anchor for the active zoom tool drag.
    int _interactiveZoomAnchorY;                ///< Pixel anchor for the active zoom tool drag.

    ///@brief Draws fractal maps into the SFML image and then draws the output sprite.
    ///@param window Target window.
    void DrawMaps(sf::RenderWindow* window);

    ///@brief Draws orbit lines, normal lines, and circles over the fractal.
    ///@param window Target window.
    void DrawGeometry(sf::RenderWindow* window) const;

    ///@brief Recreates all SFML image, texture, and sprite objects for the current fractal size.
    void ResetDisplayImages();

    ///@brief Clears cached images used for zoom-back.
    void ClearImageCache();

    ///@brief Resets panning input, velocity, and pending render offset.
    void ResetMovement();

    ///@brief Returns the fractal's current world-coordinate view.
    PreciseRect CaptureCurrentView() const;

    ///@brief Applies a world-coordinate view to the fractal.
    void ApplyView(const PreciseRect& view);

    ///@brief Returns the default world-coordinate view for the current fractal type at the given size.
    PreciseRect GetDefaultViewForSize(unsigned int width, unsigned int height) const;

    ///@brief Estimates the iteration count that fits the current viewport.
    unsigned int CalculateAutomaticIterations() const;

    ///@brief Estimates viewport-driven iterations added on top of the user's manual baseline.
    unsigned int CalculateAutomaticIterationExtra() const;

    ///@brief Returns the safe viewport-driven iteration increase for the active rendering model.
    unsigned int GetMaximumAutomaticIterationExtra() const;

    ///@brief Updates the automatic baseline so the current viewport uses the requested count.
    void SetAutomaticIterationBaseForCurrentIterations(unsigned int iterations);

    ///@brief Applies automatic iterations when that mode is active.
    void ApplyAutomaticIterations();

    ///@brief Saves the current view and the best available image for zoom-back.
    void SaveZoom(std::optional<sf::Image> image, bool imageComplete);

    ///@brief Replaces the temporary preview and resets its sprite to full-canvas coordinates.
    void SetTemporaryPreviewImage(const sf::Image& image, bool drawPreview);

    ///@brief Clears the zoom-back state and captures the current view as the outermost zoom.
    void ResetZoomHistory();

    ///@brief Expands the current view when zoom-back has no saved view.
    void ExpandCurrentView();

    ///@brief Calculates the preview source rectangle for an anchored zoom.
    sf::Rect<int> GetPixelZoomRect(int pixelX, int pixelY, double scale) const;

    ///@brief Calculates the target world view for an anchored zoom.
    PreciseRect GetPixelZoomView(int pixelX, int pixelY, double scale) const;

    ///@brief Calculates where one world-coordinate view appears inside another in screen pixels.
    sf::Rect<int> GetViewRectInsideView(const PreciseRect& innerView, const PreciseRect& outerView) const;

    ///@brief Applies the sprite transform that previews an anchored zoom scale.
    void ApplyZoomPreviewTransform(int pixelX, int pixelY, double scale);

    ///@brief Zooms into a pixel rectangle and applies the supplied world-coordinate target view.
    void SetAreaOfView(const sf::Rect<int>& pixelCoordinates, const PreciseRect& targetView);

    ///@brief Captures the best current preview, including any visible partial render pixels.
    sf::Image CapturePreviewImage() const;

    ///@brief Starts the temporary image easing from the selected source rectangle to the full canvas.
    void StartZoomAnimation(const sf::Rect<int>& pixelCoordinates);

    ///@brief Starts the temporary image easing from full canvas into a target rectangle.
    void StartZoomBackAnimation(const sf::Rect<int>& targetCoordinates);

    ///@brief Advances the temporary zoom preview animation.
    ///@return True on the frame where the preview reaches the target viewport.
    bool UpdateZoomAnimation(double elapsedSeconds);

    ///@brief Finishes any active temporary zoom animation.
    void StopZoomAnimation();

public:
    ///@brief Constructs an SFML fractal presenter bound to a fractal.
    ///@param fractal Fractal model to present.
    explicit FractalPresenter(Fractal* fractal);

    ///@brief Changes the fractal model shown by this presenter.
    ///@param fractal New fractal model.
    void SetFractal(Fractal* fractal);

    ///@brief Gets the fractal model currently shown by this presenter.
    ///@return Pointer to the current fractal model.
    Fractal* GetFractal() const;

    ///@brief Sets whether SFML right-click events should zoom back.
    ///@param mode True to handle right-click zoom-back in SFML.
    void SetHandleRightClickZoomBack(bool mode);

    ///@brief Sets mouse-wheel zoom behavior.
    ///@param zoomStepPercent Percent zoomed in by one wheel notch.
    ///@param inertiaMilliseconds Preview easing duration in milliseconds.
    void SetZoomOptions(int zoomStepPercent, int inertiaMilliseconds);

    ///@brief Returns the fraction of the current view kept for one-wheel zoom.
    double GetMouseWheelZoomScale() const;

    ///@brief Handles SFML input events that affect the fractal view.
    ///@param event SFML event to process.
    void HandleEvent(const sf::Event& event);

    ///@brief Updates inertial panning state for the elapsed time.
    ///@param elapsedSeconds Seconds elapsed since the previous movement update.
    void Move(double elapsedSeconds);

    ///@brief Returns true while the fractal is actively panning.
    bool IsMoving() const;

    ///@brief Returns true while a zoom preview or animation is active.
    bool IsZoomPreviewActive() const;

    ///@brief Starts movement in the given direction.
    ///@param direction Direction to activate.
    void SetMovement(Direction direction);

    ///@brief Stops movement in the given direction.
    ///@param direction Direction to deactivate.
    void ReleaseMovement(Direction direction);

    ///@brief Starts direct mouse panning without keyboard smoothing.
    void BeginMousePan();

    ///@brief Pans the fractal view by an exact mouse-drag delta.
    ///@param pixelDeltaX Horizontal drag delta in pixels.
    ///@param pixelDeltaY Vertical drag delta in pixels.
    void PanByMousePixels(int pixelDeltaX, int pixelDeltaY);

    ///@brief Finishes direct mouse panning and commits the shifted render.
    void EndMousePan();

    ///@brief Resizes the fractal maps and SFML presentation layers to match the window.
    ///@param window Window whose size will be copied.
    void Resize(const sf::RenderWindow* window);

    ///@brief Updates render state and draws the fractal presentation to the window.
    ///@param window Target window.
    ///@param elapsedSeconds Seconds elapsed since the previous presentation update.
    void Show(sf::RenderWindow* window, double elapsedSeconds);

    ///@brief Zooms into the selected pixel rectangle and prepares the temporary zoom preview.
    ///@param pixelCoordinates Selection rectangle in pixel coordinates.
    void SetAreaOfView(const sf::Rect<int>& pixelCoordinates);

    ///@brief Zooms around a pixel, preserving the world coordinate under that pixel.
    void ZoomAtPixel(int pixelX, int pixelY);

    ///@brief Zooms around a pixel by the supplied view scale.
    void ZoomAtPixel(int pixelX, int pixelY, double scale);

    ///@brief Starts a live zoom-tool preview around a pixel.
    bool BeginInteractiveZoomAtPixel(int pixelX, int pixelY);

    ///@brief Updates the live zoom-tool preview without changing the fractal view.
    void UpdateInteractiveZoom(double scale);

    ///@brief Applies the live zoom-tool preview and starts rendering the target view.
    void CommitInteractiveZoom(double scale);

    ///@brief Cancels the live zoom-tool preview without changing the fractal view.
    void CancelInteractiveZoom();

    ///@brief Restores the previous zoom level, using a cached image when possible.
    void ZoomBack();

    ///@brief Return the farthest zoom viewed by the user.
    Rect GetOutermostZoom() const;

    ///@brief Return the farthest zoom viewed by the user without losing precision.
    PreciseRect GetPreciseOutermostZoom() const;

    ///@brief Return the current zoom rect.
    Rect GetCurrentZoom() const;

    ///@brief Returns true when the current view differs from the outermost zoom.
    bool HasZoomed() const;

    ///@brief Forces a redrawing and clears cached zoom-back images.
    void Redraw();

    ///@brief Replaces the current world-coordinate view and resets zoom history.
    void SetView(const Rect& view);

    ///@brief Increases the iteration count and invalidates cached images.
    void IncreaseIterations();

    ///@brief Decreases the iteration count and invalidates cached images.
    void DecreaseIterations();

    ///@brief Sets the iteration count and invalidates cached images.
    ///@param iterations New maximum iteration count.
    void ChangeIterations(unsigned int iterations);

    ///@brief Enables or disables viewport-driven iteration counts.
    void SetAutomaticIterations(bool mode);

    ///@brief Returns true when iterations are controlled by the viewport.
    bool AutomaticIterationsEnabled() const;

    ///@brief Sets the Julia constant and invalidates cached images.
    ///@param real Real component.
    ///@param imaginary Imaginary component.
    void SetK(double real, double imaginary);

    ///@brief Sets the color gradient and invalidates cached images.
    ///@param gradient New gradient.
    void SetGradient(const wxGradient& gradient);

    ///@brief Sets the gradient palette size and invalidates cached images.
    ///@param size New palette size.
    void SetGradientSize(unsigned int size);

    ///@brief Sets the renderer-value length used for one full palette cycle.
    ///@param cycleLength New cycle length.
    void SetColorCycleLength(double cycleLength);

    ///@brief Sets animated color rotation speed in palette units per second.
    ///@param speed New color rotation speed.
    void SetColorRotationSpeed(double speed) const;

    ///@brief Sets how renderer values are mapped into the active palette.
    void SetPaletteMappingMode(PaletteMappingMode mode);

    ///@brief Sets the exponent used by exponential palette mapping.
    void SetPaletteMappingExponent(double exponent);

    ///@brief Sets the color palette style.
    ///@param palette New palette style.
    void SetColorPalette(ColorPaletteTypes palette);

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
    void ToggleColorRotation();

    ///@brief Sets the color variation offset and invalidates cached images.
    ///@param offset Color variation offset.
    void SetColorVariationOffset(unsigned int offset);

    ///@brief Sets the rendering algorithm and invalidates cached images.
    ///@param algorithm New rendering algorithm.
    void SetAlgorithm(RenderingAlgorithmType algorithm);

    /// @brief Applies a rendering algorithm and related coloring flags when supported.
    /// @param algorithm Rendering algorithm to select.
    /// @param smoothRender Enables smooth escape-time coloring.
    /// @param orbitTrap Enables orbit-trap coloring.
    /// @return true when the current fractal supports the requested rendering options.
    bool SetRenderingOptions(RenderingAlgorithmType algorithm, bool smoothRender, bool orbitTrap);

    ///@brief Sets the rendering precision strategy and invalidates cached images.
    ///@param mode New rendering precision mode.
    void SetRenderingPrecisionMode(RenderingPrecisionMode mode);
    void SetAntiAliasingScale(unsigned int scale);

    ///@brief Sets orbit trap mode and invalidates cached images.
    ///@param mode Orbit trap mode.
    void SetOrbitTrapMode(bool mode);

    ///@brief Sets smooth render mode and invalidates cached images.
    ///@param mode Smooth render mode.
    void SetSmoothRender(bool mode);
};
