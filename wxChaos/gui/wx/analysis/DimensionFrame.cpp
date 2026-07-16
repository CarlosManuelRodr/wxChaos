#include <limits>
#include <mpParser.h>
#include <wx/bmpbndl.h>
#include <wx/spinctrl.h>
#include "common/AppTheme.h"
#include "AppPaths.h"
#include "analysis/DimensionFrame.h"
#include "TextUtils.h"
#include "export/ImageExportSizeDialog.h"
#include "fractal/FormulaDialog.h"
#include "BmpImageWriter.h"
#include "AngelscriptBindings.h"
#include "docs/DocumentViewer.h"
#include "SystemUtilities.h"
using namespace std;

wxDEFINE_EVENT(wxEVT_DIMENSION_FRAME_CLOSED, wxCommandEvent);

DimensionFrame::DimensionFrame(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
                               const wxSize& size, const long style)
                               : wxFrame(parent, id, wxGetTranslation(title), pos, size, style), _previewTimer(this)
{
    _threadNumber = Get_Cores();
    _dimensionCalculator.resize(_threadNumber);
    _dimThreads.resize(_threadNumber);

    _previewSize = 400;
    _target = nullptr;
    _fractalOptionsDialog = nullptr;
    _fractalOptionsPanel = nullptr;
    _renderingPreview = false;
    _calculatingDimension = false;
    _suppressPreviewUpdate = true;
    _previewRenderQueued = false;
    _hasPreviewMap = false;
    _scriptSelected = false;
    _firstRender = true;
    _userFormula.userFormula = "z = z^2 + c";
    _userFormula.type = FormulaType::Complex;
    _userFormula.julia = false;
    _userFormula.bailout = 4;
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
    fractalSectionSizer->Add(CreateSectionHeader(_mainPanel, _("Fractal parameters"),
                                                 "fractal_light.svg", "fractal_dark.svg"),
                             0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    const auto fractalBoxSizer = new wxBoxSizer(wxVERTICAL);

    _fractalChoice = new wxChoice(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, 0);
    fractalBoxSizer->Add(CreateFractalParameterRow(_("Fractal"), _fractalChoice), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    const auto parameterGridSizer = new wxFlexGridSizer(0, 2, 0, 8);
    parameterGridSizer->AddGrowableCol(0, 1);
    parameterGridSizer->AddGrowableCol(1, 1);

    _minXCtrl = CreateCoordinateSpin("-1.5");
    parameterGridSizer->Add(CreateFractalParameterRow(_("Min X"), _minXCtrl), 1, wxEXPAND | wxALL, 5);

    _maxXCtrl = CreateCoordinateSpin("1.5");
    parameterGridSizer->Add(CreateFractalParameterRow(_("Max X"), _maxXCtrl), 1, wxEXPAND | wxALL, 5);

    _minYCtrl = CreateCoordinateSpin("-0.4");
    parameterGridSizer->Add(CreateFractalParameterRow(_("Min Y"), _minYCtrl), 1, wxEXPAND | wxALL, 5);

    _maxYCtrl = CreateCoordinateSpin("0.4");
    _maxYCtrl->Enable(false);
    parameterGridSizer->Add(CreateFractalParameterRow(_("Max Y"), _maxYCtrl), 1, wxEXPAND | wxALL, 5);

    _iterCtrl = new wxSpinCtrl(_mainPanel, wxID_ANY, "20000", wxDefaultPosition, wxDefaultSize,
                               wxSP_ARROW_KEYS, 1, 100000000, 20000);
    parameterGridSizer->Add(CreateFractalParameterRow(_("Iterations"), _iterCtrl), 1, wxEXPAND | wxALL, 5);

    _sizeCtrl = new wxSpinCtrl(_mainPanel, wxID_ANY, "3000", wxDefaultPosition, wxDefaultSize,
                               wxSP_ARROW_KEYS, 32, 100000, 3000);
    parameterGridSizer->Add(CreateFractalParameterRow(_("Square image size (pixels)"), _sizeCtrl), 1, wxEXPAND | wxALL, 5);
    fractalBoxSizer->Add(parameterGridSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);

    _fractalOptionsButton = new wxButton(_mainPanel, wxID_ANY, _("Configure fractal options"), wxDefaultPosition, wxDefaultSize, 0);
    fractalBoxSizer->Add(_fractalOptionsButton, 0, wxALL | wxEXPAND, 10);

    _formulaButton = new wxButton(_mainPanel, wxID_ANY, _("Edit user formula"), wxDefaultPosition, wxDefaultSize, 0);
    _formulaButton->Hide();
    fractalBoxSizer->Add(_formulaButton, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    fractalSectionSizer->Add(fractalBoxSizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    paramBoxSizer->Add(fractalSectionSizer, 0, wxEXPAND, 5);

    const auto dimBoxSizer = new wxBoxSizer(wxVERTICAL);
    dimBoxSizer->Add(CreateSectionHeader(_mainPanel, _("Box-counting parameters"),
                                         "box_count_light.svg", "box_count_dark.svg"),
                     0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _divTxt = new wxStaticText(_mainPanel, wxID_ANY, _("Divisions"), wxDefaultPosition, wxDefaultSize, 0);
    _divTxt->Wrap(-1);
    dimBoxSizer->Add(_divTxt, 0, wxALL, 5);

    const auto divBoxSizer = new wxBoxSizer(wxVERTICAL);

    _divNotebook = new wxNotebook(_mainPanel, wxID_ANY, wxDefaultPosition, wxSize(-1, 150), 0);
    _byFunctionPanel = new wxPanel(_divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto byFunctionBoxSizer = new wxBoxSizer(wxVERTICAL);
    const auto functionRowSizer = new wxBoxSizer(wxHORIZONTAL);

    _funcTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, _("Function:"), wxDefaultPosition, wxDefaultSize, 0);
    _funcTxt->Wrap(-1);
    functionRowSizer->Add(_funcTxt, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _fDeclTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, "f(x) = ", wxDefaultPosition, wxDefaultSize, 0);
    _fDeclTxt->Wrap(-1);
    functionRowSizer->Add(_fDeclTxt, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _funcCtrl = new wxTextCtrl(_byFunctionPanel, wxID_ANY, "2*x", wxDefaultPosition, wxDefaultSize, 0);
    functionRowSizer->Add(_funcCtrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    byFunctionBoxSizer->Add(functionRowSizer, 0, wxEXPAND);

    const auto rangeSizer = new wxBoxSizer(wxHORIZONTAL);

    _goesFromTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, _("x goes from"), wxDefaultPosition, wxDefaultSize, 0);
    _goesFromTxt->Wrap(-1);
    rangeSizer->Add(_goesFromTxt, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _xMinSpin = new wxSpinCtrl(_byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(110, -1),
                               wxSP_ARROW_KEYS, 1, 1000000, 1);
    rangeSizer->Add(_xMinSpin, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _goesToTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, _("to"), wxDefaultPosition, wxDefaultSize, 0);
    _goesToTxt->Wrap(-1);
    rangeSizer->Add(_goesToTxt, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _xMaxSpin = new wxSpinCtrl(_byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(110, -1),
                               wxSP_ARROW_KEYS, 1, 1000000, 50);
    rangeSizer->Add(_xMaxSpin, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    byFunctionBoxSizer->Add(rangeSizer, 0, wxEXPAND);

    _byFunctionPanel->SetSizer(byFunctionBoxSizer);
    _byFunctionPanel->Layout();
    byFunctionBoxSizer->Fit(_byFunctionPanel);
    _divNotebook->AddPage(_byFunctionPanel, _("By function"), true);
    _byListPanel = new wxPanel(_divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto byListBoxSizer = new wxBoxSizer(wxVERTICAL);

    _listCtrl = new wxTextCtrl(_byListPanel, wxID_ANY, "2,4,5,6,9,100,200", wxDefaultPosition, wxSize(-1, 80), wxTE_MULTILINE);
    byListBoxSizer->Add(_listCtrl, 1, wxALL | wxEXPAND, 5);

    _byListPanel->SetSizer(byListBoxSizer);
    _byListPanel->Layout();
    byListBoxSizer->Fit(_byListPanel);
    _divNotebook->AddPage(_byListPanel, _("By list"), false);

    divBoxSizer->Add(_divNotebook, 0, wxEXPAND | wxALL, 5);
    dimBoxSizer->Add(divBoxSizer, 0, wxEXPAND, 5);

    dimBoxSizer->Add(CreateSectionHeader(_mainPanel, _("Plotting"), "plot_light.svg", "plot_dark.svg"),
                     0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    const auto plotBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _dataCheck = new wxCheckBox(_mainPanel, wxID_ANY, _("Plot data"), wxDefaultPosition, wxDefaultSize, 0);
    plotBoxSizer->Add(_dataCheck, 0, wxALL, 5);

    _dataFitCheck = new wxCheckBox(_mainPanel, wxID_ANY, _("Plot fitted data"), wxDefaultPosition, wxDefaultSize, 0);
    plotBoxSizer->Add(_dataFitCheck, 0, wxALL, 5);
    dimBoxSizer->Add(plotBoxSizer, 0, wxEXPAND, 5);

    dimBoxSizer->AddStretchSpacer(1);

    _resolutionWarning = new wxStaticText(_mainPanel, wxID_ANY, wxEmptyString,
                                          wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
    _resolutionWarning->Wrap(420);
    _resolutionWarning->SetForegroundColour(AppTheme::IsDark() ? wxColour(242, 190, 95) : wxColour(128, 82, 0));
    _resolutionWarning->Hide();
    dimBoxSizer->Add(_resolutionWarning, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    const auto buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _calcButton = new wxButton(_mainPanel, wxID_ANY, _("Calculate"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_calcButton, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    _closeButton = new wxButton(_mainPanel, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_closeButton, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    const wxSize actionButtonSize(_calcButton->GetBestSize().GetHeight(), _calcButton->GetBestSize().GetHeight());
    _helpButton = new wxBitmapButton(_mainPanel, wxID_ANY,
                                     CreateIconBundle("help_light.svg", "help_dark.svg", wxSize(18, 18)),
                                     wxDefaultPosition, actionButtonSize, wxBU_AUTODRAW);
    _helpButton->SetMinSize(actionButtonSize);
    buttonBoxSizer->Add(_helpButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    paramBoxSizer->Add(dimBoxSizer, 1, wxEXPAND, 5);
    paramBoxSizer->Add(buttonBoxSizer, 0, wxEXPAND, 5);
    subMainBoxSizer->Add(paramBoxSizer, 1, wxEXPAND, 5);

    const auto outputBoxSizer = new wxBoxSizer(wxVERTICAL);
    outputBoxSizer->Add(CreateSectionHeader(_mainPanel, _("Box count preview"), "box_light.svg", "box_dark.svg"),
                        0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _previewImage = new ImagePanel(_mainPanel, wxID_ANY, _previewSize);
    _previewImage->SetMinSize(wxSize(_previewSize, _previewSize));
    outputBoxSizer->Add(_previewImage, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 8);

    const auto previewOptionsSizer = new wxBoxSizer(wxVERTICAL);
    const auto numberOfDivisionsSizer = new wxBoxSizer(wxHORIZONTAL);
    _nDivTxt = new wxStaticText(_mainPanel, wxID_ANY, _("Number of divisions"), wxDefaultPosition, wxDefaultSize, 0);
    _nDivTxt->Wrap(-1);
    numberOfDivisionsSizer->Add(_nDivTxt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    _numberOfDivisionsSpinCtrl = new wxSpinCtrl(_mainPanel, wxID_ANY, "20", wxDefaultPosition, wxSize(110, -1),
                                                wxSP_ARROW_KEYS, 1, 200, 0);
    numberOfDivisionsSizer->Add(_numberOfDivisionsSpinCtrl, 0, wxALIGN_CENTER_VERTICAL);
    previewOptionsSizer->Add(numberOfDivisionsSizer, 0, wxALL, 5);

    _savePreviewButton = new wxButton(_mainPanel, wxID_ANY, _("Save preview"), wxDefaultPosition, wxDefaultSize, 0);
    previewOptionsSizer->Add(_savePreviewButton, 0, wxALL | wxEXPAND, 5);
    outputBoxSizer->Add(previewOptionsSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    _outLine = new wxStaticLine(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    outputBoxSizer->Add(_outLine, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    outputBoxSizer->Add(CreateSectionHeader(_mainPanel, _("Dimension count log"), "log_light.svg", "log_dark.svg"),
                        0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _logCtrl = new wxRichTextCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxTE_READONLY | wxVSCROLL | wxHSCROLL | wxNO_BORDER | wxWANTS_CHARS);
    outputBoxSizer->Add(_logCtrl, 1, wxALL | wxEXPAND, 5);

    _progressBar = new wxGauge(_mainPanel, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
    outputBoxSizer->Add(_progressBar, 0, wxALL | wxEXPAND, 5);

    _progressTxt = new wxStaticText(_mainPanel, wxID_ANY, _("Progress: Stopped"), wxDefaultPosition, wxDefaultSize, 0);
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
    _logCtrl->WriteText(_("Dimension calculator log.\n\n"));

    PopulateFractalChoices();

    // Set the default fractal.
    SelectDefaultFractal();
    this->CreateFractal(_previewSize);
    UpdateFormulaButtonVisibility();
    _myOpt = _target->GetOptions();
    SetControlsFromOptions(_myOpt);
    ApplySelectedFractalPreset();

    // Connect Events.
    this->Bind(wxEVT_CLOSE_WINDOW, &DimensionFrame::OnDestroy, this);
    this->Bind(wxEVT_UPDATE_UI, &DimensionFrame::OnUpdateUI, this);
    this->Bind(wxEVT_TIMER, &DimensionFrame::OnPreviewTimer, this, _previewTimer.GetId());
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &DimensionFrame::OnClose, this, wxID_EXIT);
    _fractalChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &DimensionFrame::OnChangeFractal, this);
    _fractalOptionsButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnFractalOptions, this);
    _formulaButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnFormula, this);
    _calcButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnCalculate, this);
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnClose, this);
    _savePreviewButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnSavePreview, this);
    _helpButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnHelp, this);
    _minXCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &DimensionFrame::OnPreviewDoubleParameterChanged, this);
    _maxXCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &DimensionFrame::OnPreviewDoubleParameterChanged, this);
    _minYCtrl->Bind(wxEVT_SPINCTRLDOUBLE, &DimensionFrame::OnPreviewDoubleParameterChanged, this);
    _iterCtrl->Bind(wxEVT_SPINCTRL, &DimensionFrame::OnPreviewParameterChanged, this);
    _sizeCtrl->Bind(wxEVT_SPINCTRL, &DimensionFrame::OnPreviewParameterChanged, this);
    _numberOfDivisionsSpinCtrl->Bind(wxEVT_SPINCTRL, &DimensionFrame::OnPreviewGridChanged, this);
    _funcCtrl->Bind(wxEVT_TEXT, &DimensionFrame::OnDivisionDefinitionChanged, this);
    _xMaxSpin->Bind(wxEVT_SPINCTRL, &DimensionFrame::OnDivisionDefinitionChanged, this);
    _listCtrl->Bind(wxEVT_TEXT, &DimensionFrame::OnDivisionDefinitionChanged, this);
    _divNotebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &DimensionFrame::OnDivisionModeChanged, this);
    _suppressPreviewUpdate = false;
    UpdateResolutionWarning();
    SchedulePreviewRender();
}
DimensionFrame::~DimensionFrame()
{
    // Disconnect Events.
    this->Unbind(wxEVT_CLOSE_WINDOW, &DimensionFrame::OnDestroy, this);
    this->Unbind(wxEVT_UPDATE_UI, &DimensionFrame::OnUpdateUI, this);
    this->Unbind(wxEVT_TIMER, &DimensionFrame::OnPreviewTimer, this, _previewTimer.GetId());
    _fractalChoice->Unbind(wxEVT_COMMAND_CHOICE_SELECTED, &DimensionFrame::OnChangeFractal, this);
    _fractalOptionsButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnFractalOptions, this);
    _formulaButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnFormula, this);
    _calcButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnCalculate, this);
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnClose, this);
    _savePreviewButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnSavePreview, this);
    _helpButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnHelp, this);
    _minXCtrl->Unbind(wxEVT_SPINCTRLDOUBLE, &DimensionFrame::OnPreviewDoubleParameterChanged, this);
    _maxXCtrl->Unbind(wxEVT_SPINCTRLDOUBLE, &DimensionFrame::OnPreviewDoubleParameterChanged, this);
    _minYCtrl->Unbind(wxEVT_SPINCTRLDOUBLE, &DimensionFrame::OnPreviewDoubleParameterChanged, this);
    _iterCtrl->Unbind(wxEVT_SPINCTRL, &DimensionFrame::OnPreviewParameterChanged, this);
    _sizeCtrl->Unbind(wxEVT_SPINCTRL, &DimensionFrame::OnPreviewParameterChanged, this);
    _numberOfDivisionsSpinCtrl->Unbind(wxEVT_SPINCTRL, &DimensionFrame::OnPreviewGridChanged, this);
    _funcCtrl->Unbind(wxEVT_TEXT, &DimensionFrame::OnDivisionDefinitionChanged, this);
    _xMaxSpin->Unbind(wxEVT_SPINCTRL, &DimensionFrame::OnDivisionDefinitionChanged, this);
    _listCtrl->Unbind(wxEVT_TEXT, &DimensionFrame::OnDivisionDefinitionChanged, this);
    _divNotebook->Unbind(wxEVT_NOTEBOOK_PAGE_CHANGED, &DimensionFrame::OnDivisionModeChanged, this);

    if (_calculatingDimension)
    {
        // Stop render.
        _target->StopRender();
        _calcButton->SetLabel(_("Calculate"));
        _calculatingDimension = false;
    }
    StopDimensionThreads();

    if (_renderingPreview)
        _target->StopRender();

    _fractalFactory.DeleteFractal();
    delete _fractalOptionsDialog;
}

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

wxSizer* DimensionFrame::CreateFractalParameterRow(const wxString& label, wxWindow* control) const
{
    const auto rowSizer = new wxBoxSizer(wxVERTICAL);
    const auto labelText = new wxStaticText(_mainPanel, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, 0);
    labelText->Wrap(-1);
    rowSizer->Add(labelText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 2);
    rowSizer->Add(control, 0, wxEXPAND);
    return rowSizer;
}

wxSpinCtrlDouble* DimensionFrame::CreateCoordinateSpin(const wxString& value) const
{
    const auto spin = new wxSpinCtrlDouble(_mainPanel, wxID_ANY, value, wxDefaultPosition, wxDefaultSize,
                                           wxSP_ARROW_KEYS, -100000000.0, 100000000.0, TextUtils::ToDouble(value), 0.01);
    spin->SetDigits(8);
    return spin;
}

Options DimensionFrame::ReadDimensionOptions()
{
    Options options = _target->GetOptions();
    options.minX = _minXCtrl->GetValue();
    options.maxX = _maxXCtrl->GetValue();
    options.minY = _minYCtrl->GetValue();
    options.maxY = options.minY + (options.maxX - options.minX);
    options.maxIterations = _iterCtrl->GetValue();
    options.antiAliasingScale = 1;
    _size = _sizeCtrl->GetValue();
    _maxYCtrl->SetValue(options.maxY);
    return options;
}

void DimensionFrame::UpdateDerivedMaxY() const
{
    const double maxY = _minYCtrl->GetValue() + (_maxXCtrl->GetValue() - _minXCtrl->GetValue());
    _maxYCtrl->SetValue(maxY);
}

void DimensionFrame::SetControlsFromOptions(const Options& options)
{
    _suppressPreviewUpdate = true;
    _minXCtrl->SetValue(options.minX);
    _maxXCtrl->SetValue(options.maxX);
    _minYCtrl->SetValue(options.minY);
    _iterCtrl->SetValue(static_cast<int>(options.maxIterations));
    UpdateDerivedMaxY();
    _suppressPreviewUpdate = false;
}

void DimensionFrame::SchedulePreviewRender()
{
    if (_suppressPreviewUpdate || _calculatingDimension)
        return;

    UpdateDerivedMaxY();
    if (_renderingPreview)
    {
        _previewRenderQueued = true;
        StopPreviewRender();
        return;
    }

    _previewTimer.StartOnce(450);
}

void DimensionFrame::StartPreviewRender()
{
    if (_calculatingDimension || _target == nullptr)
        return;

    _previewTimer.Stop();
    _target->Resize(_previewSize, _previewSize);
    _myOpt = ReadDimensionOptions();
    _target->SetOptions(_myOpt);
    _target->PrepareRender({0, 0});
    _target->Render();

    _calcButton->Enable(false);
    _savePreviewButton->Enable(false);

    if (_scriptSelected)
        _progressBar->Enable(false);

    _renderingPreview = true;
}

void DimensionFrame::StopPreviewRender() const
{
    if (_target != nullptr)
        _target->StopRender();

    _progressBar->SetValue(0);
    _progressTxt->SetLabel(_("Progress: Stopped"));
    _calcButton->Enable(true);
    _savePreviewButton->Enable(true);

    if (_scriptSelected)
        _progressBar->Enable(true);
}

void DimensionFrame::RefreshPreviewOverlayOnly()
{
    if (!_hasPreviewMap || _target == nullptr)
    {
        SchedulePreviewRender();
        return;
    }

    _previewImage->SetMap(_boxCountMap, _numberOfDivisionsSpinCtrl->GetValue());
    _previewImage->Refresh();
}

void DimensionFrame::UpdateBoxCountMap()
{
    if (_target != nullptr)
        _boxCountMap.Build(*_target);
}

void DimensionFrame::ConfigureDimensionWorkers()
{
    const int sizeDiv = _size / _threadNumber;
    for (int i = 0; i < _threadNumber; i++)
    {
        const int end = i < _threadNumber - 1 ? (i + 1) * sizeDiv : _size;
        _dimensionCalculator[i].SetMap(&_boxCountMap, i * sizeDiv, end);
    }
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

    if (choice < 0)
        return;

    if (choice < static_cast<int>(_builtInFractalList.size()))
    {
        _fractalFactory.CreateFractal(_builtInFractalList[choice], size, size);
        _scriptSelected = false;
    }
    else
    {
        const int idx = choice - static_cast<int>(_builtInFractalList.size());
        if (idx < 0 || idx >= static_cast<int>(_scriptList.size()))
            return;

        _fractalFactory.CreateScriptFractal(size, size, _loadedScripts[_scriptList[idx]]);
        _scriptSelected = true;
    }

    _target = _fractalFactory.GetFractal();
    if (_target != nullptr && _target->GetType() == FractalType::UserDefinedEscapeTime)
        _target->SetFormula(_userFormula);

    if (_fractalOptionsPanel != nullptr)
        _fractalOptionsPanel->SetTarget(_target);
}
void DimensionFrame::OnChangeFractal(wxCommandEvent&)
{
    _suppressPreviewUpdate = true;
    this->CreateFractal(_previewSize);
    UpdateFormulaButtonVisibility();
    _myOpt = _target->GetOptions();
    SetControlsFromOptions(_myOpt);
    ApplySelectedFractalPreset();
    if (_fractalOptionsPanel != nullptr)
        _fractalOptionsPanel->SetTarget(_target);
    _hasPreviewMap = false;
    _suppressPreviewUpdate = false;
    UpdateResolutionWarning();
    SchedulePreviewRender();
}

void DimensionFrame::OnPreviewTimer(wxTimerEvent&)
{
    StartPreviewRender();
}

void DimensionFrame::OnPreviewParameterChanged(wxCommandEvent&)
{
    UpdateResolutionWarning();
    SchedulePreviewRender();
}

void DimensionFrame::OnPreviewDoubleParameterChanged(wxSpinDoubleEvent&)
{
    SchedulePreviewRender();
}

void DimensionFrame::OnPreviewGridChanged(wxCommandEvent&)
{
    RefreshPreviewOverlayOnly();
}

void DimensionFrame::OnDivisionDefinitionChanged(wxCommandEvent&)
{
    UpdateResolutionWarning();
}

void DimensionFrame::OnDivisionModeChanged(wxBookCtrlEvent& event)
{
    UpdateResolutionWarning();
    event.Skip();
}

optional<int> DimensionFrame::GetUpperDivisionCount() const
{
    if (_divNotebook->GetSelection() == 0)
    {
        try
        {
            mup::ParserX parser;
            parser.SetExpr(_funcCtrl->GetValue().utf8_string());
            mup::Value xValue(static_cast<double>(_xMaxSpin->GetValue()));
            parser.DefineVar("x", mup::Variable(&xValue));
            const long long divisions = parser.Eval().GetInteger();
            if (divisions <= 0 || divisions > numeric_limits<int>::max())
                return nullopt;
            return static_cast<int>(divisions);
        }
        catch (mup::ParserError&)
        {
            return nullopt;
        }
    }

    const vector<int> divisions = TextUtils::ParseIntList(_listCtrl->GetValue());
    if (divisions.empty() || divisions.back() <= 0)
        return nullopt;
    return divisions.back();
}

void DimensionFrame::UpdateResolutionWarning()
{
    constexpr double minimumReliableBoxSizePixels = 2.0;
    const optional<int> upperDivisionCount = GetUpperDivisionCount();
    const double boxSizePixels = upperDivisionCount.has_value()
        ? static_cast<double>(_sizeCtrl->GetValue()) / *upperDivisionCount
        : numeric_limits<double>::infinity();
    const bool shouldShow = boxSizePixels <= minimumReliableBoxSizePixels;

    if (shouldShow)
    {
        _resolutionWarning->SetLabel(wxString::Format(
            _("The finest boxes are only %.2f pixels wide at the current image size. Increase the square image size "
              "or reduce the upper division limit for a more reliable dimension estimate."),
            boxSizePixels));
        _resolutionWarning->Wrap(420);
    }

    if (_resolutionWarning->IsShown() == shouldShow)
        return;

    _resolutionWarning->Show(shouldShow);
    _mainPanel->GetSizer()->Layout();
    _mainPanel->FitInside();
}
void DimensionFrame::OnClose(wxCommandEvent&)
{
    this->Close(true);
}
void DimensionFrame::OnCalculate(wxCommandEvent&)
{
    if (!_calculatingDimension)
    {
        _previewTimer.Stop();
        if (_renderingPreview)
        {
            StopPreviewRender();
            _renderingPreview = false;
            _previewRenderQueued = false;
        }

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
                _logCtrl->WriteText(_("Parser error: "));
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
            _myOpt = ReadDimensionOptions();

            // Compare with previous options.
            // ReSharper disable once CppTooWideScopeInitStatement
            const Options tempOpt = _target->GetOptions();

            if (tempOpt.minX != _myOpt.minX || tempOpt.maxX != _myOpt.maxX || tempOpt.minY != _myOpt.minY ||
                tempOpt.maxY != _myOpt.maxY || tempOpt.maxIterations != _myOpt.maxIterations || tempOpt.screenWidth != _size ||
                _target->GetChangeFractalProp() || _firstRender)
            {
                // If a change was made or the render fractal was just created.
                _target->Resize(_size, _size);
                _target->SetOptions(_myOpt);
                _target->PrepareRender({0, 0});
                _target->Render();
                _firstRender = false;
            }

            if (!_div.empty())
            {
                _divIndex = -1;
                _calcButton->SetLabel(_("Stop"));
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
        _calcButton->SetLabel(_("Calculate"));
        _savePreviewButton->Enable(true);
        if (_scriptSelected) _progressBar->Enable(true);
        _logCtrl->WriteText(_("Calculation stopped\n"));
        _progressBar->SetValue(0);
        _progressTxt->SetLabel(_("Calculation stopped\n"));
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
                    _progressTxt->SetLabel(_("Rendering"));
                else
                {
                    _progressBar->SetValue(_progress);
                    _progressTxt->SetLabel(_("Progress: ") + TextUtils::ToWxString(_progress) + "%");
                }
            }
            else
            {
                if (_previewRenderQueued)
                {
                    _previewRenderQueued = false;
                    _renderingPreview = false;
                    SchedulePreviewRender();
                    _clock.restart();
                    return;
                }

                // Set output image.
                UpdateBoxCountMap();
                _previewImage->SetMap(_boxCountMap, _numberOfDivisionsSpinCtrl->GetValue());
                _previewImage->Refresh();
                _hasPreviewMap = true;
                _progressBar->SetValue(0);
                _progressTxt->SetLabel(_("Progress: Done"));
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
                        UpdateBoxCountMap();
                        ConfigureDimensionWorkers();
                        this->WriteText(_("Starting box count.\n"));
                        this->WriteText(_("Epsilon   |   BoxCount.\n"));
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
                        _progressTxt->SetLabel(_("Progress: ") + TextUtils::ToWxString(_progress) + "%");

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
                        const auto n = static_cast<double>(_epsilon.size());
                        for (int i = 0; i < n; i++)
                        {
                            sumXY += logEpsilon[i] * logCount[i];
                            sumX += logEpsilon[i];
                            sumY += logCount[i];
                            sumXSquared += pow(logEpsilon[i], 2);
                        }
                        const double dimensionFit = (n * sumXY - sumX * sumY) / (n * sumXSquared - pow(sumX, 2));
                        this->WriteText(_("Dimension = "));
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

                            auto plot = new PlotWindow(_epsilon, doubleCount, this, wxID_ANY, _("Data plot"));
                            plot->Show(true);
                        }

                        // Draw fitted plot.
                        if (_dataFitCheck->GetValue())
                        {
                            LineParams myLine{};
                            myLine.m = dimensionFit;
                            myLine.b = b;
                            auto plot = new PlotWindow(myLine, logEpsilon, logCount, this, wxID_ANY, _("Fitted data plot"));
                            plot->Show(true);
                        }

                        // Update progress bar.
                        _progressBar->SetValue(0);
                        _progressTxt->SetLabel(_("Progress: Done"));
                        this->WriteText(_("Done\n"));

                        _calcButton->SetLabel(_("Calculate"));
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
                    _progressTxt->SetLabel(_("Calculating dimension"));
                else
                {
                    _progressBar->SetValue(_progress);
                    _progressTxt->SetLabel(_("Progress: ") + TextUtils::ToWxString(_progress) + "%");
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
void DimensionFrame::OnFractalOptions(wxCommandEvent&)
{
    if (_fractalOptionsDialog == nullptr)
    {
        _fractalOptionsDialog = new wxDialog(this, wxID_ANY, _("Fractal options"), wxDefaultPosition, wxSize(420, 560), wxCAPTION | wxCLOSE_BOX);
        const auto dialogSizer = new wxBoxSizer(wxVERTICAL);
        const auto scroll = new wxScrolledWindow(_fractalOptionsDialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
        scroll->SetScrollRate(5, 5);
        const auto scrollSizer = new wxBoxSizer(wxVERTICAL);
        _fractalOptionsPanel = new FractalOptionsPanel(scroll, true);
        _fractalOptionsPanel->SetApplyHandler([this]()
        {
            SchedulePreviewRender();
        });
        scrollSizer->Add(_fractalOptionsPanel, 1, wxEXPAND | wxALL, 5);
        scroll->SetSizer(scrollSizer);
        dialogSizer->Add(scroll, 1, wxEXPAND | wxALL, 1);
        _fractalOptionsDialog->SetSizer(dialogSizer);
    }

    _fractalOptionsPanel->SetTarget(_target);

    if (_fractalOptionsDialog->IsVisible())
        _fractalOptionsDialog->SetFocus();
    else
        _fractalOptionsDialog->Show(true);

    // Adjust position.
    int h, w;
    GetDesktopResolution(h, w);
    if (this->GetPosition().x + this->GetSize().GetWidth() + 5 < w && this->GetPosition().y < h)
        _fractalOptionsDialog->Move(this->GetPosition().x + this->GetSize().GetWidth() + 5, this->GetPosition().y);
}

void DimensionFrame::OnFormula(wxCommandEvent&)
{
    auto* dialog = new FormulaDialog(
        _userFormula,
        [this](const FormulaOptions& formula)
        {
            _userFormula = formula;
            _userFormula.type = FormulaType::Complex;
            if (_target != nullptr && _target->GetType() == FractalType::UserDefinedEscapeTime)
                _target->SetFormula(_userFormula);

            _hasPreviewMap = false;
            SchedulePreviewRender();
        },
        this);
    dialog->Show(true);
}

void DimensionFrame::OnSavePreview(wxCommandEvent&)
{
    auto openFileDialog = new wxFileDialog(this, _("Select file name"), "",
                               "dimension_preview.bmp", "BMP file (*.bmp)|*.bmp", wxFD_SAVE);
    if (openFileDialog->ShowModal() == wxID_OK)
    {
        wxString wxFileName = openFileDialog->GetPath();
        wxString fileName = wxFileName.c_str();

        // Render the fractal.
        _myOpt = ReadDimensionOptions();
        _target->Resize(_size, _size);
        _target->SetOptions(_myOpt);
        _target->PrepareRender({0, 0});
        _target->Render();

        auto* saveProgress = new ImageExportProgressDialog(_target, this, false);
        saveProgress->ShowModal();

        if (saveProgress->IsFinished())
        {
            // Allocate.
            bool **tempSetMap, **colorMap;
            UpdateBoxCountMap();
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
                    tempSetMap[i][j] = _boxCountMap.IsOccupied(i, j);
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
                                if (_boxCountMap.IsOccupied(w, h))
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
                wxMessageBox(_("Failed to save image to file: ") + wxString::FromUTF8(outputPath.c_str()), _("Error"), wxOK | wxICON_ERROR);

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
void DimensionFrame::AddBuiltInFractalChoice(const wxString& label, const FractalType type)
{
    _fractalChoice->Append(label);
    _builtInFractalList.push_back(type);
}

void DimensionFrame::PopulateFractalChoices()
{
    _fractalChoice->Clear();
    _builtInFractalList.clear();
    _scriptList.clear();

    AddBuiltInFractalChoice(_("Logistic map"), FractalType::LogisticMap);
    AddBuiltInFractalChoice(_("Henon map"), FractalType::HenonMap);
    AddBuiltInFractalChoice(_("Koch Snowflake"), FractalType::KochSnowflake);
    AddBuiltInFractalChoice(_("Sierpinski Triangle (Vector)"), FractalType::VectorSierpinskiTriangle);
    AddBuiltInFractalChoice(_("Sierpinski Carpet"), FractalType::SierpinskiCarpet);

    GetScriptFractals();
}

void DimensionFrame::SelectDefaultFractal()
{
    constexpr FractalType defaultFractal = FractalType::VectorSierpinskiTriangle;
    for (std::size_t i = 0; i < _builtInFractalList.size(); i++)
    {
        if (_builtInFractalList[i] == defaultFractal)
        {
            _fractalChoice->SetSelection(static_cast<int>(i));
            return;
        }
    }

    if (!_builtInFractalList.empty())
        _fractalChoice->SetSelection(0);
}

const DimensionCalculatorPreset* DimensionFrame::FindDimensionPreset(const FractalType fractalType)
{
    // Note to self: Add or edit known-good calculator presets here.
    static const BuiltInDimensionPreset presets[] = {
        {FractalType::VectorSierpinskiTriangle, {-1.16, 1.16, -0.89, 28, "5*x", 1, 100, 5000}},
        {FractalType::LogisticMap, {2.80000000, 4.00000000, -0.04000000, 1000, "5*x", 1, 100, 5000}},
        {FractalType::HenonMap, {-1.50000000, 1.50000000, -1.41000000, 50000, "5*x", 1, 100, 5000}},
        {FractalType::KochSnowflake, {-1.30000000, 1.30000000, -1.31000000, 15, "5*x", 1, 100, 5000}},
        {FractalType::SierpinskiCarpet, {-1.05000000, 1.05000000, -1.05000000, 10, "10*x", 10, 100, 5000}}
    };

    for (const BuiltInDimensionPreset& preset : presets)
    {
        if (preset.fractalType == fractalType)
            return &preset.preset;
    }
    return nullptr;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void DimensionFrame::ApplySelectedFractalPreset()
{
    if (_target == nullptr)
        return;

    if (_scriptSelected)
    {
        const int scriptChoice = _fractalChoice->GetCurrentSelection()
                               - static_cast<int>(_builtInFractalList.size());
        if (scriptChoice < 0 || scriptChoice >= static_cast<int>(_scriptList.size()))
            return;

        const ScriptData& script = _loadedScripts[_scriptList[scriptChoice]];
        if (script.dimensionCalculatorPreset.has_value())
            ApplyDimensionPreset(*script.dimensionCalculatorPreset);
        return;
    }

    const DimensionCalculatorPreset* preset = FindDimensionPreset(_target->GetType());
    if (preset != nullptr)
        ApplyDimensionPreset(*preset);
}

void DimensionFrame::ApplyDimensionPreset(const DimensionCalculatorPreset& preset)
{
    _minXCtrl->SetValue(preset.minX);
    _maxXCtrl->SetValue(preset.maxX);
    _minYCtrl->SetValue(preset.minY);
    _iterCtrl->SetValue(static_cast<int>(preset.iterations));
    _funcCtrl->SetValue(wxString::FromUTF8(preset.divisionFunction));
    _xMinSpin->SetValue(preset.functionXMin);
    _xMaxSpin->SetValue(preset.functionXMax);
    _sizeCtrl->SetValue(preset.imageSize);
    _divNotebook->SetSelection(0);
    UpdateDerivedMaxY();
}

bool DimensionFrame::IsUserDefinedEscapeTimeSelected() const
{
    const int choice = _fractalChoice->GetCurrentSelection();
    return choice >= 0 &&
           choice < static_cast<int>(_builtInFractalList.size()) &&
           _builtInFractalList[choice] == FractalType::UserDefinedEscapeTime;
}

void DimensionFrame::UpdateFormulaButtonVisibility() const
{
    _formulaButton->Show(IsUserDefinedEscapeTimeSelected());
    _mainPanel->GetSizer()->Layout();
}

void DimensionFrame::GetScriptFractals()
{
    _loadedScripts = GetValidUserScripts();

    // Gets script parameters.
    for (unsigned int i = 0; i < _loadedScripts.size(); i++)
    {
        if (!_loadedScripts[i].disableSetMap && _loadedScripts[i].dimensionCalculatorEnabled
            && _loadedScripts[i].dimensionCalculatorPreset.has_value())
        {
            _scriptList.push_back(i);
            _fractalChoice->Append(wxString(_loadedScripts[i].name.c_str(), wxConvUTF8));
        }
    }
}
void DimensionFrame::OnHelp(wxCommandEvent&)
{
    const auto diag = new DocumentViewer(AppPaths::ResourceFile({"Documents", "fractal_dimension.html"}),
                                         this, wxID_ANY, _("Calculate dimension help"),
                                         wxDefaultPosition, wxSize(1500, 960));
    diag->Show(true);
}
