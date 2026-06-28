// ReSharper disable CppEnumeratorNeverUsed
#include "MainFrame.h"
#include "ImageExportSizeDialog.h"
#include "AngelscriptBindings.h"
#include "AppPaths.h"
#include "HTMLViewer.h"
#include "TextUtils.h"
#include "AppTheme.h"

using namespace std;

constexpr unsigned int SCRIPT_ID_INDEX = 8510;

/**
* @brief Gets the desktop resolution. Used to adjust menu position.
*/
void GetDesktopResolution(int& width, int& height)
{
    RECT desktop;
    // ReSharper disable once CppLocalVariableMayBeConst
    HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    width = desktop.right;
    height = desktop.bottom;
}

// Fractal Frame
MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "wxChaos", wxDefaultPosition, wxSize(1180, 820))
{
    _fractalType = FractalType::Mandelbrot; // This will clash with config.ini options. I need to find a better way to handle this.

    // Init handlers.
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxICOHandler);

    // WX.
    this->SetSizeHints(wxSize(900, 650), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);
    this->GetParserOpt();    // Gets configuration from config.ini.
    AppTheme::SetAppearance(_appConfig.appearance);
    this->SetUpGUI();

    _juliaPreviewFrame = nullptr;
    _dimensionCalculator = nullptr;
    _changeKeyboardGuide = false;
    _introConstActive = false;
    _iterationsDialogIsActive = false;
    _informationFrameIsActive = false;
    _formulaDialogIsActive = false;
    _selectedScriptIndex = std::nullopt;

    this->UpdateMenu();

    // Set parameters found in the config.ini file.
    if (_appConfig.juliaMode) this->UpdateJuliaMode();
    if (_appConfig.colorPaletteWindow)
    {
        _rendererOptions = new RendererOptionsFrame(_fractalCanvas->GetFractalPresenter(), this,
            [this](const Options& options) { UpdateJuliaRendererOptions(options); });
        _rendererOptions->Show(true);
    }
    if (_appConfig.constantWindow)
    {
        _juliaConstantDialog = new JuliaConstantDialog(&_introConstActive, _fractalCanvas->GetFractalPresenter(), this);
        _juliaConstantDialog->Show(true);
        _introConstActive = true;
    }
    if (!_appConfig.colorSet)
        _fractalCanvas->GetFractalPresenter()->SetFractalSetColorMode(false);

    if (_appConfig.firstUse)
    {
        this->ShowFirstUseDialog();
        _appConfig.firstUse = false;
    }

    if (_fractalType != FractalType::Mandelbrot && _fractalType != FractalType::Manowar)
        _juliaMode->Enable(false);

    this->GetScriptFractals();
    this->ConnectEvents();
    if (_appConfig.commandConsole)
        this->ShowCommandConsole();
}
void MainFrame::ShowFirstUseDialog()
{
    const auto firstUseDialog = new HTMLViewer(
        AppPaths::ResourceFile({"Tutorials", "mainTut.html"}),
        this,
        wxID_ANY,
        wxString("Welcome to wxChaos"),
        wxDefaultPosition,
        wxSize(960, 700)
        );

    firstUseDialog->Show(true);
    _fractalCanvas->ShowHelpImage();
}
void MainFrame::ConnectEvents()
{
    this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnQuit, this, wxID_EXIT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSettings, this, ID_SETTINGS);
    this->Bind(wxEVT_SETTINGS_FRAME_CLOSED, &MainFrame::OnSettingsFrameClosed, this);
    this->Bind(wxEVT_SIZE, &MainFrame::OnResize, this);
    this->Bind(wxEVT_FRACTAL_CANVAS_STATUS_TEXT, &MainFrame::OnCanvasStatusText, this);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnJuliaMode, this, ID_JULIA_MODE);
    this->Bind(wxEVT_JULIA_MODE_CLOSED, &MainFrame::OnJuliaModeClosed, this);
    this->Bind(wxEVT_RENDERER_OPTIONS_CLOSED, &MainFrame::OnRendererOptionsClosed, this);
    this->Bind(wxEVT_SCRIPT_EDITOR_CLOSED, &MainFrame::OnScriptEditorClosed, this);
    this->Bind(wxEVT_DIMENSION_FRAME_CLOSED, &MainFrame::OnDimensionFrameClosed, this);
    this->Bind(wxEVT_COMMAND_CONSOLE_CLOSED, &MainFrame::OnCommandConsoleClosed, this);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnWelcomeDialog, this, ID_WELCOME_DIALOG);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnAbout, this, ID_ABOUT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnKeyboardGuide, this, ID_KEYBOARD_GUIDE);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSave, this, ID_SAVE);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnPalette, this, ID_PALETTE);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeMandelbrot, this, ID_MANDELBROT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeMandelbrotZN, this, ID_MANDELBROT_ZN);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeJulia, this, ID_JULIA);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeJuliaZN, this, ID_JULIA_ZN);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeNewton, this, ID_NEWTON);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeSinusoidal, this, ID_SINUSOIDAL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeMagnet, this, ID_MAGNET);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeJellyfish, this, ID_JELLYFISH);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeManowar, this, ID_MANOWAR);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeManowarJulia, this, ID_MANOWAR_JULIA);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeFixedPoint1, this, ID_FIXEDPOINT1);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeFixedPoint2, this, ID_FIXEDPOINT2);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeFixedPoint3, this, ID_FIXEDPOINT3);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeFixedPoint4, this, ID_FIXEDPOINT4);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeTricorn, this, ID_TRICORN);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeBurningShip, this, ID_BURNING_SHIP);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeBurningShipJulia, this, ID_BURNING_SHIP_JULIA);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeFractory, this, ID_FRACTORY);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeCell, this, ID_CELL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeSierpinskiTriangle, this, ID_SIERPINSKI_TRIANGLE);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeDPendulum, this, ID_DOUBLE_PENDULUM);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeUserDefined, this, ID_USER_DEFINED);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeFPUserDefined, this, ID_FIXED_POINT_USER_DEFINED);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeNewtonUserDefined, this, ID_NEWTON_USER_DEFINED);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnAbortRender, this, ID_ABORT_RENDER);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateAbortRender, this, ID_ABORT_RENDER);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnReset, this, ID_RESET);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnRedraw, this, ID_REDRAW);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnMoreIt, this, ID_INCREASE_IT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnLessIt, this, ID_DECREASE_IT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateManualIterations, this, ID_IT_MANUAL);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateManualIterations, this, ID_INCREASE_IT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateManualIterations, this, ID_DECREASE_IT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnShowOrbit, this, ID_SHOW_ORBIT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateShowOrbit, this, ID_SHOW_ORBIT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnManIntroConst, this, ID_ENTER_MAN_CONSTANT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSldIntroConst, this, ID_ENTER_SLD_CONSTANT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateSliderMode, this, ID_ENTER_SLD_CONSTANT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSetIterations, this, ID_IT_MANUAL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnAutomaticIterations, this, ID_AUTOMATIC_ITERATIONS);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateAutomaticIterations, this, ID_AUTOMATIC_ITERATIONS);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnFormulaDialog, this, ID_FORMULA_DIALOG);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnFractalOptions, this, ID_OPTION_PANEL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnUserManual, this, ID_USER_MANUAL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnScriptEditor, this, ID_SCRIPT_EDITOR);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnZoomRecorder, this, ID_ZOOM_RECORDER);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnDimensionCalculator, this, ID_DIMENSION_CALCULATOR);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCommandConsole, this, ID_COMMAND_CONSOLE);
}

