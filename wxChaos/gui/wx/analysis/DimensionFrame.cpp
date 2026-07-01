#include <limits>
#include <mpParser.h>
#include <wx/bmpbndl.h>
#include <wx/spinctrl.h>
#include "common/AppTheme.h"
#include "AppPaths.h"
#include "analysis/DimensionFrame.h"
#include "TextUtils.h"
#include "export/ImageExportSizeDialog.h"
#include "BmpImageWriter.h"
#include "AngelscriptBindings.h"
#include "docs/DocumentViewer.h"
#include "SystemUtilities.h"
using namespace std;

wxDEFINE_EVENT(wxEVT_DIMENSION_FRAME_CLOSED, wxCommandEvent);

double GetMaxElement(const vector<double> &in)
{
    double max = -std::numeric_limits<double>::infinity();
    for (const double i : in)
    {
        if (i > max)
            max = i;
    }
    return max;
}
double GetMinElement(const vector<double> &in)
{
    double min = std::numeric_limits<double>::infinity();
    for (const double i : in)
    {
        if (i < min)
            min = i;
    }
    return min;
}

// DimensionFrame
wxPanel* DimensionFrame::CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                             const wxString& darkIcon)
{
    const auto header = new wxPanel(parent, wxID_ANY);
    header->SetBackgroundColour(AppTheme::ControlBackground());

    const auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    const wxSize iconSize(24, 24);
    const auto iconBitmap = new wxStaticBitmap(header, wxID_ANY, CreateIconBundle(lightIcon, darkIcon, iconSize));
    iconBitmap->SetBackgroundColour(AppTheme::ControlBackground());
    headerSizer->Add(iconBitmap, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP | wxBOTTOM, 10);

    const auto title = new wxStaticText(header, wxID_ANY, text);
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(titleFont.GetPointSize() + 2);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    title->SetBackgroundColour(AppTheme::ControlBackground());
    title->SetForegroundColour(AppTheme::Foreground());
    headerSizer->Add(title, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

    header->SetSizer(headerSizer);
    return header;
}

wxBitmapBundle DimensionFrame::CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                                const wxSize& size)
{
    const wxString icon = AppTheme::IsDark() ? darkIcon : lightIcon;
    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), size);
}

