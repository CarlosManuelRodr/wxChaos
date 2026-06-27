// ReSharper disable CppDFAUnreachableFunctionCall
#include <wx/progdlg.h>
#include <wx/bitmap.h>
#include <wx/artprov.h>
#include <wx/icon.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include "SFML/System.hpp"
#include "AppPaths.h"
#include "ZoomRecorder.h"
#include "ZoomRenderer.h"

// ZoomRecorder implementation.
ZoomRecorder::ZoomRecorder(FractalCanvas* fractalCanvas, wxWindow* parent, const wxWindowID id, const wxString& title,
                           const wxPoint& pos, const wxSize& size, const long style) : wxDialog(parent, id, title, pos, size, style)
{
    // fractal factory initialization
    _fractalCanvasPtr = fractalCanvas;
    this->CreateFractalFactory();
    _fractalFactory.GetFractal()->SetPreciseView(_outermostZoom);

    // UI initialization
    this->SetSizeHints(wxSize(900, 680), wxSize(1400, 900));

    const wxIcon icon(AppPaths::ResourceFile({"icon.ico"}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);

    _panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto previewAndButtonsSizer = new wxBoxSizer(wxVERTICAL);
    const auto previewSizer = new wxStaticBoxSizer(new wxStaticBox(_panel, wxID_ANY, "Preview"), wxVERTICAL);

    _previewBitmap = new wxStaticBitmap(previewSizer->GetStaticBox(), wxID_ANY,
        _fractalFactory.GetFractal()->GetRenderedWxBitmap(),
        wxDefaultPosition, wxDefaultSize, 0);
    previewSizer->Add(_previewBitmap, 0, wxALL | wxEXPAND, 5);

    _previewFrameText = new wxStaticText(previewSizer->GetStaticBox(), wxID_ANY, "Frame:", wxDefaultPosition, wxDefaultSize, 0);
    _previewFrameText->Wrap(-1);
    previewSizer->Add(_previewFrameText, 0, wxALL, 5);

    _previewSlider = new wxSlider(previewSizer->GetStaticBox(), wxID_ANY, 0, 0, 1799, wxDefaultPosition, wxDefaultSize,
        wxSL_AUTOTICKS | wxSL_BOTTOM | wxSL_HORIZONTAL | wxSL_LABELS);
    previewSizer->Add(_previewSlider, 0, wxALL | wxEXPAND, 5);
    previewAndButtonsSizer->Add(previewSizer, 1, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    _saveButton = new wxButton(_panel, wxID_ANY, "Save video", wxDefaultPosition, wxDefaultSize, 0);
    _saveButton->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR));
    buttonSizer->Add(_saveButton, 0, wxALL, 5);

    _cancelButton = new wxButton(_panel, wxID_ANY, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
    _cancelButton->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE, wxART_TOOLBAR));
    buttonSizer->Add(_cancelButton, 0, wxALL, 5);

    previewAndButtonsSizer->Add(buttonSizer, 0, 0, 5);
    panelSizer->Add(previewAndButtonsSizer, 0, wxEXPAND, 5);

    auto* optionsSizer = new wxStaticBoxSizer(new wxStaticBox(_panel, wxID_ANY, "Options"), wxVERTICAL);

    _videoDurationText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, "Video duration:", wxDefaultPosition, wxDefaultSize, 0);
    _videoDurationText->Wrap(-1);
    optionsSizer->Add(_videoDurationText, 0, wxALL, 5);

    const auto videoDurationSizer = new wxBoxSizer(wxHORIZONTAL);

    _minutesSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 1);
    _minutesSpinCtrl->SetMaxSize(wxSize(70, -1));

    videoDurationSizer->Add(_minutesSpinCtrl, 0, wxTOP | wxBOTTOM, 5);

    _minutesText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, "Minutes", wxDefaultPosition, wxDefaultSize, 0);
    _minutesText->Wrap(-1);
    videoDurationSizer->Add(_minutesText, 0, wxALL, 5);

    _secondsSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 0);
    _secondsSpinCtrl->SetMaxSize(wxSize(70, -1));

    videoDurationSizer->Add(_secondsSpinCtrl, 0, wxTOP | wxBOTTOM | wxRIGHT, 5);

    _secondsText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, "Seconds", wxDefaultPosition, wxDefaultSize, 0);
    _secondsText->Wrap(-1);
    videoDurationSizer->Add(_secondsText, 0, wxALL, 5);
    optionsSizer->Add(videoDurationSizer, 0, wxLEFT, 5);

    _framerateText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, "Framerate:", wxDefaultPosition, wxDefaultSize, 0);
    _framerateText->Wrap(-1);
    optionsSizer->Add(_framerateText, 0, wxALL, 5);

    const auto framerateSize = new wxBoxSizer(wxHORIZONTAL);

    _framerateSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 120, 30);
    framerateSize->Add(_framerateSpinCtrl, 0, wxALL, 5);

    _framesPerSecondText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, "Frames Per Second", wxDefaultPosition, wxDefaultSize, 0);
    _framesPerSecondText->Wrap(-1);
    framerateSize->Add(_framesPerSecondText, 0, wxALL, 5);
    optionsSizer->Add(framerateSize, 0, wxEXPAND, 5);

    _rotateCheckbox = new wxCheckBox(optionsSizer->GetStaticBox(), wxID_ANY, "Rotate colors", wxDefaultPosition, wxDefaultSize, 0);
    optionsSizer->Add(_rotateCheckbox, 0, wxALL, 5);

    _colorRotateSpeedText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, "Color rotation speed:", wxDefaultPosition, wxDefaultSize, 0);
    _colorRotateSpeedText->Wrap(-1);
    optionsSizer->Add(_colorRotateSpeedText, 0, wxALL, 5);

    _colorSpeedCtrl = new wxSpinCtrlDouble(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 5, 0.1, 0.1);
    _colorSpeedCtrl->SetDigits(2);
    optionsSizer->Add(_colorSpeedCtrl, 0, wxALL, 5);

    panelSizer->Add(optionsSizer, 1, wxEXPAND, 5);

    _panel->SetSizer(panelSizer);
    _panel->Layout();
    panelSizer->Fit(_panel);
    mainSizer->Add(_panel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainSizer);
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

PreciseRect ZoomRecorder::CreateRecordingFractal(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, const int width, const int height)
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
