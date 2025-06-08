#include "wxSFMLCanvas.h"
#include "SFMLCompat.h"

#ifdef __WXGTK__
    #include <gdk/gdkx.h>
    #include <gtk/gtk.h>
    #include <wx/gtk/win_gtk.h>
#endif

// Event table.
BEGIN_EVENT_TABLE( wxSFMLCanvas, wxControl )
    EVT_IDLE( wxSFMLCanvas::OnIdle )
    EVT_PAINT( wxSFMLCanvas::OnPaint )
    EVT_ERASE_BACKGROUND( wxSFMLCanvas::OnEraseBackground )
END_EVENT_TABLE()


wxSFMLCanvas::wxSFMLCanvas(wxWindow* Parent, wxWindowID Id, const wxPoint& Position, const wxSize& Size, long Style) : 
    wxControl(Parent, Id, Position, Size, Style)
{
    #ifdef __WXGTK__
        gtk_widget_realize( m_wxwindow );
        gtk_widget_set_double_buffered( m_wxwindow, false );
        GdkWindow* Win = GTK_PIZZA( m_wxwindow )->bin_window;
        XFlush( GDK_WINDOW_XDISPLAY( Win ) );
        sf::RenderWindow::Create( GDK_WINDOW_XWINDOW( Win ) );
    #else
        sf::RenderWindow::Create(GetHandle());
    #endif
}

wxSFMLCanvas::~wxSFMLCanvas() {}
void wxSFMLCanvas::OnUpdate() {}
void wxSFMLCanvas::OnEraseBackground(wxEraseEvent&) {}

void wxSFMLCanvas::OnIdle(wxIdleEvent&)
{
    Refresh();
}

void wxSFMLCanvas::OnPaint(wxPaintEvent&)
{
    wxPaintDC Dc(this);
    OnUpdate();
    display();    // Draws SFML window.
}