#include "canvas/wxSFMLCanvas.h"
#include "common/NativeWindowHandle.h"
#include "config/AppConfigStore.h"

#include <algorithm>
#include <cmath>

wxSFMLCanvas::wxSFMLCanvas(wxWindow* parent, const wxWindowID id, const wxPoint& position, const wxSize& size,
                           const long style) : wxControl(parent, id, position, size, style)
{
    SetName("wxChaosSfmlCanvas");
    wxWindowBase::SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxWindow::SetDoubleBuffered(false);

    _frameTimer.SetOwner(this);
    Bind(wxEVT_TIMER, &wxSFMLCanvas::OnFrameTimer, this, _frameTimer.GetId());
    Bind(wxEVT_PAINT, &wxSFMLCanvas::OnPaintEvent, this);
    Bind(wxEVT_ERASE_BACKGROUND, &wxSFMLCanvas::OnEraseBackground, this);
    SetTargetFrameRate(60);
}

wxSFMLCanvas::~wxSFMLCanvas() = default;

void wxSFMLCanvas::SetTargetFrameRate(const int frameRate)
{
    const int normalizedFrameRate = std::max(AppConfig::MinimumTargetFrameRate, frameRate);
    const int frameIntervalMilliseconds = std::max(
        1, static_cast<int>(std::round(1000.0 / normalizedFrameRate)));
    _frameTimer.Start(frameIntervalMilliseconds);
}

void wxSFMLCanvas::StopSfmlRefresh()
{
    _sfmlRefreshEnabled = false;
    _frameTimer.Stop();
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

void wxSFMLCanvas::OnFrameTimer(wxTimerEvent&)
{
    if (!_sfmlRefreshEnabled)
        return;

    const wxSize size = GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
        return;

    if (!EnsureSfmlWindowCreated())
        return;

    Refresh(false);
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
