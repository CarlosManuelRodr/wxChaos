#include "wxSFMLCanvas.h"

// Event table.
BEGIN_EVENT_TABLE(wxSFMLCanvas, wxControl)
EVT_IDLE(wxSFMLCanvas::OnIdle)
EVT_PAINT(wxSFMLCanvas::OnPaint)
EVT_ERASE_BACKGROUND(wxSFMLCanvas::OnEraseBackground)
END_EVENT_TABLE()

wxSFMLCanvas::wxSFMLCanvas(wxWindow* Parent, wxWindowID Id, const wxPoint& Position, const wxSize& Size, long Style) :
    wxControl(Parent, Id, Position, Size, Style)
{
    Window::create(wxWindow::GetHandle());
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