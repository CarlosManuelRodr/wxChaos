#include <wx/spinctrl.h>
#include "ImageExportSizeDialog.h"
#include "TextUtils.h"
using namespace std;

// ImageExportProgressDialog
ImageExportProgressDialog::ImageExportProgressDialog(Fractal* targetFractal, wxWindow* parent, bool saveProgressAvailable, const wxWindowID id,
                                   const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                                   : wxDialog(parent, id, title, pos, size, style)
{
    // WX Dialog.
    _myFractal = targetFractal;
    _saveProgressAvailable = saveProgressAvailable;
    _finished = false;
    _clock.restart();
    this->SetSizeHints(wxSize(480, 180), wxSize(480, 180));

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    const auto progressSizer = new wxBoxSizer(wxVERTICAL);

    _myType = _myFractal->GetType();
    if (_myType == FractalType::ScriptFractal)
    {
        _progressLabel = new wxStaticText(this, wxID_ANY, "Saving...", wxDefaultPosition, wxDefaultSize, 0);
        _progressLabel->Wrap(-1);
        progressSizer->Add(_progressLabel, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
        _progress = nullptr;
    }
    else
    {
        _progress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
        progressSizer->Add(_progress, 0, wxALL | wxEXPAND, 5);

        _progressLabel = new wxStaticText(this, wxID_ANY, wxString("Rendering: ") + "0%", wxDefaultPosition, wxDefaultSize, 0);
        _progressLabel->Wrap(-1);
        progressSizer->Add(_progressLabel, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
    }

    _staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    progressSizer->Add(_staticLine, 0, wxEXPAND | wxALL, 5);

    mainSizer->Add(progressSizer, 2, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    _cancelButton = new wxButton(this, wxID_ANY, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_cancelButton, 0, wxALL, 5);

    mainSizer->Add(buttonSizer, 1, wxEXPAND, 5);

    this->SetSizer(mainSizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);
    this->Bind(wxEVT_UPDATE_UI, &ImageExportProgressDialog::CalcProgress, this);
    _cancelButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportProgressDialog::OnCancel, this);
}
ImageExportProgressDialog::~ImageExportProgressDialog()
{
    this->Unbind(wxEVT_UPDATE_UI, &ImageExportProgressDialog::CalcProgress, this);
    _cancelButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportProgressDialog::OnCancel, this);
}
void ImageExportProgressDialog::OnCancel(wxCommandEvent&)
{
    _myFractal->StopRender();
    this->Close(true);
}
void ImageExportProgressDialog::CalcProgress(wxUpdateUIEvent&)
{
    if (_clock.getElapsedTime().asSeconds() >= 0.05f)
    {
        // Updates progress gauge.
        if (_myFractal->GetType() != FractalType::ScriptFractal)
        {
            const int progressValue = _myFractal->GetRenderProgress();
            _progressLabel->SetLabel(wxString("Rendering: ") + TextUtils::ToWxString(progressValue) + "%");

            _progress->SetValue(progressValue);
            if (progressValue >= 100 && !_myFractal->IsRendering())
            {
                _finished = true;
                this->Close(true);
            }
        }
        else
        {
            if (!_myFractal->IsRendering())
            {
                _finished = true;
                this->Close(true);
            }
        }
        _clock.restart();
    }
}
bool ImageExportProgressDialog::IsFinished() const
{
    return _finished;
}

// ImageExportSizeDialog
ImageExportSizeDialog::ImageExportSizeDialog(FractalCanvas* mFCanvas, const string& filePath, const int ext, const FractalType type,
                               const Fractal* target, wxWindow* parent, const string& scriptPath, const wxWindowID id,
                               const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                               : wxDialog(parent, id, title, pos, size, style)
{
    // WX Dialog.
    extension = ext;
    fCanvas = mFCanvas;
    opt = target->GetOptions();
    path = filePath;
    myScriptPath = scriptPath;
    screenRatio = static_cast<double>(opt.screenWidth) / static_cast<double>(opt.screenHeight);
    fractalType = type;

    this->SetSizeHints(wxSize(420, 300), wxSize(420, 300));

    const auto sizer = new wxBoxSizer(wxVERTICAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    const auto sizeSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto bSizer6 = new wxBoxSizer(wxVERTICAL);

    selectText = new wxStaticText(mainPanel, wxID_ANY, "Select image size", wxDefaultPosition, wxDefaultSize, 0);
    selectText->Wrap(-1);
    bSizer6->Add(selectText, 0, wxALL, 5);

    widthText = new wxStaticText(mainPanel, wxID_ANY, "Width", wxDefaultPosition, wxDefaultSize, 0);
    widthText->Wrap(-1);
    bSizer6->Add(widthText, 0, wxALL, 5);

    widthSpin = new wxSpinCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000000, 1);
    bSizer6->Add(widthSpin, 0, wxALL, 5);
    widthSpin->SetValue(static_cast<int>(opt.screenWidth));
    sizeSizer->Add(bSizer6, 1, wxEXPAND, 5);

    const auto bSizer8 = new wxBoxSizer(wxVERTICAL);

    dumbText = new wxStaticText(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    dumbText->Wrap(-1);
    bSizer8->Add(dumbText, 0, wxALL, 5);

    heightText = new wxStaticText(mainPanel, wxID_ANY, "Height", wxDefaultPosition, wxDefaultSize, 0);
    heightText->Wrap(-1);
    bSizer8->Add(heightText, 0, wxALL, 5);

    heightSpin = new wxSpinCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000000, 1);
    bSizer8->Add(heightSpin, 0, wxALL, 5);
    heightSpin->SetValue(static_cast<int>(opt.screenHeight));

    sizeSizer->Add(bSizer8, 1, wxEXPAND, 5);
    panelSizer->Add(sizeSizer, 1, wxEXPAND, 5);

    const auto okSizer = new wxBoxSizer(wxVERTICAL);

    iterationsText = new wxStaticText(mainPanel, wxID_ANY, "Iterations", wxDefaultPosition, wxDefaultSize, 0);
    iterationsText->Wrap(-1);
    okSizer->Add(iterationsText, 0, wxALL, 5);

    iterationsSpin = new wxSpinCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20000000, 1);
    okSizer->Add(iterationsSpin, 0, wxALL, 5);
    iterationsSpin->SetValue(static_cast<int>(opt.maxIter));

    okButton = new wxButton(mainPanel, wxID_ANY, "Ok", wxDefaultPosition, wxDefaultSize, 0);
    okSizer->Add(okButton, 0, wxALL, 5);
    panelSizer->Add(okSizer, 1, wxEXPAND, 5);

    mainPanel->SetSizer(panelSizer);
    mainPanel->Layout();
    panelSizer->Fit(mainPanel);
    sizer->Add(mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    widthSpin->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeWidth, this);
    heightSpin->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeHeight, this);
    okButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportSizeDialog::OnOk, this);
}

