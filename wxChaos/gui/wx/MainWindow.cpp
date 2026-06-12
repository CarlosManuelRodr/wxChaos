// ReSharper disable CppEnumeratorNeverUsed
#include "MainWindow.h"
#include "AngelscriptBindings.h"
#include "AppPaths.h"
#include "HTMLViewer.h"
#include "TextUtils.h"
#include "global.h"

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
MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, wxT("wxChaos"), wxDefaultPosition, wxSize(1180, 820))
{
    _fractalType = FractalType::Mandelbrot; // This will clash with config.ini options. I need to find a better way to handle this.

    // Init handlers.
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxICOHandler);

    // WX.
    this->SetSizeHints(wxSize(900, 650), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({wxT("icon.ico")}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);
    this->GetParserOpt();    // Gets configuration from config.ini.
    this->SetUpGUI();

    _juliaModePtr = nullptr;
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
        _rendererOptions = new RendererOptions(_fractalCanvas->GetSFMLFractalPtr(), this,
            [this](const Options& options) { UpdateJuliaRendererOptions(options); });
        _rendererOptions->Show(true);
    }
    if (_appConfig.constantWindow)
    {
        _juliaConstantDialog = new JuliaConstantDialog(&_introConstActive, _fractalCanvas->GetSFMLFractalPtr(), this);
        _juliaConstantDialog->Show(true);
        _introConstActive = true;
    }
    if (!_appConfig.colorSet)
        _fractalCanvas->GetSFMLFractalPtr()->SetFractalSetColorMode(false);

    if (_appConfig.firstUse)
        this->ShowFirstUseDialog();

    if (_fractalType != FractalType::Mandelbrot && _fractalType != FractalType::Manowar)
        _juliaMode->Enable(false);

    this->GetScriptFractals();
    this->ConnectEvents();
}
void MainFrame::ShowFirstUseDialog()
{
    const auto firstUseDialog = new HTMLViewer(
        AppPaths::ResourceFile({wxT("Tutorials"), wxT("mainTut.html")}),
        this,
        wxID_ANY,
        wxString(wxT("Welcome to wxChaos")),
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
    this->Bind(wxEVT_SIZE, &MainFrame::OnResize, this);
    this->Bind(wxEVT_FRACTAL_CANVAS_STATUS_TEXT, &MainFrame::OnCanvasStatusText, this);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnJuliaMode, this, ID_JULIA_MODE);
    this->Bind(wxEVT_JULIA_MODE_CLOSED, &MainFrame::OnJuliaModeClosed, this);
    this->Bind(wxEVT_RENDERER_OPTIONS_CLOSED, &MainFrame::OnRendererOptionsClosed, this);
    this->Bind(wxEVT_SCRIPT_EDITOR_CLOSED, &MainFrame::OnScriptEditorClosed, this);
    this->Bind(wxEVT_DIMENSION_FRAME_CLOSED, &MainFrame::OnDimensionFrameClosed, this);
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
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeSinoidal, this, ID_SINOIDAL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeMagnet, this, ID_MAGNET);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeMedusa, this, ID_MEDUSA);
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
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeSierpinskyTriangle, this, ID_SIERPINSKY_TRIANGLE);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeDPendulum, this, ID_DOUBLE_PENDULUM);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeUserDefined, this, ID_USER_DEFINED);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ChangeFPUserDefined, this, ID_FIXED_POINT_USER_DEFINED);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnAbortRender, this, ID_ABORT_RENDER);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateAbortRender, this, ID_ABORT_RENDER);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnReset, this, ID_RESET);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnRedraw, this, ID_REDRAW);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnMoreIt, this, ID_INCREASE_IT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnLessIt, this, ID_DECREASE_IT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnShowOrbit, this, ID_SHOW_ORBIT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateShowOrbit, this, ID_SHOW_ORBIT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnManIntroConst, this, ID_ENTER_MAN_CONSTANT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSldIntroConst, this, ID_ENTER_SLD_CONSTANT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateSliderMode, this, ID_ENTER_SLD_CONSTANT);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnItManual, this, ID_IT_MANUAL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnFormulaDialog, this, ID_FORMULA_DIALOG);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnFractalOptions, this, ID_OPTION_PANEL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnUserManual, this, ID_USER_MANUAL);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnScriptEditor, this, ID_SCRIPT_EDITOR);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnZoomRecorder, this, ID_ZOOM_RECORDER);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnDimensionCalculator, this, ID_DIMENSION_CALCULATOR);
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
    wxMenuItem* mandelbrot, *mandelbrotZN, *julia, *juliaZN, *newton, *sinoidal, *magnet;
    wxMenuItem* medusa, *manowar, *manowarJulia, *sierpinskyTriangle, *fixedPoint1, *fixedPoint2;
    wxMenuItem* fixedPoint3, *fixedPoint4, *userDefined, *fpUserDefined;
    wxMenuItem* tricorn, *burningShip, *burningShipJulia, *fractory, *cell, *dPendulum;

