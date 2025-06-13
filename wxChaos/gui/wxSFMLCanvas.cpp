#include "wxSFMLCanvas.h"

#ifdef __WXGTK__
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#endif

// Event table.
BEGIN_EVENT_TABLE(wxSFMLCanvas, wxControl)
EVT_IDLE(wxSFMLCanvas::OnIdle)
EVT_PAINT(wxSFMLCanvas::OnPaint)
EVT_ERASE_BACKGROUND(wxSFMLCanvas::OnEraseBackground)
END_EVENT_TABLE()


wxSFMLCanvas::wxSFMLCanvas(wxWindow* Parent, wxWindowID Id, const wxPoint& Position, const wxSize& Size, long Style) :
    wxControl(Parent, Id, Position, Size, Style)
{
#ifdef __WXGTK__
    // GTK implementation requires to go deeper to get the
    // underlying X11 window ID
    gtk_widget_realize(GetHandle());
    gtk_widget_set_double_buffered(GetHandle(), false);
    GdkWindow* Win = gtk_widget_get_window(GetHandle());
    XFlush(GDK_WINDOW_XDISPLAY(Win));
    create(GDK_WINDOW_XID(Win));
#else
    // Windows implementation is straightforward
    create(GetHandle());
#endif
}

wxSFMLCanvas::~wxSFMLCanvas() {}
void wxSFMLCanvas::OnUpdate() {}
void wxSFMLCanvas::OnEraseBackground(wxEraseEvent&) {}

void wxSFMLCanvas::OnIdle(wxIdleEvent&)
{
    // Send a paint message when the control is idle, to ensure maximum framerate
    Refresh();
}

void wxSFMLCanvas::OnPaint(wxPaintEvent&)
{
    // Prepare the control to be repainted
    wxPaintDC Dc(this);

    // Let the derived class do its specific stuff
    OnUpdate();

    // Display on screen
    display();
}