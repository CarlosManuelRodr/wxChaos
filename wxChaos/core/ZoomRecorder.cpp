#include <wx/progdlg.h>
#include <iomanip>
#include <SFML/System.hpp>
#include <cstdlib>
#include "ZoomRecorder.h"
#include "Filesystem.h"
using namespace std;

/**
* @class ZoomRenderer
* @brief This is a worker thread that performs the zoom rendering.
*/
class ZoomRenderer : public wxThread
{
private:
    FractalCanvas* fractalCanvasPtr;
    int currentFrame;
    int totalFrames;
    int width, height;
    double zoomSpeed, colorSpeed;
    string filepath;
public:
    ZoomRenderer(string p_filepath, FractalCanvas* p_fcanvas, int p_width, int p_height, int p_total_frames,
        double p_zoom_speed, double p_color_speed)
    {
        filepath = p_filepath;
        fractalCanvasPtr = p_fcanvas;
        currentFrame = 0;
        totalFrames = p_total_frames;
        width = p_width;
        height = p_height;
        zoomSpeed = p_zoom_speed;
        colorSpeed = p_color_speed;
    }

    string FixedLengthToString(const int i, const int length)
    {
        ostringstream ostr;
        if (i < 0)
            ostr << '-';

        ostr << setfill('0') << setw(length) << (i < 0 ? -i : i);
        return ostr.str();
    }

    int GetProgress()
    {
        return currentFrame;
    }

    ExitCode Entry()
    {
        // Create and set-up fractal handler
        FractalHandler fractalHandler;
        Rect outermostZoom, innermostZoom;

        FractalType fractalType = fractalCanvasPtr->GetFractalType();
        Options fractalOptions = fractalCanvasPtr->GetFractalPtr()->GetOptions();
        fractalOptions.screenWidth = width;
        fractalOptions.screenHeight = height;
        fractalOptions.xFactor = (fractalOptions.maxX - fractalOptions.minX) / (fractalOptions.screenWidth - 1);
        fractalOptions.yFactor = (fractalOptions.maxY - fractalOptions.minY) / (fractalOptions.screenHeight - 1);

        outermostZoom = fractalCanvasPtr->GetFractalPtr()->GetOutermostZoom();
        innermostZoom = fractalCanvasPtr->GetFractalPtr()->GetCurrentZoom();

        if (fractalType == FractalType::ScriptFractal)
        {
            ScriptFractal* scriptFractalPtr = reinterpret_cast<ScriptFractal*>(fractalCanvasPtr->GetFractalPtr());
            fractalHandler.CreateScriptFractal(width, height, scriptFractalPtr->GetPath());
        }
        else
            fractalHandler.CreateFractal(fractalType, width, height);

        fractalHandler.SetFormula(fractalCanvasPtr->GetFormula());
        fractalHandler.GetFractalPtr()->SetOptions(fractalOptions);

        // Render frames
        int outputFileDigits = int(log10(totalFrames) + 1);
        Vector2Double outermostLo = outermostZoom.GetLowerBound();
        Vector2Double outermostHi = outermostZoom.GetHigherBound();
        Vector2Double innermostLo = innermostZoom.GetLowerBound();
        Vector2Double innermostHi = innermostZoom.GetHigherBound();

        for (currentFrame = 0; currentFrame < totalFrames; currentFrame++)
        {
            double t = static_cast<double>(currentFrame);
            Rect viewport;
            viewport.SetLowerBound(outermostLo + (1 - exp(-zoomSpeed * t / totalFrames)) * (innermostLo - outermostLo));
            viewport.SetHigherBound(outermostHi - (1 - exp(-zoomSpeed * t / totalFrames)) * (outermostHi - innermostHi));

            fractalHandler.GetFractalPtr()->SetAreaOfView(viewport);

            if (colorSpeed != -1)
                fractalHandler.GetFractalPtr()->SetVarGradient(colorSpeed * t);
            else
                fractalHandler.GetFractalPtr()->SetVarGradient(0);

            sf::Image out = fractalHandler.GetFractalPtr()->GetRenderedImage();
            string filename = "frame_" + FixedLengthToString(currentFrame, outputFileDigits) + ".jpg";
            string fullPath = FileNameJoin({ filepath, filename });
            out.saveToFile(fullPath);
        }

        // Render video from frames.
        const string ffmpegPath = GetAbsPath({ "FFmpeg", "ffmpeg.exe" });
        const string fileTemplate = "frame_%0" + to_string(outputFileDigits) + "d.jpg";
        const string inputFrames = FileNameJoin({ filepath, fileTemplate });
        const string outputVideo = FileNameJoin({ filepath, "Zoom.mp4" });
        const string renderVideoCommand = ffmpegPath + " -i " + inputFrames +
            " -c:v libx264 -vf fps=30 -vf \"crop = trunc(iw / 2) * 2:trunc(ih / 2) * 2\" -pix_fmt yuv420p " + outputVideo;

        system(renderVideoCommand.c_str());

        return 0;
    }
};

