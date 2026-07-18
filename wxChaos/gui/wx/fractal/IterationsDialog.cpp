#include <algorithm>
#include <iterator>
#include <limits>
#include <wx/dcclient.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include "AppPaths.h"
#include "fractal/IterationsDialog.h"

IterationsDialog::IterationsDialog(bool* Active, FractalPresenter* presenter, wxWindow* parent, wxWindow* focusAfterClose,
                                   const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
                                   const long style)
                                   : wxFrame(parent, id, title, pos, size, style)
{
    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.ico"}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    _active = Active;
    _fractalPresenter = presenter;
    _target = _fractalPresenter->GetFractal();
    _focusAfterClose = focusAfterClose;
    this->SetSizeHints(wxSize(560, 340), wxDefaultSize);

    const auto sizer = new wxBoxSizer(wxVERTICAL);

    _panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto subSizer = new wxBoxSizer(wxVERTICAL);

    const auto titleLabel = new wxStaticText(_panel, wxID_ANY, _("Iterations"));
    wxFont titleFont = titleLabel->GetFont();
    titleFont.SetPointSize(titleFont.GetPointSize() + 2);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleLabel->SetFont(titleFont);
    subSizer->Add(titleLabel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 14);

    subSizer->Add(new wxStaticLine(_panel, wxID_ANY), 0, wxEXPAND | wxTOP | wxBOTTOM, 10);

    _number = _target->GetIterations();
    const auto inputSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto label = new wxStaticText(_panel, wxID_ANY, _("Max iter:"));
    inputSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    const auto spinValue = static_cast<int>(std::min(
        _number,
        static_cast<unsigned int>(std::numeric_limits<int>::max())));
    _iterationsSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                         wxSP_ARROW_KEYS, 1, std::numeric_limits<int>::max(), spinValue);
    inputSizer->Add(_iterationsSpinCtrl, 1, wxALIGN_CENTER_VERTICAL);
    subSizer->Add(inputSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 20);

    _iterationsSlider = new wxSlider(_panel, wxID_ANY, IterationsToSliderValue(_number), 0, 400,
                                     wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizer->Add(_iterationsSlider, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 18);

    _scalePanel = new wxPanel(_panel, wxID_ANY, wxDefaultPosition, wxSize(-1, FromDIP(32)));
    _scalePanel->Bind(wxEVT_PAINT, &IterationsDialog::OnScalePaint, this);
    subSizer->Add(_scalePanel, 0, wxEXPAND | wxLEFT | wxRIGHT, 20);

    const auto hint = new wxStaticText(_panel, wxID_ANY,
        _("Higher values reveal more detail\nbut increase render time."));
    wxFont hintFont = hint->GetFont();
    hintFont.SetStyle(wxFONTSTYLE_ITALIC);
    hint->SetFont(hintFont);
    subSizer->Add(hint, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 20);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    _acceptButton = new wxButton(_panel, wxID_ANY, _("Ok"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->AddStretchSpacer(1);
    buttonSizer->Add(_acceptButton, 0, wxALL, 5);

    _applyButton = new wxButton(_panel, wxID_ANY, _("Apply"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_applyButton, 0, wxALL, 5);
    subSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 10);

    _panel->SetSizer(subSizer);
    _panel->Layout();
    subSizer->Fit(_panel);
    sizer->Add(_panel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    sizer->Fit(this);
    this->wxTopLevelWindowBase::SetMinSize(this->GetSize());
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    _iterationsSlider->Bind(wxEVT_SLIDER, &IterationsDialog::OnSlider, this);
    _iterationsSpinCtrl->Bind(wxEVT_SPINCTRL, &IterationsDialog::OnSpin, this);
    _iterationsSpinCtrl->Bind(wxEVT_TEXT, &IterationsDialog::OnSpin, this);
    _acceptButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnOk, this);
    _applyButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnApply, this);
    Bind(wxEVT_CLOSE_WINDOW, &IterationsDialog::OnClose, this);
}

IterationsDialog::~IterationsDialog()
{
    *_active = false;
    _iterationsSlider->Unbind(wxEVT_SLIDER, &IterationsDialog::OnSlider, this);
    _iterationsSpinCtrl->Unbind(wxEVT_SPINCTRL, &IterationsDialog::OnSpin, this);
    _iterationsSpinCtrl->Unbind(wxEVT_TEXT, &IterationsDialog::OnSpin, this);
    _scalePanel->Unbind(wxEVT_PAINT, &IterationsDialog::OnScalePaint, this);
    _acceptButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnOk, this);
    _applyButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnApply, this);
    Unbind(wxEVT_CLOSE_WINDOW, &IterationsDialog::OnClose, this);
}