void MainFrame::ResetColorRotationTool() const
{
    if (_interactionToolbar != nullptr)
        _interactionToolbar->ResetColorRotationTool();
}

void MainFrame::CreateInteractionToolbar()
{
    _interactionToolbar = new FractalToolbar(this);
    _interactionToolbar->SetToolChangedHandler([this](const FractalInteractionTool tool)
    {
        _fractalCanvas->SetInteractionTool(tool);
    });
    _interactionToolbar->SetColorRotationHandler([this]
    {
        if (_fractalCanvas == nullptr || _fractalCanvas->GetFractal()->IsRendering())
            return false;

        _fractalCanvas->GetFractalPresenter()->ToggleColorRotation();
        return true;
    });
}

void MainFrame::CreateStatusBarControls()
{
    _statusBar = this->CreateStatusBar(2, wxST_SIZEGRIP, wxID_ANY);
    const int widths[] = {180, -1};
    _statusBar->SetStatusWidths(2, widths);
    _statusBar->SetStatusText(wxEmptyString, 0);

    _renderStatusWidget = new RenderStatusWidget(
        _statusBar,
        _fractalCanvas->GetFractalPresenter(),
        [this] { OpenIterationsDialog(); });
    _statusBar->Bind(wxEVT_SIZE, [this](wxSizeEvent& event)
    {
        LayoutStatusBarControls();
        event.Skip();
    });
    LayoutStatusBarControls();
}

void MainFrame::LayoutStatusBarControls() const
{
    if (_statusBar == nullptr || _renderStatusWidget == nullptr)
        return;

    wxRect rect;
    if (!_statusBar->GetFieldRect(0, rect))
        return;

    constexpr int horizontalMargin = 4;
    constexpr int verticalMargin = 3;
    _renderStatusWidget->SetSize(
        rect.x + horizontalMargin,
        rect.y + verticalMargin,
        rect.width - horizontalMargin * 2,
        rect.height - verticalMargin * 2);
}

void MainFrame::OpenIterationsDialog()
{
    if (!_iterationsDialogIsActive)
    {
        _iterationsDialog = new IterationsDialog(&_iterationsDialogIsActive, _fractalCanvas->GetFractalPresenter(), this);
        _iterationsDialog->Show(true);
        _iterationsDialogIsActive = true;
    }
    else
    {
        _iterationsDialog->Raise();
        _iterationsDialog->SetFocus();
    }
}