// ZoomRecorder implementation.
ZoomRecorder::ZoomRecorder(FractalCanvas* mFCanvas, wxWindow* parent, wxWindowID id,
    const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    // Fractal handler initialization
    fractalCanvasPtr = mFCanvas;
    this->CreateFractalHandler();
    fractalHandler.GetFractalPtr()->SetAreaOfView(outermostZoom);

    // UI initialization
    this->SetSizeHints(wxSize(600, 374), wxSize(1200, 578));

    wxIcon icon(GetWxAbsPath({ "Resources", "icon.ico" }), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* previewAndButtonsSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* previewSizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Preview")), wxVERTICAL);

    previewBitmap = new wxStaticBitmap(previewSizer->GetStaticBox(), wxID_ANY,
        fractalHandler.GetFractalPtr()->GetRenderedWxBitmap(),
        wxDefaultPosition, wxDefaultSize, 0);
    previewSizer->Add(previewBitmap, 0, wxALL | wxEXPAND, 5);

    previewFrameText = new wxStaticText(previewSizer->GetStaticBox(), wxID_ANY, wxT("Frame:"), wxDefaultPosition, wxDefaultSize, 0);
    previewFrameText->Wrap(-1);
    previewSizer->Add(previewFrameText, 0, wxALL, 5);

    previewSlider = new wxSlider(previewSizer->GetStaticBox(), wxID_ANY, 0, 0, 1800, wxDefaultPosition, wxDefaultSize,
        wxSL_AUTOTICKS | wxSL_BOTTOM | wxSL_HORIZONTAL | wxSL_LABELS);
    previewSizer->Add(previewSlider, 0, wxALL | wxEXPAND, 5);
    previewAndButtonsSizer->Add(previewSizer, 1, wxEXPAND, 5);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    saveButton = new wxButton(panel, wxID_ANY, wxT("Save video"), wxDefaultPosition, wxDefaultSize, 0);
    saveButton->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR));
    buttonSizer->Add(saveButton, 0, wxALL, 5);

    cancelButton = new wxButton(panel, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    cancelButton->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE, wxART_TOOLBAR));
    buttonSizer->Add(cancelButton, 0, wxALL, 5);

    // TODO: Implement help menu.
    /*helpButton = new wxButton(panel, wxID_ANY, wxT("Instructions"), wxDefaultPosition, wxDefaultSize, 0);
    helpButton->SetBitmap(wxArtProvider::GetBitmap(wxART_QUESTION, wxART_TOOLBAR));
    buttonSizer->Add(helpButton, 0, wxALL, 5);*/

    previewAndButtonsSizer->Add(buttonSizer, 0, 0, 5);
    panelSizer->Add(previewAndButtonsSizer, 0, wxEXPAND, 5);

    wxStaticBoxSizer* optionsSizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Options")), wxVERTICAL);

    videoDurationText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Video duration:"), wxDefaultPosition, wxDefaultSize, 0);
    videoDurationText->Wrap(-1);
    optionsSizer->Add(videoDurationText, 0, wxALL, 5);

    wxBoxSizer* videoDurationSizer = new wxBoxSizer(wxHORIZONTAL);

    minutesSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 1);
    minutesSpinCtrl->SetMaxSize(wxSize(70, -1));

    videoDurationSizer->Add(minutesSpinCtrl, 0, wxTOP | wxBOTTOM, 5);

    minutesText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Minutes"), wxDefaultPosition, wxDefaultSize, 0);
    minutesText->Wrap(-1);
    videoDurationSizer->Add(minutesText, 0, wxALL, 5);

    secondsSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 0);
    secondsSpinCtrl->SetMaxSize(wxSize(70, -1));

    videoDurationSizer->Add(secondsSpinCtrl, 0, wxTOP | wxBOTTOM | wxRIGHT, 5);

    secondsText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Seconds"), wxDefaultPosition, wxDefaultSize, 0);
    secondsText->Wrap(-1);
    videoDurationSizer->Add(secondsText, 0, wxALL, 5);
    optionsSizer->Add(videoDurationSizer, 0, wxLEFT, 5);

    framerateText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Framerate:"), wxDefaultPosition, wxDefaultSize, 0);
    framerateText->Wrap(-1);
    optionsSizer->Add(framerateText, 0, wxALL, 5);

    wxBoxSizer* framerateSize = new wxBoxSizer(wxHORIZONTAL);

    framerateSpinCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 120, 30);
    framerateSize->Add(framerateSpinCtrl, 0, wxALL, 5);

    framesPerSecondText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Frames Per Second"), wxDefaultPosition, wxDefaultSize, 0);
    framesPerSecondText->Wrap(-1);
    framerateSize->Add(framesPerSecondText, 0, wxALL, 5);
    optionsSizer->Add(framerateSize, 0, wxEXPAND, 5);

    zoomSpeedText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Zoom speed:"), wxDefaultPosition, wxDefaultSize, 0);
    zoomSpeedText->Wrap(-1);
    optionsSizer->Add(zoomSpeedText, 0, wxALL, 5);

    zoomSpeedCtrl = new wxSpinCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 4);
    optionsSizer->Add(zoomSpeedCtrl, 0, wxALL, 5);

    rotateCheckbox = new wxCheckBox(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Rotate colors"), wxDefaultPosition, wxDefaultSize, 0);
    optionsSizer->Add(rotateCheckbox, 0, wxALL, 5);

    colorRotateSpeedText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Color rotation speed:"), wxDefaultPosition, wxDefaultSize, 0);
    colorRotateSpeedText->Wrap(-1);
    optionsSizer->Add(colorRotateSpeedText, 0, wxALL, 5);

    colorSpeedCtrl = new wxSpinCtrlDouble(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 5, 0.1, 0.1);
    colorSpeedCtrl->SetDigits(2);
    optionsSizer->Add(colorSpeedCtrl, 0, wxALL, 5);

    panelSizer->Add(optionsSizer, 1, wxEXPAND, 5);

    panel->SetSizer(panelSizer);
    panel->Layout();
    panelSizer->Fit(panel);
    mainSizer->Add(panel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainSizer);
    this->Layout();
    this->Centre(wxBOTH);

    // Connect Events
    previewSlider->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    saveButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ZoomRecorder::OnSaveVideo), NULL, this);
    cancelButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ZoomRecorder::OnCancel), NULL, this);
    //helpButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ZoomRecorder::OnShowInstructions), NULL, this);
    minutesSpinCtrl->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ZoomRecorder::OnUpdateTotalFrames), NULL, this);
    secondsSpinCtrl->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ZoomRecorder::OnUpdateTotalFrames), NULL, this);
    framerateSpinCtrl->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ZoomRecorder::OnUpdateTotalFrames), NULL, this);
    zoomSpeedCtrl->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ZoomRecorder::OnChangeSpeed), NULL, this);
    rotateCheckbox->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ZoomRecorder::OnColorRotate), NULL, this);
    colorSpeedCtrl->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(ZoomRecorder::OnChangeSpeedDbl), NULL, this);
}

