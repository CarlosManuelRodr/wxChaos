#include <wx/spinctrl.h>
#include "AppPaths.h"
#include "common/AppTheme.h"
#include "export/ImageExportSizeDialog.h"
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
wxPanel* ImageExportSizeDialog::CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                                    const wxString& darkIcon)
{
    const auto header = new wxPanel(parent, wxID_ANY);
    header->SetBackgroundColour(AppTheme::ControlBackground());

    const auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    const wxSize iconSize(28, 28);
    const auto iconBitmap = new wxStaticBitmap(header, wxID_ANY, CreateIconBundle(lightIcon, darkIcon, iconSize));
    iconBitmap->SetBackgroundColour(AppTheme::ControlBackground());
    headerSizer->Add(iconBitmap, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP | wxBOTTOM, 14);

    const auto headerText = new wxStaticText(header, wxID_ANY, text);
    wxFont headerFont = headerText->GetFont();
    headerFont.SetPointSize(headerFont.GetPointSize() + 1);
    headerFont.SetWeight(wxFONTWEIGHT_BOLD);
    headerText->SetFont(headerFont);
    headerText->SetBackgroundColour(AppTheme::ControlBackground());
    headerText->SetForegroundColour(AppTheme::Foreground());
    headerSizer->Add(headerText, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

    header->SetSizer(headerSizer);
    header->SetMinSize(wxSize(-1, 56));
    return header;
}

wxBitmapBundle ImageExportSizeDialog::CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                                       const wxSize& size)
{
    const wxString icon = AppTheme::IsDark() ? darkIcon : lightIcon;
    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), size);
}

void ImageExportSizeDialog::SetButtonIcon(wxButton* button, const wxString& lightIcon, const wxString& darkIcon) const
{
    button->SetBitmap(CreateIconBundle(lightIcon, darkIcon, wxSize(20, 20)));
    button->SetBitmapMargins(FromDIP(6), 0);
}

ImageExportSizeDialog::ImageExportSizeDialog(FractalCanvas* fractalCanvas, const FractalType type, const Fractal* target,
                               wxWindow* parent, const string& scriptPath, const wxWindowID id, const wxString& title,
                               const wxPoint& pos, const wxSize& size, const long style)
                               : wxDialog(parent, id, title, pos, size, style)
{
    // WX Dialog.
    _extension = 0;
    _fractalCanvas = fractalCanvas;
    _options = target->GetOptions();
    _path.clear();
    _myScriptPath = scriptPath;
    _screenRatio = static_cast<double>(_options.screenWidth) / static_cast<double>(_options.screenHeight);
    _fractalType = type;

    this->SetSizeHints(wxSize(480, 280), wxDefaultSize);

    const auto sizer = new wxBoxSizer(wxVERTICAL);

    _mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(CreateSectionHeader(_mainPanel, "Image export", "picture_light.svg", "picture_dark.svg"),
                    0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    const auto exportSizer = new wxFlexGridSizer(3, 2, 12, 12);
    exportSizer->AddGrowableCol(1, 1);

    _widthText = new wxStaticText(_mainPanel, wxID_ANY, "Width", wxDefaultPosition, wxDefaultSize, 0);
    _widthText->Wrap(-1);
    exportSizer->Add(_widthText, 0, wxALIGN_CENTER_VERTICAL);

    _widthSpin = new wxSpinCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000000, 1);
    exportSizer->Add(_widthSpin, 1, wxEXPAND);
    _widthSpin->SetValue(static_cast<int>(_options.screenWidth));

    _heightText = new wxStaticText(_mainPanel, wxID_ANY, "Height", wxDefaultPosition, wxDefaultSize, 0);
    _heightText->Wrap(-1);
    exportSizer->Add(_heightText, 0, wxALIGN_CENTER_VERTICAL);

    _heightSpin = new wxSpinCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000000, 1);
    exportSizer->Add(_heightSpin, 1, wxEXPAND);
    _heightSpin->SetValue(static_cast<int>(_options.screenHeight));

    _iterationsText = new wxStaticText(_mainPanel, wxID_ANY, "Iterations", wxDefaultPosition, wxDefaultSize, 0);
    _iterationsText->Wrap(-1);
    exportSizer->Add(_iterationsText, 0, wxALIGN_CENTER_VERTICAL);

    _iterationsSpin = new wxSpinCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20000000, 1);
    exportSizer->Add(_iterationsSpin, 1, wxEXPAND);
    _iterationsSpin->SetValue(static_cast<int>(_options.maxIter));

    panelSizer->Add(exportSizer, 0, wxEXPAND | wxALL, 16);

    _largeImageWarningText = new wxStaticText(_mainPanel, wxID_ANY,
        "For very large images, the BMP format is recommended because the exporter is optimized for very large sizes.");
    _largeImageWarningText->Wrap(420);
    _largeImageWarningText->SetForegroundColour(AppTheme::IsDark() ? wxColour(242, 190, 95) : wxColour(128, 82, 0));
    _largeImageWarningText->Hide();
    panelSizer->Add(_largeImageWarningText, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();

    _saveButton = new wxButton(_mainPanel, wxID_ANY, "Save", wxDefaultPosition, wxDefaultSize, 0);
    SetButtonIcon(_saveButton, "save_light.svg", "save_dark.svg");
    buttonSizer->Add(_saveButton, 0, wxALL, 5);

    _cancelButton = new wxButton(_mainPanel, wxID_ANY, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
    SetButtonIcon(_cancelButton, "close_light.svg", "close_dark.svg");
    buttonSizer->Add(_cancelButton, 0, wxALL, 5);

    panelSizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    _mainPanel->SetSizer(panelSizer);
    _mainPanel->Layout();
    panelSizer->Fit(_mainPanel);
    sizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    UpdateLargeImageWarning();
    sizer->Fit(this);
    this->wxTopLevelWindowBase::SetMinSize(this->GetSize());
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    _widthSpin->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeWidth, this);
    _heightSpin->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeHeight, this);
    _saveButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportSizeDialog::OnSave, this);
    _cancelButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportSizeDialog::OnCancel, this);
}

