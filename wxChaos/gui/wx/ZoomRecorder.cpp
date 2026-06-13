// ReSharper disable CppDFAUnreachableFunctionCall
#include <wx/progdlg.h>
#include <wx/bitmap.h>
#include <wx/artprov.h>
#include <wx/icon.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <utility>
#include "SFML/System.hpp"
#include "AppPaths.h"
#include "ZoomRecorder.h"
using namespace std;

/**
* @class ZoomRenderer
* @brief This is a worker thread that performs the zoom rendering.
*/
class ZoomRenderer : public wxThread
{
    FractalCanvas* fractalCanvasPtr;
    int currentFrame;
    int totalFrames;
    int width, height;
    double zoomSpeed, colorSpeed;
    string filepath;

    static string QuoteCommandArg(const string& value)
    {
        string quoted = "\"";
        for (const char ch : value)
        {
            if (ch == '"')
                quoted += "\\\"";
            else
                quoted += ch;
        }
        quoted += "\"";
        return quoted;
    }

protected:
    ExitCode Entry() override {
        // Create and set-up fractal handler
        FractalHandler fractalHandler;
        Rect outermostZoom, innermostZoom;

        FractalType fractalType = fractalCanvasPtr->GetFractalType();
        Options fractalOptions = fractalCanvasPtr->GetFractalPtr()->GetOptions();
        fractalOptions.screenWidth = width;
        fractalOptions.screenHeight = height;
        fractalOptions.xFactor = (fractalOptions.maxX - fractalOptions.minX) / (fractalOptions.screenWidth - 1);
        fractalOptions.yFactor = (fractalOptions.maxY - fractalOptions.minY) / (fractalOptions.screenHeight - 1);

        outermostZoom = fractalCanvasPtr->GetSFMLFractalPtr()->GetOutermostZoom();
        innermostZoom = fractalCanvasPtr->GetSFMLFractalPtr()->GetCurrentZoom();

        if (fractalType == FractalType::ScriptFractal)
        {
            auto scriptFractalPtr = reinterpret_cast<ScriptFractal*>(fractalCanvasPtr->GetFractalPtr());
            fractalHandler.CreateScriptFractal(width, height, scriptFractalPtr->GetPath());
        }
        else
            fractalHandler.CreateFractal(fractalType, width, height);

        fractalHandler.SetFormula(fractalCanvasPtr->GetFormula());
        fractalHandler.GetFractalPtr()->SetOptions(fractalOptions);

        // Render frames
        int outputFileDigits = int(log10(totalFrames) + 1);
        Vector2Double outermostLo = outermostZoom.GetLowerBound();
        Vector2Double outermostHi = outermostZoom.GetUpperBound();
        Vector2Double innermostLo = innermostZoom.GetLowerBound();
        Vector2Double innermostHi = innermostZoom.GetUpperBound();

        for (currentFrame = 0; currentFrame < totalFrames; currentFrame++)
        {
            double t = currentFrame;
            Rect viewport;
            viewport.SetLowerBound(outermostLo + (1 - exp(-zoomSpeed * t / totalFrames)) * (innermostLo - outermostLo));
            viewport.SetUpperBound(outermostHi - (1 - exp(-zoomSpeed * t / totalFrames)) * (outermostHi - innermostHi));

            fractalHandler.GetFractalPtr()->SetView(viewport);

            if (colorSpeed != -1)
                fractalHandler.GetFractalPtr()->SetVarGradient(static_cast<int>(colorSpeed * t));
            else
                fractalHandler.GetFractalPtr()->SetVarGradient(0);

            sf::Image out = fractalHandler.GetFractalPtr()->GetRenderedImage();
            string filename = "frame_" + FixedLengthToString(currentFrame, outputFileDigits) + ".jpg";
            string fullPath = AppPaths::JoinStd(filepath, filename);

            // ReSharper disable once CppExpressionWithoutSideEffects
            out.saveToFile(fullPath);
        }

        // Render video from frames.
        const string ffmpegPath = AppPaths::FfmpegFileStd();
        const string fileTemplate = "frame_%0" + to_string(outputFileDigits) + "d.jpg";
        const string inputFrames = AppPaths::JoinStd(filepath, fileTemplate);
        const string outputVideo = AppPaths::JoinStd(filepath, "Zoom.mp4");
        const string renderVideoCommand = QuoteCommandArg(ffmpegPath) + " -i " + QuoteCommandArg(inputFrames) +
            " -c:v libx264 -vf fps=30 -vf \"crop = trunc(iw / 2) * 2:trunc(ih / 2) * 2\" -pix_fmt yuv420p " + QuoteCommandArg(outputVideo);

        system(renderVideoCommand.c_str());

        return nullptr;
    }

public:
    ZoomRenderer(string p_filepath, FractalCanvas* p_fcanvas, int p_width, int p_height, int p_total_frames,
                 double p_zoom_speed, double p_color_speed)
    {
        filepath = std::move(p_filepath);
        fractalCanvasPtr = p_fcanvas;
        currentFrame = 0;
        totalFrames = p_total_frames;
        width = p_width;
        height = p_height;
        zoomSpeed = p_zoom_speed;
        colorSpeed = p_color_speed;
    }

