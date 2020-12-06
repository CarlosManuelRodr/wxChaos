#include "MainWindow.h"
#include "AngelScriptFunc.h"
#include "HTMLViewer.h"
#include "global.h"
#include <fstream>

#ifdef _WIN32
#include <Shellapi.h>
#endif

const unsigned int SCRIPT_ID_INDEX = 8510;
MainFrame* mainFramePtr = NULL;

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
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}
#endif

// Fractal Frame
MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, wxT("wxChaos"), wxDefaultPosition, wxSize(700, 538))
{
    // Init handlers.
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxICOHandler);

    // Prepare directories.
    ConfigureDirectory();

    // WX.
    mainFramePtr = this;
    this->SetSizeHints(wxSize( 300,300 ), wxDefaultSize);

#ifdef __linux__
    wxIcon icon(GetWxAbsPath("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
#elif _WIN32
    wxIcon icon(GetWxAbsPath("Resources\\icon.ico"), wxBITMAP_TYPE_ICO);
#endif
    this->SetIcon(icon);
    this->GetParserOpt();    // Gets configuration from config.ini.
    this->SetUpGUI();

    juliaModeState = false;
    changeJuliaMode = false;
    changeKeybGuide = false;
    colorFrameActive = false;
    introConstActive = false;
    iterDiagActive = false;
    infoFrameActive = false;
    formDiagActive = false;
    pause = false;
    selectedScriptIndex = -1;

    this->UpdateMenu();

    // Set parameters found in the config.ini file.
    if(opt.juliaMode) this->UpdateJuliaMode();
    if(opt.colorPaletteWindow)
    {
        colorFrameActive = true;
        pal = new ColorFrame(&colorFrameActive, fractalCanvas->GetFractalPtr(), this);
        pal->Show(true);
    }
    if(opt.constantWindow)
    {
        diag = new ConstDialog(&introConstActive, fractalCanvas->GetFractalPtr(), this);
        diag->Show(true);
        introConstActive = true;
    }
    if(!opt.colorSet) fractalCanvas->GetFractalPtr()->SetFractalSetColorMode(false);

    if (opt.firstUse)
        this->ShowFirstUseDialog();

    if(fractalType != FractalType::Mandelbrot && fractalType != FractalType::Manowar)
        juliaMode->Enable(false);

    this->GetScriptFractals();
    this->ConnectEvents();
}
void MainFrame::ShowFirstUseDialog()
{
#ifdef __linux__
    HTMLViewer* diag = new HTMLViewer(GetWxAbsPath("Resources/Tutorials/mainTut.html"), this, wxID_ANY,
        wxString(wxT(menuWelcomeTxt)), wxDefaultPosition, wxSize(550, 400));
#elif _WIN32
    HTMLViewer* diag = new HTMLViewer(GetWxAbsPath("Resources\\Tutorials\\mainTut.html"), this, wxID_ANY,
        wxString(wxT(menuWelcomeTxt)), wxDefaultPosition, wxSize(550, 400));
#endif

    diag->Show(true);
    fractalCanvas->ShowHelpImage();
}
void MainFrame::ConnectEvents()
{
    this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnClose ) );
    this->Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnQuit));
    this->Connect(wxEVT_SIZE, wxSizeEventHandler(MainFrame::OnResize));
    this->Connect(ID_JULIA_MODE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnJuliaMode));
    this->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::JuliaHandle));
    this->Connect(ID_WELCOME_DIALOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnWelcomeDialog));
    this->Connect(ID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAbout));
    this->Connect(ID_KEYBGUIDE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnKeybGuide));
    this->Connect(ID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSave));
    this->Connect(ID_PALETA, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnPalette));
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
    this->Connect(ID_LOGISTIC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeLogistic));
    this->Connect(ID_HENON_MAP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeHenonMap));
    this->Connect(ID_SIERP_TRIANGLE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeSierpTriangle));
    this->Connect(ID_DPENDULUM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeDPendulum));
    this->Connect(ID_USER_DEFINED, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeUserDefined));
    this->Connect(ID_FPUSER_DEFINED, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeFPUserDefined));
    this->Connect(ID_PAUSE_CONTINUE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnPauseContinue));
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
#ifdef __linux__
    this->Connect(ID_OPEN_SCRIPT_FOLDER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnOpenScriptFolder));
#endif
}