#ifdef _WIN32
#define menuSeparator wxT('\t')
#elif __linux__
#define menuSeparator wxT("    ")
#endif

    mandelbrot = new wxMenuItem(_formula, ID_MANDELBROT, wxString(wxT("Mandelbrot")) + menuSeparator + wxT("z = z^2 + c"), wxEmptyString, wxITEM_NORMAL);
    mandelbrotZN = new wxMenuItem(_formula, ID_MANDELBROT_ZN, wxString(wxT("Mandelbrot")) + menuSeparator + wxT("z = z^n + c"), wxEmptyString, wxITEM_NORMAL);
    julia = new wxMenuItem(_formula, ID_JULIA, wxString(wxT("Mandelbrot (Julia)")) + menuSeparator + wxT("z = z^2 + k"), wxEmptyString, wxITEM_NORMAL);
    juliaZN = new wxMenuItem(_formula, ID_JULIA_ZN, wxString(wxT("Mandelbrot (Julia)")) + menuSeparator + wxT("z = z^n + k"), wxEmptyString, wxITEM_NORMAL);
    newton = new wxMenuItem(_formula, ID_NEWTON, wxString(wxT("Newton")) + menuSeparator + wxT("z^3 - 1 = 0"), wxEmptyString, wxITEM_NORMAL);
    sinoidal = new wxMenuItem(_formula, ID_SINOIDAL, wxString(wxT("Sine (Julia)")) + menuSeparator + wxT("Z = c*Sin(Z)"), wxEmptyString, wxITEM_NORMAL);
    magnet = new wxMenuItem(_formula, ID_MAGNET, wxString(wxT("Magnet")), wxEmptyString, wxITEM_NORMAL);
    medusa = new wxMenuItem(_formula, ID_MEDUSA, wxString(wxT("Jellyfish")), wxEmptyString, wxITEM_NORMAL);
    manowar = new wxMenuItem(_formula, ID_MANOWAR, wxString(wxT("Manowar")), wxEmptyString, wxITEM_NORMAL);
    manowarJulia = new wxMenuItem(_formula, ID_MANOWAR_JULIA, wxString(wxT("Manowar (Julia)")), wxEmptyString, wxITEM_NORMAL);
    sierpinskyTriangle = new wxMenuItem(_formula, ID_SIERPINSKY_TRIANGLE, wxString(wxT("Sierpinski Triangle")), wxEmptyString, wxITEM_NORMAL);
    fixedPoint1 = new wxMenuItem(_formula, ID_FIXEDPOINT1, wxString(wxT("Fixed Point")) + menuSeparator + wxT("z = sin(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint2 = new wxMenuItem(_formula, ID_FIXEDPOINT2, wxString(wxT("Fixed Point")) + menuSeparator + wxT("z = cos(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint3 = new wxMenuItem(_formula, ID_FIXEDPOINT3, wxString(wxT("Fixed Point")) + menuSeparator + wxT("z = tan(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint4 = new wxMenuItem(_formula, ID_FIXEDPOINT4, wxString(wxT("Fixed Point")) + menuSeparator + wxT("z = z^2"), wxEmptyString, wxITEM_NORMAL);
    tricorn = new wxMenuItem(_formula, ID_TRICORN, wxString(wxT("Tricorn")), wxEmptyString, wxITEM_NORMAL);
    burningShip = new wxMenuItem(_formula, ID_BURNING_SHIP, wxString(wxT("Burning Ship")), wxEmptyString, wxITEM_NORMAL);
    burningShipJulia = new wxMenuItem(_formula, ID_BURNING_SHIP_JULIA, wxString(wxT("Burning Ship (Julia)")), wxEmptyString, wxITEM_NORMAL);
    fractory = new wxMenuItem(_formula, ID_FRACTORY, wxString(wxT("Fractory")), wxEmptyString, wxITEM_NORMAL);
    cell = new wxMenuItem(_formula, ID_CELL, wxString(wxT("Cell")), wxEmptyString, wxITEM_NORMAL);
    dPendulum = new wxMenuItem(_formula, ID_DOUBLE_PENDULUM, wxString(wxT("Double pendulum")), wxEmptyString, wxITEM_NORMAL);
    userDefined = new wxMenuItem(_formula, ID_USER_DEFINED, wxString(wxT("User Formula (Complex)")), wxEmptyString, wxITEM_NORMAL);
    fpUserDefined = new wxMenuItem(_formula, ID_FIXED_POINT_USER_DEFINED, wxString(wxT("User Formula (Fixed Point)")), wxEmptyString, wxITEM_NORMAL);

    _typeComplex = new wxMenu();
    _typeNumericalMethod = new wxMenu();
    _typePhysics = new wxMenu();
    _typeOther = new wxMenu();

    _typeComplex->Append(mandelbrot);
    _typeComplex->Append(mandelbrotZN);
    _typeComplex->Append(julia);
    _typeComplex->Append(juliaZN);
    _typeComplex->Append(sinoidal);
    _typeComplex->Append(medusa);
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
    _typeOther->Append(sierpinskyTriangle);

    _formula->Append(-1, wxT("Complex"), _typeComplex);
    _formula->Append(-1, wxT("Numerical method"), _typeNumericalMethod);
    _formula->Append(-1, wxT("Physic"), _typePhysics);
    _formula->Append(-1, wxT("Other"), _typeOther);
    _formula->Append(userDefined);
    _formula->Append(fpUserDefined);
    _fractalMenu->Append(wxID_ANY, wxT("Formula"), _formula);

    // Julia constant.
    _introConstant = new wxMenu();
    _manualJuliaConstant = new wxMenuItem(_introConstant, ID_ENTER_MAN_CONSTANT, wxString(wxT("Manual")), wxEmptyString, wxITEM_NORMAL);
    _introConstant->Append(_manualJuliaConstant);
    _manualJuliaConstant->Enable(false);

    _sliderJuliaConstant = new wxMenuItem(_introConstant, ID_ENTER_SLD_CONSTANT, wxString(wxT("Slider")) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_CHECK);
    _introConstant->Append(_sliderJuliaConstant);
    _sliderJuliaConstant->Enable(false);
    _sliderJuliaConstant->Check(false);
    _fractalMenu->Append(-1, wxT("Enter Julia constant"), _introConstant);

    // Julia constant and show orbit.
    _juliaMode = new wxMenuItem(_fractalMenu, ID_JULIA_MODE, wxString(wxT("Julia mode")), wxEmptyString, wxITEM_CHECK);
    _showOrbit = new wxMenuItem(_fractalMenu, ID_SHOW_ORBIT, wxString(wxT("Show orbit")) + wxT('\t') + wxT("F2"), wxEmptyString, wxITEM_CHECK);

    _fractalMenu->Append(_juliaMode);

    _fractalMenu->Append(_showOrbit);
    _juliaMode->Check(false);
    _showOrbit->Check(false);

    // File menu.
    _fileMenu->Append(ID_SAVE, wxString(wxT("Save image")) + wxT('\t') + wxT("F4"));
    _fileMenu->Append(wxID_EXIT, wxT("Quit"));

    // Tools menu.
    _toolMenu->Append(ID_SCRIPT_EDITOR, wxT("Script editor"), wxT("Create new fractals with an scripting language."));
    _toolMenu->Append(ID_ZOOM_RECORDER, wxT("Zoom recorder"), wxT("Record a video zoom."));
    _toolMenu->Append(ID_DIMENSION_CALCULATOR, wxT("Dimension calculator"), wxT("Calculate fractal dimension."));

    // Iterations.
    _manualIterations = new wxMenuItem(_iterationsMenu, ID_IT_MANUAL, wxString(wxT("Manual iterations")), wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_manualIterations);
    _moreIterations = new wxMenuItem(_iterationsMenu, ID_INCREASE_IT, wxString(wxT("Increase iterations")) + wxT('\t') + wxT("L"), wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_moreIterations);

    _lessIterations = new wxMenuItem(_iterationsMenu, ID_DECREASE_IT, wxString(wxT("Decrease iterations")) + wxT('\t') + wxT("K"), wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_lessIterations);

    // Fractal menu.
    _fractalOptionsItem = new wxMenuItem(_fractalMenu, ID_OPTION_PANEL, wxString(wxT("Fractal options")), wxEmptyString, wxITEM_CHECK);    // Txt: "Fractal options"
    _fractalMenu->Append(_fractalOptionsItem);
    _fractalMenu->Append(ID_FORMULA_DIALOG, wxT("Enter user formula")); // Txt: "Enter user formula"
    _fractalMenu->AppendSeparator();

    _abortRenderItem = _fractalMenu->Append(ID_ABORT_RENDER, wxString(wxT("Abort")) + wxT('\t') + wxT("P"));
    _abortRenderItem->Enable(false);
    _fractalMenu->Append(ID_REDRAW, wxString(wxT("Redraw")) + wxT('\t') + wxT("F5"));
    _fractalMenu->Append(ID_RESET, wxString(wxT("Reset")));
    _rendererMenu->Append(ID_PALETTE, wxT("Renderer options"));

    // Help menu.
    _helpMenu->Append(ID_USER_MANUAL, wxT("User manual"));
    _keyboardGuide = new wxMenuItem(_helpMenu, ID_KEYBOARD_GUIDE, wxString(wxT("Keyboard guide")), wxEmptyString, wxITEM_CHECK);
    _helpMenu->Append(_keyboardGuide);
    _helpMenu->Append(ID_WELCOME_DIALOG, wxT("Open welcome guide"));
    _helpMenu->Append(ID_ABOUT, wxT("About"));


    _menubar->Append(_fileMenu, wxT("File"));
    _menubar->Append(_fractalMenu, wxT("Fractal"));
    _menubar->Append(_iterationsMenu, wxT("Iterations"));
    _menubar->Append(_rendererMenu, wxT("Renderer"));
    _menubar->Append(_toolMenu, wxT("Tools"));
    _menubar->Append(_helpMenu, wxT("Help"));
    this->SetMenuBar(_menubar);

    _sizer = new wxBoxSizer(wxHORIZONTAL);
    _fractalSizer = new wxBoxSizer(wxVERTICAL);
    _sizer->Add(_fractalSizer, 7, wxEXPAND, 5);

    const auto panelSizer = new wxBoxSizer(wxVERTICAL);

    // Option panel.
    _optionPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    _optionPanel->SetScrollRate(5, 5);
    _optionPanel->Hide();
    _showOptionsPanel = false;
    _optionSizer = new wxBoxSizer(wxVERTICAL);

    _fractalOptionsBitmap = new wxStaticBitmap(_optionPanel, wxID_ANY, wxBitmap(AppPaths::ResourceFile({wxT("prop.png")}), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
    _optionSizer->Add(_fractalOptionsBitmap, 0, wxALL, 0);

    _optionPanel->SetSizer(_optionSizer);
    _optionPanel->Layout();
    _optionSizer->Fit(_optionPanel);
    panelSizer->Add(_optionPanel, 1, wxEXPAND | wxALL, 1);

    _sizer->Add(panelSizer, 2, wxEXPAND, 5);
    this->SetSizer(_sizer);
    this->Layout();
    this->Centre(wxVERTICAL);
    _statusBar = this->CreateStatusBar(1, wxST_SIZEGRIP, wxID_ANY);

    _size = _fractalSizer->GetSize();

    // Creates fractalCanvas.
    _fractalType = _appConfig.type;
    _fractalCanvas = new FractalCanvas(_fractalType, this, wxID_ANY, wxPoint(0, 0), _size, wxBORDER_NONE);

    wxGradient grad;
    grad.SetMin(0);
    grad.SetMax(_appConfig.paletteSize);
    grad.FromString(wxString(_appConfig.colorStyleGrad.c_str(), wxConvUTF8));
    _fractalCanvas->GetSFMLFractalPtr()->SetGradient(grad);

    _fractalCanvas->GetSFMLFractalPtr()->ChangeIterations(_appConfig.maxIterations);
    _fractalCanvas->GetSFMLFractalPtr()->SetExteriorColorMode(_appConfig.colorFractal);
    _fractalCanvas->GetSFMLFractalPtr()->SetFractalSetColorMode(_appConfig.colorSet);
    _fractalSizer->Add(_fractalCanvas, 1, wxEXPAND | wxALL, 0);
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
void MainFrame::CloseAll()
{
    DestroyJuliaMode(true);
    DestroyDimensionFrame();
    delete _fractalCanvas;
}
void MainFrame::DestroyJuliaMode(const bool requestClose)
{
    if (_juliaModePtr == nullptr)
        return;

    if (requestClose)
        _juliaModePtr->Close();

    _juliaModePtr->Wait();
    delete _juliaModePtr;
    _juliaModePtr = nullptr;
    _juliaMode->Check(false);
    _fractalCanvas->SetJuliaMode(false);
}
void MainFrame::OnResize(wxSizeEvent&)
{
    this->Layout();
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
void MainFrame::OnWelcomeDialog(wxCommandEvent&)
{
    this->ShowFirstUseDialog();
}
void MainFrame::OnAbout(wxCommandEvent&)
{
    // About dialog.
    const auto dlg = new AboutDialog(this);
    dlg->SetAppName(wxT("wxChaos"));
    dlg->SetVersion(wxString::FromUTF8(APP_VERSION));
    dlg->SetCopyright(wxString::Format(wxT("%s"),
        wxT("Carlos Manuel Rodriguez y Martinez.\nEmail: fis.carlosmanuel@gmail.com\nAn open source fractal generator.\nThis program is distributed under the GPLv3 license.")));
    dlg->SetCustomBuildInfo(wxString::Format(wxT("%s. %s"),
        AboutDialog::GetBuildInfo(AboutDialog::wxBUILDINFO_LONG).GetData(),
        wxT("")));
    dlg->SetHeaderBitmap(wxBitmap(AppPaths::ResourceFile({wxT("wxChaosAbout.bmp")}), wxBITMAP_TYPE_ANY));
    dlg->ApplyInfo();
    dlg->ShowModal();
    dlg->Destroy();
}
void MainFrame::OnSave(wxCommandEvent&)
{
    // Saves the fractal image.
    const auto saveFileDialog = new wxFileDialog(
        this,
        wxT("Select file name"),
        wxT(""),
        wxT("fractal.png"),
        wxT("PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp"),
        wxFD_SAVE
        );

    if (saveFileDialog->ShowModal() == wxID_OK)
    {
        const wxString fileName = saveFileDialog->GetPath();
        const int ext = saveFileDialog->GetFilterIndex();
        const auto path = string(fileName.mb_str());
        SizeDialogSave* sizeDialogSave;

        if (_fractalType == FractalType::ScriptFractal && _selectedScriptIndex.has_value())
            sizeDialogSave = new SizeDialogSave(_fractalCanvas, path, ext, _fractalType, _fractalCanvas->GetFractalPtr(), this, _loadedScripts[*_selectedScriptIndex].file);
        else
            sizeDialogSave = new SizeDialogSave(_fractalCanvas, path, ext, _fractalType, _fractalCanvas->GetFractalPtr(), this);

        sizeDialogSave->Show(true);
    }
    saveFileDialog->Destroy();
}
void MainFrame::OnPalette(wxCommandEvent&)
{
    // Color palette frame.
    if (_rendererOptions == nullptr)
    {
        _rendererOptions = new RendererOptions(_fractalCanvas->GetSFMLFractalPtr(), this,
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
        _formulaDialog = new FormulaDialog(ID_USER_DEFINED, ID_FIXED_POINT_USER_DEFINED, _sliderJuliaConstant, _manualJuliaConstant, &_formulaDialogIsActive, _fractalCanvas, this);
        _formulaDialog->Show(true);

        // Adjust position.
        int h, w;
        GetDesktopResolution(h, w);
        if (this->GetPosition().x+this->GetSize().GetWidth()+5 < w && this->GetPosition().y < h)
            _formulaDialog->Move(this->GetPosition().x+this->GetSize().GetWidth()+5, this->GetPosition().y);

        _fractalType = FractalType::UserDefined;
    }
    else
        _formulaDialog->SetFocus();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnRedraw(wxCommandEvent&)
{
    _fractalCanvas->GetSFMLFractalPtr()->Redraw();
}
void MainFrame::OnReset(wxCommandEvent&)
{
    _fractalCanvas->Reset();
    wxGradient grad;
    grad.FromString(wxString(_appConfig.colorStyleGrad.c_str(), wxConvUTF8));
    grad.SetMin(0);
    grad.SetMax(_appConfig.paletteSize);
    _fractalCanvas->GetSFMLFractalPtr()->SetGradient(grad);
    this->UpdateMenu();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnMoreIt(wxCommandEvent&)
{
    _fractalCanvas->GetSFMLFractalPtr()->IncreaseIterations();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnLessIt(wxCommandEvent&)
{
    _fractalCanvas->GetSFMLFractalPtr()->DecreaseIterations();
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
        _juliaConstantDialog = new JuliaConstantDialog(&_introConstActive, _fractalCanvas->GetSFMLFractalPtr(), this);
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
void MainFrame::OnCanvasStatusText(wxCommandEvent& event)
{
    _statusBar->SetStatusText(event.GetString());
}
void MainFrame::OnItManual(wxCommandEvent&)
{
    // Manual iterations.
    if (!_iterationsDialogIsActive)
    {
        _iterationsDialog = new IterationsDialog(&_iterationsDialogIsActive, _fractalCanvas->GetSFMLFractalPtr(), this);
        _iterationsDialog->Show(true);
        _iterationsDialogIsActive = true;
    }
    else
    {
        _iterationsDialog->Show(false);
        _iterationsDialogIsActive = false;
        delete _iterationsDialog;
    }
}
void MainFrame::OnAbortRender(wxCommandEvent&)
{
    _fractalCanvas->AbortRender();
}
void MainFrame::OnUpdateAbortRender(wxUpdateUIEvent& event)
{
    event.Enable(_fractalCanvas != nullptr && _fractalCanvas->CanAbortRender());
}
void MainFrame::OnUpdateShowOrbit(wxUpdateUIEvent& event)
{
    event.Check(_fractalCanvas != nullptr && _fractalCanvas->IsOrbitMode());
}
void MainFrame::OnUpdateSliderMode(wxUpdateUIEvent& event)
{
    event.Check(_fractalCanvas != nullptr && _fractalCanvas->IsSliderMode());
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
void MainFrame::OnApplyPanelOpt(wxCommandEvent&)
{
    // Pass parameters to the fractal and redraws it.
    const PanelOptions* pOptions = _fractalCanvas->GetFractalPtr()->GetOptPanel();
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
    _fractalCanvas->GetSFMLFractalPtr()->Redraw();
}
// ReSharper disable once CppMemberFunctionMayBeStatic
void MainFrame::OnUserManual(wxCommandEvent&) // NOLINT(*-convert-member-functions-to-static)
{
    wxLaunchDefaultApplication(AppPaths::DocFile(wxT("User_Manual.pdf")));
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
    const SFMLFractal* fractal = _fractalCanvas->GetSFMLFractalPtr();

    if (const Rect currentZoom = fractal->GetCurrentZoom(), outermostZoom = fractal->GetOutermostZoom();
        outermostZoom._left == currentZoom._left &&
        outermostZoom._right == currentZoom._right &&
        outermostZoom._bottom == currentZoom._bottom &&
        outermostZoom._top == currentZoom._top)
    {
        wxMessageBox(
            wxT("To use the zoom recorder you need to first perform a zoom on the main window."),
            wxT("Zoom recorder"),
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
void MainFrame::ChangeSinoidal(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Sinoidal, false);
}
void MainFrame::ChangeMagnet(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Magnetic, false);
}
void MainFrame::ChangeMedusa(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Medusa, false);
}
void MainFrame::ChangeManowar(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::Manowar, true);
}
void MainFrame::ChangeManowarJulia(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::ManowarJulia, false);
}
void MainFrame::ChangeSierpinskyTriangle(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::SierpinskyTriangle, false);
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
    this->ChangeFractal(FractalType::UserDefined, false);
}
void MainFrame::ChangeFPUserDefined(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::FixedPointUserDefined, false);
}
void MainFrame::ChangeFractal(const FractalType type, const bool enableJulia)
{
    _selectedScriptIndex.reset();  // Deselect the script fractal.
    if (_fractalType != type || _fractalType == FractalType::UserDefined || _fractalType == FractalType::FixedPointUserDefined)
    {
        const Options fractOpt = _fractalCanvas->GetFractalPtr()->GetOptions();
        _fractalCanvas->ChangeType(type);
        _fractalCanvas->GetSFMLFractalPtr()->SetGradient(fractOpt.gradient);
        _fractalType = type;
        this->UpdateMenu();
        _juliaMode->Enable(enableJulia);
    }
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void MainFrame::ChangeScriptItem(wxCommandEvent& event)
{
    const unsigned int id = event.GetId() - SCRIPT_ID_INDEX;
    _selectedScriptIndex = id;

    if (_fractalCanvas->GetFractalPtr()->IsRendering())
        _fractalCanvas->GetFractalPtr()->StopRender();

    const Options fractOpt = _fractalCanvas->GetFractalPtr()->GetOptions();
    _fractalCanvas->ChangeToScript(_loadedScripts[id]);
    _fractalCanvas->GetSFMLFractalPtr()->SetGradient(fractOpt.gradient);

    _fractalType = FractalType::ScriptFractal;
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
    PanelOptions* pOptions = _fractalCanvas->GetFractalPtr()->GetOptPanel();

    // If there are elements in pOptions creates panel.
    if (pOptions->GetElementsSize() > 0)
    {
        unsigned int labelIndex;
        unsigned int index;
        _fractalOptionsItem->Enable(true);
        if (!_labels.empty() || !_textControls.empty() || !_spinControls.empty() || !_checkBoxes.empty())
        {
            // If there are elements from a previous panel, deletes them.
            this->DeleteOptPanel();
        }

        if (pOptions->GetForceShow())
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
        for (int i=0; i<pOptions->GetElementsSize(); i++)
        {
            switch(pOptions->GetPanelOptType(i))
            {
            case PanelOptionType::Label:
                {
                    _labels.push_back(new wxStaticText(_optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = _labels.size()-1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionSizer->Add(_labels[labelIndex], 0, wxALL, 5);
                    _foundLabels.push_back(i);
                }
                break;
            case PanelOptionType::TextCtrl:
                {
                    _labels.push_back(new wxStaticText(_optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = _labels.size()-1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionSizer->Add(_labels[labelIndex], 0, wxALL, 5);

                    _textControls.push_back(new wxTextCtrl(_optionPanel, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    index = _textControls.size()-1;
                    _optionSizer->Add(_textControls[index], 0, wxALL|wxEXPAND, 5);
                    _foundTextControls.push_back(i);
                }
                break;
            case PanelOptionType::Spin:
                {
                    _labels.push_back(new wxStaticText(_optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = _labels.size()-1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionSizer->Add(_labels[labelIndex], 0, wxALL, 5);

                    _spinControls.push_back(new wxSpinCtrl(_optionPanel, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0 ));
                    index = _spinControls.size()-1;
                    _optionSizer->Add(_spinControls[index], 0, wxALL|wxEXPAND, 5);
                    _foundSpinControls.push_back(i);
                }
                break;
            case PanelOptionType::CheckBox:
                {
                    _checkBoxes.push_back(new wxCheckBox(_optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    index = _checkBoxes.size()-1;
                    if (pOptions->GetDefault(i) == wxT("true"))
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
        _panelButton = new wxButton(_optionPanel, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
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
    for (auto & spinControl : _spinControls)
        spinControl->Destroy();

    _spinControls.clear();
    _foundSpinControls.clear();
    for (auto & checkBoxe : _checkBoxes)
        checkBoxe->Destroy();

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
        _rendererOptions->SetTarget(_fractalCanvas->GetSFMLFractalPtr());
    if (_iterationsDialogIsActive)
        _iterationsDialog->SetTarget(_fractalCanvas->GetSFMLFractalPtr());

    _showOrbit->Check(false);
    if (_fractalCanvas->GetFractalPtr()->HasOrbit())
        _showOrbit->Enable(true);
    else
        _showOrbit->Enable(false);

    _moreIterations->Enable(true);
    _lessIterations->Enable(true);

    // Closes constant dialog.
    if (_introConstActive)
    {
        _juliaConstantDialog->Show(false);
        _introConstActive = false;
        delete _juliaConstantDialog;
    }

    // Adjust Julia constant menu items.
    if (_fractalCanvas->GetFractalPtr()->IsJuliaVariety())
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
    if (_fractalType != FractalType::UserDefined && _fractalType != FractalType::FixedPointUserDefined)
    {
        if (_formulaDialogIsActive)
        {
            _formulaDialog->Destroy();
            _formulaDialogIsActive = false;
        }
    }

    _abortRenderItem->SetItemLabel(wxString(wxT("Abort"))+ wxT('\t') + wxT("P"));
    _abortRenderItem->Enable(false);

    // If Julia mode is opened, closes it.
    DestroyJuliaMode(true);
    this->UpdateOptionsPanel();
}
void MainFrame::UpdateJuliaMode()
{
    // Destroy Julia window.
    if (_juliaModePtr != nullptr)
    {
        _juliaMode->Check(false);
        _juliaModePtr->Close();
    }
    // Creates Julia fractal with parameters from the main fractal.
    else
    {
        _juliaMode->Check(true);

        FractalType juliaType;
        switch(_fractalType)
        {
        case FractalType::Mandelbrot:
            juliaType = FractalType::Julia;
            break;
        case FractalType::MandelbrotZN:
            juliaType = FractalType::JuliaZN;
            break;
        case FractalType::Manowar:
            juliaType = FractalType::ManowarJulia;
            break;
        case FractalType::BurningShip:
            juliaType = FractalType::BurningShipJulia;
            break;
        default:
            juliaType = FractalType::Julia;
        };

        _juliaModePtr = new JuliaMode(this, _fractalCanvas, juliaType, _fractalCanvas->GetFractalPtr()->GetOptions());
        _juliaModePtr->Launch();
        _fractalCanvas->SetJuliaMode(true);
    }
}
void MainFrame::ReloadScripts()
{
    // Remove current menu entries.
    this->RemoveScriptMenuElements();

    // Get new scripts.
    this->GetScriptFractals();
}
void MainFrame::UpdateJuliaRendererOptions(const Options& options)
{
    if (_juliaModePtr != nullptr)
        _juliaModePtr->SetRendererOptions(options);
}
