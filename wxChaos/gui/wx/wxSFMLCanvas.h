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

private:
    virtual void OnUpdate();
    void OnIdle(wxIdleEvent&);
    void OnPaintEvent(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&);
};