ZoomRecorder::~ZoomRecorder()
{
    // Disconnect Events
    previewSlider->Disconnect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Disconnect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Disconnect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Disconnect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Disconnect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Disconnect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Disconnect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    previewSlider->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ZoomRecorder::OnScrollPreview), NULL, this);
    saveButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ZoomRecorder::OnSaveVideo), NULL, this);
    cancelButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ZoomRecorder::OnCancel), NULL, this);
    //helpButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ZoomRecorder::OnShowInstructions), NULL, this);
    minutesSpinCtrl->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ZoomRecorder::OnUpdateTotalFrames), NULL, this);
    secondsSpinCtrl->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ZoomRecorder::OnUpdateTotalFrames), NULL, this);
    framerateSpinCtrl->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ZoomRecorder::OnUpdateTotalFrames), NULL, this);
    zoomSpeedCtrl->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ZoomRecorder::OnChangeSpeed), NULL, this);
    rotateCheckbox->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ZoomRecorder::OnColorRotate), NULL, this);
    colorSpeedCtrl->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(ZoomRecorder::OnChangeSpeedDbl), NULL, this);
}

void ZoomRecorder::CreateFractalHandler()
{
    FractalType fractalType = fractalCanvasPtr->GetFractalType();
    Options fractalOptions = fractalCanvasPtr->GetFractalPtr()->GetOptions();
    fractalOptions.screenWidth = 250;
    fractalOptions.screenHeight = 166;
    fractalOptions.xFactor = (fractalOptions.maxX - fractalOptions.minX) / (fractalOptions.screenWidth - 1);
    fractalOptions.yFactor = (fractalOptions.maxY - fractalOptions.minY) / (fractalOptions.screenHeight - 1);

    outermostZoom = fractalCanvasPtr->GetFractalPtr()->GetOutermostZoom();
    innermostZoom = fractalCanvasPtr->GetFractalPtr()->GetCurrentZoom();

    if (fractalType == FractalType::ScriptFractal)
    {
        ScriptFractal* scriptFractalPtr = reinterpret_cast<ScriptFractal*>(fractalCanvasPtr->GetFractalPtr());
        fractalHandler.CreateScriptFractal(250, 166, scriptFractalPtr->GetPath());
    }
    else
        fractalHandler.CreateFractal(fractalType, 250, 166);

    fractalHandler.SetFormula(fractalCanvasPtr->GetFormula());

    // Copy parameters.
    fractalHandler.GetFractalPtr()->SetOptions(fractalOptions);
}
void ZoomRecorder::RenderPreview(int zoom, int zoomSpeed, double colorSpeed)
{
    const double totalFrames = this->CalculateTotalFrames();
    const double zoomSpeedFloat = static_cast<double>(zoomSpeed);

    Vector2Double outermostLo = outermostZoom.GetLowerBound();
    Vector2Double outermostHi = outermostZoom.GetHigherBound();
    Vector2Double innermostLo = innermostZoom.GetLowerBound();
    Vector2Double innermostHi = innermostZoom.GetHigherBound();

    double t = static_cast<double>(zoom);
    Rect viewport;
    viewport.SetLowerBound(outermostLo + (1 - exp(-zoomSpeedFloat * t / totalFrames)) * (innermostLo - outermostLo));
    viewport.SetHigherBound(outermostHi - (1 - exp(-zoomSpeedFloat * t / totalFrames)) * (outermostHi - innermostHi));

    fractalHandler.GetFractalPtr()->SetAreaOfView(viewport);

    if (colorSpeed != -1)
        fractalHandler.GetFractalPtr()->SetVarGradient(colorSpeed * t);
    else
        fractalHandler.GetFractalPtr()->SetVarGradient(0);

    previewBitmap->SetBitmap(fractalHandler.GetFractalPtr()->GetRenderedWxBitmap());
}
void ZoomRecorder::RenderPreview()
{
    if (rotateCheckbox->GetValue())
        this->RenderPreview(previewSlider->GetValue(), zoomSpeedCtrl->GetValue(), colorSpeedCtrl->GetValue());
    else
        this->RenderPreview(previewSlider->GetValue(), zoomSpeedCtrl->GetValue());
}
int ZoomRecorder::CalculateTotalFrames()
{
    const int seconds = secondsSpinCtrl->GetValue();
    const int minutes = minutesSpinCtrl->GetValue();
    const int framerate = framerateSpinCtrl->GetValue();

    return (60 * minutes + seconds) * framerate;
}
void ZoomRecorder::UpdateTotalFrames()
{
    const int totalFrames = this->CalculateTotalFrames();
    previewSlider->SetMax(totalFrames);
    previewSlider->SetValue(0);
    this->RenderPreview(0);
}