    static string FixedLengthToString(const int i, const int length)
    {
        ostringstream ostr;
        if (i < 0)
            ostr << '-';

        ostr << setfill('0') << setw(length) << (i < 0 ? -i : i);
        return ostr.str();
    }

    int GetProgress() const {
        return currentFrame;
    }
};

// ZoomRecorder implementation.
ZoomRecorder::ZoomRecorder(FractalCanvas* fractalCanvas, wxWindow* parent, const wxWindowID id, const wxString& title,
                           const wxPoint& pos, const wxSize& size, const long style) : wxDialog(parent, id, title, pos, size, style)
{
    // Fractal handler initialization
    _fractalCanvasPtr = fractalCanvas;
    this->CreateFractalHandler();
    _fractalHandler.GetFractalPtr()->SetView(_outermostZoom);

    // UI initialization
    this->SetSizeHints(wxSize(900, 680), wxSize(1400, 900));

    const wxIcon icon(AppPaths::ResourceFile({wxT("icon.ico")}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);

    _panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto previewAndButtonsSizer = new wxBoxSizer(wxVERTICAL);
    const auto previewSizer = new wxStaticBoxSizer(new wxStaticBox(_panel, wxID_ANY, wxT("Preview")), wxVERTICAL);

    _previewBitmap = new wxStaticBitmap(previewSizer->GetStaticBox(), wxID_ANY,
        _fractalHandler.GetFractalPtr()->GetRenderedWxBitmap(),
        wxDefaultPosition, wxDefaultSize, 0);
    previewSizer->Add(_previewBitmap, 0, wxALL | wxEXPAND, 5);

    _previewFrameText = new wxStaticText(previewSizer->GetStaticBox(), wxID_ANY, wxT("Frame:"), wxDefaultPosition, wxDefaultSize, 0);
    _previewFrameText->Wrap(-1);
    previewSizer->Add(_previewFrameText, 0, wxALL, 5);

    _previewSlider = new wxSlider(previewSizer->GetStaticBox(), wxID_ANY, 0, 0, 1800, wxDefaultPosition, wxDefaultSize,
        wxSL_AUTOTICKS | wxSL_BOTTOM | wxSL_HORIZONTAL | wxSL_LABELS);
    previewSizer->Add(_previewSlider, 0, wxALL | wxEXPAND, 5);
    previewAndButtonsSizer->Add(previewSizer, 1, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    _saveButton = new wxButton(_panel, wxID_ANY, wxT("Save video"), wxDefaultPosition, wxDefaultSize, 0);
    _saveButton->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR));
    buttonSizer->Add(_saveButton, 0, wxALL, 5);

    _cancelButton = new wxButton(_panel, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    _cancelButton->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE, wxART_TOOLBAR));
    buttonSizer->Add(_cancelButton, 0, wxALL, 5);

    previewAndButtonsSizer->Add(buttonSizer, 0, 0, 5);
    panelSizer->Add(previewAndButtonsSizer, 0, wxEXPAND, 5);

    auto* optionsSizer = new wxStaticBoxSizer(new wxStaticBox(_panel, wxID_ANY, wxT("Options")), wxVERTICAL);

    _videoDurationText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Video duration:"), wxDefaultPosition, wxDefaultSize, 0);
    _videoDurationText->Wrap(-1);
    optionsSizer->Add(_videoDurationText, 0, wxALL, 5);

    const auto videoDurationSizer = new wxBoxSizer(wxHORIZONTAL);

    _minutesSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 1);
    _minutesSpinCtrl->SetMaxSize(wxSize(70, -1));

    videoDurationSizer->Add(_minutesSpinCtrl, 0, wxTOP | wxBOTTOM, 5);

    _minutesText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Minutes"), wxDefaultPosition, wxDefaultSize, 0);
    _minutesText->Wrap(-1);
    videoDurationSizer->Add(_minutesText, 0, wxALL, 5);

    _secondsSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 0);
    _secondsSpinCtrl->SetMaxSize(wxSize(70, -1));

    videoDurationSizer->Add(_secondsSpinCtrl, 0, wxTOP | wxBOTTOM | wxRIGHT, 5);

    _secondsText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Seconds"), wxDefaultPosition, wxDefaultSize, 0);
    _secondsText->Wrap(-1);
    videoDurationSizer->Add(_secondsText, 0, wxALL, 5);
    optionsSizer->Add(videoDurationSizer, 0, wxLEFT, 5);

    _framerateText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Framerate:"), wxDefaultPosition, wxDefaultSize, 0);
    _framerateText->Wrap(-1);
    optionsSizer->Add(_framerateText, 0, wxALL, 5);

    const auto framerateSize = new wxBoxSizer(wxHORIZONTAL);

    _framerateSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 120, 30);
    framerateSize->Add(_framerateSpinCtrl, 0, wxALL, 5);

    _framesPerSecondText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Frames Per Second"), wxDefaultPosition, wxDefaultSize, 0);
    _framesPerSecondText->Wrap(-1);
    framerateSize->Add(_framesPerSecondText, 0, wxALL, 5);
    optionsSizer->Add(framerateSize, 0, wxEXPAND, 5);

    _zoomSpeedText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Zoom speed:"), wxDefaultPosition, wxDefaultSize, 0);
    _zoomSpeedText->Wrap(-1);
    optionsSizer->Add(_zoomSpeedText, 0, wxALL, 5);

    _zoomSpeedCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 4);
    optionsSizer->Add(_zoomSpeedCtrl, 0, wxALL, 5);

    _rotateCheckbox = new wxCheckBox(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Rotate colors"), wxDefaultPosition, wxDefaultSize, 0);
    optionsSizer->Add(_rotateCheckbox, 0, wxALL, 5);

    _colorRotateSpeedText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Color rotation speed:"), wxDefaultPosition, wxDefaultSize, 0);
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
    _zoomSpeedCtrl->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnChangeSpeed, this);
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
    _zoomSpeedCtrl->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ZoomRecorder::OnChangeSpeed, this);
    _rotateCheckbox->Unbind(wxEVT_COMMAND_CHECKBOX_CLICKED, &ZoomRecorder::OnColorRotate, this);
    _colorSpeedCtrl->Unbind(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, &ZoomRecorder::OnChangeSpeedDbl, this);
}

