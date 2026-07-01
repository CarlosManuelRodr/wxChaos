#include <wx/spinctrl.h>
#include "ImageExportSizeDialog.h"
#include "TextUtils.h"
using namespace std;

// ImageExportProgressDialog
ImageExportProgressDialog::ImageExportProgressDialog(Fractal* targetFractal, wxWindow* parent, bool saveProgressAvailable,
                                                     const wxWindowID id, const wxString& title, const wxPoint& pos,
                                                     const wxSize& size, const long style)
                                                     : wxDialog(parent, id, title, pos, size, style)
{
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
ImageExportSizeDialog::ImageExportSizeDialog(FractalCanvas* fractalCanvas, const string& filePath, const int ext, const FractalType type,
                               const Fractal* target, wxWindow* parent, const string& scriptPath, const wxWindowID id,
                               const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                               : wxDialog(parent, id, title, pos, size, style)
{
    // WX Dialog.
    _extension = ext;
    _fractalCanvas = fractalCanvas;
    _options = target->GetOptions();
    _path = filePath;
    _myScriptPath = scriptPath;
    _screenRatio = static_cast<double>(_options.screenWidth) / static_cast<double>(_options.screenHeight);
    _fractalType = type;

    this->SetSizeHints(wxSize(420, 300), wxSize(420, 300));

    const auto sizer = new wxBoxSizer(wxVERTICAL);

    _mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    const auto sizeSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto bSizer6 = new wxBoxSizer(wxVERTICAL);

    _selectText = new wxStaticText(_mainPanel, wxID_ANY, "Select image size", wxDefaultPosition, wxDefaultSize, 0);
    _selectText->Wrap(-1);
    bSizer6->Add(_selectText, 0, wxALL, 5);

    _widthText = new wxStaticText(_mainPanel, wxID_ANY, "Width", wxDefaultPosition, wxDefaultSize, 0);
    _widthText->Wrap(-1);
    bSizer6->Add(_widthText, 0, wxALL, 5);

    _widthSpin = new wxSpinCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000000, 1);
    bSizer6->Add(_widthSpin, 0, wxALL, 5);
    _widthSpin->SetValue(static_cast<int>(_options.screenWidth));
    sizeSizer->Add(bSizer6, 1, wxEXPAND, 5);

    const auto bSizer8 = new wxBoxSizer(wxVERTICAL);

    _dumbText = new wxStaticText(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    _dumbText->Wrap(-1);
    bSizer8->Add(_dumbText, 0, wxALL, 5);

    _heightText = new wxStaticText(_mainPanel, wxID_ANY, "Height", wxDefaultPosition, wxDefaultSize, 0);
    _heightText->Wrap(-1);
    bSizer8->Add(_heightText, 0, wxALL, 5);

    _heightSpin = new wxSpinCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000000, 1);
    bSizer8->Add(_heightSpin, 0, wxALL, 5);
    _heightSpin->SetValue(static_cast<int>(_options.screenHeight));

    sizeSizer->Add(bSizer8, 1, wxEXPAND, 5);
    panelSizer->Add(sizeSizer, 1, wxEXPAND, 5);

    const auto okSizer = new wxBoxSizer(wxVERTICAL);

    _iterationsText = new wxStaticText(_mainPanel, wxID_ANY, "Iterations", wxDefaultPosition, wxDefaultSize, 0);
    _iterationsText->Wrap(-1);
    okSizer->Add(_iterationsText, 0, wxALL, 5);

    _iterationsSpin = new wxSpinCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20000000, 1);
    okSizer->Add(_iterationsSpin, 0, wxALL, 5);
    _iterationsSpin->SetValue(static_cast<int>(_options.maxIter));

    _okButton = new wxButton(_mainPanel, wxID_ANY, "Ok", wxDefaultPosition, wxDefaultSize, 0);
    okSizer->Add(_okButton, 0, wxALL, 5);
    panelSizer->Add(okSizer, 1, wxEXPAND, 5);

    _mainPanel->SetSizer(panelSizer);
    _mainPanel->Layout();
    panelSizer->Fit(_mainPanel);
    sizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    _widthSpin->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeWidth, this);
    _heightSpin->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeHeight, this);
    _okButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportSizeDialog::OnOk, this);
}

ImageExportSizeDialog::~ImageExportSizeDialog()
{
    _widthSpin->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeWidth, this);
    _heightSpin->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeHeight, this);
    _okButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportSizeDialog::OnOk, this);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ImageExportSizeDialog::ChangeWidth(wxSpinEvent&)
{
    double value = _widthSpin->GetValue();
    value /= _screenRatio;
    _heightSpin->SetValue(static_cast<int>(value));
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ImageExportSizeDialog::ChangeHeight(wxSpinEvent&)
{
    double value = _heightSpin->GetValue();
    value *= _screenRatio;
    _widthSpin->SetValue(static_cast<int>(value));
}
void ImageExportSizeDialog::OnOk(wxCommandEvent&)
{
    // Creates fractal.
    if (_fractalType == FractalType::ScriptFractal)
        _fractalFactory.CreateScriptFractal(_widthSpin->GetValue(), _heightSpin->GetValue(), _myScriptPath);
    else
        _fractalFactory.CreateFractal(_fractalType, _widthSpin->GetValue(), _heightSpin->GetValue());

    _fractalFactory.SetFormula(_fractalCanvas->GetFormula());

    // Copy parameters.
    _options.maxIter = _iterationsSpin->GetValue();
    _fractalFactory.GetFractal()->SetOptions(_options);

    // Saves image according to extension.
    const auto diag = new ImageExportProgressDialog(_fractalFactory.GetFractal(), this);
    _fractalFactory.GetFractal()->Render();
    diag->ShowModal();
    if (diag->IsFinished())
    {
        if (_extension == 0 || _extension == 1)  // PNG or JPG
        {
            _fractalFactory.GetFractal()->SetRendered(true);
            const sf::Image out = _fractalFactory.GetFractal()->GetRenderedImage();
            if (const bool result = out.saveToFile(_path); !result)
                wxMessageBox("Failed to save image to file: " + _path, "Error", wxOK | wxICON_ERROR);
        }
        else  // BMP
        {
            _fractalFactory.GetFractal()->SetRendered(true);
            if (const bool result = _fractalFactory.GetFractal()->SaveBmp(_path); !result)
                wxMessageBox("Failed to save image to file: " + _path, "Error", wxOK | wxICON_ERROR);
        }
    }

    // Cleanup and close dialog.
    diag->Destroy();
    _fractalFactory.DeleteFractal();
    this->Close(true);
}
