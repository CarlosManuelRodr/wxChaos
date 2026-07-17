/** 
* @file wxSFMLCanvas.h 
* @brief Integrates SFML graphics into a wxWidgets interface.
*
* @author Laurent Gomila
*
* @date 7/19/2012
*/

#pragma once

#include <wx/wx.h>
#include <wx/timer.h>
#include <SFML/Graphics.hpp>

/**
* @class wxSFMLCanvas
* @brief wxControl wrapper around an SFML RenderWindow.
*
* wxSFMLCanvas creates the native SFML window from the wxWidgets handle and
* drives the virtual OnUpdate hook from idle and paint events so derived classes
* can render SFML content inside a wx layout.
*/
class wxSFMLCanvas : public wxControl, public sf::RenderWindow
{
public:
    explicit wxSFMLCanvas(wxWindow* parent = nullptr, wxWindowID id = -1, const wxPoint& position = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize, long style = 0);
    ~wxSFMLCanvas() override;

    /** @brief Sets the target canvas presentation frequency in hertz. */
    void SetTargetFrameRate(int frameRate);

protected:
    /// @brief Stops idle paint scheduling while the owning window is closing.
    void StopSfmlRefresh();

    /// @brief Creates the SFML RenderWindow once the wx control has a valid native size.
    /// @return true when the SFML RenderWindow is available.
    bool EnsureSfmlWindowCreated();

    /// @brief Reports whether the SFML RenderWindow has been created.
    /// @return true after EnsureSfmlWindowCreated succeeds.
    [[nodiscard]] bool IsSfmlWindowCreated() const;

    /// @brief Converts a wx mouse event position to the embedded SFML window's pixel coordinates.
    [[nodiscard]] wxPoint GetRenderMousePosition(const wxMouseEvent& event) const;

private:
    bool _sfmlWindowCreated{};
    bool _sfmlRefreshEnabled{true};
    wxTimer _frameTimer;

    virtual void OnUpdate();
    void OnFrameTimer(wxTimerEvent&);
    void OnPaintEvent(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&);
};