ImageExportSizeDialog::~ImageExportSizeDialog()
{
    widthSpin->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeWidth, this);
    heightSpin->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeHeight, this);
    okButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportSizeDialog::OnOk, this);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ImageExportSizeDialog::ChangeWidth(wxSpinEvent&)
{
    double value = widthSpin->GetValue();
    value /= screenRatio;
    heightSpin->SetValue(static_cast<int>(value));
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ImageExportSizeDialog::ChangeHeight(wxSpinEvent&)
{
    double value = heightSpin->GetValue();
    value *= screenRatio;
    widthSpin->SetValue(static_cast<int>(value));
}
void ImageExportSizeDialog::OnOk(wxCommandEvent&)
{
    // Creates fractal.
    if (fractalType == FractalType::ScriptFractal)
        fractalFactory.CreateScriptFractal(widthSpin->GetValue(), heightSpin->GetValue(), myScriptPath);
    else
        fractalFactory.CreateFractal(fractalType, widthSpin->GetValue(), heightSpin->GetValue());

    fractalFactory.SetFormula(fCanvas->GetFormula());

    // Copy parameters.
    opt.maxIter = iterationsSpin->GetValue();
    fractalFactory.GetFractalPtr()->SetOptions(opt);

    // Saves image according to extension.
    const auto diag = new ImageExportProgressDialog(fractalFactory.GetFractalPtr(), this);
    fractalFactory.GetFractalPtr()->Render();
    diag->ShowModal();
    if (diag->IsFinished())
    {
        if (extension == 0 || extension == 1)  // PNG or JPG
        {
            fractalFactory.GetFractalPtr()->SetRendered(true);
            const sf::Image out = fractalFactory.GetFractalPtr()->GetRenderedImage();
            const bool result = out.saveToFile(path);
            if (!result)
                wxMessageBox("Failed to save image to file: " + path, "Error", wxOK | wxICON_ERROR);
        }
        else  // BMP
        {
            fractalFactory.GetFractalPtr()->SetRendered(true);
            const bool result = fractalFactory.GetFractalPtr()->SaveBmp(path);
            if (!result)
                wxMessageBox("Failed to save image to file: " + path, "Error", wxOK | wxICON_ERROR);
        }
    }

    // Cleanup and close dialog.
    diag->Destroy();
    fractalFactory.DeleteFractal();
    this->Close(true);
}