ImageExportSizeDialog::~ImageExportSizeDialog()
{
    _widthSpin->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeWidth, this);
    _heightSpin->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &ImageExportSizeDialog::ChangeHeight, this);
    _saveButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportSizeDialog::OnSave, this);
    _cancelButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageExportSizeDialog::OnCancel, this);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ImageExportSizeDialog::ChangeWidth(wxSpinEvent&)
{
    double value = _widthSpin->GetValue();
    value /= _screenRatio;
    _heightSpin->SetValue(static_cast<int>(value));
    UpdateLargeImageWarning();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ImageExportSizeDialog::ChangeHeight(wxSpinEvent&)
{
    double value = _heightSpin->GetValue();
    value *= _screenRatio;
    _widthSpin->SetValue(static_cast<int>(value));
    UpdateLargeImageWarning();
}

void ImageExportSizeDialog::UpdateLargeImageWarning()
{
    constexpr int largeImageThreshold = 50000;
    const bool shouldShow = _widthSpin->GetValue() >= largeImageThreshold &&
                            _heightSpin->GetValue() >= largeImageThreshold;
    if (_largeImageWarningText->IsShown() == shouldShow)
        return;

    _largeImageWarningText->Show(shouldShow);
    _mainPanel->GetSizer()->Layout();
    this->SetMinSize(wxSize(480, 280));
    this->GetSizer()->Fit(this);
    this->SetMinSize(this->GetSize());
}
void ImageExportSizeDialog::OnSave(wxCommandEvent&)
{
    const auto saveFileDialog = new wxFileDialog(
        this,
        "Select file name",
        "",
        "fractal.png",
        "PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog->ShowModal() != wxID_OK)
    {
        saveFileDialog->Destroy();
        return;
    }

    const wxString fileName = saveFileDialog->GetPath();
    _extension = saveFileDialog->GetFilterIndex();
    _path = string(fileName.mb_str());
    saveFileDialog->Destroy();

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
    this->EndModal(wxID_OK);
}

void ImageExportSizeDialog::OnCancel(wxCommandEvent&)
{
    this->EndModal(wxID_CANCEL);
}
