// ReSharper disable CppDFAUnreachableFunctionCall
#include <algorithm>
#include <cmath>
#include <wx/progdlg.h>
#include <wx/bitmap.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/icon.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
#include "SFML/System.hpp"
#include "AppPaths.h"
#include "common/AppTheme.h"
#include "export/NativeVideoWriter.h"
#include "export/ZoomRecorder.h"
#include "export/ZoomRenderer.h"
#include "canvas/FractalCanvas.h"
#include "sfml/FractalPresenter.h"
#include "Fractal.h"
#include "fractals/raster/ScriptFractal.h"

ZoomRecorder::ZoomRecorder(FractalCanvas* fractalCanvas, wxWindow* parent, const wxWindowID id, const wxString& title,
                           const wxPoint& pos, const wxSize& size, const long style)
                           : wxDialog(parent, id, wxGetTranslation(title), pos, size, style)
{
    // fractal factory initialization
    _fractalCanvasPtr = fractalCanvas;
    InitializeRenderSizes();
    this->CreateFractalFactory();
    _fractalFactory.GetFractal()->SetPreciseView(_outermostZoom);

    // UI initialization
    this->SetSizeHints(wxSize(820, 360), wxSize(1400, 900));

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.ico"}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);

    _panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto previewAndButtonsSizer = new wxBoxSizer(wxVERTICAL);
    const auto previewSizer = new wxBoxSizer(wxVERTICAL);
    previewSizer->Add(CreateSectionHeader(_panel, _("Preview"), "preview_light.svg", "preview_dark.svg"),
                      0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _previewBitmap = new wxStaticBitmap(_panel, wxID_ANY,
        _fractalFactory.GetFractal()->GetRenderedWxBitmap(),
        wxDefaultPosition, wxDefaultSize, 0);
    _previewBitmap->SetMinSize(wxSize(_previewWidth, _previewHeight));
    previewSizer->Add(_previewBitmap, 0, wxALL | wxEXPAND, 10);

    _previewFrameText = new wxStaticText(_panel, wxID_ANY, _("Frame:"), wxDefaultPosition, wxDefaultSize, 0);
    _previewFrameText->Wrap(-1);
    previewSizer->Add(_previewFrameText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    _previewSlider = new wxSlider(_panel, wxID_ANY, 0, 0, 1799, wxDefaultPosition, wxDefaultSize,
        wxSL_AUTOTICKS | wxSL_BOTTOM | wxSL_HORIZONTAL | wxSL_LABELS);
    previewSizer->Add(_previewSlider, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    previewAndButtonsSizer->Add(previewSizer, 0, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    _saveButton = new wxButton(_panel, wxID_ANY, _("Save video"), wxDefaultPosition, wxDefaultSize, 0);
    SetButtonIcon(_saveButton, "save_light.svg", "save_dark.svg");
    buttonSizer->Add(_saveButton, 1, wxALL | wxEXPAND, 5);

    _cancelButton = new wxButton(_panel, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    SetButtonIcon(_cancelButton, "close_light.svg", "close_dark.svg");
    buttonSizer->Add(_cancelButton, 1, wxALL | wxEXPAND, 5);

    previewAndButtonsSizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    panelSizer->Add(previewAndButtonsSizer, 0, wxEXPAND, 5);

    auto* optionsSizer = new wxBoxSizer(wxVERTICAL);
    optionsSizer->Add(CreateSectionHeader(_panel, _("Zoom options"), "camcorder_light.svg", "camcorder_dark.svg"),
                      0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    const auto optionGridSizer = new wxFlexGridSizer(2, 10, 12);
    optionGridSizer->AddGrowableCol(1, 1);

    _videoDurationText = new wxStaticText(_panel, wxID_ANY, _("Video duration:"), wxDefaultPosition, wxDefaultSize, 0);
    _videoDurationText->Wrap(-1);
    optionGridSizer->Add(_videoDurationText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    const auto videoDurationSizer = new wxBoxSizer(wxHORIZONTAL);

    _minutesSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 1);
    SetSpinControlWidth(_minutesSpinCtrl);
    videoDurationSizer->Add(_minutesSpinCtrl, 0, wxRIGHT, 5);

    _minutesText = new wxStaticText(_panel, wxID_ANY, _("Minutes"), wxDefaultPosition, wxDefaultSize, 0);
    _minutesText->Wrap(-1);
    videoDurationSizer->Add(_minutesText, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    _secondsSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 0);
    SetSpinControlWidth(_secondsSpinCtrl);
    videoDurationSizer->Add(_secondsSpinCtrl, 0, wxRIGHT, 5);

    _secondsText = new wxStaticText(_panel, wxID_ANY, _("Seconds"), wxDefaultPosition, wxDefaultSize, 0);
    _secondsText->Wrap(-1);
    videoDurationSizer->Add(_secondsText, 0, wxALIGN_CENTER_VERTICAL);
    optionGridSizer->Add(videoDurationSizer, 1, wxEXPAND);

    _framerateText = new wxStaticText(_panel, wxID_ANY, _("Framerate:"), wxDefaultPosition, wxDefaultSize, 0);
    _framerateText->Wrap(-1);
    optionGridSizer->Add(_framerateText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    const auto framerateSize = new wxBoxSizer(wxHORIZONTAL);

    _framerateSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 120, 30);
    SetSpinControlWidth(_framerateSpinCtrl);
    framerateSize->Add(_framerateSpinCtrl, 0, wxRIGHT, 5);

    _framesPerSecondText = new wxStaticText(_panel, wxID_ANY, _("Frames Per Second"), wxDefaultPosition, wxDefaultSize, 0);
    _framesPerSecondText->Wrap(-1);
    framerateSize->Add(_framesPerSecondText, 0, wxALIGN_CENTER_VERTICAL);
    optionGridSizer->Add(framerateSize, 1, wxEXPAND);

    optionGridSizer->Add(new wxStaticText(_panel, wxID_ANY, _("Resolution:")),
                         0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    const auto resolutionSizer = new wxBoxSizer(wxHORIZONTAL);
    _widthSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    wxSP_ARROW_KEYS, 2, 16384, _recordingWidth);
    SetSpinControlWidth(_widthSpinCtrl);
    _widthSpinCtrl->SetToolTip(_("Output video width in pixels"));
    resolutionSizer->Add(_widthSpinCtrl, 0, wxRIGHT, 5);
    resolutionSizer->Add(new wxStaticText(_panel, wxID_ANY, wxString::FromUTF8("×")),
                         0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    _heightSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                     wxSP_ARROW_KEYS, 2, 16384, _recordingHeight);
    SetSpinControlWidth(_heightSpinCtrl);
    _heightSpinCtrl->SetToolTip(_("Output video height in pixels"));
    resolutionSizer->Add(_heightSpinCtrl, 0, wxRIGHT, 5);
    resolutionSizer->Add(new wxStaticText(_panel, wxID_ANY, _("pixels")), 0, wxALIGN_CENTER_VERTICAL);
    optionGridSizer->Add(resolutionSizer, 1, wxEXPAND);

    optionGridSizer->Add(new wxStaticText(_panel, wxID_ANY, _("Bitrate:")),
                         0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    const auto bitRateSizer = new wxBoxSizer(wxHORIZONTAL);
    const unsigned int recommendedBitRate = NativeVideoWriter::GetRecommendedBitRate(
        static_cast<unsigned int>(_recordingWidth), static_cast<unsigned int>(_recordingHeight),
        static_cast<unsigned int>(_framerateSpinCtrl->GetValue()));
    const int recommendedMegabits = static_cast<int>(
        recommendedBitRate / 1000000U + (recommendedBitRate % 1000000U != 0));
    _bitRateSpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                      wxSP_ARROW_KEYS, 1, 2000, recommendedMegabits);
    SetSpinControlWidth(_bitRateSpinCtrl);
    _bitRateSpinCtrl->SetToolTip(_("Higher bitrates preserve more detail but produce larger files"));
    bitRateSizer->Add(_bitRateSpinCtrl, 0, wxRIGHT, 5);
    bitRateSizer->Add(new wxStaticText(_panel, wxID_ANY, _("Mbps")), 0, wxALIGN_CENTER_VERTICAL);
    optionGridSizer->Add(bitRateSizer, 1, wxEXPAND);

    optionGridSizer->Add(new wxStaticText(_panel, wxID_ANY, _("Encoding quality:")),
                         0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    const auto qualitySizer = new wxBoxSizer(wxHORIZONTAL);
    _qualitySpinCtrl = new wxSpinCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                      wxSP_ARROW_KEYS, 0, 100, 50);
    SetSpinControlWidth(_qualitySpinCtrl);
    _qualitySpinCtrl->SetToolTip(_("Higher quality improves compression but takes longer to encode"));
    qualitySizer->Add(_qualitySpinCtrl, 0, wxRIGHT, 5);
    qualitySizer->Add(new wxStaticText(_panel, wxID_ANY, _("%")), 0, wxALIGN_CENTER_VERTICAL);
    optionGridSizer->Add(qualitySizer, 1, wxEXPAND);

    _rotateCheckbox = new wxCheckBox(_panel, wxID_ANY, _("Rotate colors"), wxDefaultPosition, wxDefaultSize, 0);
    optionGridSizer->AddSpacer(1);
    optionGridSizer->Add(_rotateCheckbox, 0, wxEXPAND);

    _colorRotateSpeedText = new wxStaticText(_panel, wxID_ANY, _("Color rotation speed:"), wxDefaultPosition, wxDefaultSize, 0);
    _colorRotateSpeedText->Wrap(-1);
    optionGridSizer->Add(_colorRotateSpeedText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    _colorSpeedCtrl = new wxSpinCtrlDouble(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 5, 0.1, 0.1);
    _colorSpeedCtrl->SetDigits(2);
    SetSpinControlWidth(_colorSpeedCtrl);
    optionGridSizer->Add(_colorSpeedCtrl, 0, wxEXPAND);

    optionsSizer->Add(optionGridSizer, 0, wxEXPAND | wxALL, 12);
    panelSizer->Add(optionsSizer, 1, wxEXPAND, 5);

    _panel->SetSizer(panelSizer);
    _panel->Layout();
    panelSizer->Fit(_panel);
    mainSizer->Add(_panel, 0, wxEXPAND | wxALL, 1);

    this->SetSizer(mainSizer);
    mainSizer->Fit(this);
    const wxSize fittedSize = this->GetSize();
    this->wxTopLevelWindowBase::SetMinSize(wxSize(std::max(820, fittedSize.GetWidth()), fittedSize.GetHeight()));
    this->SetSize(wxSize(std::max(900, fittedSize.GetWidth()), fittedSize.GetHeight()));
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
    _widthSpinCtrl->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnWidthChanged, this);
    _heightSpinCtrl->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnHeightChanged, this);
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
    _widthSpinCtrl->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnWidthChanged, this);
    _heightSpinCtrl->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnHeightChanged, this);
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
    _outermostZoom = _fractalCanvasPtr->GetFractalPresenter()->GetPreciseOutermostZoom();
    CreateRecordingFractal(_fractalFactory, _fractalCanvasPtr, _previewWidth, _previewHeight);
    _innermostZoom = _fractalCanvasPtr->GetFractal()->GetPreciseView();
}

void ZoomRecorder::InitializeRenderSizes()
{
    constexpr int maxPreviewWidth = 420;
    constexpr int maxPreviewHeight = 280;

    const sf::Vector2u renderSize = _fractalCanvasPtr->GetRenderSize();
    _recordingWidth = std::max(1, static_cast<int>(renderSize.x));
    _recordingHeight = std::max(1, static_cast<int>(renderSize.y));
    _recordingAspectRatio = static_cast<double>(_recordingWidth) / _recordingHeight;

    const double previewScale = std::min({
        1.0,
        static_cast<double>(maxPreviewWidth) / _recordingWidth,
        static_cast<double>(maxPreviewHeight) / _recordingHeight
    });

    _previewWidth = std::max(1, static_cast<int>(std::round(_recordingWidth * previewScale)));
    _previewHeight = std::max(1, static_cast<int>(std::round(_recordingHeight * previewScale)));
}

void ZoomRecorder::SetSpinControlWidth(wxWindow* control) const
{
    control->SetMinSize(wxSize(FromDIP(110), -1));
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
    wxFileDialog saveDialog(
        this,
        _("Save zoom video"),
        wxEmptyString,
        "Zoom.mp4",
        "MP4 video (*.mp4)|*.mp4",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveDialog.ShowModal() != wxID_OK)
        return;

    wxFileName outputFile(saveDialog.GetPath());
    if (outputFile.GetExt().empty())
        outputFile.SetExt("mp4");

    // Create ZoomRenderer and execute it.
    const int totalFrames = this->GetTotalFrames();
    const int framerate = _framerateSpinCtrl->GetValue();
    const int width = _widthSpinCtrl->GetValue();
    const int height = _heightSpinCtrl->GetValue();
    const NativeVideoEncodingOptions encodingOptions{
        static_cast<unsigned int>(_bitRateSpinCtrl->GetValue()) * 1000000U,
        static_cast<unsigned int>(_qualitySpinCtrl->GetValue())
    };
    const double colorSpeed = _colorSpeedCtrl->GetValue();
    const std::string outputPath = AppPaths::ToStdPath(outputFile.GetFullPath());

    wxProgressDialog progressDialog(_("Generating video..."), _("Please wait until the process is complete."), totalFrames, this);
    auto* renderer = new ZoomRenderer(outputPath, _fractalCanvasPtr, width, height, totalFrames, framerate,
                                      encodingOptions, colorSpeed);
    wxThreadError err = renderer->Create();

    if (err != wxTHREAD_NO_ERROR)
    {
        delete renderer;
        wxMessageBox(_("Couldn't create thread!"));
        return;
    }

    err = renderer->Run();

    if (err != wxTHREAD_NO_ERROR)
    {
        delete renderer;
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

    renderer->Wait();
    const std::string error = renderer->GetError();
    delete renderer;

    progressDialog.Show(false);
    if (!error.empty())
    {
        wxMessageBox(wxString::FromUTF8(error.c_str()), _("Could not save video"), wxOK | wxICON_ERROR, this);
        return;
    }

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

void ZoomRecorder::OnWidthChanged(wxSpinEvent&)
{
    if (_updatingResolution)
        return;

    _updatingResolution = true;
    const int requestedHeight = static_cast<int>(std::round(_widthSpinCtrl->GetValue() / _recordingAspectRatio));
    const int height = std::clamp(requestedHeight, _heightSpinCtrl->GetMin(), _heightSpinCtrl->GetMax());
    _heightSpinCtrl->SetValue(height);
    if (height != requestedHeight)
        _widthSpinCtrl->SetValue(static_cast<int>(std::round(height * _recordingAspectRatio)));
    _updatingResolution = false;
}

void ZoomRecorder::OnHeightChanged(wxSpinEvent&)
{
    if (_updatingResolution)
        return;

    _updatingResolution = true;
    const int requestedWidth = static_cast<int>(std::round(_heightSpinCtrl->GetValue() * _recordingAspectRatio));
    const int width = std::clamp(requestedWidth, _widthSpinCtrl->GetMin(), _widthSpinCtrl->GetMax());
    _widthSpinCtrl->SetValue(width);
    if (width != requestedWidth)
        _heightSpinCtrl->SetValue(static_cast<int>(std::round(width / _recordingAspectRatio)));
    _updatingResolution = false;
}