void MainFrame::SetUpGUI()
{
    // Init menu.
    menubar = new wxMenuBar();
    fileMenu = new wxMenu();
    fractalMenu = new wxMenu();
    iterationsMenu = new wxMenu();
    toolMenu = new wxMenu();
    colorMenu = new wxMenu();
    helpMenu = new wxMenu();
    formula = new wxMenu();
    pal = NULL;

    // Formulas.
    wxMenuItem* mandelbrot, * mandelbrotZN, * julia, * juliaZN, * newton, * sinoidal, * magnet;
    wxMenuItem* medusa, * manowar, * manowarJulia, * sierpinskyTriangle, * fixedPoint1, * fixedPoint2;
    wxMenuItem* fixedPoint3, * fixedPoint4, * logisticMap, * userDefined, * fpUserDefined;
    wxMenuItem* tricorn, * burningShip, * burningShipJulia, * fractory, * cell, * dPendulum;
    wxMenuItem* henonMap;

#ifdef _WIN32
#define menuSeparator wxT('\t')
#elif __linux__
#define menuSeparator wxT("    ")
#endif

    mandelbrot = new wxMenuItem(formula, ID_MANDELBROT, wxString(wxT(menuMandelbrotTxt)) + menuSeparator + wxT("z = z^2 + c"), wxEmptyString, wxITEM_NORMAL);
    mandelbrotZN = new wxMenuItem(formula, ID_MANDELBROT_ZN, wxString(wxT(menuMandelbrotTxt)) + menuSeparator + wxT("z = z^n + c"), wxEmptyString, wxITEM_NORMAL);
    julia = new wxMenuItem(formula, ID_JULIA, wxString(wxT(menuJuliaTxt)) + menuSeparator + wxT("z = z^2 + k"), wxEmptyString, wxITEM_NORMAL);
    juliaZN = new wxMenuItem(formula, ID_JULIA_ZN, wxString(wxT(menuJuliaTxt)) + menuSeparator + wxT("z = z^n + k"), wxEmptyString, wxITEM_NORMAL);
    newton = new wxMenuItem(formula, ID_NEWTON, wxString(wxT(menuNewtonTxt)) + menuSeparator + wxT("z^3 - 1 = 0"), wxEmptyString, wxITEM_NORMAL);
    sinoidal = new wxMenuItem(formula, ID_SINOIDAL, wxString(wxT(menuSineTxt)) + menuSeparator + wxT("Z = c*Sin(Z)"), wxEmptyString, wxITEM_NORMAL);
    magnet = new wxMenuItem(formula, ID_MAGNET, wxString(wxT(menuMagnetTxt)), wxEmptyString, wxITEM_NORMAL);
    medusa = new wxMenuItem(formula, ID_MEDUSA, wxString(wxT(menuJellyfishTxt)), wxEmptyString, wxITEM_NORMAL);
    manowar = new wxMenuItem(formula, ID_MANOWAR, wxString(wxT(menuManowarTxt)), wxEmptyString, wxITEM_NORMAL);
    manowarJulia = new wxMenuItem(formula, ID_MANOWAR_JULIA, wxString(wxT(menuManowarJuliaTxt)), wxEmptyString, wxITEM_NORMAL);
    sierpinskyTriangle = new wxMenuItem(formula, ID_SIERP_TRIANGLE, wxString(wxT(menuSierpinskiTxt)), wxEmptyString, wxITEM_NORMAL);
    fixedPoint1 = new wxMenuItem(formula, ID_FIXEDPOINT1, wxString(wxT(menuFixedPointTxt)) + menuSeparator + wxT("z = sin(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint2 = new wxMenuItem(formula, ID_FIXEDPOINT2, wxString(wxT(menuFixedPointTxt)) + menuSeparator + wxT("z = cos(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint3 = new wxMenuItem(formula, ID_FIXEDPOINT3, wxString(wxT(menuFixedPointTxt)) + menuSeparator + wxT("z = tan(z)"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint4 = new wxMenuItem(formula, ID_FIXEDPOINT4, wxString(wxT(menuFixedPointTxt)) + menuSeparator + wxT("z = z^2"), wxEmptyString, wxITEM_NORMAL);
    tricorn = new wxMenuItem(formula, ID_TRICORN, wxString(wxT(menuTricornTxt)), wxEmptyString, wxITEM_NORMAL);
    burningShip = new wxMenuItem(formula, ID_BURNING_SHIP, wxString(wxT(menuBurningShipTxt)), wxEmptyString, wxITEM_NORMAL);
    burningShipJulia = new wxMenuItem(formula, ID_BURNING_SHIP_JULIA, wxString(wxT(menuBurningShipJuliaTxt)), wxEmptyString, wxITEM_NORMAL);
    fractory = new wxMenuItem(formula, ID_FRACTORY, wxString(wxT(menuFractoryTxt)), wxEmptyString, wxITEM_NORMAL);
    cell = new wxMenuItem(formula, ID_CELL, wxString(wxT(menuCellTxt)), wxEmptyString, wxITEM_NORMAL);
    logisticMap = new wxMenuItem(formula, ID_LOGISTIC, wxString(wxT(menuLogisticTxt)), wxEmptyString, wxITEM_NORMAL);
    henonMap = new wxMenuItem(formula, ID_HENON_MAP, wxString(wxT(menuHenonMapTxt)), wxEmptyString, wxITEM_NORMAL);
    dPendulum = new wxMenuItem(formula, ID_DPENDULUM, wxString(wxT(menuDoublePendulumTxt)), wxEmptyString, wxITEM_NORMAL);
    userDefined = new wxMenuItem(formula, ID_USER_DEFINED, wxString(wxT(menuUserFormComplexTxt)), wxEmptyString, wxITEM_NORMAL);
    fpUserDefined = new wxMenuItem(formula, ID_FPUSER_DEFINED, wxString(wxT(menuUserFormFixedTxt)), wxEmptyString, wxITEM_NORMAL);

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
    typeOther->Append(logisticMap);
    typeOther->Append(henonMap);

    formula->Append(-1, wxT(menuComplexTxt), typeComplex);
    formula->Append(-1, wxT(menuNumMetTxt), typeNumMet);
    formula->Append(-1, wxT(menuPhysicTxt), typePhysics);
    formula->Append(-1, wxT(menuOtherTxt), typeOther);
    formula->Append(userDefined);
    formula->Append(fpUserDefined);
    fractalMenu->Append(wxID_ANY, wxT(menuFormulaTxt), formula);

    // Julia constant.
    introConstant = new wxMenu();
    manual = new wxMenuItem(introConstant, ID_ENTER_MAN_CONSTANT, wxString(wxT(menuManualConstTxt)), wxEmptyString, wxITEM_NORMAL);
    introConstant->Append(manual);
    manual->Enable(false);

    statusData.slider = slider = new wxMenuItem(introConstant, ID_ENTER_SLD_CONSTANT, wxString(wxT(menuSliderTxt)) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_CHECK);
    introConstant->Append(slider);
    slider->Enable(false);
    slider->Check(false);
    fractalMenu->Append(-1, wxT(menuEnterConstTxt), introConstant);

    // Julia constant and show orbit.
    juliaMode = new wxMenuItem(fractalMenu, ID_JULIA_MODE, wxString(wxT(menuJuliaModeTxt)), wxEmptyString, wxITEM_CHECK);
    statusData.showOrbit = showOrbit = new wxMenuItem(fractalMenu, ID_SHOW_ORBIT, wxString(wxT(menuShowOrbitTxt)) + wxT('\t') + wxT("F2"), wxEmptyString, wxITEM_CHECK);

    fractalMenu->Append(juliaMode);

    fractalMenu->Append(showOrbit);
    juliaMode->Check(false);
    showOrbit->Check(false);

    // File menu.
    fileMenu->Append(ID_SAVE, wxString(wxT(menuSaveImageTxt)) + wxT('\t') + wxT("F4"));
    fileMenu->Append(wxID_EXIT, wxT(menuQuitTxt));

    // Tools menu.

    // Iteracions.
    itManual = new wxMenuItem(iterationsMenu, ID_IT_MANUAL, wxString(wxT(menuManualIterTxt)), wxEmptyString, wxITEM_NORMAL);
    iterationsMenu->Append(itManual);
    MoreIter = new wxMenuItem(iterationsMenu, ID_INCREASE_IT, wxString(wxT(menuIncIterTxt)) + wxT('\t') + wxT("L"), wxEmptyString, wxITEM_NORMAL);
    iterationsMenu->Append(MoreIter);

    lessIter = new wxMenuItem(iterationsMenu, ID_DECREASE_IT, wxString(wxT(menuDecIterTxt)) + wxT('\t') + wxT("K"), wxEmptyString, wxITEM_NORMAL);
    iterationsMenu->Append(lessIter);

    // Fractal menu.
    fractOptItem = new wxMenuItem(fractalMenu, ID_OPTPANEL, wxString(wxT(menuFractalOptTxt)), wxEmptyString, wxITEM_CHECK);    // Txt: "Fractal options"
    fractalMenu->Append(fractOptItem);
    fractalMenu->Append(ID_FORMULA_DIALOG, wxT(menuEnterUserFormTxt)); // Txt: "Enter user formula"
    wxMenuItem* separador;
    separador = fractalMenu->AppendSeparator();

    pauseBtn.pauseContinue = fractalMenu->Append(ID_PAUSE_CONTINUE, wxString(wxT(menuPauseTxt)) + wxT('\t') + wxT("P"));    // Txt: Pause
    pauseBtn.state = false;
    fractalMenu->Append(ID_REDRAW, wxString(wxT(menuRedrawTxt)) + wxT('\t') + wxT("F5"));
    fractalMenu->Append(ID_RESET, wxString(wxT(menuResetTxt)));

    colorMenu->Append(ID_PALETA, wxT(menuColorOptTxt));

    // Help menu.
    helpMenu->Append(ID_USER_MANUAL, wxT(menuUserManTxt));
#ifdef __linux__
    helpMenu->Append(ID_OPEN_SCRIPT_FOLDER, wxT(menuOpenScriptFolder));
#endif
    keyboardGuide = new wxMenuItem(helpMenu, ID_KEYBGUIDE, wxString(wxT(menuKeybGuideTxt)), wxEmptyString, wxITEM_CHECK);
    helpMenu->Append(keyboardGuide);
    helpMenu->Append(ID_WELCOME_DIALOG, wxT(welcomeDialogTxt));
    helpMenu->Append(ID_ABOUT, wxT(menuAboutTxt));


    menubar->Append(fileMenu, wxT(menuFileTxt));
    menubar->Append(fractalMenu, wxT(menuFractalTxt));
    menubar->Append(iterationsMenu, wxT(menuIterationsTxt));
    menubar->Append(colorMenu, wxT(menuColorTxt));
    menubar->Append(toolMenu, wxT(menuToolsTxt));
    menubar->Append(helpMenu, wxT(menuHelpTxt));
    this->SetMenuBar(menubar);

    sizer = new wxBoxSizer(wxHORIZONTAL);

    fractalSizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(fractalSizer, 7, wxEXPAND, 5);

    wxBoxSizer* panelSizer;
    panelSizer = new wxBoxSizer(wxVERTICAL);

    // Option panel.
    optionPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    optionPanel->SetScrollRate(5, 5);
    optionPanel->Hide();
    showOptPanel = false;
    optionSizer = new wxBoxSizer(wxVERTICAL);

    propBitmap = new wxStaticBitmap(optionPanel, wxID_ANY, wxBitmap(GetWxAbsPath("Resources/prop.png"), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
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
    colorStyle = opt.colorStyleGaussian;
    fractalType = opt.type;
    fractalCanvas = new FractalCanvas(statusData, &pauseBtn, fractalType, this, wxID_ANY, wxPoint(0, 0), size, wxBORDER_NONE);

    if (opt.mode == ColorMode::Gaussian)
        fractalCanvas->GetFractalPtr()->SetGaussianColorStyle(colorStyle);
    else
    {
        wxGradient grad;
        grad.setMin(0);
        grad.setMax(opt.paletteSize);
        grad.fromString(wxString(opt.colorStyleGrad.c_str(), wxConvUTF8));
        fractalCanvas->GetFractalPtr()->SetGradient(grad);
    }
    fractalCanvas->GetFractalPtr()->ChangeIterations(opt.maxIterations);
    fractalCanvas->GetFractalPtr()->SetExtColorMode(opt.colorFractal);
    fractalCanvas->GetFractalPtr()->SetFractalSetColorMode(opt.colorSet);

    fractalSizer->Add(fractalCanvas, 1, wxEXPAND | wxALL, 0);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    this->CloseAll();
    this->Destroy();
}
void MainFrame::OnQuit(wxCommandEvent &event)
{
    Close(true);
}
void MainFrame::CloseAll()
{
    if(juliaModeState)
    {
        juliaMode->Check(false);
#ifdef _WIN32
        ptr->Terminate();
        delete ptr;
#endif
        juliaModeState = false;
        changeJuliaMode = false;
        fractalCanvas->SetJuliaMode(false);
    }
    delete fractalCanvas;
}
void MainFrame::OnResize(wxSizeEvent &event)
{
    this->Layout();
}
void MainFrame::OnJuliaMode(wxCommandEvent &event)
{
    // Opens the Julia mode.
    this->UpdateJuliaMode();
}
void MainFrame::JuliaHandle(wxUpdateUIEvent &event)
{
    // Waits until the Julia window is closed to delete it.
    if(changeJuliaMode && !juliaModeState)
    {
        juliaMode->Check(false);
        ptr->Close();
        ptr->Wait();

        delete ptr;
        juliaModeState = false;
        changeJuliaMode = false;
        fractalCanvas->SetJuliaMode(false);
    }
}
void MainFrame::OnWelcomeDialog(wxCommandEvent& event)
{
    this->ShowFirstUseDialog();
}
void MainFrame::OnAbout(wxCommandEvent &event)
{
    // About dialog.
    AboutDialog *dlg = new AboutDialog(this);
    dlg->SetAppName(wxT("wxChaos"));
    dlg->SetVersion(wxString::FromUTF8(APP_VERSION));
    dlg->SetCopyright(wxString::Format(wxT("%s"),
        wxT(menuAboutLegendTxt)));
    dlg->SetCustomBuildInfo(wxString::Format(wxT("%s. %s"),
        AboutDialog::GetBuildInfo(AboutDialog::wxBUILDINFO_LONG).GetData(),
        wxT("")));
    dlg->SetHeaderBitmap(wxBitmap(GetWxAbsPath("Resources/wxChaosAbout.bmp"), wxBITMAP_TYPE_ANY));
    dlg->ApplyInfo();
    dlg->ShowModal();
    dlg->Destroy();
}
void MainFrame::OnSave(wxCommandEvent &event)
{
    // Saves the fractal image.
    wxFileDialog* saveFileDialog = new wxFileDialog(this, wxT(menuSelectFileTxt), wxT(""),
                                    wxT("fractal.png"), wxT("PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp"), wxFD_SAVE);
    wxString fileName;
    if(saveFileDialog->ShowModal() == wxID_OK)
    {
        fileName = saveFileDialog->GetPath();
        int ext = saveFileDialog->GetFilterIndex();
        string path = string(fileName.mb_str());
        SizeDialogSave *diag;

        if(fractalType == FractalType::ScriptFractal)
            diag = new SizeDialogSave(fractalCanvas, path, ext, fractalType, fractalCanvas->GetFractalPtr(), this, scriptDataVect[selectedScriptIndex].file);
        else
            diag = new SizeDialogSave(fractalCanvas, path, ext, fractalType, fractalCanvas->GetFractalPtr(), this);
        diag->Show(true);
    }
    saveFileDialog->Destroy();
}
void MainFrame::OnPalette(wxCommandEvent &event)
{
    // Color palette frame.
    if(!colorFrameActive)
    {
        colorFrameActive = true;
        pal = new ColorFrame(&colorFrameActive, fractalCanvas->GetFractalPtr(), this);
        pal->Show(true);

        // Adjust position.
        int h, w;
        GetDesktopResolution(h, w);
        if(this->GetPosition().x+this->GetSize().GetWidth()+5 < w && this->GetPosition().y < h)
            pal->Move(this->GetPosition().x+this->GetSize().GetWidth()+5, this->GetPosition().y);
    }
    else pal->SetFocus();
}
void MainFrame::OnFormulaDialog(wxCommandEvent &event)
{
    // User formula menu.
    if(!formDiagActive)
    {
        formDiagActive = true;
        formDialog = new FormulaDialog(ID_USER_DEFINED, ID_FPUSER_DEFINED, &colorStyle, slider, manual, &formDiagActive, fractalCanvas, this);
        formDialog->Show(true);

        // Adjust position.
        int h, w;
        GetDesktopResolution(h, w);
        if(this->GetPosition().x+this->GetSize().GetWidth()+5 < w && this->GetPosition().y < h)
            formDialog->Move(this->GetPosition().x+this->GetSize().GetWidth()+5, this->GetPosition().y);
        fractalType = FractalType::UserDefined;
    }
    else formDialog->SetFocus();
}
void MainFrame::OnRedraw(wxCommandEvent &event)
{
    if(pauseBtn.state)
    {
        pauseBtn.state = false;
        if(fractalType == FractalType::ScriptFractal)
            pauseBtn.pauseContinue->SetItemLabel(wxT(menuAbortTxt));
        else
            pauseBtn.pauseContinue->SetItemLabel(wxT(menuPauseTxt));
        fractalCanvas->GetFractalPtr()->DeleteSavedZooms();
    }
    fractalCanvas->GetFractalPtr()->Redraw();
}
void MainFrame::OnReset(wxCommandEvent &event)
{
    fractalCanvas->Reset();
    if(fractalCanvas->GetFractalPtr()->GetColorMode() == ColorMode::Gradient)
    {
        wxGradient grad;
        grad.fromString(wxString(opt.colorStyleGrad.c_str(), wxConvUTF8));
        grad.setMin(0);
        grad.setMax(opt.paletteSize);
        fractalCanvas->GetFractalPtr()->SetGradient(grad);
    }
    else
        fractalCanvas->GetFractalPtr()->SetGaussianColorStyle(colorStyle);
    this->UpdateMenu();
}
void MainFrame::OnMoreIt(wxCommandEvent &event)
{
    fractalCanvas->GetFractalPtr()->MoreIter();
}
void MainFrame::OnLessIt(wxCommandEvent &event)
{
    fractalCanvas->GetFractalPtr()->LessIter();
}
void MainFrame::OnShowOrbit(wxCommandEvent &event)
{
    bool modo = showOrbit->IsChecked();
    fractalCanvas->SetOrbitMode(modo);
    showOrbit->Check(modo);
}
void MainFrame::OnManIntroConst(wxCommandEvent &event)
{
    // Manual constant.
    if(!introConstActive)
    {
        diag = new ConstDialog(&introConstActive, fractalCanvas->GetFractalPtr(), this);
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
void MainFrame::OnSldIntroConst(wxCommandEvent &event)
{
    // Slider constant.
    bool modo = slider->IsChecked();
    fractalCanvas->SetSliderMode(modo);
    slider->Check(modo);
}
void MainFrame::OnKeybGuide(wxCommandEvent &event)
{
    // Keyboard guide.
    changeKeybGuide = !changeKeybGuide;
    fractalCanvas->SetKeybGuide(changeKeybGuide);
    keyboardGuide->Check(changeKeybGuide);
}
void MainFrame::OnItManual(wxCommandEvent &event)
{
    // Manual iterations.
    if(!iterDiagActive)
    {
        iterDiag = new IterDialog(&iterDiagActive, fractalCanvas->GetFractalPtr(), this);
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
void MainFrame::OnPauseContinue(wxCommandEvent &event)
{
    // Pauses the rendering.
    if(pauseBtn.state)
    {
        if(fractalType == FractalType::ScriptFractal)
            pauseBtn.pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));
        else
            pauseBtn.pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));
        pauseBtn.state = false;
    }
    else
    {
        if(fractalType == FractalType::ScriptFractal)
            pauseBtn.pauseContinue->SetItemLabel(wxString(wxT(menuRelaunchTxt))+ wxT('\t') + wxT("P"));
        else
            pauseBtn.pauseContinue->SetItemLabel(wxString(wxT(menuContinueTxt))+ wxT('\t') + wxT("P"));
        pauseBtn.state = true;
    }
    fractalCanvas->GetFractalPtr()->PauseContinue();
}
void MainFrame::OnFractalOptions(wxCommandEvent &event)
{
    // Ajust the panel.
    if(!showOptPanel)
    {
        fractOptItem->Check(true);
        optionPanel->Show();
        wxSize windowSize = this->GetSize();
        if(!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()+175, windowSize.GetHeight());
        this->GetSizer()->Layout();
        showOptPanel = true;
    }
    else
    {
        fractOptItem->Check(false);
        optionPanel->Hide();
        wxSize windowSize = this->GetSize();
        if(!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()-175, windowSize.GetHeight());
        this->GetSizer()->Layout();
        showOptPanel = false;
    }
}
void MainFrame::OnApplyPanelOpt(wxCommandEvent& event)
{
    // Pass parameters to the fractal and redraws it.
    PanelOptions* pOptions = fractalCanvas->GetFractalPtr()->GetOptPanel();
    for(unsigned int i=0; i<foundTextControls.size(); i++)
    {
        *pOptions->GetDoubleElement(i) = string_to_double(textControls[i]->GetValue());
    }
    for(unsigned int i=0; i<foundSpinControls.size(); i++)
    {
        *pOptions->GetIntElement(i) = spinControls[i]->GetValue();
    }
    for(unsigned int i=0; i<foundCheckBoxes.size(); i++)
    {
        if(checkBoxes[i]->GetValue())
            *pOptions->GetBoolElement(i) = true;
        else
            *pOptions->GetBoolElement(i) = false;
    }
    if(pauseBtn.state)
    {
        pauseBtn.state = false;
        if(fractalType == FractalType::ScriptFractal)
            pauseBtn.pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));
        else
            pauseBtn.pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));
        fractalCanvas->GetFractalPtr()->DeleteSavedZooms();
    }
    fractalCanvas->SetFocus();
    fractalCanvas->GetFractalPtr()->Redraw();
}
void MainFrame::OnOpenScriptFolder(wxCommandEvent& event)
{
#ifdef __linux__
    string fileViewer, filePath;
    string desktop = exec("ps -o pid= -C plasma-desktop > /dev/null && echo \"kde running\" || echo \"kde not running\" ");

    if(desktop == "kde running")
    {
        fileViewer = filePath = "dolphin ";
    }
    else
    {
        fileViewer = filePath = "nautilus ";
    }
    filePath += GetAbsPath("UserScripts");

    if(system(filePath.c_str()))
    {
        wxString error = wxT("Error: ");
         error += wxString(fileViewer.c_str(), wxConvUTF8);
        error += wxT(menuUsrManualError);
        wxLogError(error);
    }
#endif
}
void MainFrame::OnUserManual(wxCommandEvent &event)
{
    // Open pdf.
#ifdef _WIN32
    string filePath = GetWorkingDirectory(FORCE_APP_DIRECT);
    filePath += "/Doc/User_Manual.pdf";

    #ifdef UNICODE
        ShellExecute( NULL, _T("open"), std::wstring(filePath.begin(), filePath.end()).c_str(), _T(""), _T("C:\\"), SW_SHOWNORMAL);
    #else
        ShellExecute( NULL, "open", filePath.c_str(), "", "C:\\", SW_SHOWNORMAL);
    #endif
#elif __linux__
    string filePath, pdfViewer;
    string desktop = exec("ps -o pid= -C plasma-desktop > /dev/null && echo \"kde running\" || echo \"kde not running\" ");

    if(desktop == "kde running")
    {
        pdfViewer = filePath = "okular ";
    }
    else
    {
        pdfViewer = filePath = "evince ";
    }

    filePath += GetAbsPath("Doc/User_Manual.pdf");
    if(system(filePath.c_str()))
    {
        wxString error = wxT("Error: ");
        error += wxString(pdfViewer.c_str(), wxConvUTF8);
        error += wxT(menuUsrManualError);
        wxLogError(error);
    }
#endif
}


// Changes the fractal type.
void MainFrame::ChangeMandelbrot(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Mandelbrot, true);
}
void MainFrame::ChangeMandelbrotZN(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::MandelbrotZN, true);
}
void MainFrame::ChangeJulia(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Julia, false);
}
void MainFrame::ChangeJuliaZN(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::JuliaZN, false);
}
void MainFrame::ChangeNewton(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::NewtonRaphsonMethod, false);
}
void MainFrame::ChangeSinoidal(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Sinoidal, false);
}
void MainFrame::ChangeMagnet(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Magnetic, false);
}
void MainFrame::ChangeMedusa(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Medusa, false);
}
void MainFrame::ChangeManowar(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Manowar, true);
}
void MainFrame::ChangeManowarJulia(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::ManowarJulia, false);
}
void MainFrame::ChangeSierpTriangle(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::SierpinskyTriangle, false);
}
void MainFrame::ChangeFixedPoint1(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::FixedPoint1, false);
}
void MainFrame::ChangeFixedPoint2(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::FixedPoint2, false);
}
void MainFrame::ChangeFixedPoint3(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::FixedPoint3, false);
}
void MainFrame::ChangeFixedPoint4(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::FixedPoint4, false);
}
void MainFrame::ChangeTricorn(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Tricorn, false);
}
void MainFrame::ChangeBurningShip(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::BurningShip, true);
}
void MainFrame::ChangeBurningShipJulia(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::BurningShipJulia, false);
}
void MainFrame::ChangeFractory(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Fractory, false);
}
void MainFrame::ChangeCell(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Cell, false);
}
void MainFrame::ChangeLogistic(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::Logistic, false);
}
void MainFrame::ChangeHenonMap(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::HenonMap, false);
}
void MainFrame::ChangeDPendulum(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::DoublePendulum, false);
}
void MainFrame::ChangeUserDefined(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::UserDefined, false);
}
void MainFrame::ChangeFPUserDefined(wxCommandEvent &event)
{
    this->ChangeFractal(FractalType::FixedPointUserDefined, false);
}
void MainFrame::ChangeFractal(FractalType fType, bool enableJulia)
{
    selectedScriptIndex = -1;  // Deselect the script fractal.
    if(fractalType != fType || fractalType == FractalType::UserDefined || fractalType == FractalType::FixedPointUserDefined)
    {
        Options fractOpt = fractalCanvas->GetFractalPtr()->GetOptions();
        fractalCanvas->ChangeType(fType);

        if(fType == FractalType::Mandelbrot || fType == FractalType::Julia) // This is because they have smooth coloring.
        {
            if(opt.mode == ColorMode::Gradient)
                fractalCanvas->GetFractalPtr()->SetGradient(fractOpt.gradient);
            else
                fractalCanvas->GetFractalPtr()->SetGaussianColorStyle(colorStyle);
        }
        else fractalCanvas->GetFractalPtr()->SetGaussianColorStyle(colorStyle);

        fractalType = fType;
        this->UpdateMenu();
        juliaMode->Enable(enableJulia);
    }
}
void MainFrame::ChangeScriptItem(wxCommandEvent &event)
{
    unsigned int id = static_cast<unsigned int>(event.GetId() - SCRIPT_ID_INDEX);
    selectedScriptIndex = id;
    if(fractalCanvas->GetFractalPtr()->GetWatchdog()->ThreadRunning()) fractalCanvas->GetFractalPtr()->PauseContinue();
    Options fractOpt = fractalCanvas->GetFractalPtr()->GetOptions();
    fractalCanvas->ChangeToScript(scriptDataVect[id]);
    fractalCanvas->GetFractalPtr()->SetGaussianColorStyle(colorStyle);

    fractalType = FractalType::ScriptFractal;
    this->UpdateMenu();
    juliaMode->Enable(false);
}

