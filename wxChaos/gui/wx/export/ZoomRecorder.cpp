// ReSharper disable CppDFAUnreachableFunctionCall
#include <wx/progdlg.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include "SFML/System.hpp"
#include "AppPaths.h"
#include "common/AppTheme.h"
#include "export/ZoomRecorder.h"
#include "export/ZoomRenderer.h"

ZoomRecorder::ZoomRecorder(FractalCanvas* fractalCanvas, wxWindow* parent, const wxWindowID id, const wxString& title,
                           const wxPoint& pos, const wxSize& size, const long style) : wxDialog(parent, id, title, pos, size, style)
{
    // fractal factory initialization
    _fractalCanvasPtr = fractalCanvas;
    this->CreateFractalFactory();
    _fractalFactory.GetFractal()->SetPreciseView(_outermostZoom);

    // UI initialization
    this->SetSizeHints(wxSize(820, 360), wxSize(1400, 900));

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);

    _panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto previewAndButtonsSizer = new wxBoxSizer(wxVERTICAL);
    const auto previewSizer = new wxBoxSizer(wxVERTICAL);
    previewSizer->Add(CreateSectionHeader(_panel, "Preview", "preview_light.svg", "preview_dark.svg"),
                      0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _previewBitmap = new wxStaticBitmap(_panel, wxID_ANY,
        _fractalFactory.GetFractal()->GetRenderedWxBitmap(),
        wxDefaultPosition, wxDefaultSize, 0);
    previewSizer->Add(_previewBitmap, 0, wxALL | wxEXPAND, 10);

    _previewFrameText = new wxStaticText(_panel, wxID_ANY, "Frame:", wxDefaultPosition, wxDefaultSize, 0);
    _previewFrameText->Wrap(-1);
    previewSizer->Add(_previewFrameText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    _previewSlider = new wxSlider(_panel, wxID_ANY, 0, 0, 1799, wxDefaultPosition, wxDefaultSize,
        wxSL_AUTOTICKS | wxSL_BOTTOM | wxSL_HORIZONTAL | wxSL_LABELS);
    previewSizer->Add(_previewSlider, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    previewAndButtonsSizer->Add(previewSizer, 0, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    _saveButton = new wxButton(_panel, wxID_ANY, "Save video", wxDefaultPosition, wxDefaultSize, 0);
    SetButtonIcon(_saveButton, "save_light.svg", "save_dark.svg");
    buttonSizer->AddStretchSpacer();
    buttonSizer->Add(_saveButton, 0, wxALL, 5);

    _cancelButton = new wxButton(_panel, wxID_ANY, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
    SetButtonIcon(_cancelButton, "close_light.svg", "close_dark.svg");
    buttonSizer->Add(_cancelButton, 0, wxALL, 5);

    previewAndButtonsSizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    panelSizer->Add(previewAndButtonsSizer, 0, wxEXPAND, 5);

    auto* optionsSizer = new wxBoxSizer(wxVERTICAL);
    optionsSizer->Add(CreateSectionHeader(_panel, "Zoom options", "camcorder_light.svg", "camcorder_dark.svg"),
                      0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    const auto optionGridSizer = new wxFlexGridSizer(2, 10, 12);
    optionGridSizer->AddGrowableCol(1, 1);

    _videoDurationText = new wxStaticText(_panel, wxID_ANY, "Video duration:", wxDefaultPosition, wxDefaultSize, 0);
    _videoDurationText->Wrap(-1);
    optionGridSizer->Add(_videoDurationText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    const auto videoDurationSizer = new wxBoxSizer(wxHORIZONTAL);

    _minutesSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 1);
    _minutesSpinCtrl->SetMaxSize(wxSize(70, -1));
    videoDurationSizer->Add(_minutesSpinCtrl, 0, wxRIGHT, 5);

    _minutesText = new wxStaticText(_panel, wxID_ANY, "Minutes", wxDefaultPosition, wxDefaultSize, 0);
    _minutesText->Wrap(-1);
    videoDurationSizer->Add(_minutesText, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    _secondsSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 0);
    _secondsSpinCtrl->SetMaxSize(wxSize(70, -1));
    videoDurationSizer->Add(_secondsSpinCtrl, 0, wxRIGHT, 5);

    _secondsText = new wxStaticText(_panel, wxID_ANY, "Seconds", wxDefaultPosition, wxDefaultSize, 0);
    _secondsText->Wrap(-1);
    videoDurationSizer->Add(_secondsText, 0, wxALIGN_CENTER_VERTICAL);
    optionGridSizer->Add(videoDurationSizer, 1, wxEXPAND);

    _framerateText = new wxStaticText(_panel, wxID_ANY, "Framerate:", wxDefaultPosition, wxDefaultSize, 0);
    _framerateText->Wrap(-1);
    optionGridSizer->Add(_framerateText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    const auto framerateSize = new wxBoxSizer(wxHORIZONTAL);

    _framerateSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 120, 30);
    framerateSize->Add(_framerateSpinCtrl, 0, wxRIGHT, 5);

    _framesPerSecondText = new wxStaticText(_panel, wxID_ANY, "Frames Per Second", wxDefaultPosition, wxDefaultSize, 0);
    _framesPerSecondText->Wrap(-1);
    framerateSize->Add(_framesPerSecondText, 0, wxALIGN_CENTER_VERTICAL);
    optionGridSizer->Add(framerateSize, 1, wxEXPAND);

    _rotateCheckbox = new wxCheckBox(_panel, wxID_ANY, "Rotate colors", wxDefaultPosition, wxDefaultSize, 0);
    optionGridSizer->AddSpacer(1);
    optionGridSizer->Add(_rotateCheckbox, 0, wxEXPAND);

    _colorRotateSpeedText = new wxStaticText(_panel, wxID_ANY, "Color rotation speed:", wxDefaultPosition, wxDefaultSize, 0);
    _colorRotateSpeedText->Wrap(-1);
    optionGridSizer->Add(_colorRotateSpeedText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    _colorSpeedCtrl = new wxSpinCtrlDouble(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 5, 0.1, 0.1);
    _colorSpeedCtrl->SetDigits(2);
    optionGridSizer->Add(_colorSpeedCtrl, 0, wxEXPAND);

    optionsSizer->Add(optionGridSizer, 0, wxEXPAND | wxALL, 12);
    panelSizer->Add(optionsSizer, 1, wxEXPAND, 5);

    _panel->SetSizer(panelSizer);
    _panel->Layout();
    panelSizer->Fit(_panel);
    mainSizer->Add(_panel, 0, wxEXPAND | wxALL, 1);

    this->SetSizer(mainSizer);
    mainSizer->Fit(this);
    const int fittedHeight = this->GetSize().GetHeight();
    this->wxTopLevelWindowBase::SetMinSize(wxSize(820, fittedHeight));
    this->SetSize(wxSize(900, fittedHeight));
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    // Connect Events
    _previewSlider->Bind(wxEVT_SCROLL_TOP, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Bind(wxEVT_SCROLL_BOTTOM, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Bind(wxEVT_SCROLL_LINEUP, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Bind(wxEVT_SCROLL_LINEDOWN, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Bind(wxEVT_SCROLL_PAGEUP, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Bind(wxEVT_SCROLL_PAGEDOWN, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Bind(wxEVT_SCROLL_THUMBTRACK, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Bind(wxEVT_SCROLL_THUMBRELEASE, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Bind(wxEVT_SCROLL_CHANGED, &ZoomRecorder::OnScrollPreview, this);
    _saveButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ZoomRecorder::OnSaveVideo, this);
    _cancelButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ZoomRecorder::OnCancel, this);
    _minutesSpinCtrl->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnUpdateTotalFrames, this);
    _secondsSpinCtrl->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnUpdateTotalFrames, this);
    _framerateSpinCtrl->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnUpdateTotalFrames, this);
    _rotateCheckbox->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &ZoomRecorder::OnColorRotate, this);
    _colorSpeedCtrl->Bind(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, &ZoomRecorder::OnChangeSpeedDbl, this);
}

ZoomRecorder::~ZoomRecorder()
{
    // Disconnect Events
    _previewSlider->Unbind(wxEVT_SCROLL_TOP, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Unbind(wxEVT_SCROLL_BOTTOM, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Unbind(wxEVT_SCROLL_LINEUP, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Unbind(wxEVT_SCROLL_LINEDOWN, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Unbind(wxEVT_SCROLL_PAGEUP, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Unbind(wxEVT_SCROLL_PAGEDOWN, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Unbind(wxEVT_SCROLL_THUMBTRACK, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Unbind(wxEVT_SCROLL_THUMBRELEASE, &ZoomRecorder::OnScrollPreview, this);
    _previewSlider->Unbind(wxEVT_SCROLL_CHANGED, &ZoomRecorder::OnScrollPreview, this);
    _saveButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ZoomRecorder::OnSaveVideo, this);
    _cancelButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ZoomRecorder::OnCancel, this);
    _minutesSpinCtrl->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnUpdateTotalFrames, this);
    _secondsSpinCtrl->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnUpdateTotalFrames, this);
    _framerateSpinCtrl->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnUpdateTotalFrames, this);
    _rotateCheckbox->Unbind(wxEVT_COMMAND_CHECKBOX_CLICKED, &ZoomRecorder::OnColorRotate, this);
    _colorSpeedCtrl->Unbind(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, &ZoomRecorder::OnChangeSpeedDbl, this);
}

wxPanel* ZoomRecorder::CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                           const wxString& darkIcon)
{
    const auto header = new wxPanel(parent, wxID_ANY);
    header->SetBackgroundColour(AppTheme::ControlBackground());

    const auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    const wxSize iconSize(24, 24);
    const auto iconBitmap = new wxStaticBitmap(header, wxID_ANY, CreateIconBundle(lightIcon, darkIcon, iconSize));
    iconBitmap->SetBackgroundColour(AppTheme::ControlBackground());
    headerSizer->Add(iconBitmap, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP | wxBOTTOM, 8);

    const auto headerText = new wxStaticText(header, wxID_ANY, text);
    wxFont headerFont = headerText->GetFont();
    headerFont.SetPointSize(headerFont.GetPointSize() + 1);
    headerFont.SetWeight(wxFONTWEIGHT_BOLD);
    headerText->SetFont(headerFont);
    headerText->SetBackgroundColour(AppTheme::ControlBackground());
    headerText->SetForegroundColour(AppTheme::Foreground());
    headerSizer->Add(headerText, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

    header->SetSizer(headerSizer);
    header->SetMinSize(wxSize(-1, 44));
    return header;
}

wxBitmapBundle ZoomRecorder::CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                              const wxSize& size)
{
    const wxString icon = AppTheme::IsDark() ? darkIcon : lightIcon;
    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), size);
}

void ZoomRecorder::SetButtonIcon(wxButton* button, const wxString& lightIcon, const wxString& darkIcon) const
{
    button->SetBitmap(CreateIconBundle(lightIcon, darkIcon, wxSize(20, 20)));
    button->SetBitmapMargins(FromDIP(6), 0);
}

void ZoomRecorder::CreateFractalInstance(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, const int width, const int height)
{
    const FractalType fractalType = fractalCanvas->GetFractalType();
    if (fractalType == FractalType::ScriptFractal)
    {
        auto* scriptFractalPtr = reinterpret_cast<ScriptFractal*>(fractalCanvas->GetFractal());
        fractalFactory.CreateScriptFractal(width, height, scriptFractalPtr->GetPath());
    }
    else
        fractalFactory.CreateFractal(fractalType, width, height);
}

PreciseRect ZoomRecorder::CreateRecordingFractal(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas,
                                                 const int width, const int height)
{
    CreateFractalInstance(fractalFactory, fractalCanvas, width, height);
    const PreciseRect defaultView = fractalFactory.GetFractal()->GetPreciseView();
    fractalFactory.SetFormula(fractalCanvas->GetFormula());
    fractalFactory.GetFractal()->SetOptions(fractalCanvas->GetFractal()->GetOptions());
    return defaultView;
}

Rect ZoomRecorder::GetDefaultView(FractalCanvas* fractalCanvas, const int width, const int height)
{
    FractalFactory fractalFactory;
    ZoomRecorder::CreateFractalInstance(fractalFactory, fractalCanvas, width, height);
    return fractalFactory.GetFractal()->GetView();
}

void ZoomRecorder::CreateFractalFactory()
{
    _outermostZoom = CreateRecordingFractal(_fractalFactory, _fractalCanvasPtr, 250, 166);
    _innermostZoom = _fractalCanvasPtr->GetFractal()->GetPreciseView();
}
void ZoomRecorder::RenderPreview(const int zoom, const double colorSpeed) const
{
    const int totalFrames = this->GetTotalFrames();
    const double t = zoom;
    const double progress = ZoomRenderer::GetFrameProgress(zoom, totalFrames);
    const PreciseRect viewport = ZoomRenderer::GetZoomViewport(_outermostZoom, _innermostZoom, progress);

    _fractalFactory.GetFractal()->SetPreciseView(viewport);

    if (colorSpeed != -1)
        _fractalFactory.GetFractal()->SetVarGradient(static_cast<int>(colorSpeed * t));
    else
        _fractalFactory.GetFractal()->SetVarGradient(0);

    _previewBitmap->SetBitmap(_fractalFactory.GetFractal()->GetRenderedWxBitmap());
}
void ZoomRecorder::RenderPreview()
{
    if (_rotateCheckbox->GetValue())
        this->RenderPreview(_previewSlider->GetValue(), _colorSpeedCtrl->GetValue());
    else
        this->RenderPreview(_previewSlider->GetValue());
}
int ZoomRecorder::GetTotalFrames() const
{
    const int seconds = _secondsSpinCtrl->GetValue();
    const int minutes = _minutesSpinCtrl->GetValue();
    const int framerate = _framerateSpinCtrl->GetValue();
    const int totalFrames = (60 * minutes + seconds) * framerate;

    return totalFrames > 0 ? totalFrames : 1;
}
void ZoomRecorder::UpdateTotalFrames()
{
    const int totalFrames = this->GetTotalFrames();
    _previewSlider->SetMax(totalFrames - 1);
    _previewSlider->SetValue(0);
    this->RenderPreview(0);
}

// ZoomRecorder events.

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ZoomRecorder::OnScrollPreview(wxScrollEvent& event)
{
    if (_rotateCheckbox->GetValue())
        this->RenderPreview(event.GetPosition(), _colorSpeedCtrl->GetValue());
    else
        this->RenderPreview(event.GetPosition());
}
void ZoomRecorder::OnSaveVideo(wxCommandEvent&)
{
    // Select the output directory
    auto* dirDialog = new wxDirDialog(this);
    wxString selectedFile;

    if (dirDialog->ShowModal() == wxID_OK)
    {
        selectedFile = dirDialog->GetPath();
        dirDialog->Destroy();
    }
    else
    {
        dirDialog->Destroy();
        return;
    }

    // Create ZoomRenderer and execute it.
    const int totalFrames = this->GetTotalFrames();
    const double colorSpeed = _colorSpeedCtrl->GetValue();
    // wxString::mb_str() returns a wxCharBuffer which cannot be implicitly
    // converted to std::string on GCC.  Explicitly construct the std::string
    // from the buffer.
    const std::string selectedDirPath(selectedFile.mb_str());

    wxProgressDialog progressDialog("Generating video...", "Please wait until the process is complete.", totalFrames, this);
    auto* renderer = new ZoomRenderer(selectedDirPath, _fractalCanvasPtr, 2500, 1660, totalFrames, colorSpeed);
    wxThreadError err = renderer->Create();

    if (err != wxTHREAD_NO_ERROR)
    {
        wxMessageBox(_("Couldn't create thread!"));
        return;
    }

    err = renderer->Run();

    if (err != wxTHREAD_NO_ERROR)
    {
        wxMessageBox(_("Couldn't run thread!"));
        return;
    }

    progressDialog.Show(true);
    sf::Clock clock;
    clock.restart();

    while (renderer->IsRunning())
    {
        if (clock.getElapsedTime().asSeconds() >= 0.05)
        {
            progressDialog.Update(renderer->GetProgress());
            clock.restart();
        }
    }

    progressDialog.Show(false);
    this->EndModal(0);
}
void ZoomRecorder::OnCancel(wxCommandEvent&)
{
    _fractalFactory.DeleteFractal();
    this->EndModal(0);
}
void ZoomRecorder::OnUpdateTotalFrames(wxSpinEvent&)
{
    this->UpdateTotalFrames();
}
void ZoomRecorder::OnColorRotate(wxCommandEvent&)
{
    this->RenderPreview();
}
void ZoomRecorder::OnChangeSpeedDbl(wxSpinDoubleEvent&)
{
    this->RenderPreview();
}