void ZoomRecorder::CreateFractalHandler()
{
    FractalType fractalType = _fractalCanvasPtr->GetFractalType();
    Options fractalOptions = _fractalCanvasPtr->GetFractalPtr()->GetOptions();
    fractalOptions.screenWidth = 250;
    fractalOptions.screenHeight = 166;
    fractalOptions.xFactor = (fractalOptions.maxX - fractalOptions.minX) / (fractalOptions.screenWidth - 1);
    fractalOptions.yFactor = (fractalOptions.maxY - fractalOptions.minY) / (fractalOptions.screenHeight - 1);

    _outermostZoom = _fractalCanvasPtr->GetSFMLFractalPtr()->GetOutermostZoom();
    _innermostZoom = _fractalCanvasPtr->GetSFMLFractalPtr()->GetCurrentZoom();

    if (fractalType == FractalType::ScriptFractal)
    {
        auto* scriptFractalPtr = reinterpret_cast<ScriptFractal*>(_fractalCanvasPtr->GetFractalPtr());
        _fractalHandler.CreateScriptFractal(250, 166, scriptFractalPtr->GetPath());
    }
    else
        _fractalHandler.CreateFractal(fractalType, 250, 166);

    _fractalHandler.SetFormula(_fractalCanvasPtr->GetFormula());

    // Copy parameters.
    _fractalHandler.GetFractalPtr()->SetOptions(fractalOptions);
}
void ZoomRecorder::RenderPreview(const int zoom, const int zoomSpeed, const double colorSpeed) const
{
    const double totalFrames = this->GetTotalFrames();
    const double zoomSpeedFloat = zoomSpeed;

    Vector2Double outermostLo = _outermostZoom.GetLowerBound();
    Vector2Double outermostHi = _outermostZoom.GetUpperBound();
    Vector2Double innermostLo = _innermostZoom.GetLowerBound();
    Vector2Double innermostHi = _innermostZoom.GetUpperBound();

    const double t = zoom;
    Rect viewport;
    viewport.SetLowerBound(outermostLo + (1 - exp(-zoomSpeedFloat * t / totalFrames)) * (innermostLo - outermostLo));
    viewport.SetUpperBound(outermostHi - (1 - exp(-zoomSpeedFloat * t / totalFrames)) * (outermostHi - innermostHi));

    _fractalHandler.GetFractalPtr()->SetView(viewport);

    if (colorSpeed != -1)
        _fractalHandler.GetFractalPtr()->SetVarGradient(static_cast<int>(colorSpeed * t));
    else
        _fractalHandler.GetFractalPtr()->SetVarGradient(0);

    _previewBitmap->SetBitmap(_fractalHandler.GetFractalPtr()->GetRenderedWxBitmap());
}
void ZoomRecorder::RenderPreview()
{
    if (_rotateCheckbox->GetValue())
        this->RenderPreview(_previewSlider->GetValue(), _zoomSpeedCtrl->GetValue(), _colorSpeedCtrl->GetValue());
    else
        this->RenderPreview(_previewSlider->GetValue(), _zoomSpeedCtrl->GetValue());
}
int ZoomRecorder::GetTotalFrames() const
{
    const int seconds = _secondsSpinCtrl->GetValue();
    const int minutes = _minutesSpinCtrl->GetValue();
    const int framerate = _framerateSpinCtrl->GetValue();

    return (60 * minutes + seconds) * framerate;
}
void ZoomRecorder::UpdateTotalFrames()
{
    const int totalFrames = this->GetTotalFrames();
    _previewSlider->SetMax(totalFrames);
    _previewSlider->SetValue(0);
    this->RenderPreview(0);
}

