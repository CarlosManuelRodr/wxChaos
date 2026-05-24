/** 
* @file wxSFMLCanvas.h 
* @brief Integrates SFML graphics into a wxWidgets interface.
*
* @author Laurent Gomila
*
* @date 7/19/2012
*/

#pragma once
#ifndef WX_SFML_CANVAS_HPP
#define WX_SFML_CANVAS_HPP

#include <wx/wx.h>
#include <SFML/Graphics.hpp>

/**
* @class wxSFMLCanvas
* @brief Binding class between SFML and wxWidgets.
*/
class wxSFMLCanvas : public wxControl, public sf::RenderWindow
{
public:
    explicit wxSFMLCanvas(wxWindow* parent = nullptr, wxWindowID id = -1, const wxPoint& position = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize, long style = 0);
    ~wxSFMLCanvas() override;

private:
    DECLARE_EVENT_TABLE()

    virtual void OnUpdate();
    void OnIdle(wxIdleEvent&);
    void OnPaint(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&);
};

#endif