DimensionFrame::DimensionFrame(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
                               const wxSize& size, const long style) : wxFrame(parent, id, title, pos, size, style)
{
    _threadNumber = Get_Cores();
    _dimensionCalculator.resize(_threadNumber);
    _dimThreads.resize(_threadNumber);

    _previewSize = 400;
    _target = nullptr;
    _confFractOptDialog = nullptr;
    _renderingPreview = false;
    _calculatingDimension = false;
    _scriptSelected = false;
    _firstRender = true;
    _clock.restart();

    this->SetSizeHints(wxSize(960, 700), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);

    const auto mainBoxSizer = new wxBoxSizer(wxVERTICAL);
    _mainPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    _mainPanel->SetScrollRate(5, 5);

    const auto subMainBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto paramBoxSizer = new wxBoxSizer(wxVERTICAL);
    const auto fractalSectionSizer = new wxBoxSizer(wxVERTICAL);
    fractalSectionSizer->Add(CreateSectionHeader(_mainPanel, "Fractal parameters",
                                                 "fractal_light.svg", "fractal_dark.svg"),
                             0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    const auto fractalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto borderBoxSizer = new wxBoxSizer(wxVERTICAL);

    const wxString fractalChoiceChoices[] = { "Mandelbrot", "MandelbrotZN", "Mandelbrot (Julia)", "MandelbrotZN (Julia)", "Sine (Julia)", "Jellyfish",
                                              "Manowar", "Manowar (Julia)", "Tricorn", "Burning Ship", "Burning Ship (Julia)",
                                              "Fractory", "Cell", "Magnet", "Double pendulum" };
    constexpr int fractalChoiceNChoices = sizeof(fractalChoiceChoices) / sizeof(wxString);
    _fractalChoice = new wxChoice(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, fractalChoiceNChoices, fractalChoiceChoices, 0);
    borderBoxSizer->Add(_fractalChoice, 0, wxALL | wxEXPAND, 5);

    _minXTxt = new wxStaticText(_mainPanel, wxID_ANY, "MinX", wxDefaultPosition, wxDefaultSize, 0);
    _minXTxt->Wrap(-1);
    borderBoxSizer->Add(_minXTxt, 0, wxALL, 5);

    _minXCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, "-1.5", wxDefaultPosition, wxDefaultSize, 0);
    borderBoxSizer->Add(_minXCtrl, 0, wxALL | wxEXPAND, 5);

    _maxXTxt = new wxStaticText(_mainPanel, wxID_ANY, "MaxX", wxDefaultPosition, wxDefaultSize, 0);
    _maxXTxt->Wrap(-1);
    borderBoxSizer->Add(_maxXTxt, 0, wxALL, 5);

    _maxXCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, "1.5", wxDefaultPosition, wxDefaultSize, 0);
    borderBoxSizer->Add(_maxXCtrl, 0, wxALL | wxEXPAND, 5);

    _minYTxt = new wxStaticText(_mainPanel, wxID_ANY, "MinY", wxDefaultPosition, wxDefaultSize, 0);
    _minYTxt->Wrap(-1);
    borderBoxSizer->Add(_minYTxt, 0, wxALL, 5);

    _minYCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, "-0.4", wxDefaultPosition, wxDefaultSize, 0);
    borderBoxSizer->Add(_minYCtrl, 0, wxALL | wxEXPAND, 5);

    _manualMaxYChk = new wxCheckBox(_mainPanel, wxID_ANY, "Manual MaxY", wxDefaultPosition, wxDefaultSize, 0);
    borderBoxSizer->Add(_manualMaxYChk, 0, wxALL, 5);
    _manualMaxYChk->SetValue(true);

    _maxYTxt = new wxStaticText(_mainPanel, wxID_ANY, "MaxY", wxDefaultPosition, wxDefaultSize, 0);
    _maxYTxt->Wrap(-1);
    borderBoxSizer->Add(_maxYTxt, 0, wxALL, 5);

    _maxYCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, "0.4", wxDefaultPosition, wxDefaultSize, 0);

    borderBoxSizer->Add(_maxYCtrl, 0, wxALL | wxEXPAND, 5);
    fractalBoxSizer->Add(borderBoxSizer, 1, wxEXPAND, 5);

    const auto fOptBoxSizer = new wxBoxSizer(wxVERTICAL);

    _iterTxt = new wxStaticText(_mainPanel, wxID_ANY, "Iterations", wxDefaultPosition, wxDefaultSize, 0);
    _iterTxt->Wrap(-1);
    fOptBoxSizer->Add(_iterTxt, 0, wxALL, 5);

    _iterCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, "20000", wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxSizer->Add(_iterCtrl, 0, wxALL | wxEXPAND, 5);

    _sizeTxt = new wxStaticText(_mainPanel, wxID_ANY, "Image size (pixels)", wxDefaultPosition, wxDefaultSize, 0);
    _sizeTxt->Wrap(-1);
    fOptBoxSizer->Add(_sizeTxt, 0, wxALL, 5);

    _sizeCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, "3000", wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxSizer->Add(_sizeCtrl, 0, wxALL | wxEXPAND, 5);

    _fractalOptionsButton = new wxButton(_mainPanel, wxID_ANY, "Configure fractal options", wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxSizer->Add(_fractalOptionsButton, 0, wxALL | wxEXPAND, 5);

    fractalBoxSizer->Add(fOptBoxSizer, 1, wxEXPAND, 5);
    fractalSectionSizer->Add(fractalBoxSizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    paramBoxSizer->Add(fractalSectionSizer, 0, wxEXPAND, 5);

    const auto dimBoxSizer = new wxBoxSizer(wxVERTICAL);
    dimBoxSizer->Add(CreateSectionHeader(_mainPanel, "Box-counting parameters",
                                         "box_count_light.svg", "box_count_dark.svg"),
                     0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _divTxt = new wxStaticText(_mainPanel, wxID_ANY, "Divisions", wxDefaultPosition, wxDefaultSize, 0);
    _divTxt->Wrap(-1);
    dimBoxSizer->Add(_divTxt, 0, wxALL, 5);

    const auto divBoxSizer = new wxBoxSizer(wxVERTICAL);

    _divNotebook = new wxNotebook(_mainPanel, wxID_ANY, wxDefaultPosition, wxSize(-1, 150), 0);
    _byFunctionPanel = new wxPanel(_divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto byFunctionBoxSizer = new wxBoxSizer(wxVERTICAL);
    const auto functionRowSizer = new wxBoxSizer(wxHORIZONTAL);

    _funcTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, "Function:", wxDefaultPosition, wxDefaultSize, 0);
    _funcTxt->Wrap(-1);
    functionRowSizer->Add(_funcTxt, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _fDeclTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, "f(x) = ", wxDefaultPosition, wxDefaultSize, 0);
    _fDeclTxt->Wrap(-1);
    functionRowSizer->Add(_fDeclTxt, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _funcCtrl = new wxTextCtrl(_byFunctionPanel, wxID_ANY, "2*x", wxDefaultPosition, wxDefaultSize, 0);
    functionRowSizer->Add(_funcCtrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    byFunctionBoxSizer->Add(functionRowSizer, 0, wxEXPAND);

    const auto rangeSizer = new wxBoxSizer(wxHORIZONTAL);

    _goesFromTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, "x goes from", wxDefaultPosition, wxDefaultSize, 0);
    _goesFromTxt->Wrap(-1);
    rangeSizer->Add(_goesFromTxt, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _xMinSpin = new wxSpinCtrl(_byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(110, -1),
                               wxSP_ARROW_KEYS, 1, 1000000, 1);
    rangeSizer->Add(_xMinSpin, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _goesToTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, "to", wxDefaultPosition, wxDefaultSize, 0);
    _goesToTxt->Wrap(-1);
    rangeSizer->Add(_goesToTxt, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _xMaxSpin = new wxSpinCtrl(_byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(110, -1),
                               wxSP_ARROW_KEYS, 1, 1000000, 50);
    rangeSizer->Add(_xMaxSpin, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    byFunctionBoxSizer->Add(rangeSizer, 0, wxEXPAND);

    _byFunctionPanel->SetSizer(byFunctionBoxSizer);
    _byFunctionPanel->Layout();
    byFunctionBoxSizer->Fit(_byFunctionPanel);
    _divNotebook->AddPage(_byFunctionPanel, "By function", true);
    _byListPanel = new wxPanel(_divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto byListBoxSizer = new wxBoxSizer(wxVERTICAL);

    _listCtrl = new wxTextCtrl(_byListPanel, wxID_ANY, "2,4,5,6,9,100,200", wxDefaultPosition, wxSize(-1, 80), wxTE_MULTILINE);
    byListBoxSizer->Add(_listCtrl, 1, wxALL | wxEXPAND, 5);

    _byListPanel->SetSizer(byListBoxSizer);
    _byListPanel->Layout();
    byListBoxSizer->Fit(_byListPanel);
    _divNotebook->AddPage(_byListPanel, "By list", false);

    divBoxSizer->Add(_divNotebook, 0, wxEXPAND | wxALL, 5);
    dimBoxSizer->Add(divBoxSizer, 0, wxEXPAND, 5);

    dimBoxSizer->Add(CreateSectionHeader(_mainPanel, "Plotting", "plot_light.svg", "plot_dark.svg"),
                     0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    const auto plotBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _dataCheck = new wxCheckBox(_mainPanel, wxID_ANY, "Plot data", wxDefaultPosition, wxDefaultSize, 0);
    plotBoxSizer->Add(_dataCheck, 0, wxALL, 5);

    _dataFitCheck = new wxCheckBox(_mainPanel, wxID_ANY, "Plot fitted data", wxDefaultPosition, wxDefaultSize, 0);
    plotBoxSizer->Add(_dataFitCheck, 0, wxALL, 5);
    dimBoxSizer->Add(plotBoxSizer, 0, wxEXPAND, 5);

    const auto buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _calcButton = new wxButton(_mainPanel, wxID_ANY, "Calculate", wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_calcButton, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    _closeButton = new wxButton(_mainPanel, wxID_ANY, "Close", wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_closeButton, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    const wxSize actionButtonSize(_calcButton->GetBestSize().GetHeight(), _calcButton->GetBestSize().GetHeight());
    _helpButton = new wxBitmapButton(_mainPanel, wxID_ANY,
                                     CreateIconBundle("help_light.svg", "help_dark.svg", wxSize(18, 18)),
                                     wxDefaultPosition, actionButtonSize, wxBU_AUTODRAW);
    _helpButton->SetMinSize(actionButtonSize);
    buttonBoxSizer->Add(_helpButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    paramBoxSizer->Add(dimBoxSizer, 0, wxEXPAND, 5);
    paramBoxSizer->AddStretchSpacer(1);
    paramBoxSizer->Add(buttonBoxSizer, 0, wxEXPAND, 5);
    subMainBoxSizer->Add(paramBoxSizer, 1, wxEXPAND, 5);

    const auto outputBoxSizer = new wxBoxSizer(wxVERTICAL);
    outputBoxSizer->Add(CreateSectionHeader(_mainPanel, "Box count preview", "box_light.svg", "box_dark.svg"),
                        0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _previewImage = new ImagePanel(_mainPanel, wxID_ANY, _previewSize);
    _previewImage->SetMinSize(wxSize(_previewSize, _previewSize));
    outputBoxSizer->Add(_previewImage, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 8);

    const auto previewOptionsSizer = new wxBoxSizer(wxVERTICAL);
    const auto numberOfDivisionsSizer = new wxBoxSizer(wxHORIZONTAL);
    _nDivTxt = new wxStaticText(_mainPanel, wxID_ANY, "Number of divisions", wxDefaultPosition, wxDefaultSize, 0);
    _nDivTxt->Wrap(-1);
    numberOfDivisionsSizer->Add(_nDivTxt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    _numberOfDivisionsSpinCtrl = new wxSpinCtrl(_mainPanel, wxID_ANY, "20", wxDefaultPosition, wxSize(110, -1),
                                                wxSP_ARROW_KEYS, 1, 200, 0);
    numberOfDivisionsSizer->Add(_numberOfDivisionsSpinCtrl, 0, wxALIGN_CENTER_VERTICAL);
    previewOptionsSizer->Add(numberOfDivisionsSizer, 0, wxALL, 5);

    const auto renderPreBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    _previewButton = new wxButton(_mainPanel, wxID_ANY, "Render preview", wxDefaultPosition, wxDefaultSize, 0);
    renderPreBoxSizer->Add(_previewButton, 1, wxALL | wxEXPAND, 5);

    _savePreviewButton = new wxButton(_mainPanel, wxID_ANY, "Save preview", wxDefaultPosition, wxDefaultSize, 0);
    renderPreBoxSizer->Add(_savePreviewButton, 1, wxALL | wxEXPAND, 5);
    previewOptionsSizer->Add(renderPreBoxSizer, 0, wxEXPAND);
    outputBoxSizer->Add(previewOptionsSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    _outLine = new wxStaticLine(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    outputBoxSizer->Add(_outLine, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    outputBoxSizer->Add(CreateSectionHeader(_mainPanel, "Dimension count log", "log_light.svg", "log_dark.svg"),
                        0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _logCtrl = new wxRichTextCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxTE_READONLY | wxVSCROLL | wxHSCROLL | wxNO_BORDER | wxWANTS_CHARS);
    outputBoxSizer->Add(_logCtrl, 1, wxALL | wxEXPAND, 5);

    _progressBar = new wxGauge(_mainPanel, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
    outputBoxSizer->Add(_progressBar, 0, wxALL | wxEXPAND, 5);

    _progressTxt = new wxStaticText(_mainPanel, wxID_ANY, "Progress: Stopped", wxDefaultPosition, wxDefaultSize, 0);
    _progressTxt->Wrap(-1);
    outputBoxSizer->Add(_progressTxt, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    subMainBoxSizer->Add(outputBoxSizer, 1, wxEXPAND, 5);

    _mainPanel->SetSizer(subMainBoxSizer);
    _mainPanel->Layout();
    subMainBoxSizer->Fit(_mainPanel);
    mainBoxSizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainBoxSizer);
    this->wxTopLevelWindowBase::Layout();

    this->Centre(wxBOTH);

    // Set welcome log text.
    _logCtrl->WriteText("Dimension calculator log.\n\n");

    this->GetScriptFractals();

    // Set the default fractal.
    _fractalChoice->SetSelection(0);
    this->CreateFractal(_previewSize);
    _myOpt = _target->GetOptions();
    _minXCtrl->SetValue(TextUtils::ToWxString(_myOpt.minX));
    _maxXCtrl->SetValue(TextUtils::ToWxString(_myOpt.maxX));
    _minYCtrl->SetValue(TextUtils::ToWxString(_myOpt.minY));
    _maxYCtrl->SetValue(TextUtils::ToWxString(_myOpt.maxY));
    _iterCtrl->SetValue(TextUtils::ToWxString(static_cast<int>(_myOpt.maxIter)));

    // Connect Events.
    this->Bind(wxEVT_CLOSE_WINDOW, &DimensionFrame::OnDestroy, this);
    this->Bind(wxEVT_UPDATE_UI, &DimensionFrame::OnUpdateUI, this);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &DimensionFrame::OnClose, this, wxID_EXIT);
    _fractalChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &DimensionFrame::OnChangeFractal, this);
    _fractalOptionsButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnFractalOpt, this);
    _previewButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnRenderPreview, this);
    _calcButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnCalculate, this);
    _manualMaxYChk->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &DimensionFrame::OnManualMaxY, this);
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnClose, this);
    _savePreviewButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnSavePreview, this);
    _helpButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnHelp, this);
}
DimensionFrame::~DimensionFrame()
{
    // Disconnect Events.
    this->Unbind(wxEVT_CLOSE_WINDOW, &DimensionFrame::OnDestroy, this);
    this->Unbind(wxEVT_UPDATE_UI, &DimensionFrame::OnUpdateUI, this);
    _fractalChoice->Unbind(wxEVT_COMMAND_CHOICE_SELECTED, &DimensionFrame::OnChangeFractal, this);
    _fractalOptionsButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnFractalOpt, this);
    _previewButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnRenderPreview, this);
    _calcButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnCalculate, this);
    _manualMaxYChk->Unbind(wxEVT_COMMAND_CHECKBOX_CLICKED, &DimensionFrame::OnManualMaxY, this);
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnClose, this);
    _savePreviewButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnSavePreview, this);
    _helpButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnHelp, this);

    if (_calculatingDimension)
    {
        // Stop render.
        _target->StopRender();
        _calcButton->SetLabel("Calculate");
        _calculatingDimension = false;
    }
    StopDimensionThreads();

    if (_renderingPreview)
        _target->StopRender();

    _fractalFactory.DeleteFractal();
    delete _confFractOptDialog;
}

void DimensionFrame::JoinDimensionThreads()
{
    for (auto& thread : _dimThreads)
    {
        if (thread)
            thread->wait();
        thread.reset();
    }
}

void DimensionFrame::StopDimensionThreads()
{
    for (auto& calculator : _dimensionCalculator)
    {
        if (calculator.IsRunning())
            calculator.Terminate();
    }
    JoinDimensionThreads();
}

void DimensionFrame::CreateFractal(int size)
{
    _firstRender = true;
    const int choice = _fractalChoice->GetCurrentSelection();
    switch (choice)
    {
        case FL_MANDELBROT:
        {
            _fractalFactory.CreateFractal(FractalType::Mandelbrot, size, size);
            break;
        }
        case FL_MANDELBROT_ZN:
        {
            _fractalFactory.CreateFractal(FractalType::MandelbrotZN, size, size);
            break;
        }
        case FL_JULIA:
        {
            _fractalFactory.CreateFractal(FractalType::Julia, size, size);
            break;
        }
        case FL_JULIA_ZN:
        {
            _fractalFactory.CreateFractal(FractalType::JuliaZN, size, size);
            break;
        }
        case FL_SINUSOIDAL:
        {
            _fractalFactory.CreateFractal(FractalType::Sinusoidal, size, size);
            break;
        }
        case FL_JELLYFISH:
        {
            _fractalFactory.CreateFractal(FractalType::Jellyfish, size, size);
            break;
        }
        case FL_MANOWAR:
        {
            _fractalFactory.CreateFractal(FractalType::Manowar, size, size);
            break;
        }
        case FL_MANOWAR_JULIA:
        {
            _fractalFactory.CreateFractal(FractalType::ManowarJulia, size, size);
            break;
        }
        case FL_TRICORN:
        {
            _fractalFactory.CreateFractal(FractalType::Tricorn, size, size);
            break;
        }
        case FL_BURNING_SHIP:
        {
            _fractalFactory.CreateFractal(FractalType::BurningShip, size, size);
            break;
        }
        case FL_BURNING_SHIP_JULIA:
        {
            _fractalFactory.CreateFractal(FractalType::BurningShipJulia, size, size);
            break;
        }
        case FL_FRACTORY:
        {
            _fractalFactory.CreateFractal(FractalType::Fractory, size, size);
            break;
        }
        case FL_CELL:
        {
            _fractalFactory.CreateFractal(FractalType::Cell, size, size);
            break;
        }
        case FL_MAGNET:
        {
            _fractalFactory.CreateFractal(FractalType::Magnetic, size, size);
            break;
        }
        case FL_DOUBLE_PENDULUM:
        {
            _fractalFactory.CreateFractal(FractalType::DoublePendulum, size, size);
            break;
        }
        default: ;
    }

    // Script fractals.
    if (choice >= COUNT)
    {
        const int idx = choice - COUNT;
        _fractalFactory.CreateScriptFractal(size, size, _loadedScripts[_scriptList[idx]]);
        _scriptSelected = true;
    }
    else
        _scriptSelected = false;

    _target = _fractalFactory.GetFractal();
    if (_confFractOptDialog != nullptr)
        _confFractOptDialog->SetNewTarget(_target);
}
void DimensionFrame::OnRenderPreview(wxCommandEvent&)
{
    if (!_renderingPreview)
    {
        _target->Resize(_previewSize, _previewSize);
        _myOpt = _target->GetOptions();

        _myOpt.minX = TextUtils::ToDouble(_minXCtrl->GetValue());
        _myOpt.maxX = TextUtils::ToDouble(_maxXCtrl->GetValue());
        _myOpt.minY = TextUtils::ToDouble(_minYCtrl->GetValue());

        if (_manualMaxYChk->GetValue())
            _myOpt.maxY = TextUtils::ToDouble(_maxYCtrl->GetValue());
        else
            _myOpt.maxY = _myOpt.minY + (_myOpt.maxX - _myOpt.minX);

        _myOpt.maxIter = TextUtils::ToInt(_iterCtrl->GetValue());

        _target->SetOptions(_myOpt);
        _target->PrepareRender();
        _target->Render();

        this->WriteText("Starting to render preview\n");
        _previewButton->SetLabel("Stop");
        _calcButton->Enable(false);
        _savePreviewButton->Enable(false);

        if (_scriptSelected)
            _progressBar->Enable(false);

        _renderingPreview = true;
    }
    else
    {
        _target->StopRender();
        _logCtrl->WriteText("Preview render stopped\n");
        _previewButton->SetLabel("Render preview");
        _progressBar->SetValue(0);
        _progressTxt->SetLabel(wxString("Progress: Stopped"));
        _calcButton->Enable(true);
        _savePreviewButton->Enable(true);

        if (_scriptSelected)
            _progressBar->Enable(true);

        _renderingPreview = false;
    }
}
void DimensionFrame::OnChangeFractal(wxCommandEvent&)
{
    // Set default parameters.
    this->CreateFractal(_previewSize);
    _myOpt = _target->GetOptions();
    _minXCtrl->SetValue(TextUtils::ToWxString(_myOpt.minX));
    _maxXCtrl->SetValue(TextUtils::ToWxString(_myOpt.maxX));
    _minYCtrl->SetValue(TextUtils::ToWxString(_myOpt.minY));
    _maxYCtrl->SetValue(TextUtils::ToWxString(_myOpt.maxY));
    _iterCtrl->SetValue(TextUtils::ToWxString(static_cast<int>(_myOpt.maxIter)));
}
// ReSharper disable once CppMemberFunctionMayBeConst
void DimensionFrame::OnManualMaxY(wxCommandEvent&)
{
    if (_manualMaxYChk->GetValue())
        _maxYCtrl->Enable(true);
    else
        _maxYCtrl->Enable(false);
}
void DimensionFrame::OnClose(wxCommandEvent&)
{
    this->Close(true);
}
void DimensionFrame::OnCalculate(wxCommandEvent&)
{
    if (!_calculatingDimension)
    {
        // Create divisions vector.
        _div.clear();
        mup::ParserX parser;
        parser.SetExpr(_funcCtrl->GetValue().utf8_string());

        mup::Value xVal;
        parser.DefineVar("x", mup::Variable(&xVal));
        const int xMin = _xMinSpin->GetValue();
        const int xMax = _xMaxSpin->GetValue();

        bool errorStatus = false;
        if (_divNotebook->GetSelection() == 0)
        {
            // Get div values from parser.
            try
            {
                for (int x = xMin; x < xMax; x++)
                {
                    xVal = static_cast<double>(x);
                    _div.push_back(static_cast<int>(parser.Eval().GetInteger()));
                }
            }
            catch (mup::ParserError& error)
            {
                const wxString err = error.GetMsg();
                _logCtrl->WriteText("Parser error: ");
                _logCtrl->WriteText(err);
                _logCtrl->WriteText("\n");
                errorStatus = true;
            }
        }
        else
        {
            const wxString listNumbers = _listCtrl->GetValue();
            _div = TextUtils::ParseIntList(listNumbers);
            if (_div.empty())
                errorStatus = true;
        }

        if (!errorStatus)
        {
            // Create fractal.
            _size = TextUtils::ToInt(wxString(_sizeCtrl->GetValue()));
            _myOpt = _target->GetOptions();
            _myOpt.minX = TextUtils::ToDouble(_minXCtrl->GetValue());
            _myOpt.maxX = TextUtils::ToDouble(_maxXCtrl->GetValue());
            _myOpt.minY = TextUtils::ToDouble(_minYCtrl->GetValue());

            if (_manualMaxYChk->GetValue())
                _myOpt.maxY = TextUtils::ToDouble(_maxYCtrl->GetValue());
            else
                _myOpt.maxY = _myOpt.minY + (_myOpt.maxX - _myOpt.minX);

            _myOpt.maxIter = TextUtils::ToInt(_iterCtrl->GetValue());

            // Compare with previous options.
            const Options tempOpt = _target->GetOptions();

            if (tempOpt.minX != _myOpt.minX || tempOpt.maxX != _myOpt.maxX || tempOpt.minY != _myOpt.minY ||
                tempOpt.maxY != _myOpt.maxY || tempOpt.maxIter != _myOpt.maxIter || tempOpt.screenWidth != _size ||
                _target->GetChangeFractalProp() || _firstRender)
            {
                // If a change was made or the render fractal was just created.
                _target->Resize(_size, _size);
                _target->SetOptions(_myOpt);
                _target->PrepareRender();
                _target->Render();
                _firstRender = false;
            }

            // Divide thread assignment.
            const int sizeDiv = _size / _threadNumber;
            for (int i = 0; i < _threadNumber; i++)
            {
                if (i < _threadNumber - 1)
                    _dimensionCalculator[i].SetMap(_target->GetSetMap(), _size, i * sizeDiv, (i + 1) * sizeDiv);
                else
                    _dimensionCalculator[i].SetMap(_target->GetSetMap(), _size, i * sizeDiv, _size);
            }

            if (!_div.empty())
            {
                _divIndex = -1;
                _calcButton->SetLabel("Stop");
                _previewButton->Enable(false);
                _savePreviewButton->Enable(false);
                if (_scriptSelected) _progressBar->Enable(false);
                _calculatingDimension = true;
            }
        }
    }
    else
    {
        // Stop render.
        _target->StopRender();

        StopDimensionThreads();
        _calcButton->SetLabel("Calculate");
        _previewButton->Enable(true);
        _savePreviewButton->Enable(true);
        if (_scriptSelected) _progressBar->Enable(true);
        _logCtrl->WriteText("Calculation stopped\n");
        _progressBar->SetValue(0);
        _progressTxt->SetLabel(wxString("Calculation stopped\n"));
        _calculatingDimension = false;
        _firstRender = true;
    }
}
void DimensionFrame::OnUpdateUI(wxUpdateUIEvent&)
{
    if (_clock.getElapsedTime().asSeconds() >= 0.05)
    {
        if (_renderingPreview)
        {
            if (_target->IsRendering())
            {
                // Update progress while rendering preview.
                _progress = _target->GetRenderProgress();
                if (_scriptSelected)
                    _progressTxt->SetLabel(wxString("Rendering"));
                else
                {
                    _progressBar->SetValue(_progress);
                    _progressTxt->SetLabel(wxString("Progress: ") + TextUtils::ToWxString(_progress) + "%");
                }
            }
            else
            {
                // Set output image.
                _previewImage->SetMap(_target->GetSetMap(), _numberOfDivisionsSpinCtrl->GetValue());
                _previewImage->Refresh();
                _progressBar->SetValue(0);
                _progressTxt->SetLabel(wxString("Progress: Done"));
                this->WriteText("Done\n");
                _previewButton->SetLabel("Render preview");
                _calcButton->Enable(true);
                _savePreviewButton->Enable(true);

                if (_scriptSelected)
                    _progressBar->Enable(true);

                _renderingPreview = false;
            }
        }
        if (_calculatingDimension)
        {
            if (!_target->IsRendering())
            {
                // Check if there are threads running.
                bool threadRunning = false;
                for (int i = 0; i < _threadNumber; i++)
                {
                    if (_dimensionCalculator[i].IsRunning())
                        threadRunning = true;
                }

                if (!threadRunning)
                {
                    JoinDimensionThreads();

                    if (_divIndex == -1)
                    {
                        // Launch the first pack of threads.
                        this->WriteText("Starting box count.\n");
                        this->WriteText("Epsilon   |   BoxCount.\n");
                        this->WriteText("-------------------\n");
                        _divIndex++;
                        for (int i = 0; i < _threadNumber; i++)
                        {
                            _dimensionCalculator[i].SetDiv(_div[_divIndex]);
                            _dimThreads[i] = std::make_unique<sf::Thread>(&BoxCountWorker::Run, &_dimensionCalculator[i]);
                            _dimThreads[i]->launch();
                        }

                        _epsilon.clear();
                        _boxCount.clear();
                    }
                    else if (_divIndex < static_cast<int>(_div.size()) - 1)
                    {
                        // Update progress bar.
                        _progress = static_cast<int>(50 * (1 + static_cast<double>(_divIndex) / static_cast<double>(_div.size())));
                        _progressBar->SetValue(_progress);
                        _progressTxt->SetLabel(wxString("Progress: ") + TextUtils::ToWxString(_progress) + "%");

                        // Get box count.
                        int boxNumber = 0;
                        for (int i = 0; i < _threadNumber; i++)
                            boxNumber += _dimensionCalculator[i].GetBoxCount();

                        _epsilon.push_back(static_cast<double>(_size) / static_cast<double>(_div[_divIndex]));
                        _boxCount.push_back(boxNumber);

                        // Update log text.
                        wxString logOut = TextUtils::ToWxString(_epsilon[_divIndex]);
                        logOut += ", ";
                        logOut += TextUtils::ToWxString(_boxCount[_divIndex]);
                        logOut += "\n";
                        this->WriteText(logOut);

                        // Prepare new size and launch.
                        _divIndex++;
                        for (int i = 0; i < _threadNumber; i++)
                        {
                            _dimensionCalculator[i].SetDiv(_div[_divIndex]);
                            _dimThreads[i] = std::make_unique<sf::Thread>(&BoxCountWorker::Run, &_dimensionCalculator[i]);
                            _dimThreads[i]->launch();
                        }
                    }
                    else
                    {
                        // Calculate dimension.
                        vector<double> logEpsilon, logCount;
                        for (unsigned int i = 0; i < _epsilon.size(); i++)
                        {
                            logEpsilon.push_back(log(1.0 / _epsilon[i]));
                            logCount.push_back(log(static_cast<double>(_boxCount[i])));
                        }

                        // Do least squares fitting for m.
                        double sumX, sumY, sumXSquared;
                        double sumXY = sumX = sumY = sumXSquared = 0;
                        const double n = static_cast<double>(_epsilon.size());
                        for (int i = 0; i < n; i++)
                        {
                            sumXY += logEpsilon[i] * logCount[i];
                            sumX += logEpsilon[i];
                            sumY += logCount[i];
                            sumXSquared += pow(logEpsilon[i], 2);
                        }
                        const double dimensionFit = (n * sumXY - sumX * sumY) / (n * sumXSquared - pow(sumX, 2));
                        this->WriteText("Dimension = ");
                        this->WriteText(TextUtils::ToWxString(dimensionFit));
                        this->WriteText("\n");

                        // Least squares for b.
                        double b = (sumY * sumXSquared - sumX * sumXY) / (n * sumXSquared - pow(sumX, 2));

                        // Draw Plot
                        if (_dataCheck->GetValue())
                        {
                            vector<double> doubleCount;
                            doubleCount.reserve(_boxCount.size());
                            for (int i : _boxCount)
                                doubleCount.push_back(i);

                            auto plot = new PlotWindow(_epsilon, doubleCount, this, wxID_ANY, "Data plot");    // Txt: "Data plot"
                            plot->Show(true);
                        }

                        // Draw fitted plot.
                        if (_dataFitCheck->GetValue())
                        {
                            LineParams myLine{};
                            myLine.m = dimensionFit;
                            myLine.b = b;
                            auto plot = new PlotWindow(myLine, logEpsilon, logCount, this, wxID_ANY, "Fitted data plot");    // Txt: "Fitted data plot"
                            plot->Show(true);
                        }

                        // Update progress bar.
                        _progressBar->SetValue(0);
                        _progressTxt->SetLabel(wxString("Progress: Done"));
                        this->WriteText("Done\n");

                        _calcButton->SetLabel("Calculate");
                        _previewButton->Enable(true);
                        _savePreviewButton->Enable(true);
                        if (_scriptSelected) _progressBar->Enable(true);
                        _calculatingDimension = false;
                    }
                }
            }
            else
            {
                // Updates progress bar while rendering.
                _progress = _target->GetRenderProgress() / 2;

                if (_scriptSelected)
                    _progressTxt->SetLabel(wxString("Calculating dimension"));
                else
                {
                    _progressBar->SetValue(_progress);
                    _progressTxt->SetLabel(wxString("Progress: ") + TextUtils::ToWxString(_progress) + "%");
                }
            }
        }
        _clock.restart();
    }
}
void DimensionFrame::OnDestroy(wxCloseEvent&)
{
    wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_DIMENSION_FRAME_CLOSED));
    this->Destroy();
}
void DimensionFrame::WriteText(const wxString &txt) const
{
    _logCtrl->MoveEnd();
    _logCtrl->WriteText(txt);
    _logCtrl->ShowPosition(_logCtrl->GetCaretPosition());
}
void DimensionFrame::OnFractalOpt(wxCommandEvent&)
{
    if (_confFractOptDialog == nullptr)
        _confFractOptDialog = new ConfigFractalOptionsDialog(_target, this);

    if (_confFractOptDialog->IsVisible())
        _confFractOptDialog->SetFocus();
    else
        _confFractOptDialog->Show(true);

    // Adjust position.
    int h, w;
    GetDesktopResolution(h, w);
    if (this->GetPosition().x + this->GetSize().GetWidth() + 5 < w && this->GetPosition().y < h)
        _confFractOptDialog->Move(this->GetPosition().x + this->GetSize().GetWidth() + 5, this->GetPosition().y);
}
void DimensionFrame::OnSavePreview(wxCommandEvent&)
{
    auto openFileDialog = new wxFileDialog(this, "Select file name", "",
                               "dimension_preview.bmp", "BMP file (*.bmp)|*.bmp", wxFD_SAVE);
    if (openFileDialog->ShowModal() == wxID_OK)
    {
        wxString wxFileName = openFileDialog->GetPath();
        wxString fileName = wxFileName.c_str();

        // Render the fractal.
        _size = TextUtils::ToInt(wxString(_sizeCtrl->GetValue().c_str()));
        _target->Resize(_size, _size);
        _myOpt = _target->GetOptions();
        _myOpt.minX = TextUtils::ToDouble(_minXCtrl->GetValue());
        _myOpt.maxX = TextUtils::ToDouble(_maxXCtrl->GetValue());
        _myOpt.minY = TextUtils::ToDouble(_minYCtrl->GetValue());

        if (_manualMaxYChk->GetValue())
            _myOpt.maxY = TextUtils::ToDouble(_maxYCtrl->GetValue());
        else
            _myOpt.maxY = _myOpt.minY + (_myOpt.maxX - _myOpt.minX);

        _myOpt.maxIter = TextUtils::ToInt(_iterCtrl->GetValue());

        _target->SetOptions(_myOpt);
        _target->PrepareRender();
        _target->Render();

        auto* saveProgress = new ImageExportProgressDialog(_target, this, false);
        saveProgress->ShowModal();

        if (saveProgress->IsFinished())
        {
            // Allocate.
            bool** setMap, ** tempSetMap, ** colorMap;
            setMap = _target->GetSetMap();
            int nDiv = _numberOfDivisionsSpinCtrl->GetValue();

            tempSetMap = new bool* [_size];
            colorMap = new bool* [_size];
            for (int i = 0; i < _size; i++)
            {
                tempSetMap[i] = new bool[_size];
                colorMap[i] = new bool[_size];
            }
            // Copy and init.
            for (int i = 0; i < _size; i++)
            {
                for (int j = 0; j < _size; j++)
                {
                    tempSetMap[i][j] = setMap[i][j];
                    colorMap[i][j] = false;
                }
            }

            //int N = 0;
            double local_epsilon = static_cast<double>(_size) / static_cast<double>(nDiv);

            for (int ey = 0; ey < nDiv; ey++)
            {
                for (int ex = 0; ex < nDiv; ex++)
                {
                    bool found = false;
                    for (int w = static_cast<int>(ex * local_epsilon); w < (ex + 1) * local_epsilon && !found; w++)
                    {
                        for (int h = static_cast<int>(ey * local_epsilon); h < (ey + 1) * local_epsilon; h++)
                        {
                            if (w < _size && h < _size)
                            {
                                if (setMap[w][h] == true)
                                {
                                    found = true;
                                    // Color square
                                    for (int y = static_cast<int>(ey * local_epsilon); y < (ey + 1) * local_epsilon; y++)
                                    {
                                        for (int x = static_cast<int>(ex * local_epsilon); x < (ex + 1) * local_epsilon; x++)
                                        {
                                            if (x < _size && y < _size)
                                                colorMap[x][y] = true;
                                        }
                                    }
                                    break;
                                }
                            }
                            else
                                break;
                        }
                    }
                }
            }

            // Horizontal lines.
            for (int ey = 0; ey < nDiv; ey++)
            {
                int y = static_cast<int>(ey * local_epsilon);
                for (int x = 0; x < _size; x++)
                    tempSetMap[x][y] = true;
            }
            // Last line.
            for (int x = 0; x < _size; x++)
                tempSetMap[x][_size - 1] = true;

            // Vertical lines
            for (int ex = 0; ex < nDiv; ex++)
            {
                int x = static_cast<int>(ex * local_epsilon);
                for (int y = 0; y < _size; y++)
                    tempSetMap[x][y] = true;
            }
            // Last line.
            for (int y = 0; y < _size; y++)
                tempSetMap[_size - 1][y] = true;

            // Write BMP.
            const std::string outputPath(fileName.mb_str());
            BmpImageWriter writer(outputPath, _size, _size);
            std::vector<BmpPixel> data(_size);
            bool writeSuccess = writer.IsOpen();

            // Copy maps values to BmpImageWriter.
            for (int j = 0; j < _size && writeSuccess; j++)
            {
                for (int i = 0; i < _size; i++)
                {
                    if (tempSetMap[i][j] == true)
                    {
                        data[i].r = 0;
                        data[i].g = 0;
                        data[i].b = 0;
                    }
                    else if (colorMap[i][j] == true)
                    {
                        data[i].r = static_cast<unsigned>(0x32);
                        data[i].g = static_cast<unsigned>(0x32);
                        data[i].b = static_cast<unsigned>(0xFF);
                    }
                    else
                    {
                        data[i].r = static_cast<unsigned>(0xFF);
                        data[i].g = static_cast<unsigned>(0xFF);
                        data[i].b = static_cast<unsigned>(0xFF);
                    }
                }
                writeSuccess = writer.WriteRow(data);
            }
            writeSuccess = writer.Close() && writeSuccess;
            if (!writeSuccess)
                wxMessageBox("Failed to save image to file: " + outputPath, "Error", wxOK | wxICON_ERROR);

            // Cleanup.
            for (int i = 0; i < _size; i++)
            {
                delete[] tempSetMap[i];
                delete[] colorMap[i];
            }
            delete[] tempSetMap;
            delete[] colorMap;
        }
        saveProgress->Destroy();
    }
    openFileDialog->Destroy();
}
void DimensionFrame::GetScriptFractals()
{
    _loadedScripts = GetValidUserScripts();

    // Gets script parameters.
    for (unsigned int i = 0; i < _loadedScripts.size(); i++)
    {
        if (!_loadedScripts[i].noSetMap)
        {
            _scriptList.push_back(i);
            _fractalChoice->Append(wxString(_loadedScripts[i].name.c_str(), wxConvUTF8));
        }
    }
}
void DimensionFrame::OnHelp(wxCommandEvent&)
{
    const auto diag = new DocumentViewer(AppPaths::ResourceFile({"Tutorials", "dimTut.html"}),
                                         this, wxID_ANY, wxString("Calculate dimension help"));
    diag->ShowModal();
    diag->Destroy();
}