// ZoomRecorder events.

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ZoomRecorder::OnScrollPreview(wxScrollEvent& event)
{
    if (_rotateCheckbox->GetValue())
        this->RenderPreview(event.GetPosition(), _zoomSpeedCtrl->GetValue(), _colorSpeedCtrl->GetValue());
    else
        this->RenderPreview(event.GetPosition(), _zoomSpeedCtrl->GetValue());
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
    const double zoomSpeed = _zoomSpeedCtrl->GetValue();
    const double colorSpeed = _colorSpeedCtrl->GetValue();
    // wxString::mb_str() returns a wxCharBuffer which cannot be implicitly
    // converted to std::string on GCC.  Explicitly construct the std::string
    // from the buffer.
    const std::string selectedDirPath(selectedFile.mb_str());

    wxProgressDialog progressDialog(wxT("Generating video..."), wxT("Please wait until the process is complete."), totalFrames, this);
    auto* renderer = new ZoomRenderer(selectedDirPath, _fractalCanvasPtr, 2500, 1660, totalFrames, zoomSpeed, colorSpeed);
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
    _fractalHandler.DeleteFractal();
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
void ZoomRecorder::OnChangeSpeed(wxSpinEvent&)
{
    this->RenderPreview();
}
void ZoomRecorder::OnChangeSpeedDbl(wxSpinDoubleEvent&)
{
    this->RenderPreview();
}