void MainFrame::SetUpGUI()
{
    // Init menu.
    _menubar = new wxMenuBar();
    _fileMenu = new wxMenu();
    _fractalMenu = new wxMenu();
    _iterationsMenu = new wxMenu();
    _toolMenu = new wxMenu();
    _rendererMenu = new wxMenu();
    _helpMenu = new wxMenu();
    _formula = new wxMenu();
    _rendererOptions = nullptr;

    // Formulas.
    wxMenuItem* mandelbrot, *mandelbrotZN, *julia, *juliaZN, *newton, *sinusoidal, *magnet;
    wxMenuItem* jellyfish, *manowar, *manowarJulia, *sierpinskiTriangle, *fixedPoint1, *fixedPoint2;
    wxMenuItem* fixedPoint3, *fixedPoint4, *userDefined, *fpUserDefined, *newtonUserDefined;
    wxMenuItem* tricorn, *burningShip, *burningShipJulia, *fractory, *cell, *dPendulum;

#ifdef _WIN32
#define menuSeparator '\t'
#elif __linux__
#define menuSeparator "    "
#endif

    mandelbrot = new wxMenuItem(_formula, ID_MANDELBROT, wxString("Mandelbrot") + menuSeparator + "z = z^2 + c", wxEmptyString, wxITEM_NORMAL);
    mandelbrotZN = new wxMenuItem(_formula, ID_MANDELBROT_ZN, wxString("Mandelbrot") + menuSeparator + "z = z^n + c", wxEmptyString, wxITEM_NORMAL);
    julia = new wxMenuItem(_formula, ID_JULIA, wxString("Mandelbrot (Julia)") + menuSeparator + "z = z^2 + k", wxEmptyString, wxITEM_NORMAL);
    juliaZN = new wxMenuItem(_formula, ID_JULIA_ZN, wxString("Mandelbrot (Julia)") + menuSeparator + "z = z^n + k", wxEmptyString, wxITEM_NORMAL);
    newton = new wxMenuItem(_formula, ID_NEWTON, wxString("Newton") + menuSeparator + "z^3 - 1 = 0", wxEmptyString, wxITEM_NORMAL);
    sinusoidal = new wxMenuItem(_formula, ID_SINUSOIDAL, wxString("Sine (Julia)") + menuSeparator + "Z = c*Sin(Z)", wxEmptyString, wxITEM_NORMAL);
    magnet = new wxMenuItem(_formula, ID_MAGNET, wxString("Magnet"), wxEmptyString, wxITEM_NORMAL);
    jellyfish = new wxMenuItem(_formula, ID_JELLYFISH, wxString("Jellyfish"), wxEmptyString, wxITEM_NORMAL);
    manowar = new wxMenuItem(_formula, ID_MANOWAR, wxString("Manowar"), wxEmptyString, wxITEM_NORMAL);
    manowarJulia = new wxMenuItem(_formula, ID_MANOWAR_JULIA, wxString("Manowar (Julia)"), wxEmptyString, wxITEM_NORMAL);
    sierpinskiTriangle = new wxMenuItem(_formula, ID_SIERPINSKI_TRIANGLE, wxString("Sierpinski Triangle"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint1 = new wxMenuItem(_formula, ID_FIXEDPOINT1, wxString("Fixed Point") + menuSeparator + "z = sin(z)", wxEmptyString, wxITEM_NORMAL);
    fixedPoint2 = new wxMenuItem(_formula, ID_FIXEDPOINT2, wxString("Fixed Point") + menuSeparator + "z = cos(z)", wxEmptyString, wxITEM_NORMAL);
    fixedPoint3 = new wxMenuItem(_formula, ID_FIXEDPOINT3, wxString("Fixed Point") + menuSeparator + "z = tan(z)", wxEmptyString, wxITEM_NORMAL);
    fixedPoint4 = new wxMenuItem(_formula, ID_FIXEDPOINT4, wxString("Fixed Point") + menuSeparator + "z = z^2", wxEmptyString, wxITEM_NORMAL);
    tricorn = new wxMenuItem(_formula, ID_TRICORN, wxString("Tricorn"), wxEmptyString, wxITEM_NORMAL);
    burningShip = new wxMenuItem(_formula, ID_BURNING_SHIP, wxString("Burning Ship"), wxEmptyString, wxITEM_NORMAL);
    burningShipJulia = new wxMenuItem(_formula, ID_BURNING_SHIP_JULIA, wxString("Burning Ship (Julia)"), wxEmptyString, wxITEM_NORMAL);
    fractory = new wxMenuItem(_formula, ID_FRACTORY, wxString("Fractory"), wxEmptyString, wxITEM_NORMAL);
    cell = new wxMenuItem(_formula, ID_CELL, wxString("Cell"), wxEmptyString, wxITEM_NORMAL);
    dPendulum = new wxMenuItem(_formula, ID_DOUBLE_PENDULUM, wxString("Double pendulum"), wxEmptyString, wxITEM_NORMAL);
    userDefined = new wxMenuItem(_formula, ID_USER_DEFINED, wxString("User Formula (Complex)"), wxEmptyString, wxITEM_NORMAL);
    fpUserDefined = new wxMenuItem(_formula, ID_FIXED_POINT_USER_DEFINED, wxString("User Formula (Fixed Point)"), wxEmptyString, wxITEM_NORMAL);
    newtonUserDefined = new wxMenuItem(_formula, ID_NEWTON_USER_DEFINED, wxString("User Formula (Newton-Raphson)"), wxEmptyString, wxITEM_NORMAL);

    _typeComplex = new wxMenu();
    _typeNumericalMethod = new wxMenu();
    _typePhysics = new wxMenu();
    _typeOther = new wxMenu();

    _typeComplex->Append(mandelbrot);
    _typeComplex->Append(mandelbrotZN);
    _typeComplex->Append(julia);
    _typeComplex->Append(juliaZN);
    _typeComplex->Append(sinusoidal);
    _typeComplex->Append(jellyfish);
    _typeComplex->Append(manowar);
    _typeComplex->Append(manowarJulia);
    _typeComplex->Append(tricorn);
    _typeComplex->Append(burningShip);
    _typeComplex->Append(burningShipJulia);
    _typeComplex->Append(fractory);
    _typeComplex->Append(cell);
    _typeNumericalMethod->Append(newton);
    _typeNumericalMethod->Append(fixedPoint1);
    _typeNumericalMethod->Append(fixedPoint2);
    _typeNumericalMethod->Append(fixedPoint3);
    _typeNumericalMethod->Append(fixedPoint4);
    _typePhysics->Append(magnet);
    _typePhysics->Append(dPendulum);
    _typeOther->Append(sierpinskiTriangle);

    _formula->Append(-1, "Complex", _typeComplex);
    _formula->Append(-1, "Numerical method", _typeNumericalMethod);
    _formula->Append(-1, "Physic", _typePhysics);
    _formula->Append(-1, "Other", _typeOther);
    _formula->Append(userDefined);
    _formula->Append(fpUserDefined);
    _formula->Append(newtonUserDefined);
    _fractalMenu->Append(wxID_ANY, "Formula", _formula);

    // Julia constant.
    _introConstant = new wxMenu();
    _manualJuliaConstant = new wxMenuItem(_introConstant, ID_ENTER_MAN_CONSTANT, wxString("Manual"), wxEmptyString, wxITEM_NORMAL);
    _introConstant->Append(_manualJuliaConstant);
    _manualJuliaConstant->Enable(false);

    _sliderJuliaConstant = new wxMenuItem(_introConstant, ID_ENTER_SLD_CONSTANT, wxString("Slider") + '\t' + "F1", wxEmptyString, wxITEM_CHECK);
    _introConstant->Append(_sliderJuliaConstant);
    _sliderJuliaConstant->Enable(false);
    _sliderJuliaConstant->Check(false);
    _fractalMenu->Append(-1, "Enter Julia constant", _introConstant);

    // Julia constant and show orbit.
    _juliaMode = new wxMenuItem(_fractalMenu, ID_JULIA_MODE, wxString("Julia mode"), wxEmptyString, wxITEM_CHECK);
    _showOrbit = new wxMenuItem(_fractalMenu, ID_SHOW_ORBIT, wxString("Show orbit") + '\t' + "F2", wxEmptyString, wxITEM_CHECK);

    _fractalMenu->Append(_juliaMode);

    _fractalMenu->Append(_showOrbit);
    _juliaMode->Check(false);
    _showOrbit->Check(false);

    // File menu.
    _fileMenu->Append(ID_SAVE, wxString("Save image") + '\t' + "F4");
    _fileMenu->AppendSeparator();
    _fileMenu->Append(ID_SETTINGS, "Settings...");
    _fileMenu->AppendSeparator();
    _fileMenu->Append(wxID_EXIT, "Quit");

    // Tools menu.
    _toolMenu->Append(ID_SCRIPT_EDITOR, "Script editor", "Create new fractals with an scripting language.");
    _toolMenu->Append(ID_COMMAND_CONSOLE, "Command console", "Control and inspect the current fractal with commands.");
    _toolMenu->Append(ID_ZOOM_RECORDER, "Zoom recorder", "Record a video zoom.");
    _toolMenu->Append(ID_DIMENSION_CALCULATOR, "Dimension calculator", "Calculate fractal dimension.");

    // Iterations.
    _setIterations = new wxMenuItem(_iterationsMenu, ID_IT_MANUAL, wxString("Set iterations"), wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_setIterations);
    _moreIterations = new wxMenuItem(_iterationsMenu, ID_INCREASE_IT, wxString("Increase iterations") + '\t' + "L", wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_moreIterations);

    _lessIterations = new wxMenuItem(_iterationsMenu, ID_DECREASE_IT, wxString("Decrease iterations") + '\t' + "K", wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_lessIterations);
    _iterationsMenu->AppendSeparator();
    _automaticIterations = new wxMenuItem(_iterationsMenu, ID_AUTOMATIC_ITERATIONS, wxString("Automatic iterations"), wxEmptyString, wxITEM_CHECK);
    _iterationsMenu->Append(_automaticIterations);

    // Fractal menu.
    _fractalOptionsItem = new wxMenuItem(_fractalMenu, ID_OPTION_PANEL, wxString("Fractal options"), wxEmptyString, wxITEM_CHECK);    // Txt: "Fractal options"
    _fractalMenu->Append(_fractalOptionsItem);
    _fractalMenu->Append(ID_FORMULA_DIALOG, "Enter user formula"); // Txt: "Enter user formula"
    _fractalMenu->AppendSeparator();

    _abortRenderItem = _fractalMenu->Append(ID_ABORT_RENDER, wxString("Abort") + '\t' + "P");
    _abortRenderItem->Enable(false);
    _fractalMenu->Append(ID_REDRAW, wxString("Redraw") + '\t' + "F5");
    _fractalMenu->Append(ID_RESET, wxString("Reset"));
    _rendererMenu->Append(ID_PALETTE, "Renderer options");

    // Help menu.
    _helpMenu->Append(ID_USER_MANUAL, "User manual");
    _keyboardGuide = new wxMenuItem(_helpMenu, ID_KEYBOARD_GUIDE, wxString("Keyboard guide"), wxEmptyString, wxITEM_CHECK);
    _helpMenu->Append(_keyboardGuide);
    _helpMenu->Append(ID_WELCOME_DIALOG, "Open welcome guide");
    _helpMenu->Append(ID_ABOUT, "About");


    _menubar->Append(_fileMenu, "File");
    _menubar->Append(_fractalMenu, "Fractal");
    _menubar->Append(_iterationsMenu, "Iterations");
    _menubar->Append(_rendererMenu, "Renderer");
    _menubar->Append(_toolMenu, "Tools");
    _menubar->Append(_helpMenu, "Help");
    this->SetMenuBar(_menubar);

    _sizer = new wxBoxSizer(wxHORIZONTAL);
    CreateInteractionToolbar();
    _sizer->Add(_interactionToolbar, 0, wxEXPAND | wxALL, 0);
    _fractalSizer = new wxBoxSizer(wxVERTICAL);
    _sizer->Add(_fractalSizer, 7, wxEXPAND, 5);

    const auto panelSizer = new wxBoxSizer(wxVERTICAL);

    // Option panel.
    _optionPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    _optionPanel->SetScrollRate(5, 5);
    _optionPanel->Hide();
    _showOptionsPanel = false;
    _optionSizer = new wxBoxSizer(wxVERTICAL);

    _fractalOptionsBitmap = new wxStaticBitmap(_optionPanel, wxID_ANY, wxBitmap(AppPaths::ResourceFile({"prop.png"}), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
    _optionSizer->Add(_fractalOptionsBitmap, 0, wxALL, 0);

    _optionPanel->SetSizer(_optionSizer);
    _optionPanel->Layout();
    _optionSizer->Fit(_optionPanel);
    panelSizer->Add(_optionPanel, 1, wxEXPAND | wxALL, 1);

    _sizer->Add(panelSizer, 2, wxEXPAND, 5);
    this->SetSizer(_sizer);
    this->Layout();
    this->Centre(wxVERTICAL);

    _size = _fractalSizer->GetSize();

    // Creates fractalCanvas.
    _fractalType = _appConfig.type;
    _fractalCanvas = new FractalCanvas(_fractalType, this, wxID_ANY, wxPoint(0, 0), _size, wxBORDER_NONE);

    wxGradient grad;
    grad.SetMin(0);
    grad.SetMax(_appConfig.paletteSize);
    grad.FromString(wxString(_appConfig.colorStyleGrad.c_str(), wxConvUTF8));
    _fractalCanvas->GetFractalPresenter()->SetColorPalette(_appConfig.colorStyle);
    _fractalCanvas->GetFractalPresenter()->SetGradient(grad);
    _fractalCanvas->GetFractalPresenter()->SetColorCycleLength(_appConfig.colorCycleLength);

    _fractalCanvas->GetFractalPresenter()->ChangeIterations(_appConfig.maxIterations);
    SetAutomaticIterations(_appConfig.automaticIterations);
    _fractalCanvas->GetFractalPresenter()->SetExteriorColorMode(_appConfig.colorFractal);
    _fractalCanvas->GetFractalPresenter()->SetFractalSetColorMode(_appConfig.colorSet);
    _fractalCanvas->GetFractalPresenter()->SetZoomOptions(_appConfig.zoomStepPercent, _appConfig.zoomInertiaMilliseconds);
    _fractalSizer->Add(_fractalCanvas, 1, wxEXPAND | wxALL, 0);
    CreateStatusBarControls();
}

void MainFrame::OnClose(wxCloseEvent&)
{
    this->CloseAll();
    this->Destroy();
}
void MainFrame::OnQuit(wxCommandEvent&)
{
    Close(true);
}
void MainFrame::OnSettings(wxCommandEvent&)
{
    if (_settingsFrame == nullptr)
    {
        _settingsFrame = new SettingsFrame(this, _appConfig,
            [this](const AppConfig& config) { ApplyAppConfig(config); });
        _settingsFrame->Show();
    }
    else
    {
        _settingsFrame->Raise();
        _settingsFrame->SetFocus();
    }
}
void MainFrame::OnSettingsFrameClosed(wxCommandEvent&)
{
    _settingsFrame = nullptr;
}
void MainFrame::ApplyAppConfig(const AppConfig& config)
{
    const bool appearanceChanged = config.appearance != _appConfig.appearance;
    _appConfig = config;

    if (appearanceChanged)
    {
        wxMessageBox(
            "You need to restart wxChaos for the appearance change to take effect.",
            "Restart required", wxOK | wxICON_INFORMATION, this);
    }

    wxGradient gradient;
    gradient.SetMin(0);
    gradient.SetMax(config.paletteSize);
    gradient.FromString(wxString::FromUTF8(config.colorStyleGrad.c_str()));
    _fractalCanvas->GetFractalPresenter()->SetColorPalette(config.colorStyle);
    _fractalCanvas->GetFractalPresenter()->SetGradient(gradient);
    _fractalCanvas->GetFractalPresenter()->SetColorCycleLength(config.colorCycleLength);
    _fractalCanvas->GetFractalPresenter()->ChangeIterations(config.maxIterations);
    SetAutomaticIterations(config.automaticIterations);
    _fractalCanvas->GetFractalPresenter()->SetExteriorColorMode(config.colorFractal);
    _fractalCanvas->GetFractalPresenter()->SetFractalSetColorMode(config.colorSet);
    _fractalCanvas->GetFractalPresenter()->SetZoomOptions(config.zoomStepPercent, config.zoomInertiaMilliseconds);
    if (_juliaPreviewFrame != nullptr)
        _juliaPreviewFrame->SetZoomOptions(config.zoomStepPercent, config.zoomInertiaMilliseconds);
}
void MainFrame::SetAutomaticIterations(const bool mode) const
{
    _fractalCanvas->GetFractalPresenter()->SetAutomaticIterations(mode);
    if (_automaticIterations != nullptr)
        _automaticIterations->Check(mode);
}
void MainFrame::CloseAll()
{
    if (_settingsFrame != nullptr)
    {
        _settingsFrame->Destroy();
        _settingsFrame = nullptr;
    }
    if (_commandConsole != nullptr)
    {
        _commandConsole->Destroy();
        _commandConsole = nullptr;
    }
    DestroyJuliaMode(true);
    DestroyDimensionFrame();
    delete _fractalCanvas;
}
void MainFrame::DestroyJuliaMode(const bool requestClose)
{
    if (_juliaPreviewFrame == nullptr)
        return;

    JuliaPreviewFrame* frame = _juliaPreviewFrame;
    _juliaPreviewFrame = nullptr;

    if (requestClose)
        frame->Close();

    _juliaMode->Check(false);
    _fractalCanvas->SetJuliaMode(false);
}
void MainFrame::OnResize(wxSizeEvent&)
{
    this->Layout();
    LayoutStatusBarControls();
}
void MainFrame::OnJuliaMode(wxCommandEvent&)
{
    // Opens the Julia mode.
    this->UpdateJuliaMode();
}
void MainFrame::OnJuliaModeClosed(wxCommandEvent&)
{
    DestroyJuliaMode(false);
}
void MainFrame::OnRendererOptionsClosed(wxCommandEvent&)
{
    _rendererOptions = nullptr;
}
void MainFrame::OnScriptEditorClosed(wxCommandEvent&)
{
    _scriptEditor = nullptr;
    ReloadScripts();
}
void MainFrame::DestroyDimensionFrame()
{
    if (_dimensionCalculator == nullptr)
        return;

    DimensionFrame* frame = _dimensionCalculator;
    _dimensionCalculator = nullptr;
    frame->Close(true);
}
void MainFrame::OnDimensionFrameClosed(wxCommandEvent&)
{
    _dimensionCalculator = nullptr;
}

void MainFrame::OnCommandConsoleClosed(wxCommandEvent&)
{
    _commandConsole = nullptr;
    _appConfig.commandConsole = false;
    AppConfigStore(AppPaths::ToStdPath(AppPaths::ConfigFile())).SetCommandConsole(false);
}

void MainFrame::ShowCommandConsole()
{
    if (_commandConsole == nullptr)
    {
        _commandConsole = new CommandConsole(
            _fractalCanvas,
            [this] { ReloadScripts(); },
            [this](const double real, const double imaginary) { return OpenJuliaModeAt(real, imaginary); },
            this);
        _commandConsole->Show(true);
        _appConfig.commandConsole = true;
        AppConfigStore(AppPaths::ToStdPath(AppPaths::ConfigFile())).SetCommandConsole(true);
    }
    else
    {
        _commandConsole->Raise();
        _commandConsole->SetFocus();
    }
}
void MainFrame::OnWelcomeDialog(wxCommandEvent&)
{
    this->ShowFirstUseDialog();
}
void MainFrame::OnAbout(wxCommandEvent&)
{
    AboutDialog dialog(this);
    dialog.ShowModal();
}
void MainFrame::OnSave(wxCommandEvent&)
{
    // Saves the fractal image.
    const auto saveFileDialog = new wxFileDialog(
        this,
        "Select file name",
        "",
        "fractal.png",
        "PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp",
        wxFD_SAVE
        );

    if (saveFileDialog->ShowModal() == wxID_OK)
    {
        const wxString fileName = saveFileDialog->GetPath();
        const int ext = saveFileDialog->GetFilterIndex();
        const auto path = string(fileName.mb_str());
        ImageExportSizeDialog* sizeDialogSave;

        if (_fractalType == FractalType::ScriptFractal && _selectedScriptIndex.has_value())
            sizeDialogSave = new ImageExportSizeDialog(_fractalCanvas, path, ext, _fractalType, _fractalCanvas->GetFractal(), this, _loadedScripts[*_selectedScriptIndex].file);
        else
            sizeDialogSave = new ImageExportSizeDialog(_fractalCanvas, path, ext, _fractalType, _fractalCanvas->GetFractal(), this);

        sizeDialogSave->Show(true);
    }
    saveFileDialog->Destroy();
}
void MainFrame::OnPalette(wxCommandEvent&)
{
    // Color palette frame.
    if (_rendererOptions == nullptr)
    {
        _rendererOptions = new RendererOptionsFrame(_fractalCanvas->GetFractalPresenter(), this,
            [this](const Options& options) { UpdateJuliaRendererOptions(options); });
        _rendererOptions->Show(true);

        // Adjust position.
        int h, w;
        GetDesktopResolution(h, w);
        if (this->GetPosition().x+this->GetSize().GetWidth()+5 < w && this->GetPosition().y < h)
            _rendererOptions->Move(this->GetPosition().x+this->GetSize().GetWidth()+5, this->GetPosition().y);
    }
    else
        _rendererOptions->SetFocus();
}
void MainFrame::OnFormulaDialog(wxCommandEvent&)
{
    // User formula menu.
    if (!_formulaDialogIsActive)
    {
        _formulaDialogIsActive = true;
        _formulaDialog = new FormulaDialog(ID_USER_DEFINED, ID_FIXED_POINT_USER_DEFINED, ID_NEWTON_USER_DEFINED, _sliderJuliaConstant, _manualJuliaConstant, &_formulaDialogIsActive, _fractalCanvas, this);
        _formulaDialog->Show(true);

        // Adjust position.
        int h, w;
        GetDesktopResolution(h, w);
        if (this->GetPosition().x+this->GetSize().GetWidth()+5 < w && this->GetPosition().y < h)
            _formulaDialog->Move(this->GetPosition().x+this->GetSize().GetWidth()+5, this->GetPosition().y);

        _fractalType = FractalType::UserDefinedEscapeTime;
    }
    else
        _formulaDialog->SetFocus();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnRedraw(wxCommandEvent&)
{
    _fractalCanvas->GetFractalPresenter()->Redraw();
}
void MainFrame::OnReset(wxCommandEvent&)
{
    _fractalCanvas->Reset();
    ResetColorRotationTool();
    wxGradient grad;
    grad.FromString(wxString(_appConfig.colorStyleGrad.c_str(), wxConvUTF8));
    grad.SetMin(0);
    grad.SetMax(_appConfig.paletteSize);
    _fractalCanvas->GetFractalPresenter()->SetColorPalette(_appConfig.colorStyle);
    _fractalCanvas->GetFractalPresenter()->SetGradient(grad);
    _fractalCanvas->GetFractalPresenter()->SetColorCycleLength(_appConfig.colorCycleLength);
    this->UpdateMenu();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnMoreIt(wxCommandEvent&)
{
    _fractalCanvas->GetFractalPresenter()->IncreaseIterations();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnLessIt(wxCommandEvent&)
{
    _fractalCanvas->GetFractalPresenter()->DecreaseIterations();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnShowOrbit(wxCommandEvent&)
{
    const bool modo = _showOrbit->IsChecked();
    _fractalCanvas->SetOrbitMode(modo);
    _showOrbit->Check(modo);
}
void MainFrame::OnManIntroConst(wxCommandEvent&)
{
    // Manual constant.
    if (!_introConstActive)
    {
        _juliaConstantDialog = new JuliaConstantDialog(&_introConstActive, _fractalCanvas->GetFractalPresenter(), this);
        _juliaConstantDialog->Show(true);
        _introConstActive = true;
    }
    else
    {
        _juliaConstantDialog->Show(false);
        _introConstActive = false;
        delete _juliaConstantDialog;
    }
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnSldIntroConst(wxCommandEvent&)
{
    // Slider constant.
    const bool modo = _sliderJuliaConstant->IsChecked();
    _fractalCanvas->SetSliderMode(modo);
    _sliderJuliaConstant->Check(modo);
}
void MainFrame::OnKeyboardGuide(wxCommandEvent&)
{
    // Keyboard guide.
    _changeKeyboardGuide = !_changeKeyboardGuide;
    _fractalCanvas->SetKeyboardGuide(_changeKeyboardGuide);
    _keyboardGuide->Check(_changeKeyboardGuide);
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnCanvasStatusText(wxCommandEvent& event)
{
    _statusBar->SetStatusText(event.GetString(), 1);
}
void MainFrame::OnSetIterations(wxCommandEvent&)
{
    OpenIterationsDialog();
}
void MainFrame::OnAutomaticIterations(wxCommandEvent&)
{
    const bool mode = _automaticIterations->IsChecked();
    _appConfig.automaticIterations = mode;
    SetAutomaticIterations(mode);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnAbortRender(wxCommandEvent&)
{
    _fractalCanvas->AbortRender();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnUpdateAbortRender(wxUpdateUIEvent& event)
{
    event.Enable(_fractalCanvas != nullptr && _fractalCanvas->CanAbortRender());
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnUpdateShowOrbit(wxUpdateUIEvent& event)
{
    event.Check(_fractalCanvas != nullptr && _fractalCanvas->IsOrbitMode());
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnUpdateSliderMode(wxUpdateUIEvent& event)
{
    event.Check(_fractalCanvas != nullptr && _fractalCanvas->IsSliderMode());
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnUpdateManualIterations(wxUpdateUIEvent& event)
{
    event.Enable(_fractalCanvas != nullptr);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnUpdateAutomaticIterations(wxUpdateUIEvent& event)
{
    event.Check(_fractalCanvas != nullptr && _fractalCanvas->GetFractalPresenter()->AutomaticIterationsEnabled());
}
void MainFrame::OnFractalOptions(wxCommandEvent&)
{
    // Adjust the panel.
    if (!_showOptionsPanel)
    {
        _fractalOptionsItem->Check(true);
        _optionPanel->Show();
        const wxSize windowSize = this->GetSize();
        if (!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()+175, windowSize.GetHeight());

        this->GetSizer()->Layout();
        _showOptionsPanel = true;
    }
    else
    {
        _fractalOptionsItem->Check(false);
        _optionPanel->Hide();
        const wxSize windowSize = this->GetSize();
        if (!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()-175, windowSize.GetHeight());
        this->GetSizer()->Layout();
        _showOptionsPanel = false;
    }
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnApplyPanelOpt(wxCommandEvent&)
{
    // Pass parameters to the fractal and redraws it.
    const PanelOptions* pOptions = _fractalCanvas->GetFractal()->GetOptPanel();
    for (unsigned int i=0; i<_foundTextControls.size(); i++)
        *pOptions->GetDoubleElement(i) = TextUtils::ToDouble(_textControls[i]->GetValue());

    for (unsigned int i=0; i<_foundSpinControls.size(); i++)
        *pOptions->GetIntElement(i) = _spinControls[i]->GetValue();

    for (unsigned int i=0; i<_foundCheckBoxes.size(); i++)
    {
        if (_checkBoxes[i]->GetValue())
            *pOptions->GetBoolElement(i) = true;
        else
            *pOptions->GetBoolElement(i) = false;
    }
    _fractalCanvas->SetFocus();
    _fractalCanvas->GetFractalPresenter()->Redraw();
}
// ReSharper disable once CppMemberFunctionMayBeStatic
void MainFrame::OnUserManual(wxCommandEvent&) // NOLINT(*-convert-member-functions-to-static)
{
    wxLaunchDefaultApplication(AppPaths::DocFile("User_Manual.pdf"));
}
void MainFrame::OnScriptEditor(wxCommandEvent&)
{
    if (_scriptEditor == nullptr)
    {
        _scriptEditor = new ScriptEditor(this);
        _scriptEditor->Show(true);
    }
    else
        _scriptEditor->Close(true);
}
void MainFrame::OnZoomRecorder(wxCommandEvent&)
{
    if (const FractalPresenter* fractal = _fractalCanvas->GetFractalPresenter(); !fractal->HasZoomed())
    {
        wxMessageBox(
            "To use the zoom recorder you need to first perform a zoom on the main window.",
            "Zoom recorder",
            wxOK | wxICON_INFORMATION,
            this);
        return;
    }

    this->SetSize(wxSize(1180, 820));
    this->Layout();
    this->Update();

    ZoomRecorder zoomRecorder(_fractalCanvas, this);
    zoomRecorder.ShowModal();
}
void MainFrame::OnDimensionCalculator(wxCommandEvent&)
{
    if (_dimensionCalculator == nullptr)
    {
        _dimensionCalculator = new DimensionFrame(this);
        _dimensionCalculator->Show(true);
    }
    else
        _dimensionCalculator->SetFocus();
}

void MainFrame::OnCommandConsole(wxCommandEvent&)
{
    this->ShowCommandConsole();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
// Changes the fractal type.
void MainFrame::ChangeMandelbrot(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Mandelbrot, true);
}
void MainFrame::ChangeMandelbrotZN(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::MandelbrotZN, true);
}
void MainFrame::ChangeJulia(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Julia, false);
}
void MainFrame::ChangeJuliaZN(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::JuliaZN, false);
}
void MainFrame::ChangeNewton(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::NewtonRaphsonMethod, false);
}
void MainFrame::ChangeSinusoidal(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Sinusoidal, false);
}
void MainFrame::ChangeMagnet(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Magnetic, false);
}
void MainFrame::ChangeJellyfish(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Jellyfish, false);
}
void MainFrame::ChangeManowar(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Manowar, true);
}
void MainFrame::ChangeManowarJulia(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::ManowarJulia, false);
}
void MainFrame::ChangeSierpinskiTriangle(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::SierpinskiTriangle, false);
}
void MainFrame::ChangeFixedPoint1(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::FixedPoint1, false);
}
void MainFrame::ChangeFixedPoint2(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::FixedPoint2, false);
}
void MainFrame::ChangeFixedPoint3(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::FixedPoint3, false);
}
void MainFrame::ChangeFixedPoint4(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::FixedPoint4, false);
}
void MainFrame::ChangeTricorn(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Tricorn, false);
}
void MainFrame::ChangeBurningShip(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::BurningShip, true);
}
void MainFrame::ChangeBurningShipJulia(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::BurningShipJulia, false);
}
void MainFrame::ChangeFractory(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Fractory, false);
}
void MainFrame::ChangeCell(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Cell, false);
}
void MainFrame::ChangeDPendulum(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::DoublePendulum, false);
}
void MainFrame::ChangeUserDefined(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::UserDefinedEscapeTime, false);
}
void MainFrame::ChangeFPUserDefined(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::UserDefinedFixedPoint, false);
}
void MainFrame::ChangeNewtonUserDefined(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::UserDefinedNewtonRaphson, false);
}
void MainFrame::ChangeFractal(const FractalType type, const bool enableJulia)
{
    _selectedScriptIndex.reset();  // Deselect the script fractal.
    if (_fractalType != type || _fractalType == FractalType::UserDefinedEscapeTime || _fractalType == FractalType::UserDefinedFixedPoint
        || _fractalType == FractalType::UserDefinedNewtonRaphson)
    {
        const Options fractOpt = _fractalCanvas->GetFractal()->GetOptions();
        const ColorPaletteTypes colorPalette = _fractalCanvas->GetFractal()->GetColorPalette();
        _fractalCanvas->ChangeType(type);
        _fractalCanvas->GetFractalPresenter()->SetColorPalette(colorPalette);
        _fractalCanvas->GetFractalPresenter()->SetGradient(fractOpt.gradient);
        _fractalCanvas->GetFractalPresenter()->SetColorCycleLength(fractOpt.colorCycleLength);
        _fractalCanvas->GetFractalPresenter()->SetColorRotationSpeed(fractOpt.colorRotationSpeed);
        _fractalType = type;
        ResetColorRotationTool();
        this->UpdateMenu();
        _juliaMode->Enable(enableJulia);
    }
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void MainFrame::ChangeScriptItem(wxCommandEvent& event)
{
    const unsigned int id = event.GetId() - SCRIPT_ID_INDEX;
    _selectedScriptIndex = id;

    if (_fractalCanvas->GetFractal()->IsRendering())
        _fractalCanvas->GetFractal()->StopRender();

    const Options fractOpt = _fractalCanvas->GetFractal()->GetOptions();
    const ColorPaletteTypes colorPalette = _fractalCanvas->GetFractal()->GetColorPalette();
    _fractalCanvas->ChangeToScript(_loadedScripts[id]);
    _fractalCanvas->GetFractalPresenter()->SetColorPalette(colorPalette);
    _fractalCanvas->GetFractalPresenter()->SetGradient(fractOpt.gradient);
    _fractalCanvas->GetFractalPresenter()->SetColorCycleLength(fractOpt.colorCycleLength);
    _fractalCanvas->GetFractalPresenter()->SetColorRotationSpeed(fractOpt.colorRotationSpeed);

    _fractalType = FractalType::ScriptFractal;
    ResetColorRotationTool();
    this->UpdateMenu();
    _juliaMode->Enable(false);
}

// Methods to adjust the menu.
void MainFrame::GetParserOpt()
{
    const AppConfigStore configStore(AppPaths::ToStdPath(AppPaths::ConfigFile()));
    _appConfig = configStore.Load();
    configStore.SetFirstUse(false);
}
void MainFrame::UpdateOptionsPanel()
{
    // If there are elements in pOptions creates panel.
    if (PanelOptions* panelOptions = _fractalCanvas->GetFractal()->GetOptPanel(); panelOptions->GetElementsSize() > 0)
    {
        unsigned int labelIndex;
        unsigned int index;
        _fractalOptionsItem->Enable(true);
        if (!_labels.empty() || !_textControls.empty() || !_spinControls.empty() || !_checkBoxes.empty())
        {
            // If there are elements from a previous panel, deletes them.
            this->DeleteOptPanel();
        }

        if (panelOptions->GetForceShow())
        {
            _fractalOptionsItem->Check(true);
            _optionPanel->Show();
            const wxSize windowSize = this->GetSize();
            if (!this->IsMaximized())
                this->SetSize(windowSize.GetWidth()+175, windowSize.GetHeight());

            this->GetSizer()->Layout();
            _showOptionsPanel = true;
        }
        else
            _fractalOptionsItem->Check(false);

        // Creates elements from each kind.
        for (int i=0; i<panelOptions->GetElementsSize(); i++)
        {
            switch(panelOptions->GetPanelOptType(i))
            {
                case PanelOptionType::Label:
                    {
                        _labels.push_back(new wxStaticText(_optionPanel, wxID_ANY, wxString(panelOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                        labelIndex = _labels.size()-1;
                        _labels[labelIndex]->Wrap(-1);
                        _optionSizer->Add(_labels[labelIndex], 0, wxALL, 5);
                        _foundLabels.push_back(i);
                    }
                    break;
                case PanelOptionType::TextCtrl:
                    {
                        _labels.push_back(new wxStaticText(_optionPanel, wxID_ANY, wxString(panelOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                        labelIndex = _labels.size()-1;
                        _labels[labelIndex]->Wrap(-1);
                        _optionSizer->Add(_labels[labelIndex], 0, wxALL, 5);

                        _textControls.push_back(new wxTextCtrl(_optionPanel, wxID_ANY, wxString(panelOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                        index = _textControls.size()-1;
                        _optionSizer->Add(_textControls[index], 0, wxALL|wxEXPAND, 5);
                        _foundTextControls.push_back(i);
                    }
                    break;
                case PanelOptionType::Spin:
                    {
                        _labels.push_back(new wxStaticText(_optionPanel, wxID_ANY, wxString(panelOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                        labelIndex = _labels.size()-1;
                        _labels[labelIndex]->Wrap(-1);
                        _optionSizer->Add(_labels[labelIndex], 0, wxALL, 5);

                        _spinControls.push_back(new wxSpinCtrl(_optionPanel, wxID_ANY, wxString(panelOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0 ));
                        index = _spinControls.size()-1;
                        _optionSizer->Add(_spinControls[index], 0, wxALL|wxEXPAND, 5);
                        _foundSpinControls.push_back(i);
                    }
                    break;
                case PanelOptionType::CheckBox:
                    {
                        _checkBoxes.push_back(new wxCheckBox(_optionPanel, wxID_ANY, wxString(panelOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                        index = _checkBoxes.size()-1;
                        if (panelOptions->GetDefault(i) == "true")
                            _checkBoxes[index]->SetValue(true);
                        else
                            _checkBoxes[index]->SetValue(false);

                        _optionSizer->Add(_checkBoxes[index], 0, wxALL|wxEXPAND, 5);
                        _foundCheckBoxes.push_back(i);
                    }
                    break;
            };
        }

        // Creates button to apply options.
        _panelButton = new wxButton(_optionPanel, wxID_ANY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
        _optionSizer->Add(_panelButton, 0, wxALL, 5);
        _panelButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainFrame::OnApplyPanelOpt, this);
        _optionSizer->Layout();
        _optionPanel->SetScrollbars(20, 20, 0, 50);
    }
    else
    {
        _fractalOptionsItem->Check(false);
        _fractalOptionsItem->Enable(false);
        if (_showOptionsPanel)
            this->DeleteOptPanel();
    }
}
void MainFrame::DeleteOptPanel()
{
    // Deletes panel elements.
    for (auto & label : _labels)
        label->Destroy();

    _labels.clear();
    _foundLabels.clear();
    for (auto & textControl : _textControls)
        textControl->Destroy();

    _textControls.clear();
    _foundTextControls.clear();
    for (const auto & spinControl : _spinControls)
        spinControl->Destroy();

    _spinControls.clear();
    _foundSpinControls.clear();
    for (const auto & checkBox : _checkBoxes)
        checkBox->Destroy();

    _checkBoxes.clear();
    _foundCheckBoxes.clear();

    // Erase button, disconnect event and hide panel.
    _panelButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &MainFrame::OnApplyPanelOpt, this);
    delete _panelButton;

    if (_showOptionsPanel)
    {
        _optionPanel->Hide();
        const wxSize windowSize = this->GetSize();

        if (!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()-175, windowSize.GetHeight());

        this->GetSizer()->Layout();
        _showOptionsPanel = false;
    }
}
void MainFrame::AddScriptMenuElement(const ScriptData& scriptData, const unsigned int index)
{
    _loadedScripts.push_back(scriptData);
    int itemIndex = static_cast<int>(SCRIPT_ID_INDEX + index);
    _scriptItems.push_back(
        new wxMenuItem(
            _formula,
           itemIndex,
            wxString(scriptData.name.c_str(), wxConvUTF8),
        wxEmptyString,
        wxITEM_NORMAL
        )
        );

    if (scriptData.scriptCategory == ScriptCategory::Complex)
        _typeComplex->Append(_scriptItems[index]);
    else if (scriptData.scriptCategory == ScriptCategory::NumMet)
        _typeNumericalMethod->Append(_scriptItems[index]);
    else if (scriptData.scriptCategory == ScriptCategory::Physic)
        _typePhysics->Append(_scriptItems[index]);
    else
        _typeOther->Append(_scriptItems[index]);

    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeScriptItem, this, itemIndex);
}
void MainFrame::RemoveScriptMenuElements()
{
    // Remove current menu entries.
    for (unsigned int i = 0; i < _loadedScripts.size(); i++)
    {
        if (_loadedScripts[i].scriptCategory == ScriptCategory::Complex)
            _typeComplex->Remove(_scriptItems[i]);
        else if (_loadedScripts[i].scriptCategory == ScriptCategory::NumMet)
            _typeNumericalMethod->Remove(_scriptItems[i]);
        else if (_loadedScripts[i].scriptCategory == ScriptCategory::Physic)
            _typePhysics->Remove(_scriptItems[i]);
        else
            _typeOther->Remove(_scriptItems[i]);
    }
    _loadedScripts.clear();

    // Disconnect events and delete menu items.
    for (unsigned int i = 0; i < _scriptItems.size(); i++)
    {
        this->Unbind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeScriptItem, this, static_cast<int>(SCRIPT_ID_INDEX + i));
        delete _scriptItems[i];
    }
    _scriptItems.clear();
}
void MainFrame::GetScriptFractals()
{
    const vector<ScriptData> userScriptsData = GetValidUserScripts();
    for (unsigned i = 0; i < userScriptsData.size(); i++)
        AddScriptMenuElement(userScriptsData[i], i);
}
void MainFrame::UpdateMenu()
{
    // Adjust menu options when the fractal type is changed.
    if (_rendererOptions != nullptr)
        _rendererOptions->SetTarget(_fractalCanvas->GetFractalPresenter());
    if (_iterationsDialogIsActive)
        _iterationsDialog->SetTarget(_fractalCanvas->GetFractalPresenter());

    _showOrbit->Check(false);
    _showOrbit->Enable(_fractalCanvas->GetFractal()->HasOrbit());

    _setIterations->Enable(true);
    _moreIterations->Enable(true);
    _lessIterations->Enable(true);
    _automaticIterations->Check(_fractalCanvas->GetFractalPresenter()->AutomaticIterationsEnabled());

    // Closes constant dialog.
    if (_introConstActive)
    {
        _juliaConstantDialog->Show(false);
        _introConstActive = false;
        delete _juliaConstantDialog;
    }

    // Adjust Julia's constant menu items.
    if (_fractalCanvas->GetFractal()->IsJuliaVariety())
    {
        _manualJuliaConstant->Enable(true);
        _sliderJuliaConstant->Enable(true);
    }
    else
    {
        _manualJuliaConstant->Enable(false);
        _sliderJuliaConstant->Enable(false);
    }
    _sliderJuliaConstant->Check(false);

    // Closes formula dialog.
    if (_fractalType != FractalType::UserDefinedEscapeTime && _fractalType != FractalType::UserDefinedFixedPoint
        && _fractalType != FractalType::UserDefinedNewtonRaphson)
    {
        if (_formulaDialogIsActive)
        {
            _formulaDialog->Destroy();
            _formulaDialogIsActive = false;
        }
    }

    _abortRenderItem->SetItemLabel(wxString("Abort")+ '\t' + "P");
    _abortRenderItem->Enable(false);

    // If Julia mode is opened, closes it.
    DestroyJuliaMode(true);
    this->UpdateOptionsPanel();
}
void MainFrame::UpdateJuliaMode()
{
    // Destroy Julia window.
    if (_juliaPreviewFrame != nullptr)
    {
        _juliaMode->Check(false);
        _juliaPreviewFrame->Close();
    }
    // Creates Julia fractal with parameters from the main fractal.
    else
    {
        const Options options = _fractalCanvas->GetFractal()->GetOptions();
        if (!OpenJuliaModeAt(options.kReal, options.kImaginary))
            _juliaMode->Check(false);
    }
}

bool MainFrame::OpenJuliaModeAt(const double real, const double imaginary)
{
    FractalType juliaType;
    switch (_fractalType)
    {
        case FractalType::Mandelbrot: juliaType = FractalType::Julia; break;
        case FractalType::MandelbrotZN: juliaType = FractalType::JuliaZN; break;
        case FractalType::Manowar: juliaType = FractalType::ManowarJulia; break;
        case FractalType::BurningShip: juliaType = FractalType::BurningShipJulia; break;
        default: return false;
    }

    if (_juliaPreviewFrame != nullptr)
    {
        _juliaPreviewFrame->SetConstant(real, imaginary);
        return true;
    }

    Options options = _fractalCanvas->GetFractal()->GetOptions();
    options.kReal = real;
    options.kImaginary = imaginary;
    _juliaMode->Check(true);
    _juliaPreviewFrame = new JuliaPreviewFrame(
        this,
        _fractalCanvas,
        juliaType,
        options,
        _appConfig.zoomStepPercent,
        _appConfig.zoomInertiaMilliseconds);
    _juliaPreviewFrame->Show(true);
    _fractalCanvas->SetJuliaMode(true);
    return true;
}
void MainFrame::ReloadScripts()
{
    // Remove current menu entries.
    this->RemoveScriptMenuElements();

    // Get new scripts.
    this->GetScriptFractals();
}
void MainFrame::UpdateJuliaRendererOptions(const Options& options) const
{
    if (_juliaPreviewFrame != nullptr)
        _juliaPreviewFrame->SetRendererOptions(options);
}
