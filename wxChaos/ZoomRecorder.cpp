#include "ZoomRecorder.h"
#include "Filesystem.h"

ZoomRecorder::ZoomRecorder(FractalCanvas* mFCanvas, wxWindow* parent, wxWindowID id,
                           const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxSize(628, 374), wxSize(894, 578));

    wxIcon icon(GetWxAbsPath({ "Resources", "icon.ico" }), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* previewAndButtonsSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* previewSizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Preview")), wxVERTICAL);

    previewBitmap = new wxStaticBitmap(previewSizer->GetStaticBox(), wxID_ANY, 
                                       wxBitmap(GetWxAbsPath({ "Resources", "fractal_thumbnail.png" }), wxBITMAP_TYPE_ANY),
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

    helpButton = new wxButton(panel, wxID_ANY, wxT("Instructions"), wxDefaultPosition, wxDefaultSize, 0);
    helpButton->SetBitmap(wxArtProvider::GetBitmap(wxART_QUESTION, wxART_TOOLBAR));
    buttonSizer->Add(helpButton, 0, wxALL, 5);

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

    resolutionText = new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Resolution:"), wxDefaultPosition, wxDefaultSize, 0);
    resolutionText->Wrap(-1);
    resolutionText->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));

    optionsSizer->Add(resolutionText, 0, wxALL, 5);

    wxString resolutionChoiceChoices[] = { wxT("3840x2160 (2160p)"), wxT("1900x1600 (1080p)"), wxT("1280x768 (720p)"), wxT("854x480 (480p)") };
    int resolutionChoiceNChoices = sizeof(resolutionChoiceChoices) / sizeof(wxString);
    resolutionChoice = new wxChoice(optionsSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, resolutionChoiceNChoices, resolutionChoiceChoices, 0);
    resolutionChoice->SetSelection(0);
    optionsSizer->Add(resolutionChoice, 0, wxALL | wxEXPAND, 5);

    rotateCheckbox = new wxCheckBox(optionsSizer->GetStaticBox(), wxID_ANY, wxT("Rotate colors"), wxDefaultPosition, wxDefaultSize, 0);
    optionsSizer->Add(rotateCheckbox, 0, wxALL, 5);
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
    helpButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ZoomRecorder::OnShowInstructions), NULL, this);
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
    helpButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ZoomRecorder::OnShowInstructions), NULL, this);
}

void ZoomRecorder::OnScrollPreview(wxScrollEvent& event)
{

}
void ZoomRecorder::OnSaveVideo(wxCommandEvent& event)
{

}
void ZoomRecorder::OnCancel(wxCommandEvent& event)
{

}
void ZoomRecorder::OnShowInstructions(wxCommandEvent& event)
{

}