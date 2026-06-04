// ReSharper disable CppEnumeratorNeverUsed
#include "MainWindow.h"
#include "AngelscriptBindings.h"
#include "AppPaths.h"
#include "HTMLViewer.h"
#include "TextUtils.h"
#include "global.h"

#ifdef _WIN32
#include <Shellapi.h>

namespace
{
enum ProcessDpiAwarenessValue
{
    ProcessDpiUnaware = 0,
    ProcessSystemDpiAware = 1,
    ProcessPerMonitorDpiAware = 2
};

void EnableHighDpiSupport()
{
    const HMODULE user32 = LoadLibraryW(L"user32.dll");
    if (user32)
    {
        using SetProcessDpiAwarenessContextFn = BOOL(WINAPI*)(HANDLE);
        const auto setDpiAwarenessContext = reinterpret_cast<SetProcessDpiAwarenessContextFn>(
            GetProcAddress(user32, "SetProcessDpiAwarenessContext"));

        if (setDpiAwarenessContext &&
            setDpiAwarenessContext(reinterpret_cast<HANDLE>(-4))) // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        {
            FreeLibrary(user32);
            return;
        }
    }

    const HMODULE shcore = LoadLibraryW(L"Shcore.dll");
    if (shcore)
    {
        using SetProcessDpiAwarenessFn = HRESULT(WINAPI*)(ProcessDpiAwarenessValue);
        const auto setProcessDpiAwareness = reinterpret_cast<SetProcessDpiAwarenessFn>(
            GetProcAddress(shcore, "SetProcessDpiAwareness"));

        if (setProcessDpiAwareness)
            setProcessDpiAwareness(ProcessPerMonitorDpiAware);

        FreeLibrary(shcore);
        if (user32)
            FreeLibrary(user32);
        return;
    }

    if (user32)
    {
        using SetProcessDPIAwareFn = BOOL(WINAPI*)();
        const auto setProcessDPIAware = reinterpret_cast<SetProcessDPIAwareFn>(
            GetProcAddress(user32, "SetProcessDPIAware"));

        if (setProcessDPIAware)
            setProcessDPIAware();

        FreeLibrary(user32);
    }
}
}
#endif

using namespace std;

const unsigned int SCRIPT_ID_INDEX = 8510;
MainFrame* mainFramePtr = nullptr;

/**
* @brief Gets the desktop resolution. Used to adjust menu position.
*/
void GetDesktopResolution(int& width, int& height)
{
#ifdef _WIN32
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    width = desktop.right;
    height = desktop.bottom;
#endif
#ifdef linux
    int num_sizes;
    Rotation original_rotation;

    Display *dpy = XOpenDisplay(NULL);
    Window root = RootWindow(dpy, 0);
    XRRScreenSize *xrrs = XRRSizes(dpy, 0, &num_sizes);

    XRRScreenConfiguration *conf = XRRGetScreenInfo(dpy, root);
    short original_rate = XRRConfigCurrentRate(conf);
    SizeID original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);

    width = xrrs[original_size_id].width;
    height = xrrs[original_size_id].height;

    XCloseDisplay(dpy);
#endif
}

#ifdef __linux__
const std::string exec(const char* cmd)
{
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}
#endif

