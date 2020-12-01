#include "SizeDialogSave.h"
#include "StringFuncs.h"

// SaveProgressDiag
SaveProgressDiag::SaveProgressDiag(Fractal* targetFractal, wxWindow* parent, bool _saveProgressAvailable, wxWindowID id,
                                   const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    // WX Dialog.
    myFractal = targetFractal;
    saveProgressAvailable = _saveProgressAvailable;
    finished = false;
    clock.Reset();
    this->SetSizeHints(wxSize(366, 127), wxSize(366, 127));

    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* progressBoxxy;
    progressBoxxy = new wxBoxSizer(wxVERTICAL);

    myType = myFractal->GetType();
    if(myType == SCRIPT_FRACTAL)
    {
        progressLabel = new wxStaticText(this, wxID_ANY, wxT(savingTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Saving..."
        progressLabel->Wrap( -1 );
        progressBoxxy->Add(progressLabel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
        progress = nullptr;
    }
    else
    {
        progress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
        progressBoxxy->Add(progress, 0, wxALL|wxEXPAND, 5);

        progressLabel = new wxStaticText(this, wxID_ANY, wxString(wxT(renderingDotsTxt)) + wxT("0%"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Rendering... "
        progressLabel->Wrap(-1);
        progressBoxxy->Add(progressLabel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    }

    staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    progressBoxxy->Add(staticLine, 0, wxEXPAND | wxALL, 5);

    mainBoxxy->Add(progressBoxxy, 2, wxEXPAND, 5);

    wxBoxSizer* buttonBoxxy;
    buttonBoxxy = new wxBoxSizer(wxHORIZONTAL);

    cancelButton = new wxButton(this, wxID_ANY, wxT(cancelTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Cancel"
    buttonBoxxy->Add(cancelButton, 0, wxALL, 5);

    mainBoxxy->Add(buttonBoxxy, 1, wxEXPAND, 5);

    this->SetSizer(mainBoxxy);
    this->Layout();
    this->Centre(wxBOTH);
    this->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SaveProgressDiag::CalcProgress));
    cancelButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveProgressDiag::OnCancel), NULL, this);
}
SaveProgressDiag::~SaveProgressDiag()
{
    this->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SaveProgressDiag::CalcProgress));
    cancelButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveProgressDiag::OnCancel), NULL, this);
}
void SaveProgressDiag::OnCancel(wxCommandEvent& event)
{
    myFractal->GetWatchdog()->StopThreads();
    this->Close(true);
}
void SaveProgressDiag::CalcProgress(wxUpdateUIEvent& event)
{
    if(clock.GetElapsedTime() >= 0.05)
    {
        // Updates progress gauge.
        if(myFractal->GetType() != SCRIPT_FRACTAL)
        {
            int progressValue = myFractal->GetWatchdog()->GetThreadProgress();
            progressLabel->SetLabel(wxString(wxT(renderingDotsTxt)) + num_to_string(progressValue) + wxT("%"));    // Txt: "Rendering... "

            progress->SetValue(progressValue);
            if(progressValue >= 100 && !myFractal->IsRendering())
            {
                finished = true;
                this->Close(true);
            }
        }
        else
        {
            if(!myFractal->IsRendering())
            {
                finished = true;
                this->Close(true);
            }
        }
        clock.Reset();
    }
}
bool SaveProgressDiag::IsFinished()
{
    return finished;
}

