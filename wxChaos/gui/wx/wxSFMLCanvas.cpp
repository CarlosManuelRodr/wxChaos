#include "wxSFMLCanvas.h"

wxSFMLCanvas::wxSFMLCanvas(wxWindow* parent, const wxWindowID id, const wxPoint& position, const wxSize& size,
                           const long style) : wxControl(parent, id, position, size, style)
{
    wxWindowBase::SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxWindow::SetDoubleBuffered(false);

    Bind(wxEVT_IDLE, &wxSFMLCanvas::OnIdle, this);
    Bind(wxEVT_PAINT, &wxSFMLCanvas::OnPaintEvent, this);
    Bind(wxEVT_ERASE_BACKGROUND, &wxSFMLCanvas::OnEraseBackground, this);
    sf::RenderWindow::create(wxWindow::GetHandle());
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

void wxSFMLCanvas::OnPaintEvent(wxPaintEvent&)
{
    // Prepare the control to be repainted
    wxPaintDC Dc(this);

    // Let the derived class do its specific stuff
    OnUpdate();

    // Display on screen
    display();
}