// Methods to adjust the menu.
void MainFrame::GetParserOpt()
{
    // Gets options from the parser which reads "config.ini" file.
    string dir = GetWorkingDirectory();
    dir += "/config.ini";
    ConfigParser p(dir);
    vector<string> fractalOpt;
    vector<FractalType> fractalValues;

    if(p.FileOpened())
    {
        // Parameters to parse.
        vector<string> colorOpt;
        colorOpt.push_back("Gradient");
        colorOpt.push_back("EST_Mode");
        vector<ColorMode> colorValues;
        colorValues.push_back(ColorMode::Gradient);
        colorValues.push_back(ColorMode::Gaussian);

        fractalOpt.push_back("Mandelbrot");
        fractalOpt.push_back("MandelbrotZN");
        fractalOpt.push_back("Julia");
        fractalOpt.push_back("JuliaZN");
        fractalOpt.push_back("Newton");
        fractalOpt.push_back("Sinoidal");
        fractalOpt.push_back("Magnet");
        fractalOpt.push_back("Medusa");
        fractalOpt.push_back("Manowar");
        fractalOpt.push_back("JManowar");
        fractalOpt.push_back("Sierp_Triangle");
        fractalOpt.push_back("FixedPoint1");
        fractalOpt.push_back("FixedPoint2");
        fractalOpt.push_back("FixedPoint3");
        fractalOpt.push_back("FixedPoint4");
        fractalOpt.push_back("Tricorn");
        fractalOpt.push_back("Burning_Ship");
        fractalOpt.push_back("Burning_Ship_Julia");
        fractalOpt.push_back("Fractory");
        fractalOpt.push_back("Cell");
        fractalOpt.push_back("Logistic");
        fractalOpt.push_back("Henon_Map");
        fractalOpt.push_back("Double_Pendulum");
        fractalOpt.push_back("User_Defined");
        fractalOpt.push_back("FPUser_Defined");
        fractalValues.push_back(FractalType::Mandelbrot);
        fractalValues.push_back(FractalType::MandelbrotZN);
        fractalValues.push_back(FractalType::Julia);
        fractalValues.push_back(FractalType::JuliaZN);
        fractalValues.push_back(FractalType::NewtonRaphsonMethod);
        fractalValues.push_back(FractalType::Sinoidal);
        fractalValues.push_back(FractalType::Magnetic);
        fractalValues.push_back(FractalType::Medusa);
        fractalValues.push_back(FractalType::Manowar);
        fractalValues.push_back(FractalType::ManowarJulia);
        fractalValues.push_back(FractalType::SierpinskyTriangle);
        fractalValues.push_back(FractalType::FixedPoint1);
        fractalValues.push_back(FractalType::FixedPoint2);
        fractalValues.push_back(FractalType::FixedPoint3);
        fractalValues.push_back(FractalType::FixedPoint4);
        fractalValues.push_back(FractalType::Tricorn);
        fractalValues.push_back(FractalType::BurningShip);
        fractalValues.push_back(FractalType::BurningShipJulia);
        fractalValues.push_back(FractalType::Fractory);
        fractalValues.push_back(FractalType::Cell);
        fractalValues.push_back(FractalType::Logistic);
        fractalValues.push_back(FractalType::HenonMap);
        fractalValues.push_back(FractalType::DoublePendulum);
        fractalValues.push_back(FractalType::UserDefined);
        fractalValues.push_back(FractalType::FixedPointUserDefined);

        vector<string> styleOpt;
        styleOpt.push_back("Summer_Day");
        styleOpt.push_back("Cool_Blue");
        styleOpt.push_back("Hard_Red");
        styleOpt.push_back("Black_And_White");
        styleOpt.push_back("Pastel");
        styleOpt.push_back("Psych_Experience");
        styleOpt.push_back("Vivid_Colors");
        vector<GaussianColorStyles> styleValues;
        styleValues.push_back(SUMMER_DAY);
        styleValues.push_back(COOL_BLUE);
        styleValues.push_back(HARD_RED);
        styleValues.push_back(BLACK_AND_WHITE);
        styleValues.push_back(PASTEL);
        styleValues.push_back(PSYCH_EXPERIENCE);
        styleValues.push_back(VIVID_COLORS);

        p.OptionToVar<ColorMode>(opt.mode, "COLOR_TYPE", colorOpt, colorValues, ColorMode::Gaussian);
        p.OptionToVar<FractalType>(opt.type, "FRACTAL_TYPE", fractalOpt, fractalValues, FractalType::Mandelbrot);
        p.IntArgToVar(opt.maxIterations, "DEFAULT_ITERATION", 100);
        if(opt.mode == ColorMode::Gradient)
        {
            p.StringArgToVar(opt.colorStyleGrad, "COLOR_STYLE",    "rgb(0,0,0);rgb(255,255,255);");
            opt.colorStyleGaussian = SUMMER_DAY;
        }
        else
        {
            p.OptionToVar(opt.colorStyleGaussian, "COLOR_STYLE", styleOpt, styleValues, SUMMER_DAY);
            opt.colorStyleGrad = "rgb(0,0,0);rgb(255,255,255);";
        }

        p.IntArgToVar(opt.paletteSize, "PALETTE_SIZE", 300);
        p.BoolArgToVar(opt.constantWindow, "CONSTANT_WINDOW", false);
        p.BoolArgToVar(opt.commandConsole, "COMMAND_CONSOLE", false);
        p.BoolArgToVar(opt.juliaMode, "JULIA_MODE", false);
        p.BoolArgToVar(opt.colorPaletteWindow, "COLOR_PALETTE_WINDOW", false);
        p.BoolArgToVar(opt.colorFractal, "COLOR_FRACTAL", false);
        p.BoolArgToVar(opt.colorSet, "COLOR_SET", false);
        p.BoolArgToVar(opt.firstUse, "FIRST_USE", false);

        p.ReplaceArg("FIRST_USE", "False");
    }
    else
    {
        // Writes a file with default values.
        ofstream file;
        file.open(dir.c_str());
        file << "//Fractal options\nCOLOR_TYPE=Gradient\nPALETTE_SIZE=300\nCOLOR_STYLE=";
        file << "rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);\n";
        file << "FRACTAL_TYPE=Mandelbrot\nDEFAULT_ITERATION=100\nCONSTANT_WINDOW=False\nJULIA_MODE=False\n";
        file << "COMMAND_CONSOLE=False\nFIRST_USE=False\n\n//Color options\nCOLOR_PALETTE_WINDOW=False\nCOLOR_FRACTAL=True\n";
        file << "COLOR_SET=True\n";
        file << "APP_VERSION=" << APP_VERSION << "\n";
        file.close();

        opt.mode = ColorMode::Gradient;
        opt.type = FractalType::Mandelbrot;
        opt.maxIterations = 100;
        opt.paletteSize = 300;
        opt.colorStyleGrad = "rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);\n";
        opt.colorStyleGaussian = SUMMER_DAY;
        opt.constantWindow = false;
        opt.commandConsole = false;
        opt.juliaMode = false;
        opt.colorPaletteWindow = false;
        opt.colorFractal = true;
        opt.colorSet = true;
        opt.firstUse = false;
    }
}
void MainFrame::UpdateOptPanel()
{
    PanelOptions* pOptions = fractalCanvas->GetFractalPtr()->GetOptPanel();
    int index, labelIndex;

    // If there are elements in pOptions creates panel.
    if(pOptions->GetElementsSize() > 0)
    {
        fractOptItem->Enable(true);
        if(labels.size() != 0 || textControls.size() != 0 || spinControls.size() != 0 || checkBoxes.size() != 0)
        {
            // If there are elements from a previous panel deletes them.
            this->DeleteOptPanel();
        }

        if(pOptions->GetForceShow())
        {
            fractOptItem->Check(true);
            optionPanel->Show();
            wxSize windowSize = this->GetSize();
            if(!this->IsMaximized())
                this->SetSize(windowSize.GetWidth()+175, windowSize.GetHeight());
            this->GetSizer()->Layout();
            showOptPanel = true;
        }
        else
        {
            fractOptItem->Check(false);
        }

        // Creates elements from each kind.
        for(int i=0; i<pOptions->GetElementsSize(); i++)
        {
            switch(pOptions->GetPanelOptType(i))
            {
            case LABEL:
                {
                    labels.push_back(new wxStaticText( optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap( -1 );
                    optionSizer->Add( labels[labelIndex], 0, wxALL, 5 );
                    foundLabels.push_back(i);
                }
                break;
            case TXTCTRL:
                {
                    labels.push_back(new wxStaticText( optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap( -1 );
                    optionSizer->Add( labels[labelIndex], 0, wxALL, 5 );

                    textControls.push_back(new wxTextCtrl( optionPanel, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    index = textControls.size()-1;
                    optionSizer->Add( textControls[index], 0, wxALL|wxEXPAND, 5 );
                    foundTextControls.push_back(i);
                }
                break;
            case SPIN:
                {
                    labels.push_back(new wxStaticText( optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap( -1 );
                    optionSizer->Add( labels[labelIndex], 0, wxALL, 5 );

                    spinControls.push_back(new wxSpinCtrl( optionPanel, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0 ));
                    index = spinControls.size()-1;
                    optionSizer->Add( spinControls[index], 0, wxALL|wxEXPAND, 5 );
                    foundSpinControls.push_back(i);
                }
                break;
            case CHECKBOX:
                {
                    checkBoxes.push_back(new wxCheckBox( optionPanel, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    index = checkBoxes.size()-1;
                    if(pOptions->GetDefault(i) == wxT("true"))
                        checkBoxes[index]->SetValue(true);
                    else
                        checkBoxes[index]->SetValue(false);

                    optionSizer->Add( checkBoxes[index], 0, wxALL|wxEXPAND, 5 );
                    foundCheckBoxes.push_back(i);
                }
                break;
            };
        }

        // Creates button to apply options.
        panelButton = new wxButton( optionPanel, wxID_ANY, wxT(menuApplyTxt), wxDefaultPosition, wxDefaultSize, 0 );
        optionSizer->Add( panelButton, 0, wxALL, 5 );
        panelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnApplyPanelOpt ), NULL, this );
        optionSizer->Layout();
        optionPanel->SetScrollbars(20, 20, 0, 50);
    }
    else
    {
        fractOptItem->Check(false);
        fractOptItem->Enable(false);
        if(showOptPanel)
            this->DeleteOptPanel();
    }
}
void MainFrame::DeleteOptPanel()
{
    // Deletes panel elements.
    for(unsigned int i=0; i<labels.size(); i++)
        labels[i]->Destroy();

    labels.clear();
    foundLabels.clear();
    for(unsigned int i=0; i<textControls.size(); i++)
        textControls[i]->Destroy();

    textControls.clear();
    foundTextControls.clear();
    for(unsigned int i=0; i<spinControls.size(); i++)
        spinControls[i]->Destroy();

    spinControls.clear();
    foundSpinControls.clear();
    for(unsigned int i=0; i<checkBoxes.size(); i++)
        checkBoxes[i]->Destroy();

    checkBoxes.clear();
    foundCheckBoxes.clear();

    // Erase button, disconnect event and hide panel.
    panelButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnApplyPanelOpt ), NULL, this);
    delete panelButton;
    if(showOptPanel)
    {
        optionPanel->Hide();
        wxSize windowSize = this->GetSize();
        if(!this->IsMaximized())
            this->SetSize(windowSize.GetWidth()-175, windowSize.GetHeight());
        this->GetSizer()->Layout();
        showOptPanel = false;
    }
}
void MainFrame::GetScriptFractals()
{
    // Gets the current directory.
    string path = GetWorkingDirectory();
#ifdef _WIN32
    path += "\\UserScripts";
#elif __linux__
    path += "/UserScripts";
#endif
    FileGetter fg(path.c_str());

    // Fills vector with script files.
    vector<string> filesInFolder;
    char text[FILENAME_MAX];
    while(fg.getNextFile(text))
    {
        if(check_ext(text, "as"))
            filesInFolder.push_back(text);
    }

    // Gets script parameters.
    for(unsigned int i=0; i<filesInFolder.size(); i++)
    {
        int r;
        asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
        if(engine == NULL) break;
        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

        // Configure the script engine.
        RegisterStdString(engine);
        RegisterScriptMathComplex(engine);
        RegisterScriptMathReal(engine);
        RegisterAsFunctions(engine);

        // Register this to avoid compiler complains.
        int intVar;
        double dblVar;
        engine->RegisterGlobalProperty("double minX", &dblVar);
        engine->RegisterGlobalProperty("double maxX", &dblVar);
        engine->RegisterGlobalProperty("double minY", &dblVar);
        engine->RegisterGlobalProperty("double maxY", &dblVar);
        engine->RegisterGlobalProperty("double xFactor", &dblVar);
        engine->RegisterGlobalProperty("double yFactor", &dblVar);
        engine->RegisterGlobalProperty("double kReal", &dblVar);
        engine->RegisterGlobalProperty("double kImaginary", &dblVar);
        engine->RegisterGlobalProperty("int ho", &intVar);
        engine->RegisterGlobalProperty("int hf", &intVar);
        engine->RegisterGlobalProperty("int wo", &intVar);
        engine->RegisterGlobalProperty("int wf", &intVar);
        engine->RegisterGlobalProperty("int maxIter", &intVar);
        engine->RegisterGlobalProperty("int threadIndex", &intVar);
        engine->RegisterGlobalProperty("int screenWidth", &intVar);
        engine->RegisterGlobalProperty("int screenHeight", &intVar);
        engine->RegisterGlobalProperty("int paletteSize", &intVar);

        string filePath = GetWorkingDirectory();
    #ifdef _WIN32
        filePath += "\\UserScripts\\";
    #elif __linux__
        filePath += "/UserScripts/";
    #endif
        filePath += filesInFolder[i];
        r = CompileScriptFromPath(engine, filePath);
        if( r < 0 )
        {
            string errorMsg = "Compile error in file: ";
            errorMsg += filePath;
            // SetConsoleText(errorMsg);
            engine->Release();
            filesInFolder.erase(filesInFolder.begin()+i);
            i -= 1;
            continue;
        }

        asIScriptContext *ctx = engine->CreateContext();
        if( ctx == 0 )
        {
            engine->Release();
            filesInFolder.erase(filesInFolder.begin()+i);
            i -= 1;
            continue;
        }


        // Find the function for the function we want to execute.
        asIScriptFunction *renderFunc = engine->GetModule(0)->GetFunctionByDecl("void SetParams()");
        if( renderFunc == 0 )
        {
            ctx->Release();
            engine->Release();
            filesInFolder.erase(filesInFolder.begin()+i);
            i -= 1;
            continue;
        }

        // Prepare the script context with the function we wish to execute.
        r = ctx->Prepare(renderFunc);
        if( r < 0 )
        {
            ctx->Release();
            engine->Release();
            filesInFolder.erase(filesInFolder.begin()+i);
            i -= 1;
            continue;
        }

        // Execute the function.
        r = ctx->Execute();
        ctx->Release();
        engine->Release();
        PushScriptData(filePath);

        scriptItems.push_back(new wxMenuItem(formula, SCRIPT_ID_INDEX+i, wxString(scriptDataVect[i].name.c_str(), wxConvUTF8) , wxEmptyString, wxITEM_NORMAL));
        if(scriptDataVect[i].cat == CAT_COMPLEX)
            typeComplex->Append(scriptItems[i]);
        else if(scriptDataVect[i].cat == CAT_NUMMET)
            typeNumMet->Append(scriptItems[i]);
        else if(scriptDataVect[i].cat == CAT_PHYSIC)
            typePhysics->Append(scriptItems[i]);
        else
            typeOther->Append(scriptItems[i]);

        this->Connect(SCRIPT_ID_INDEX+i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeScriptItem));
    }
}
void MainFrame::UpdateMenu()
{
    // Adjust menu options when the fractal type is changed.
    if(colorFrameActive) pal->SetTarget(fractalCanvas->GetFractalPtr());
    if(iterDiagActive) iterDiag->SetTarget(fractalCanvas->GetFractalPtr());

    showOrbit->Check(false);
    if(fractalCanvas->GetFractalPtr()->HasOrbit()) showOrbit->Enable(true);
    else showOrbit->Enable(false);

    MoreIter->Enable(true);
    lessIter->Enable(true);

    // Closes constant dialog.
    if(introConstActive)
    {
        diag->Show(false);
        introConstActive = false;
        delete diag;
    }

    // Adjust Julia constant menu items.
    if(fractalCanvas->GetFractalPtr()->IsJuliaVariety())
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
    if(fractalType != FractalType::UserDefined && fractalType != FractalType::FixedPointUserDefined)
    {
        if(formDiagActive)
        {
            formDialog->Destroy();
            formDiagActive = false;
        }
    }
    if(fractalType == FractalType::ScriptFractal)
        pauseBtn.pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));
    else
        pauseBtn.pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));

    // If Julia mode is opened closes it.
    if(juliaModeState)
    {
        juliaMode->Check(false);
#ifdef _WIN32
        ptr->Terminate();
        delete ptr;
#endif
        juliaModeState = false;
        changeJuliaMode = false;
        fractalCanvas->SetJuliaMode(false);
    }
    this->UpdateOptPanel();
}
void MainFrame::UpdateJuliaMode()
{
    // Destroy Julia window.
    if(juliaModeState)
    {
        juliaMode->Check(false);
#ifdef _WIN32
        ptr->Close();
        ptr->Wait();
        delete ptr;
#endif
        juliaModeState = false;
        changeJuliaMode = false;
        fractalCanvas->SetJuliaMode(false);
    }
    // Creates Julia fractal with parameters from the main fractal.
    else
    {
        juliaMode->Check(true);
        juliaModeState = true;
        changeJuliaMode = true;

#ifdef _WIN32
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

        ptr = new JuliaMode(fractalCanvas, juliaType, fractalCanvas->GetFractalPtr()->GetOptions(), this);
        ptr->Launch();
#endif
        fractalCanvas->SetJuliaMode(true);
    }
}
void MainFrame::ReloadScripts()
{
    // Remove current menu entries.
    for(unsigned int i=0; i<scriptDataVect.size(); i++)
    {
        if(scriptDataVect[i].cat == CAT_COMPLEX)
            typeComplex->Remove(scriptItems[i]);
        else if(scriptDataVect[i].cat == CAT_NUMMET)
            typeNumMet->Remove(scriptItems[i]);
        else if(scriptDataVect[i].cat == CAT_PHYSIC)
            typePhysics->Remove(scriptItems[i]);
        else
            typeOther->Remove(scriptItems[i]);
    }
    scriptDataVect.clear();

    // Disconnect events and delete menu items.
    for(unsigned int i=0; i<scriptItems.size(); i++)
    {
        this->Disconnect(SCRIPT_ID_INDEX+i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ChangeScriptItem));
        delete scriptItems[i];
    }
    scriptItems.clear();

    // Get new scripts.
    this->GetScriptFractals();
}

/**
* @class MainApp
* @brief Starts the execution of the program.
*/
class MainApp : public wxApp
{
    virtual bool OnInit()
    {
       MainFrame *Main = new MainFrame;
       Main->Show();
       return true;
    }
};

// wxWidgets entry point.
IMPLEMENT_APP(MainApp);