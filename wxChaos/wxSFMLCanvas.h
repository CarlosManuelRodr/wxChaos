/** 
* @file wxSFMLCanvas.h 
* @brief Integrates SFML graphics into a wxWidgets interface.
*
* @author Laurent Gomila
*
* @date 7/19/2012
*/

#pragma once
#ifndef WXSFMLCANVAS_HPP
#define WXSFMLCANVAS_HPP

#include <wx/wx.h>
#include <SFML/Graphics.hpp>

/**
* @class wxSFMLCanvas
* @brief Binding class between SFML and wxWidgets.
*/
class wxSFMLCanvas : public wxControl, public sf::RenderWindow
{
public :
    wxSFMLCanvas(wxWindow* Parent = NULL, wxWindowID Id = -1, const wxPoint& Position = wxDefaultPosition, const wxSize& Size = wxDefaultSize, long Style = 0);
    virtual ~wxSFMLCanvas();

private :

    DECLARE_EVENT_TABLE()

    virtual void OnUpdate();
    void OnIdle(wxIdleEvent&);
    void OnPaint(wxPaintEvent&);
    void OnEraseBackground(wxEraseEvent&);
};


#endif