// Fractal Frame
MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, wxT("wxChaos"), wxDefaultPosition, wxSize(1180, 820))
{
    // Init handlers.
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxICOHandler);

    // WX.
    mainFramePtr = this;
    this->SetSizeHints(wxSize(900, 650), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({wxT("icon.ico")}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);
    this->GetParserOpt();    // Gets configuration from config.ini.
    this->SetUpGUI();

    juliaModePtr = nullptr;
    dimensionCalculator = nullptr;
    changeKeyboardGuide = false;
    rendererOptionsActive = false;
    introConstActive = false;
    iterDiagActive = false;
    infoFrameActive = false;
    formDiagActive = false;
    scriptEditorActive = false;
    pause = false;
    selectedScriptIndex = -1;

    this->UpdateMenu();

    // Set parameters found in the config.ini file.
    if (opt.juliaMode) this->UpdateJuliaMode();
    if (opt.colorPaletteWindow)
    {
        rendererOptionsActive = true;
        rendererOptions = new RendererOptions(&rendererOptionsActive, fractalCanvas->GetSFMLFractalPtr(), this);
        rendererOptions->Show(true);
    }
    if (opt.constantWindow)
    {
        diag = new ConstDialog(&introConstActive, fractalCanvas->GetSFMLFractalPtr(), this);
        diag->Show(true);
        introConstActive = true;
    }
    if (!opt.colorSet)
        fractalCanvas->GetSFMLFractalPtr()->SetFractalSetColorMode(false);

    if (opt.firstUse)
        this->ShowFirstUseDialog();

    if (fractalType != FractalType::Mandelbrot && fractalType != FractalType::Manowar)
        juliaMode->Enable(false);

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
    fractalCanvas->ShowHelpImage();
}
void MainFrame::ConnectEvents()
{
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));
    this->Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnQuit));
    this->Connect(wxEVT_SIZE, wxSizeEventHandler(MainFrame::OnResize));
    this->Connect(ID_JULIA_MODE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnJuliaMode));
    this->Connect(wxEVT_JULIA_MODE_CLOSED, wxCommandEventHandler(MainFrame::OnJuliaModeClosed));
    this->Connect(wxEVT_DIMENSION_FRAME_CLOSED, wxCommandEventHandler(MainFrame::OnDimensionFrameClosed));
    this->Connect(ID_WELCOME_DIALOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnWelcomeDialog));
    this->Connect(ID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAbout));
    this->Connect(ID_KEYBOARDGUIDE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnKeyboardGuide));
    this->Connect(ID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSave));
    this->Connect(ID_PALETTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnPalette));
    this->Connect(ID_MANDELBROT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeMandelbrot));
    this->Connect(ID_MANDELBROT_ZN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeMandelbrotZN));
    this->Connect(ID_JULIA, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeJulia));
    this->Connect(ID_JULIA_ZN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeJuliaZN));
    this->Connect(ID_NEWTON, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeNewton));
    this->Connect(ID_SINOIDAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeSinoidal));
    this->Connect(ID_MAGNET, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeMagnet));
    this->Connect(ID_MEDUSA, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeMedusa));
    this->Connect(ID_MANOWAR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeManowar));
    this->Connect(ID_MANOWAR_JULIA, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeManowarJulia));
    this->Connect(ID_FIXEDPOINT1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeFixedPoint1));
    this->Connect(ID_FIXEDPOINT2, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeFixedPoint2));
    this->Connect(ID_FIXEDPOINT3, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeFixedPoint3));
    this->Connect(ID_FIXEDPOINT4, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeFixedPoint4));
    this->Connect(ID_TRICORN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeTricorn));
    this->Connect(ID_BURNING_SHIP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeBurningShip));
    this->Connect(ID_BURNING_SHIP_JULIA, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeBurningShipJulia));
    this->Connect(ID_FRACTORY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeFractory));
    this->Connect(ID_CELL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeCell));
    this->Connect(ID_SIERP_TRIANGLE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeSierpinskyTriangle));
    this->Connect(ID_DPENDULUM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeDPendulum));
    this->Connect(ID_USER_DEFINED, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeUserDefined));
    this->Connect(ID_FPUSER_DEFINED, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeFPUserDefined));
    this->Connect(ID_PAUSE_CONTINUE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAbortRender));
    this->Connect(ID_RESET, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnReset));
    this->Connect(ID_REDRAW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnRedraw));
    this->Connect(ID_INCREASE_IT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnMoreIt));
    this->Connect(ID_DECREASE_IT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnLessIt));
    this->Connect(ID_SHOW_ORBIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnShowOrbit));
    this->Connect(ID_ENTER_MAN_CONSTANT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnManIntroConst));
    this->Connect(ID_ENTER_SLD_CONSTANT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSldIntroConst));
    this->Connect(ID_IT_MANUAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnItManual));
    this->Connect(ID_FORMULA_DIALOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnFormulaDialog));
    this->Connect(ID_OPTPANEL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnFractalOptions));
    this->Connect(ID_USER_MANUAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnUserManual));
    this->Connect(ID_SCRIPT_EDITOR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnScriptEditor));
    this->Connect(ID_ZOOM_RECORDER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnZoomRecorder));
    this->Connect(ID_DIMENSION_CALCULATOR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnDimensionCalculator));
}

