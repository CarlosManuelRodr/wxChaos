#include "wxSFMLCanvas.h"

// Event table.
BEGIN_EVENT_TABLE(wxSFMLCanvas, wxControl)
EVT_IDLE(wxSFMLCanvas::OnIdle)
EVT_PAINT(wxSFMLCanvas::OnPaint)
EVT_ERASE_BACKGROUND(wxSFMLCanvas::OnEraseBackground)
END_EVENT_TABLE()

wxSFMLCanvas::wxSFMLCanvas(wxWindow* parent, const wxWindowID id, const wxPoint& position, const wxSize& size,
                           const long style) : wxControl(parent, id, position, size, style)
{
    Window::create(wxWindow::GetHandle());
}

wxSFMLCanvas::~wxSFMLCanvas() = default;

void wxSFMLCanvas::OnUpdate() {}

// ReSharper disable once CppMemberFunctionMayBeStatic
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