#include "canvas/RenderStatusWidget.h"
#include <utility>
#include <wx/dcbuffer.h>
#include "common/AppTheme.h"

RenderStatusWidget::RenderStatusWidget(wxWindow* parent, FractalPresenter* presenter, ClickHandler clickHandler)
                                       : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(170, 24), wxBORDER_NONE),
                                         _presenter(presenter),
                                         _clickHandler(std::move(clickHandler)),
                                         _timer(this)
{
    wxWindowBase::SetMinSize(wxSize(170, 24));
    wxWindowBase::SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxWindowBase::SetCursor(wxCursor(wxCURSOR_HAND));

    Bind(wxEVT_PAINT, &RenderStatusWidget::OnPaintEvent, this);
    Bind(wxEVT_TIMER, &RenderStatusWidget::OnTimer, this);
    Bind(wxEVT_LEFT_UP, &RenderStatusWidget::OnLeftUp, this);

    _timer.Start(100);
}

RenderStatusWidget::~RenderStatusWidget()
{
    _timer.Stop();
    _presenter = nullptr;
}

void RenderStatusWidget::SetPresenter(FractalPresenter* presenter)
{
    _presenter = presenter;
    Refresh();
}

bool RenderStatusWidget::AcceptsFocus() const
{
    return false;
}

bool RenderStatusWidget::AcceptsFocusFromKeyboard() const
{
    return false;
}

void RenderStatusWidget::OnPaintEvent(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBackground(wxBrush(GetParent() != nullptr ? GetParent()->GetBackgroundColour() : AppTheme::Background()));
    dc.Clear();

    const wxSize size = GetClientSize();
    const wxRect box(1, 1, size.GetWidth() - 2, size.GetHeight() - 2);
    const wxColour border = AppTheme::IsDark() ? wxColour(77, 80, 86) : wxColour(150, 154, 160);
    const wxColour background = AppTheme::IsDark() ? wxColour(25, 27, 31) : wxColour(245, 246, 248);

    dc.SetPen(wxPen(border));
    dc.SetBrush(wxBrush(background));
    dc.DrawRoundedRectangle(box, 6);

    Fractal* fractal = _presenter != nullptr ? _presenter->GetFractal() : nullptr;
    const bool rendering = fractal != nullptr && (fractal->IsRenderStarted() || fractal->IsRendering());
    wxString text = _("Iterations: --");
    int textX = 12;

    if (fractal != nullptr)
        text = rendering ? _("Rendering...") : wxString::Format(_("Iterations: %u"), fractal->GetIterations());

    if (rendering)
    {
        constexpr int dotCount = 8;
        constexpr double pi = 3.14159265358979323846;
        const wxPoint center(16, size.GetHeight() / 2);

        for (int i = 0; i < dotCount; ++i)
        {
            const int age = (i + static_cast<int>(_spinnerFrame)) % dotCount;
            const auto channel = static_cast<unsigned char>(90 + age * 18);
            const double angle = 2.0 * pi * static_cast<double>(i) / static_cast<double>(dotCount);
            const int x = center.x + static_cast<int>(std::cos(angle) * 6.0);
            const int y = center.y + static_cast<int>(std::sin(angle) * 6.0);
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(wxColour(channel, channel, channel)));
            dc.DrawCircle(x, y, 2);
        }

        textX = 30;
    }

    dc.SetTextForeground(AppTheme::Foreground());
    dc.DrawText(text, textX, (size.GetHeight() - dc.GetCharHeight()) / 2);
}

void RenderStatusWidget::OnTimer(wxTimerEvent&)
{
    _spinnerFrame = (_spinnerFrame + 1) % 8;
    Refresh(false);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void RenderStatusWidget::OnLeftUp(wxMouseEvent&)
{
    if (_clickHandler)
        _clickHandler();
}
