#include "canvas/wxSFMLCanvas.h"
#include "common/NativeWindowHandle.h"

wxSFMLCanvas::wxSFMLCanvas(wxWindow* parent, const wxWindowID id, const wxPoint& position, const wxSize& size,
                           const long style) : wxControl(parent, id, position, size, style)
{
    SetName("wxChaosSfmlCanvas");
    wxWindowBase::SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxWindow::SetDoubleBuffered(false);

    Bind(wxEVT_IDLE, &wxSFMLCanvas::OnIdle, this);
    Bind(wxEVT_PAINT, &wxSFMLCanvas::OnPaintEvent, this);
    Bind(wxEVT_ERASE_BACKGROUND, &wxSFMLCanvas::OnEraseBackground, this);
}

wxSFMLCanvas::~wxSFMLCanvas() = default;

void wxSFMLCanvas::StopSfmlRefresh()
{
    _sfmlRefreshEnabled = false;
}

bool wxSFMLCanvas::EnsureSfmlWindowCreated()
{
    if (_sfmlWindowCreated)
        return true;

    const wxSize size = GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
        return false;

    const sf::WindowHandle nativeHandle = Platform::GetNativeWindowHandle(*this);
    if (nativeHandle == 0)
        return false;

    sf::RenderWindow::create(nativeHandle);
    _sfmlWindowCreated = true;
    return true;
}

bool wxSFMLCanvas::IsSfmlWindowCreated() const
{
    return _sfmlWindowCreated;
}

wxPoint wxSFMLCanvas::GetRenderMousePosition(const wxMouseEvent& event) const
{
#ifdef __WXGTK__
    if (_sfmlWindowCreated)
    {
        const sf::Vector2i position = sf::Mouse::getPosition(*this);
        return {position.x, position.y};
    }
#endif

    return event.GetPosition();
}

void wxSFMLCanvas::OnUpdate() {}

// ReSharper disable once CppMemberFunctionMayBeStatic
void wxSFMLCanvas::OnEraseBackground(wxEraseEvent&) {}

void wxSFMLCanvas::OnIdle(wxIdleEvent&)
{
    if (!_sfmlRefreshEnabled)
        return;

    const wxSize size = GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
        return;

    if (!EnsureSfmlWindowCreated())
        return;

    // Send a paint message when the control is idle, to ensure maximum framerate
    Refresh();
}

void wxSFMLCanvas::OnPaintEvent(wxPaintEvent&)
{
    // Prepare the control to be repainted
    wxPaintDC Dc(this);

    if (!_sfmlRefreshEnabled)
        return;

    const wxSize size = GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
        return;

    if (!EnsureSfmlWindowCreated())
        return;

    // Let the derived class do its specific stuff
    OnUpdate();

    // Display on screen
    display();
}