void MainFrame::SetUpGUI()
{
    // Init menu.
    menubar = new wxMenuBar();
    fileMenu = new wxMenu();
    fractalMenu = new wxMenu();
    iterationsMenu = new wxMenu();
    toolMenu = new wxMenu();
    rendererMenu = new wxMenu();
    helpMenu = new wxMenu();
    formula = new wxMenu();
    rendererOptions = nullptr;

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

    mandelbrot = new wxMenuItem(formula, ID_MANDELBROT, wxString(wxT("Mandelbrot")) + menuSeparator + wxT("z = z^2 + c"), wxEmptyString, wxITEM_NORMAL);
    mandelbrotZN = new wxMenuItem(formula, ID_MANDELBROT_ZN, wxString(wxT("Mandelbrot")) + menuSeparator + wxT("z = z^n + c"), wxEmptyString, wxITEM_NORMAL);
    julia = new wxMenuItem(formula, ID_JULIA, wxString(wxT("Mandelbrot (Julia)")) + menuSeparator + wxT("z = z^2 + k"), wxEmptyString, wxITEM_NORMAL);
    juliaZN = new wxMenuItem(formula, ID_JULIA_ZN, wxString(wxT("Mandelbrot (Julia)")) + menuSeparator + wxT("z = z^n + k"), wxEmptyString, wxITEM_NORMAL);
    newton = new wxMenuItem(formula, ID_NEWTON, wxString(wxT("Newton")) + menuSeparator + wxT("z^3 - 1 = 0"), wxEmptyString, wxITEM_NORMAL);
    sinoidal = new wxMenuItem(formula, ID_SINOIDAL, wxString(wxT("Sine (Julia)")) + menuSeparator + wxT("Z = c*Sin(Z)"), wxEmptyString, wxITEM_NORMAL);
    magnet = new wxMenuItem(formula, ID_MAGNET, wxString(wxT("Magnet")), wxEmptyString, wxITEM_NORMAL);
    medusa = new wxMenuItem(formula, ID_MEDUSA, wxString(wxT("Jellyfish")), wxEmptyString, wxITEM_NORMAL);
    manowar = new wxMenuItem(formula, ID_MANOWAR, wxString(wxT("Manowar")), wxEmptyString, wxITEM_NORMAL);
    manowarJulia = new wxMenuItem(formula, ID_MANOWAR_JULIA, wxString(wxT("Manowar (Julia)")), wxEmptyString, wxITEM_NORMAL);
    sierpinskyTriangle = new wxMenuItem(formula, ID_SIERP_TRIANGLE, wxString(wxT("Sierpinski Triangle")), wxEmptyString, wxITEM_NORMAL);
    fixedPoint1 = new wxMenuItem(formula, ID_FIXEDPOINT1, wxString(wxT("Fixed Point")) + menuSeparator + wxT("z = sin(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint2 = new wxMenuItem(formula, ID_FIXEDPOINT2, wxString(wxT("Fixed Point")) + menuSeparator + wxT("z = cos(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint3 = new wxMenuItem(formula, ID_FIXEDPOINT3, wxString(wxT("Fixed Point")) + menuSeparator + wxT("z = tan(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint4 = new wxMenuItem(formula, ID_FIXEDPOINT4, wxString(wxT("Fixed Point")) + menuSeparator + wxT("z = z^2"), wxEmptyString, wxITEM_NORMAL);
    tricorn = new wxMenuItem(formula, ID_TRICORN, wxString(wxT("Tricorn")), wxEmptyString, wxITEM_NORMAL);
    burningShip = new wxMenuItem(formula, ID_BURNING_SHIP, wxString(wxT("Burning Ship")), wxEmptyString, wxITEM_NORMAL);
    burningShipJulia = new wxMenuItem(formula, ID_BURNING_SHIP_JULIA, wxString(wxT("Burning Ship (Julia)")), wxEmptyString, wxITEM_NORMAL);
    fractory = new wxMenuItem(formula, ID_FRACTORY, wxString(wxT("Fractory")), wxEmptyString, wxITEM_NORMAL);
    cell = new wxMenuItem(formula, ID_CELL, wxString(wxT("Cell")), wxEmptyString, wxITEM_NORMAL);
    dPendulum = new wxMenuItem(formula, ID_DPENDULUM, wxString(wxT("Double pendulum")), wxEmptyString, wxITEM_NORMAL);
    userDefined = new wxMenuItem(formula, ID_USER_DEFINED, wxString(wxT("User Formula (Complex)")), wxEmptyString, wxITEM_NORMAL);
    fpUserDefined = new wxMenuItem(formula, ID_FPUSER_DEFINED, wxString(wxT("User Formula (Fixed Point)")), wxEmptyString, wxITEM_NORMAL);

    typeComplex = new wxMenu();
    typeNumMet = new wxMenu();
    typePhysics = new wxMenu();
    typeOther = new wxMenu();

    typeComplex->Append(mandelbrot);
    typeComplex->Append(mandelbrotZN);
    typeComplex->Append(julia);
    typeComplex->Append(juliaZN);
    typeComplex->Append(sinoidal);
    typeComplex->Append(medusa);
    typeComplex->Append(manowar);
    typeComplex->Append(manowarJulia);
    typeComplex->Append(tricorn);
    typeComplex->Append(burningShip);
    typeComplex->Append(burningShipJulia);
    typeComplex->Append(fractory);
    typeComplex->Append(cell);
    typeNumMet->Append(newton);
    typeNumMet->Append(fixedPoint1);
    typeNumMet->Append(fixedPoint2);
    typeNumMet->Append(fixedPoint3);
    typeNumMet->Append(fixedPoint4);
    typePhysics->Append(magnet);
    typePhysics->Append(dPendulum);
    typeOther->Append(sierpinskyTriangle);

    formula->Append(-1, wxT("Complex"), typeComplex);
    formula->Append(-1, wxT("Numerical method"), typeNumMet);
    formula->Append(-1, wxT("Physic"), typePhysics);
    formula->Append(-1, wxT("Other"), typeOther);
    formula->Append(userDefined);
    formula->Append(fpUserDefined);
    fractalMenu->Append(wxID_ANY, wxT("Formula"), formula);

    // Julia constant.
    introConstant = new wxMenu();
    manual = new wxMenuItem(introConstant, ID_ENTER_MAN_CONSTANT, wxString(wxT("Manual")), wxEmptyString, wxITEM_NORMAL);
    introConstant->Append(manual);
    manual->Enable(false);

    statusData.slider = slider = new wxMenuItem(introConstant, ID_ENTER_SLD_CONSTANT, wxString(wxT("Slider")) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_CHECK);
    introConstant->Append(slider);
    slider->Enable(false);
    slider->Check(false);
    fractalMenu->Append(-1, wxT("Enter Julia constant"), introConstant);

    // Julia constant and show orbit.
    juliaMode = new wxMenuItem(fractalMenu, ID_JULIA_MODE, wxString(wxT("Julia mode")), wxEmptyString, wxITEM_CHECK);
    statusData.showOrbit = showOrbit = new wxMenuItem(fractalMenu, ID_SHOW_ORBIT, wxString(wxT("Show orbit")) + wxT('\t') + wxT("F2"), wxEmptyString, wxITEM_CHECK);

    fractalMenu->Append(juliaMode);

    fractalMenu->Append(showOrbit);
    juliaMode->Check(false);
    showOrbit->Check(false);

    // File menu.
    fileMenu->Append(ID_SAVE, wxString(wxT("Save image")) + wxT('\t') + wxT("F4"));
    fileMenu->Append(wxID_EXIT, wxT("Quit"));

    // Tools menu.
    toolMenu->Append(ID_SCRIPT_EDITOR, wxT("Script editor"), wxT("Create new fractals with an scripting language."));
    toolMenu->Append(ID_ZOOM_RECORDER, wxT("Zoom recorder"), wxT("Record a video zoom."));
    toolMenu->Append(ID_DIMENSION_CALCULATOR, wxT("Dimension calculator"), wxT("Calculate fractal dimension."));

    // Iterations.
    itManual = new wxMenuItem(iterationsMenu, ID_IT_MANUAL, wxString(wxT("Manual iterations")), wxEmptyString, wxITEM_NORMAL);
    iterationsMenu->Append(itManual);
    MoreIter = new wxMenuItem(iterationsMenu, ID_INCREASE_IT, wxString(wxT("Increase iterations")) + wxT('\t') + wxT("L"), wxEmptyString, wxITEM_NORMAL);
    iterationsMenu->Append(MoreIter);

    lessIter = new wxMenuItem(iterationsMenu, ID_DECREASE_IT, wxString(wxT("Decrease iterations")) + wxT('\t') + wxT("K"), wxEmptyString, wxITEM_NORMAL);
    iterationsMenu->Append(lessIter);

    // Fractal menu.
    fractOptItem = new wxMenuItem(fractalMenu, ID_OPTPANEL, wxString(wxT("Fractal options")), wxEmptyString, wxITEM_CHECK);    // Txt: "Fractal options"
    fractalMenu->Append(fractOptItem);
    fractalMenu->Append(ID_FORMULA_DIALOG, wxT("Enter user formula")); // Txt: "Enter user formula"
    fractalMenu->AppendSeparator();

    pauseBtn.pauseContinue = fractalMenu->Append(ID_PAUSE_CONTINUE, wxString(wxT("Abort")) + wxT('\t') + wxT("P"));
    pauseBtn.state = false;
    pauseBtn.pauseContinue->Enable(false);
    fractalMenu->Append(ID_REDRAW, wxString(wxT("Redraw")) + wxT('\t') + wxT("F5"));
    fractalMenu->Append(ID_RESET, wxString(wxT("Reset")));
    rendererMenu->Append(ID_PALETTE, wxT("Renderer options"));

    // Help menu.
    helpMenu->Append(ID_USER_MANUAL, wxT("User manual"));
    keyboardGuide = new wxMenuItem(helpMenu, ID_KEYBOARDGUIDE, wxString(wxT("Keyboard guide")), wxEmptyString, wxITEM_CHECK);
    helpMenu->Append(keyboardGuide);
    helpMenu->Append(ID_WELCOME_DIALOG, wxT("Open welcome guide"));
    helpMenu->Append(ID_ABOUT, wxT("About"));


    menubar->Append(fileMenu, wxT("File"));
    menubar->Append(fractalMenu, wxT("Fractal"));
    menubar->Append(iterationsMenu, wxT("Iterations"));
    menubar->Append(rendererMenu, wxT("Renderer"));
    menubar->Append(toolMenu, wxT("Tools"));
    menubar->Append(helpMenu, wxT("Help"));
    this->SetMenuBar(menubar);

    sizer = new wxBoxSizer(wxHORIZONTAL);
    fractalSizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(fractalSizer, 7, wxEXPAND, 5);

    const auto panelSizer = new wxBoxSizer(wxVERTICAL);

    // Option panel.
    optionPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    optionPanel->SetScrollRate(5, 5);
    optionPanel->Hide();
    showOptPanel = false;
    optionSizer = new wxBoxSizer(wxVERTICAL);

    propBitmap = new wxStaticBitmap(optionPanel, wxID_ANY, wxBitmap(AppPaths::ResourceFile({wxT("prop.png")}), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
    optionSizer->Add(propBitmap, 0, wxALL, 0);

    optionPanel->SetSizer(optionSizer);
    optionPanel->Layout();
    optionSizer->Fit(optionPanel);
    panelSizer->Add(optionPanel, 1, wxEXPAND | wxALL, 1);

    sizer->Add(panelSizer, 2, wxEXPAND, 5);
    this->SetSizer(sizer);
    this->Layout();
    this->Centre(wxVERTICAL);
    statusData.status = status = this->CreateStatusBar(1, wxST_SIZEGRIP, wxID_ANY);

    size = fractalSizer->GetSize();

    // Creates fractalCanvas.
    fractalType = opt.type;
    fractalCanvas = new FractalCanvas(statusData, &pauseBtn, fractalType, this, wxID_ANY, wxPoint(0, 0), size, wxBORDER_NONE);

    wxGradient grad;
    grad.SetMin(0);
    grad.SetMax(opt.paletteSize);
    grad.FromString(wxString(opt.colorStyleGrad.c_str(), wxConvUTF8));
    fractalCanvas->GetSFMLFractalPtr()->SetGradient(grad);

    fractalCanvas->GetSFMLFractalPtr()->ChangeIterations(opt.maxIterations);
    fractalCanvas->GetSFMLFractalPtr()->SetExteriorColorMode(opt.colorFractal);
    fractalCanvas->GetSFMLFractalPtr()->SetFractalSetColorMode(opt.colorSet);
    fractalSizer->Add(fractalCanvas, 1, wxEXPAND | wxALL, 0);
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
    delete fractalCanvas;
}
void MainFrame::DestroyJuliaMode(const bool requestClose)
{
    if (juliaModePtr == nullptr)
        return;

    if (requestClose)
        juliaModePtr->Close();

    juliaModePtr->Wait();
    delete juliaModePtr;
    juliaModePtr = nullptr;
    juliaMode->Check(false);
    fractalCanvas->SetJuliaMode(false);
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
void MainFrame::DestroyDimensionFrame()
{
    if (dimensionCalculator == nullptr)
        return;

    DimensionFrame* frame = dimensionCalculator;
    dimensionCalculator = nullptr;
    frame->Close(true);
}
void MainFrame::OnDimensionFrameClosed(wxCommandEvent&)
{
    dimensionCalculator = nullptr;
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

        if (fractalType == FractalType::ScriptFractal)
            sizeDialogSave = new SizeDialogSave(fractalCanvas, path, ext, fractalType, fractalCanvas->GetFractalPtr(), this, loadedScripts[selectedScriptIndex].file);
        else
            sizeDialogSave = new SizeDialogSave(fractalCanvas, path, ext, fractalType, fractalCanvas->GetFractalPtr(), this);

        sizeDialogSave->Show(true);
    }
    saveFileDialog->Destroy();
}
void MainFrame::OnPalette(wxCommandEvent&)
{
    // Color palette frame.
    if (!rendererOptionsActive)
    {
        rendererOptionsActive = true;
        rendererOptions = new RendererOptions(&rendererOptionsActive, fractalCanvas->GetSFMLFractalPtr(), this);
        rendererOptions->Show(true);

        // Adjust position.
        int h, w;
        GetDesktopResolution(h, w);
        if (this->GetPosition().x+this->GetSize().GetWidth()+5 < w && this->GetPosition().y < h)
            rendererOptions->Move(this->GetPosition().x+this->GetSize().GetWidth()+5, this->GetPosition().y);
    }
    else
        rendererOptions->SetFocus();
}
void MainFrame::OnFormulaDialog(wxCommandEvent&)
{
    // User formula menu.
    if (!formDiagActive)
    {
        formDiagActive = true;
        formDialog = new FormulaDialog(ID_USER_DEFINED, ID_FPUSER_DEFINED, slider, manual, &formDiagActive, fractalCanvas, this);
        formDialog->Show(true);

        // Adjust position.
        int h, w;
        GetDesktopResolution(h, w);
        if (this->GetPosition().x+this->GetSize().GetWidth()+5 < w && this->GetPosition().y < h)
            formDialog->Move(this->GetPosition().x+this->GetSize().GetWidth()+5, this->GetPosition().y);

        fractalType = FractalType::UserDefined;
    }
    else
        formDialog->SetFocus();
}
void MainFrame::OnRedraw(wxCommandEvent&)
{
    pauseBtn.state = false;
    pauseBtn.pauseContinue->Enable(false);
    fractalCanvas->GetSFMLFractalPtr()->Redraw();
}
void MainFrame::OnReset(wxCommandEvent&)
{
    fractalCanvas->Reset();
    wxGradient grad;
    grad.FromString(wxString(opt.colorStyleGrad.c_str(), wxConvUTF8));
    grad.SetMin(0);
    grad.SetMax(opt.paletteSize);
    fractalCanvas->GetSFMLFractalPtr()->SetGradient(grad);
    this->UpdateMenu();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnMoreIt(wxCommandEvent&)
{
    fractalCanvas->GetSFMLFractalPtr()->IncreaseIterations();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnLessIt(wxCommandEvent&)
{
    fractalCanvas->GetSFMLFractalPtr()->DecreaseIterations();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnShowOrbit(wxCommandEvent&)
{
    const bool modo = showOrbit->IsChecked();
    fractalCanvas->SetOrbitMode(modo);
    showOrbit->Check(modo);
}
void MainFrame::OnManIntroConst(wxCommandEvent&)
{
    // Manual constant.
    if (!introConstActive)
    {
        diag = new ConstDialog(&introConstActive, fractalCanvas->GetSFMLFractalPtr(), this);
        diag->Show(true);
        introConstActive = true;
    }
    else
    {
        diag->Show(false);
        introConstActive = false;
        delete diag;
    }
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnSldIntroConst(wxCommandEvent&)
{
    // Slider constant.
    const bool modo = slider->IsChecked();
    fractalCanvas->SetSliderMode(modo);
    slider->Check(modo);
}
void MainFrame::OnKeyboardGuide(wxCommandEvent&)
{
    // Keyboard guide.
    changeKeyboardGuide = !changeKeyboardGuide;
    fractalCanvas->SetKeyboardGuide(changeKeyboardGuide);
    keyboardGuide->Check(changeKeyboardGuide);
}
void MainFrame::OnItManual(wxCommandEvent&)
{
    // Manual iterations.
    if (!iterDiagActive)
    {
        iterDiag = new IterationsDialog(&iterDiagActive, fractalCanvas->GetSFMLFractalPtr(), this);
        iterDiag->Show(true);
        iterDiagActive = true;
    }
    else
    {
        iterDiag->Show(false);
        iterDiagActive = false;
        delete iterDiag;
    }
}
void MainFrame::OnAbortRender(wxCommandEvent&)
{
    Fractal* fractal = fractalCanvas->GetFractalPtr();
    fractal->StopRender();
    fractal->SetRendered(true);
    pauseBtn.state = false;
    pauseBtn.pauseContinue->Enable(false);
}
void MainFrame::OnFractalOptions(wxCommandEvent&)
{
    // Adjust the panel.
    if (!showOptPanel)
    {
        fractOptItem->Check(true);
        optionPanel->Show();
        const wxSize windowSize = this->GetSize();
        if (!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()+175, windowSize.GetHeight());

        this->GetSizer()->Layout();
        showOptPanel = true;
    }
    else
    {
        fractOptItem->Check(false);
        optionPanel->Hide();
        const wxSize windowSize = this->GetSize();
        if (!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()-175, windowSize.GetHeight());
        this->GetSizer()->Layout();
        showOptPanel = false;
    }
}
void MainFrame::OnApplyPanelOpt(wxCommandEvent&)
{
    // Pass parameters to the fractal and redraws it.
    const PanelOptions* pOptions = fractalCanvas->GetFractalPtr()->GetOptPanel();
    for (unsigned int i=0; i<foundTextControls.size(); i++)
        *pOptions->GetDoubleElement(i) = TextUtils::ToDouble(textControls[i]->GetValue());

    for (unsigned int i=0; i<foundSpinControls.size(); i++)
        *pOptions->GetIntElement(i) = spinControls[i]->GetValue();

    for (unsigned int i=0; i<foundCheckBoxes.size(); i++)
    {
        if (checkBoxes[i]->GetValue())
            *pOptions->GetBoolElement(i) = true;
        else
            *pOptions->GetBoolElement(i) = false;
    }
    pauseBtn.state = false;
    pauseBtn.pauseContinue->Enable(false);

    fractalCanvas->SetFocus();
    fractalCanvas->GetSFMLFractalPtr()->Redraw();
}
// ReSharper disable once CppMemberFunctionMayBeStatic
void MainFrame::OnUserManual(wxCommandEvent&)
{
    wxLaunchDefaultApplication(AppPaths::DocFile(wxT("User_Manual.pdf")));
}
void MainFrame::OnScriptEditor(wxCommandEvent&)
{
    if (!scriptEditorActive)
    {
        scriptEditor = new ScriptEditor(&scriptEditorActive, this);
        scriptEditor->Show(true);
        scriptEditorActive = true;
    }
    else
    {
        scriptEditor->Show(false);
        scriptEditorActive = false;
        delete scriptEditor;
    }
}
void MainFrame::OnZoomRecorder(wxCommandEvent&)
{
    const SFMLFractal* fractal = fractalCanvas->GetSFMLFractalPtr();
    const Rect outermostZoom = fractal->GetOutermostZoom();
    const Rect currentZoom = fractal->GetCurrentZoom();

    if (outermostZoom._left == currentZoom._left &&
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

    ZoomRecorder zoomRecorder(fractalCanvas, this);
    zoomRecorder.ShowModal();
}
void MainFrame::OnDimensionCalculator(wxCommandEvent&)
{
    if (dimensionCalculator == nullptr)
    {
        dimensionCalculator = new DimensionFrame(this);
        dimensionCalculator->Show(true);
    }
    else
        dimensionCalculator->SetFocus();
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
void MainFrame::ChangeFractal(const FractalType fType, const bool enableJulia)
{
    selectedScriptIndex = -1;  // Deselect the script fractal.
    if (fractalType != fType || fractalType == FractalType::UserDefined || fractalType == FractalType::FixedPointUserDefined)
    {
        const Options fractOpt = fractalCanvas->GetFractalPtr()->GetOptions();
        fractalCanvas->ChangeType(fType);
        fractalCanvas->GetSFMLFractalPtr()->SetGradient(fractOpt.gradient);
        fractalType = fType;
        this->UpdateMenu();
        juliaMode->Enable(enableJulia);
    }
}
void MainFrame::ChangeScriptItem(wxCommandEvent& event)
{
    const unsigned int id = static_cast<unsigned int>(event.GetId() - SCRIPT_ID_INDEX);
    selectedScriptIndex = id;

    if (fractalCanvas->GetFractalPtr()->IsRendering())
        fractalCanvas->GetFractalPtr()->StopRender();

    const Options fractOpt = fractalCanvas->GetFractalPtr()->GetOptions();
    fractalCanvas->ChangeToScript(loadedScripts[id]);
    fractalCanvas->GetSFMLFractalPtr()->SetGradient(fractOpt.gradient);

    fractalType = FractalType::ScriptFractal;
    this->UpdateMenu();
    juliaMode->Enable(false);
}

// Methods to adjust the menu.
void MainFrame::GetParserOpt()
{
    const AppConfigStore configStore(AppPaths::ToStdPath(AppPaths::ConfigFile()));
    opt = configStore.Load();
    configStore.SetFirstUse(false);
}
void MainFrame::UpdateOptPanel()
{
    PanelOptions* pOptions = fractalCanvas->GetFractalPtr()->GetOptPanel();

    // If there are elements in pOptions creates panel.
    if (pOptions->GetElementsSize() > 0)
    {
        int labelIndex;
        int index;
        fractOptItem->Enable(true);
        if (labels.size() != 0 || textControls.size() != 0 || spinControls.size() != 0 || checkBoxes.size() != 0)
        {
            // If there are elements from a previous panel deletes them.
            this->DeleteOptPanel();
        }

        if (pOptions->GetForceShow())
        {
            fractOptItem->Check(true);
            optionPanel->Show();
            const wxSize windowSize = this->GetSize();
            if (!this->IsMaximized())
                this->SetSize(windowSize.GetWidth()+175, windowSize.GetHeight());

            this->GetSizer()->Layout();
            showOptPanel = true;
        }
        else
            fractOptItem->Check(false);

        // Creates elements from each kind.
        for (int i=0; i<pOptions->GetElementsSize(); i++)
        {
            switch(pOptions->GetPanelOptType(i))
            {
            case PanelOptionType::Label:
                {
                    labels.push_back(new wxStaticText(optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap(-1);
                    optionSizer->Add(labels[labelIndex], 0, wxALL, 5);
                    foundLabels.push_back(i);
                }
                break;
            case PanelOptionType::TextCtrl:
                {
                    labels.push_back(new wxStaticText(optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap(-1);
                    optionSizer->Add(labels[labelIndex], 0, wxALL, 5);

                    textControls.push_back(new wxTextCtrl(optionPanel, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    index = textControls.size()-1;
                    optionSizer->Add(textControls[index], 0, wxALL|wxEXPAND, 5);
                    foundTextControls.push_back(i);
                }
                break;
            case PanelOptionType::Spin:
                {
                    labels.push_back(new wxStaticText(optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap(-1);
                    optionSizer->Add(labels[labelIndex], 0, wxALL, 5);

                    spinControls.push_back(new wxSpinCtrl(optionPanel, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0 ));
                    index = spinControls.size()-1;
                    optionSizer->Add(spinControls[index], 0, wxALL|wxEXPAND, 5);
                    foundSpinControls.push_back(i);
                }
                break;
            case PanelOptionType::CheckBox:
                {
                    checkBoxes.push_back(new wxCheckBox(optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    index = checkBoxes.size()-1;
                    if (pOptions->GetDefault(i) == wxT("true"))
                        checkBoxes[index]->SetValue(true);
                    else
                        checkBoxes[index]->SetValue(false);

                    optionSizer->Add(checkBoxes[index], 0, wxALL|wxEXPAND, 5);
                    foundCheckBoxes.push_back(i);
                }
                break;
            };
        }

        // Creates button to apply options.
        panelButton = new wxButton(optionPanel, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
        optionSizer->Add(panelButton, 0, wxALL, 5);
        panelButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnApplyPanelOpt), nullptr, this);
        optionSizer->Layout();
        optionPanel->SetScrollbars(20, 20, 0, 50);
    }
    else
    {
        fractOptItem->Check(false);
        fractOptItem->Enable(false);
        if (showOptPanel)
            this->DeleteOptPanel();
    }
}
void MainFrame::DeleteOptPanel()
{
    // Deletes panel elements.
    for (unsigned int i=0; i<labels.size(); i++)
        labels[i]->Destroy();

    labels.clear();
    foundLabels.clear();
    for (unsigned int i=0; i<textControls.size(); i++)
        textControls[i]->Destroy();

    textControls.clear();
    foundTextControls.clear();
    for (unsigned int i=0; i<spinControls.size(); i++)
        spinControls[i]->Destroy();

    spinControls.clear();
    foundSpinControls.clear();
    for (unsigned int i=0; i<checkBoxes.size(); i++)
        checkBoxes[i]->Destroy();

    checkBoxes.clear();
    foundCheckBoxes.clear();

    // Erase button, disconnect event and hide panel.
    panelButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnApplyPanelOpt), nullptr, this);
    delete panelButton;

    if (showOptPanel)
    {
        optionPanel->Hide();
        const wxSize windowSize = this->GetSize();

        if (!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()-175, windowSize.GetHeight());

        this->GetSizer()->Layout();
        showOptPanel = false;
    }
}
void MainFrame::AddScriptMenuElement(const ScriptData& scriptData, int index)
{
    loadedScripts.push_back(scriptData);
    scriptItems.push_back(new wxMenuItem(formula, SCRIPT_ID_INDEX + index, wxString(scriptData.name.c_str(), wxConvUTF8),
        wxEmptyString, wxITEM_NORMAL));

    if (scriptData.scriptCategory == ScriptCategory::Complex)
        typeComplex->Append(scriptItems[index]);
    else if (scriptData.scriptCategory == ScriptCategory::NumMet)
        typeNumMet->Append(scriptItems[index]);
    else if (scriptData.scriptCategory == ScriptCategory::Physic)
        typePhysics->Append(scriptItems[index]);
    else
        typeOther->Append(scriptItems[index]);

    this->Connect(SCRIPT_ID_INDEX + index, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeScriptItem));
}
void MainFrame::RemoveScriptMenuElements()
{
    // Remove current menu entries.
    for (unsigned int i = 0; i < loadedScripts.size(); i++)
    {
        if (loadedScripts[i].scriptCategory == ScriptCategory::Complex)
            typeComplex->Remove(scriptItems[i]);
        else if (loadedScripts[i].scriptCategory == ScriptCategory::NumMet)
            typeNumMet->Remove(scriptItems[i]);
        else if (loadedScripts[i].scriptCategory == ScriptCategory::Physic)
            typePhysics->Remove(scriptItems[i]);
        else
            typeOther->Remove(scriptItems[i]);
    }
    loadedScripts.clear();

    // Disconnect events and delete menu items.
    for (unsigned int i = 0; i < scriptItems.size(); i++)
    {
        this->Disconnect(SCRIPT_ID_INDEX + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeScriptItem));
        delete scriptItems[i];
    }
    scriptItems.clear();
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
    if (rendererOptionsActive)
        rendererOptions->SetTarget(fractalCanvas->GetSFMLFractalPtr());
    if (iterDiagActive)
        iterDiag->SetTarget(fractalCanvas->GetSFMLFractalPtr());

    showOrbit->Check(false);
    if (fractalCanvas->GetFractalPtr()->HasOrbit())
        showOrbit->Enable(true);
    else
        showOrbit->Enable(false);

    MoreIter->Enable(true);
    lessIter->Enable(true);

    // Closes constant dialog.
    if (introConstActive)
    {
        diag->Show(false);
        introConstActive = false;
        delete diag;
    }

    // Adjust Julia constant menu items.
    if (fractalCanvas->GetFractalPtr()->IsJuliaVariety())
    {
        manual->Enable(true);
        slider->Enable(true);
    }
    else
    {
        manual->Enable(false);
        slider->Enable(false);
    }
    slider->Check(false);

    // Closes formula dialog.
    if (fractalType != FractalType::UserDefined && fractalType != FractalType::FixedPointUserDefined)
    {
        if (formDiagActive)
        {
            formDialog->Destroy();
            formDiagActive = false;
        }
    }

    pauseBtn.state = false;
    pauseBtn.pauseContinue->SetItemLabel(wxString(wxT("Abort"))+ wxT('\t') + wxT("P"));
    pauseBtn.pauseContinue->Enable(false);

    // If Julia mode is opened closes it.
    DestroyJuliaMode(true);
    this->UpdateOptPanel();
}
void MainFrame::UpdateJuliaMode()
{
    // Destroy Julia window.
    if (juliaModePtr != nullptr)
    {
        juliaMode->Check(false);
        juliaModePtr->Close();
    }
    // Creates Julia fractal with parameters from the main fractal.
    else
    {
        juliaMode->Check(true);

        FractalType juliaType;
        switch(fractalType)
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

        juliaModePtr = new JuliaMode(this, fractalCanvas, juliaType, fractalCanvas->GetFractalPtr()->GetOptions());
        juliaModePtr->Launch();
        fractalCanvas->SetJuliaMode(true);
    }
}
void MainFrame::ReloadScripts()
{
    // Remove current menu entries.
    this->RemoveScriptMenuElements();

    // Get new scripts.
    this->GetScriptFractals();
}

/**
* @class MainApp
* @brief Starts the execution of the program.
*/
class MainApp : public wxApp
{
    bool OnInit() override
    {
#ifdef _WIN32
       EnableHighDpiSupport();
#endif
       const auto main = new MainFrame;
       main->Show();
       return true;
    }
};

// wxWidgets entry point.
IMPLEMENT_APP(MainApp);