// ZoomRecorder events.
void ZoomRecorder::OnScrollPreview(wxScrollEvent& event)
{
    if (rotateCheckbox->GetValue())
        this->RenderPreview(event.GetPosition(), zoomSpeedCtrl->GetValue(), colorSpeedCtrl->GetValue());
    else
        this->RenderPreview(event.GetPosition(), zoomSpeedCtrl->GetValue());
}
void ZoomRecorder::OnSaveVideo(wxCommandEvent& event)
{
    // Select the output directory
    wxDirDialog* dirDialog = new wxDirDialog(this);
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
    const int totalFrames = this->CalculateTotalFrames();
    const double zoomSpeed = zoomSpeedCtrl->GetValue();
    const double colorSpeed = colorSpeedCtrl->GetValue();
    string selectedDirPath = selectedFile.mb_str();

    wxProgressDialog progressDialog(wxT("Generating video..."), wxT("Please wait until the process is complete."), totalFrames, this);
    ZoomRenderer* renderer = new ZoomRenderer(selectedDirPath, fractalCanvasPtr, 2500, 1660, totalFrames, zoomSpeed, colorSpeed);
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
void ZoomRecorder::OnCancel(wxCommandEvent& event)
{
    fractalHandler.DeleteFractal();
    this->EndModal(0);
}
void ZoomRecorder::OnUpdateTotalFrames(wxSpinEvent& event)
{
    this->UpdateTotalFrames();
}
void ZoomRecorder::OnColorRotate(wxCommandEvent& event)
{
    this->RenderPreview();
}
void ZoomRecorder::OnChangeSpeed(wxSpinEvent& event)
{
    this->RenderPreview();
}
void ZoomRecorder::OnChangeSpeedDbl(wxSpinDoubleEvent& event)
{
    this->RenderPreview();
}