int IterationsDialog::IterationsToSliderValue(const unsigned int iterations)
{
    constexpr double minLog = 1.0;
    constexpr double maxLog = 8.0;
    constexpr int sliderMax = 400;

    const double clamped = std::clamp(static_cast<double>(iterations), 10.0, 100000000.0);
    const double normalized = (std::log10(clamped) - minLog) / (maxLog - minLog);
    return static_cast<int>(std::round(std::clamp(normalized, 0.0, 1.0) * sliderMax));
}

unsigned int IterationsDialog::SliderValueToIterations(const int sliderValue)
{
    constexpr double minLog = 1.0;
    constexpr double maxLog = 8.0;
    constexpr int sliderMax = 400;

    const double normalized = std::clamp(static_cast<double>(sliderValue) / sliderMax, 0.0, 1.0);
    const double value = std::pow(10.0, minLog + normalized * (maxLog - minLog));
    return static_cast<unsigned int>(std::round(value));
}

void IterationsDialog::SetIterationControls(const unsigned int iterations)
{
    _number = iterations;
    _iterationsSpinCtrl->SetValue(static_cast<int>(std::min(
        _number,
        static_cast<unsigned int>(std::numeric_limits<int>::max()))));
    _iterationsSlider->SetValue(IterationsToSliderValue(_number));
}

bool IterationsDialog::ReadIterationValue(unsigned int& iterations) const
{
    const int value = _iterationsSpinCtrl->GetValue();
    if (value <= 0)
    {
        wxMessageBox(_("Iterations must be greater than zero."), _("Error"), wxOK | wxICON_ERROR);
        return false;
    }

    iterations = static_cast<unsigned int>(value);
    return true;
}

void IterationsDialog::RestoreFocusAfterClose() const
{
    wxWindow* focusAfterClose = _focusAfterClose;
    wxWindow* parent = GetParent();
    if (focusAfterClose == nullptr || parent == nullptr || parent->IsBeingDeleted())
        return;

    parent->CallAfter([focusAfterClose]
    {
        if (!focusAfterClose->IsBeingDeleted())
            focusAfterClose->SetFocus();
    });
}

// ReSharper disable once CppMemberFunctionMayBeConst
void IterationsDialog::OnScalePaint(wxPaintEvent&)
{
    wxPaintDC dc(_scalePanel);

    const wxString scaleLabels[] = { "10", "100", "1K", "10K", "100K", "1M", "10M", "100M" };
    const int trackInset = FromDIP(7);
    const int width = std::max(0, _scalePanel->GetClientSize().GetWidth() - 2 * trackInset);

    dc.SetTextForeground(_panel->GetForegroundColour());

    for (std::size_t index = 0; index < std::size(scaleLabels); ++index)
    {
        const wxSize textSize = dc.GetTextExtent(scaleLabels[index]);
        const int maxTextX = std::max(0, _scalePanel->GetClientSize().GetWidth() - textSize.GetWidth());
        const double scalePosition = static_cast<double>(index) / static_cast<double>(std::size(scaleLabels) - 1);
        const int x = trackInset + static_cast<int>(std::round(width * scalePosition)) - textSize.GetWidth() / 2;
        dc.DrawText(scaleLabels[index], std::clamp(x, 0, maxTextX), 0);
    }
}

void IterationsDialog::OnSlider(wxCommandEvent&)
{
    const unsigned int iterations = SliderValueToIterations(_iterationsSlider->GetValue());
    _iterationsSpinCtrl->SetValue(static_cast<int>(iterations));
    _number = iterations;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void IterationsDialog::OnSpin(wxCommandEvent&)
{
    if (const wxString text = _iterationsSpinCtrl->GetTextValue(); text.IsEmpty())
        return;

    if (const int value = _iterationsSpinCtrl->GetValue(); value > 0)
        _iterationsSlider->SetValue(IterationsToSliderValue(static_cast<unsigned int>(value)));
}

void IterationsDialog::OnOk(wxCommandEvent&)
{
    unsigned int iterations;
    if (!ReadIterationValue(iterations))
        return;

    _fractalPresenter->ChangeIterations(iterations);

    // Closes dialog.
    this->Close(true);
}
void IterationsDialog::OnApply(wxCommandEvent&)
{
    unsigned int iterations;
    if (!ReadIterationValue(iterations))
        return;

    _number = iterations;
    _fractalPresenter->ChangeIterations(_number);
}

void IterationsDialog::OnClose(wxCloseEvent&)
{
    RestoreFocusAfterClose();
    Destroy();
}

void IterationsDialog::SetTarget(FractalPresenter* presenter)
{
    _fractalPresenter = presenter;
    _target = _fractalPresenter->GetFractal();
    SetIterationControls(_target->GetIterations());
}