// SizeDialogSave
SizeDialogSave::SizeDialogSave(FractalCanvas *mFCanvas, string rutaArchivo, int ext, FRACTAL_TYPE type, Fractal *target, wxWindow* parent,
                                string scriptPath, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    // WX Dialog.
    extension = ext;
    fCanvas = mFCanvas;
    opt = target->GetOptions();
    path = rutaArchivo;
    myScriptPath = scriptPath;
    screenRatio = (double)opt.screenWidth/(double)opt.screenHeight;
    fractalType = type;

    this->SetSizeHints(wxSize(283,201), wxSize(283,201));

    wxBoxSizer* boxxy;
    boxxy = new wxBoxSizer(wxVERTICAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelBoxxy;
    panelBoxxy = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* sizeBoxxy;
    sizeBoxxy = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* bSizer6;
    bSizer6 = new wxBoxSizer(wxVERTICAL);

    selectText = new wxStaticText(mainPanel, wxID_ANY, wxT(selectSizeTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Select image size"
    selectText->Wrap(-1);
    bSizer6->Add(selectText, 0, wxALL, 5);

    widthText = new wxStaticText(mainPanel, wxID_ANY, wxT(widthTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txe: "Width"
    widthText->Wrap(-1);
    bSizer6->Add(widthText, 0, wxALL, 5);

    widthSpin = new wxSpinCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000000, 1);
    bSizer6->Add(widthSpin, 0, wxALL, 5);
    widthSpin->SetValue(opt.screenWidth);

    sizeBoxxy->Add(bSizer6, 1, wxEXPAND, 5);

    wxBoxSizer* bSizer8;
    bSizer8 = new wxBoxSizer(wxVERTICAL);

    dumbText = new wxStaticText(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    dumbText->Wrap(-1);
    bSizer8->Add(dumbText, 0, wxALL, 5);

    heightText = new wxStaticText(mainPanel, wxID_ANY, wxT(heightTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Height"
    heightText->Wrap(-1);
    bSizer8->Add(heightText, 0, wxALL, 5);

    heightSpin = new wxSpinCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000000, 1);
    bSizer8->Add(heightSpin, 0, wxALL, 5);
    heightSpin->SetValue(opt.screenHeight);

    sizeBoxxy->Add(bSizer8, 1, wxEXPAND, 5);

    panelBoxxy->Add(sizeBoxxy, 1, wxEXPAND, 5);

    wxBoxSizer* okBoxxy;
    okBoxxy = new wxBoxSizer(wxVERTICAL);

    iterationsText = new wxStaticText(mainPanel, wxID_ANY, wxT(iterationsTxt), wxDefaultPosition, wxDefaultSize, 0);        // Txt: "Iterations"
    iterationsText->Wrap(-1);
    okBoxxy->Add(iterationsText, 0, wxALL, 5);

    iterationsSpin = new wxSpinCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20000000, 1);
    okBoxxy->Add(iterationsSpin, 0, wxALL, 5);
    iterationsSpin->SetValue(opt.maxIter);

    okButton = new wxButton(mainPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Ok"
    okBoxxy->Add(okButton, 0, wxALL, 5);

    panelBoxxy->Add(okBoxxy, 1, wxEXPAND, 5);

    mainPanel->SetSizer(panelBoxxy);
    mainPanel->Layout();
    panelBoxxy->Fit(mainPanel);
    boxxy->Add(mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(boxxy);
    this->Layout();
    this->Centre(wxBOTH);

    widthSpin->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(SizeDialogSave::ChangeWidth), NULL, this);
    heightSpin->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(SizeDialogSave::ChangeHeight), NULL, this);
    okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SizeDialogSave::OnOk), NULL, this);
}

SizeDialogSave::~SizeDialogSave()
{
    widthSpin->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(SizeDialogSave::ChangeWidth), NULL, this);
    heightSpin->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(SizeDialogSave::ChangeHeight), NULL, this);
    okButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SizeDialogSave::OnOk), NULL, this);
}

void SizeDialogSave::ChangeWidth(wxSpinEvent& event)
{
    int value = widthSpin->GetValue();
    value /= screenRatio;
    heightSpin->SetValue(value);
}
void SizeDialogSave::ChangeHeight(wxSpinEvent& event)
{
    int value = heightSpin->GetValue();
    value *= screenRatio;
    widthSpin->SetValue(value);
}
void SizeDialogSave::OnOk(wxCommandEvent& event)
{
    // Creates fractal.
    if(fractalType == SCRIPT_FRACTAL)
        fractalHandler.CreateScriptFractal(widthSpin->GetValue(), heightSpin->GetValue(), myScriptPath);
    else
        fractalHandler.CreateFractal(fractalType, widthSpin->GetValue(), heightSpin->GetValue());

    fractalHandler.SetFormula(fCanvas->GetFormula());

    // Copy parameters.
    opt.maxIter = iterationsSpin->GetValue();
    fractalHandler.GetTarget()->SetOptions(opt);

    // Saves image according to extension.
    SaveProgressDiag* diag = new SaveProgressDiag(fractalHandler.GetTarget(), this);
    fractalHandler.GetTarget()->Render();
    diag->ShowModal();
    if(diag->IsFinished())
    {
        if(extension == 0 || extension == 1)  // PNG or JPG
        {
            fractalHandler.GetTarget()->SetRendered(true);
            sf::Image out = fractalHandler.GetTarget()->GetRenderedImage();
            out.SaveToFile(path);
        }
        else  // BMP
        {
            fractalHandler.GetTarget()->SetRendered(true);
            fractalHandler.GetTarget()->RenderBMP(path);
        }
    }

    // Cleanup and close dialog.
    diag->Destroy();
    fractalHandler.DeleteFractal();
    this->Close(true);
}