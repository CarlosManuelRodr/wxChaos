// ReSharper disable CppEnumeratorNeverUsed
#include "main/MainFrame.h"
#include <algorithm>
#include <wx/panel.h>
#include "AngelscriptBindings.h"
#include "AppPaths.h"
#include "export/ImageExportSizeDialog.h"
#include "docs/DocumentViewer.h"
#include "docs/FractalDocumentation.h"
#include "main/SettingsFrame.h"
#include "common/AppTheme.h"

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
    wxImage::AddHandler(new wxPNGHandler);
    this->SetSizeHints(wxSize(900, 650), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);
    this->GetParserOpt();    // Gets configuration from config.ini.
    AppTheme::SetAppearance(_appConfig.appearance);
    this->SetUpGUI();

    _juliaPreviewFrame = nullptr;
    _dimensionCalculator = nullptr;
    _changeKeyboardGuide = false;
    _manualJuliaConstantActive = false;
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
        _juliaConstantDialog = new JuliaConstantDialog(&_manualJuliaConstantActive, _fractalCanvas->GetFractalPresenter(), this);
        _juliaConstantDialog->Show(true);
        _manualJuliaConstantActive = true;
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
    const auto firstUseDialog = new DocumentViewer(
        AppPaths::ResourceFile({"Documents", "welcome.html"}),
        this,
        wxID_ANY,
        _("Welcome to wxChaos"),
        wxDefaultPosition,
        wxSize(1500, 960),
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        [this](const wxString& url) { return HandleDocumentationLink(url); }
        );

    firstUseDialog->Show(true);
    _fractalCanvas->ShowGuideImages();
}
void MainFrame::ConnectEvents()
{
    this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    this->Bind(wxEVT_MENU, &MainFrame::OnQuit, this, wxID_EXIT);
    this->Bind(wxEVT_MENU, &MainFrame::OnSettings, this, ID_SETTINGS);
    this->Bind(wxEVT_SIZE, &MainFrame::OnResize, this);
    this->Bind(wxEVT_FRACTAL_CANVAS_STATUS_TEXT, &MainFrame::OnCanvasStatusText, this);
    this->Bind(wxEVT_MENU, &MainFrame::OnJuliaMode, this, ID_JULIA_MODE);
    this->Bind(wxEVT_JULIA_MODE_CLOSED, &MainFrame::OnJuliaModeClosed, this);
    this->Bind(wxEVT_RENDERER_OPTIONS_CLOSED, &MainFrame::OnRendererOptionsClosed, this);
    this->Bind(wxEVT_SCRIPT_EDITOR_CLOSED, &MainFrame::OnScriptEditorClosed, this);
    this->Bind(wxEVT_DIMENSION_FRAME_CLOSED, &MainFrame::OnDimensionFrameClosed, this);
    this->Bind(wxEVT_COMMAND_CONSOLE_CLOSED, &MainFrame::OnCommandConsoleClosed, this);
    this->Bind(wxEVT_MENU, &MainFrame::OnWelcomeDialog, this, ID_WELCOME_DIALOG);
    this->Bind(wxEVT_MENU, &MainFrame::OnAbout, this, ID_ABOUT);
    this->Bind(wxEVT_MENU, &MainFrame::OnKeyboardGuide, this, ID_KEYBOARD_GUIDE);
    this->Bind(wxEVT_MENU, &MainFrame::OnSave, this, ID_SAVE);
    this->Bind(wxEVT_MENU, &MainFrame::OnPalette, this, ID_PALETTE);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeMandelbrot, this, ID_MANDELBROT);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeMandelbrotZN, this, ID_MANDELBROT_ZN);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeJulia, this, ID_JULIA);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeJuliaZN, this, ID_JULIA_ZN);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeNewton, this, ID_NEWTON);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeSinusoidal, this, ID_SINUSOIDAL);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeMagnet, this, ID_MAGNET);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeJellyfish, this, ID_JELLYFISH);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeManowar, this, ID_MANOWAR);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeManowarJulia, this, ID_MANOWAR_JULIA);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeFixedPoint1, this, ID_FIXEDPOINT1);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeFixedPoint2, this, ID_FIXEDPOINT2);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeFixedPoint3, this, ID_FIXEDPOINT3);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeFixedPoint4, this, ID_FIXEDPOINT4);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeTricorn, this, ID_TRICORN);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeBurningShip, this, ID_BURNING_SHIP);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeBurningShipJulia, this, ID_BURNING_SHIP_JULIA);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeFractory, this, ID_FRACTORY);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeCell, this, ID_CELL);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeLogisticMap, this, ID_LOGISTIC_MAP);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeHenonMap, this, ID_HENON_MAP);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeSierpinskiTriangle, this, ID_SIERPINSKI_TRIANGLE);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeDPendulum, this, ID_DOUBLE_PENDULUM);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeUserDefinedEscapeTime, this, ID_USER_DEFINED);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeUserDefinedFixedPoint, this, ID_FIXED_POINT_USER_DEFINED);
    this->Bind(wxEVT_MENU, &MainFrame::ChangeUserDefinedNewton, this, ID_NEWTON_USER_DEFINED);
    this->Bind(wxEVT_MENU, &MainFrame::OnAbortRender, this, ID_ABORT_RENDER);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateAbortRender, this, ID_ABORT_RENDER);
    this->Bind(wxEVT_MENU, &MainFrame::OnReset, this, ID_RESET);
    this->Bind(wxEVT_MENU, &MainFrame::OnRedraw, this, ID_REDRAW);
    this->Bind(wxEVT_MENU, &MainFrame::OnIncreaseIterations, this, ID_INCREASE_IT);
    this->Bind(wxEVT_MENU, &MainFrame::OnDecreaseIterations, this, ID_DECREASE_IT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateManualIterations, this, ID_IT_MANUAL);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateManualIterations, this, ID_INCREASE_IT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateManualIterations, this, ID_DECREASE_IT);
    this->Bind(wxEVT_MENU, &MainFrame::OnShowOrbit, this, ID_SHOW_ORBIT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateShowOrbit, this, ID_SHOW_ORBIT);
    this->Bind(wxEVT_MENU, &MainFrame::OnManualJuliaConstant, this, ID_ENTER_MAN_CONSTANT);
    this->Bind(wxEVT_MENU, &MainFrame::OnSliderJuliaConstant, this, ID_ENTER_SLD_CONSTANT);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateSliderMode, this, ID_ENTER_SLD_CONSTANT);
    this->Bind(wxEVT_MENU, &MainFrame::OnSetIterations, this, ID_IT_MANUAL);
    this->Bind(wxEVT_MENU, &MainFrame::OnAutomaticIterations, this, ID_AUTOMATIC_ITERATIONS);
    this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateAutomaticIterations, this, ID_AUTOMATIC_ITERATIONS);
    this->Bind(wxEVT_MENU, &MainFrame::OnFormulaDialog, this, ID_FORMULA_DIALOG);
    this->Bind(wxEVT_MENU, &MainFrame::OnToolbarVisibility, this, ID_VIEW_TOOLBAR);
    this->Bind(wxEVT_MENU, &MainFrame::OnFractalOptions, this, ID_OPTION_PANEL);
    this->Bind(wxEVT_MENU, &MainFrame::OnUserManual, this, ID_USER_MANUAL);
    this->Bind(wxEVT_MENU, &MainFrame::OnScriptEditor, this, ID_SCRIPT_EDITOR);
    this->Bind(wxEVT_MENU, &MainFrame::OnZoomRecorder, this, ID_ZOOM_RECORDER);
    this->Bind(wxEVT_MENU, &MainFrame::OnDimensionCalculator, this, ID_DIMENSION_CALCULATOR);
    this->Bind(wxEVT_MENU, &MainFrame::OnCommandConsole, this, ID_COMMAND_CONSOLE);
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
        if (_fractalCanvas != nullptr)
            _fractalCanvas->SetInteractionTool(tool);
    });
    _interactionToolbar->SetColorRotationHandler([this]
    {
        if (_fractalCanvas == nullptr || _fractalCanvas->GetFractal()->IsRendering())
            return false;

        _fractalCanvas->GetFractalPresenter()->ToggleColorRotation();
        return true;
    });
    _interactionToolbar->SetInformationHandler([this] { OpenFractalInformation(); });
}

void MainFrame::CreateStatusBarControls()
{
    _statusBar = this->CreateStatusBar(2, wxST_SIZEGRIP, wxID_ANY);
    constexpr int widths[] = {-1, 180};
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
    if (!_statusBar->GetFieldRect(1, rect))
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
        _iterationsDialog = new IterationsDialog(
            &_iterationsDialogIsActive,
            _fractalCanvas->GetFractalPresenter(),
            this,
            _fractalCanvas);
        _iterationsDialog->Show(true);
        _iterationsDialogIsActive = true;
    }
    else
    {
        _iterationsDialog->Raise();
        _iterationsDialog->SetFocus();
    }
}

void MainFrame::OpenFractalInformation()
{
    if (_fractalCanvas == nullptr)
        return;

    const wxString documentFile = _fractalCanvas->GetFractal()->GetFractalInformationFile();
    if (documentFile.empty())
        return;

    _informationViewer = new DocumentViewer(
        documentFile,
        this,
        wxID_ANY,
        _fractalCanvas->GetFractal()->GetName(),
        wxDefaultPosition,
        wxSize(1500, 960),
        wxDEFAULT_FRAME_STYLE,
        [this](const wxString& url) { return HandleDocumentationLink(url); });
    _informationFrameIsActive = true;
    DocumentViewer* viewer = _informationViewer;
    _informationViewer->Bind(wxEVT_CLOSE_WINDOW, [this, viewer](wxCloseEvent& event)
    {
        if (_informationViewer == viewer)
        {
            _informationViewer = nullptr;
            _informationFrameIsActive = false;
        }
        event.Skip();
    });
    _informationViewer->Show(true);
}

void MainFrame::OpenRendererOptions()
{
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

void MainFrame::UpdateInformationTool() const
{
    if (_interactionToolbar != nullptr && _fractalCanvas != nullptr)
        _interactionToolbar->SetInformationEnabled(_fractalCanvas->GetFractal()->HasFractalInformation());
}

bool MainFrame::HandleDocumentationLink(const wxString& url)
{
    const bool handled = ExecuteDocumentationAction(DocumentationLinkAction::Parse(url));

    if (!handled)
    {
        wxMessageBox(
            _("wxChaos does not know how to handle this documentation action:\n") + url,
            _("Documentation action"),
            wxOK | wxICON_INFORMATION,
            this);
    }

    return handled;
}

bool MainFrame::ExecuteDocumentationAction(const DocumentationLinkAction& action)
{
    switch (action.GetType())
    {
        case DocumentationLinkAction::Type::OpenFractal:
            return OpenDocumentationFractal(action);
        case DocumentationLinkAction::Type::OpenJuliaMode:
            return OpenDocumentationJuliaMode(action);
        case DocumentationLinkAction::Type::OpenLocation:
            return OpenDocumentationLocation(action.GetLocation());
        case DocumentationLinkAction::Type::ToggleTool:
            return ToggleDocumentationTool(action.GetTarget());
        case DocumentationLinkAction::Type::SetRendering:
            return SetDocumentationRendering(action.GetRenderingMethod());
        case DocumentationLinkAction::Type::OpenFormulaDialog:
            return OpenDocumentationFormulaDialog(action);
        case DocumentationLinkAction::Type::OpenFractalOptions:
            return OpenDocumentationFractalOptions(action);
        case DocumentationLinkAction::Type::Unknown:
        default:
            return false;
    }
}

bool MainFrame::OpenDocumentationFractal(const DocumentationLinkAction& action)
{
    if (action.GetTarget() == "script-editor")
    {
        OpenScriptEditorFromDocumentation();
        return true;
    }

    if (action.GetTargetFractalType() != FractalType::Undefined)
    {
        ChangeFractal(action.GetTargetFractalType(), action.TargetFractalEnablesJulia());
        Raise();
        return true;
    }

    return false;
}

bool MainFrame::OpenDocumentationJuliaMode(const DocumentationLinkAction& action)
{
    if (!action.TargetFractalEnablesJulia() || action.GetTargetFractalType() == FractalType::Undefined ||
        _fractalCanvas == nullptr)
        return false;

    if (_juliaPreviewFrame != nullptr)
    {
        DestroyJuliaMode(true);
        Raise();
        return true;
    }

    ChangeFractal(action.GetTargetFractalType(), action.TargetFractalEnablesJulia());
    const Options options = _fractalCanvas->GetFractal()->GetOptions();
    if (!OpenJuliaModeAt(options.kReal, options.kImaginary))
        return false;

    Raise();
    return true;
}

bool MainFrame::OpenDocumentationLocation(const DocumentationLinkAction::Location& location)
{
    if (location.fractalType == FractalType::Undefined || _fractalCanvas == nullptr)
        return false;

    ChangeFractal(location.fractalType, location.enableJulia);
    Fractal* fractal = _fractalCanvas->GetFractal();
    _fractalCanvas->GetFractalPresenter()->SetView(
        fractal->GetCenteredView(location.centerX, location.centerY, location.radius));
    Raise();
    return true;
}

bool MainFrame::SetDocumentationRendering(const DocumentationLinkAction::RenderingMethod& method)
{
    if (method.fractalType == FractalType::Undefined || _fractalCanvas == nullptr)
        return false;

    ChangeFractal(method.fractalType, method.enableJulia);
    if (!ApplyDocumentationRenderingToCurrentFractal(method))
        return false;

    Raise();
    return true;
}

bool MainFrame::SetDocumentationRenderingFromJuliaPreview(const DocumentationLinkAction::RenderingMethod& method)
{
    if (_juliaPreviewFrame == nullptr || _fractalCanvas == nullptr)
        return false;

    if (!ApplyDocumentationRenderingToCurrentFractal(method))
        return false;

    Raise();
    return true;
}

bool MainFrame::ApplyDocumentationRenderingToCurrentFractal(const DocumentationLinkAction::RenderingMethod& method)
{
    if (_fractalCanvas == nullptr)
        return false;

    FractalPresenter* presenter = _fractalCanvas->GetFractalPresenter();
    if (_rendererOptions != nullptr)
    {
        _rendererOptions->SetTarget(presenter);
        return _rendererOptions->SetRenderingOptions(method.algorithm, method.smoothRender, method.orbitTrap);
    }

    if (presenter == nullptr || !presenter->SetRenderingOptions(method.algorithm, method.smoothRender, method.orbitTrap))
        return false;

    UpdateJuliaRendererOptions(_fractalCanvas->GetFractal()->GetOptions());
    return true;
}

bool MainFrame::OpenDocumentationFormulaDialog(const DocumentationLinkAction& action)
{
    if (action.GetTargetFractalType() == FractalType::Undefined || _fractalCanvas == nullptr)
        return false;

    if (!_formulaDialogIsActive)
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_FORMULA_DIALOG);
        OnFormulaDialog(event);
    }

    ChangeFractal(action.GetTargetFractalType(), false);

    if (_formulaDialog == nullptr)
        return false;

    _formulaDialog->SelectFormulaType(action.GetTargetFormulaType());
    _formulaDialog->Raise();
    _formulaDialog->SetFocus();
    return true;
}

bool MainFrame::OpenDocumentationFractalOptions(const DocumentationLinkAction& action)
{
    if (action.GetTargetFractalType() == FractalType::Undefined || _fractalCanvas == nullptr)
        return false;

    ChangeFractal(action.GetTargetFractalType(), action.TargetFractalEnablesJulia());
    if (_fractalOptionsItem == nullptr || !_fractalOptionsItem->IsEnabled())
        return false;

    if (!_showOptionsPanel)
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_OPTION_PANEL);
        OnFractalOptions(event);
    }

    Raise();
    return true;
}

bool MainFrame::ToggleDocumentationTool(const wxString& tool)
{
    if (tool == "renderer-options")
    {
        OpenRendererOptions();
        return true;
    }

    if (tool == "julia-constant-slider")
        return ToggleDocumentationJuliaConstantSlider();

    if (tool != "orbit" || _fractalCanvas == nullptr || !_fractalCanvas->GetFractal()->HasOrbit())
        return false;

    const bool orbitMode = !_fractalCanvas->IsOrbitMode();
    _fractalCanvas->SetOrbitMode(orbitMode);
    if (_showOrbit != nullptr)
        _showOrbit->Check(orbitMode);

    Raise();
    return true;
}

bool MainFrame::ToggleDocumentationJuliaConstantSlider()
{
    if (_fractalCanvas == nullptr || _sliderJuliaConstant == nullptr || !_sliderJuliaConstant->IsEnabled())
        return false;

    const bool sliderMode = !_fractalCanvas->IsSliderMode();
    _fractalCanvas->SetSliderMode(sliderMode);
    _sliderJuliaConstant->Check(sliderMode);

    Raise();
    return true;
}

void MainFrame::OpenScriptEditorFromDocumentation()
{
    if (_scriptEditor == nullptr)
    {
        _scriptEditor = new ScriptEditor(this);
        _scriptEditor->Show(true);
    }

    _scriptEditor->Raise();
    _scriptEditor->SetFocus();
}

void MainFrame::SetUpGUI()
{
    // Init menu.
    _menubar = new wxMenuBar();
    _fileMenu = new wxMenu();
    _viewMenu = new wxMenu();
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
    wxMenuItem* tricorn, *burningShip, *burningShipJulia, *fractory, *cell, *logisticMap, *henonMap, *dPendulum;

#ifdef _WIN32
#define menuSeparator '\t'
#elif __linux__
#define menuSeparator "    "
#endif

    mandelbrot = new wxMenuItem(_formula, ID_MANDELBROT, _("Mandelbrot") + menuSeparator + "z = z^2 + c", wxEmptyString, wxITEM_NORMAL);
    mandelbrotZN = new wxMenuItem(_formula, ID_MANDELBROT_ZN, _("Mandelbrot Z^m") + menuSeparator + "z = z^m + c", wxEmptyString, wxITEM_NORMAL);
    julia = new wxMenuItem(_formula, ID_JULIA, _("Mandelbrot (Julia)") + menuSeparator + "z = z^2 + k", wxEmptyString, wxITEM_NORMAL);
    juliaZN = new wxMenuItem(_formula, ID_JULIA_ZN, _("Julia Z^m") + menuSeparator + "z = z^m + k", wxEmptyString, wxITEM_NORMAL);
    newton = new wxMenuItem(_formula, ID_NEWTON, _("Newton") + menuSeparator + "z^3 - 1 = 0", wxEmptyString, wxITEM_NORMAL);
    sinusoidal = new wxMenuItem(_formula, ID_SINUSOIDAL, _("Sine (Julia)") + menuSeparator + "Z = c*Sin(Z)", wxEmptyString, wxITEM_NORMAL);
    magnet = new wxMenuItem(_formula, ID_MAGNET, _("Magnet"), wxEmptyString, wxITEM_NORMAL);
    jellyfish = new wxMenuItem(_formula, ID_JELLYFISH, _("Jellyfish"), wxEmptyString, wxITEM_NORMAL);
    manowar = new wxMenuItem(_formula, ID_MANOWAR, _("Manowar"), wxEmptyString, wxITEM_NORMAL);
    manowarJulia = new wxMenuItem(_formula, ID_MANOWAR_JULIA, _("Manowar (Julia)"), wxEmptyString, wxITEM_NORMAL);
    sierpinskiTriangle = new wxMenuItem(_formula, ID_SIERPINSKI_TRIANGLE, _("Sierpinski Triangle"), wxEmptyString, wxITEM_NORMAL);
    fixedPoint1 = new wxMenuItem(_formula, ID_FIXEDPOINT1, _("Fixed Point") + menuSeparator + "z = sin(z)", wxEmptyString, wxITEM_NORMAL);
    fixedPoint2 = new wxMenuItem(_formula, ID_FIXEDPOINT2, _("Fixed Point") + menuSeparator + "z = cos(z)", wxEmptyString, wxITEM_NORMAL);
    fixedPoint3 = new wxMenuItem(_formula, ID_FIXEDPOINT3, _("Fixed Point") + menuSeparator + "z = tan(z)", wxEmptyString, wxITEM_NORMAL);
    fixedPoint4 = new wxMenuItem(_formula, ID_FIXEDPOINT4, _("Fixed Point") + menuSeparator + "z = z^2", wxEmptyString, wxITEM_NORMAL);
    tricorn = new wxMenuItem(_formula, ID_TRICORN, _("Tricorn"), wxEmptyString, wxITEM_NORMAL);
    burningShip = new wxMenuItem(_formula, ID_BURNING_SHIP, _("Burning Ship"), wxEmptyString, wxITEM_NORMAL);
    burningShipJulia = new wxMenuItem(_formula, ID_BURNING_SHIP_JULIA, _("Burning Ship (Julia)"), wxEmptyString, wxITEM_NORMAL);
    fractory = new wxMenuItem(_formula, ID_FRACTORY, _("Fractory"), wxEmptyString, wxITEM_NORMAL);
    cell = new wxMenuItem(_formula, ID_CELL, _("Cell"), wxEmptyString, wxITEM_NORMAL);
    logisticMap = new wxMenuItem(_formula, ID_LOGISTIC_MAP, _("Logistic Map"), wxEmptyString, wxITEM_NORMAL);
    henonMap = new wxMenuItem(_formula, ID_HENON_MAP, _("Henon map"), wxEmptyString, wxITEM_NORMAL);
    dPendulum = new wxMenuItem(_formula, ID_DOUBLE_PENDULUM, _("Double pendulum"), wxEmptyString, wxITEM_NORMAL);
    userDefined = new wxMenuItem(_formula, ID_USER_DEFINED, _("User Formula (Complex)"), wxEmptyString, wxITEM_NORMAL);
    fpUserDefined = new wxMenuItem(_formula, ID_FIXED_POINT_USER_DEFINED, _("User Formula (Fixed Point)"), wxEmptyString, wxITEM_NORMAL);
    newtonUserDefined = new wxMenuItem(_formula, ID_NEWTON_USER_DEFINED, _("User Formula (Newton-Raphson)"), wxEmptyString, wxITEM_NORMAL);

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
    _typeOther->Append(logisticMap);
    _typeOther->Append(henonMap);
    _typeOther->Append(sierpinskiTriangle);

    _formula->Append(-1, _("Complex"), _typeComplex);
    _formula->Append(-1, _("Numerical method"), _typeNumericalMethod);
    _formula->Append(-1, _("Physics"), _typePhysics);
    _formula->Append(-1, _("Other"), _typeOther);
    _formula->Append(userDefined);
    _formula->Append(fpUserDefined);
    _formula->Append(newtonUserDefined);
    _fractalMenu->Append(wxID_ANY, _("Formula"), _formula);

    // Julia constant.
    _introConstant = new wxMenu();
    _manualJuliaConstant = new wxMenuItem(_introConstant, ID_ENTER_MAN_CONSTANT, _("Manual"), wxEmptyString, wxITEM_NORMAL);
    _introConstant->Append(_manualJuliaConstant);
    _manualJuliaConstant->Enable(false);

    _sliderJuliaConstant = new wxMenuItem(_introConstant, ID_ENTER_SLD_CONSTANT, _("Slider") + '\t' + "F1", wxEmptyString, wxITEM_CHECK);
    _introConstant->Append(_sliderJuliaConstant);
    _sliderJuliaConstant->Enable(false);
    _sliderJuliaConstant->Check(false);
    _fractalMenu->Append(-1, _("Enter Julia constant"), _introConstant);

    // Julia constant and show orbit.
    _juliaMode = new wxMenuItem(_fractalMenu, ID_JULIA_MODE, _("Julia mode"), wxEmptyString, wxITEM_CHECK);
    _showOrbit = new wxMenuItem(_fractalMenu, ID_SHOW_ORBIT, _("Show orbit") + '\t' + "F2", wxEmptyString, wxITEM_CHECK);

    _fractalMenu->Append(_juliaMode);
    _fractalMenu->Append(_showOrbit);
    _juliaMode->Check(false);
    _showOrbit->Check(false);

    // File menu.
    _fileMenu->Append(ID_SAVE, _("Save image") + '\t' + "F4");
    _fileMenu->AppendSeparator();
    _fileMenu->Append(ID_SETTINGS, _("Settings..."));
    _fileMenu->AppendSeparator();
    _fileMenu->Append(wxID_EXIT, _("Quit"));

    // View menu.
    _toolbarItem = new wxMenuItem(_viewMenu, ID_VIEW_TOOLBAR, _("Toolbar"), wxEmptyString, wxITEM_CHECK);
    _fractalOptionsItem = new wxMenuItem(_viewMenu, ID_OPTION_PANEL, _("Fractal options"), wxEmptyString, wxITEM_CHECK);
    _viewMenu->Append(_toolbarItem);
    _viewMenu->Append(_fractalOptionsItem);
    _toolbarItem->Check(true);

    // Tools menu.
    _toolMenu->Append(ID_SCRIPT_EDITOR, _("Script editor"), _("Create new fractals with a scripting language."));
    _toolMenu->Append(ID_COMMAND_CONSOLE, _("Command console"), _("Control and inspect the current fractal with commands."));
    _toolMenu->Append(ID_ZOOM_RECORDER, _("Zoom recorder"), _("Record a video zoom."));
    _toolMenu->Append(ID_DIMENSION_CALCULATOR, _("Dimension calculator"), _("Calculate fractal dimension."));

    // Iterations.
    _setIterations = new wxMenuItem(_iterationsMenu, ID_IT_MANUAL, _("Set iterations"), wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_setIterations);
    _moreIterations = new wxMenuItem(_iterationsMenu, ID_INCREASE_IT, _("Increase iterations") + '\t' + "L", wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_moreIterations);

    _lessIterations = new wxMenuItem(_iterationsMenu, ID_DECREASE_IT, _("Decrease iterations") + '\t' + "K", wxEmptyString, wxITEM_NORMAL);
    _iterationsMenu->Append(_lessIterations);
    _iterationsMenu->AppendSeparator();
    _automaticIterations = new wxMenuItem(_iterationsMenu, ID_AUTOMATIC_ITERATIONS, _("Automatic iterations"), wxEmptyString, wxITEM_CHECK);
    _iterationsMenu->Append(_automaticIterations);

    // Fractal menu.
    _fractalMenu->Append(ID_FORMULA_DIALOG, _("Enter user formula"));
    _fractalMenu->AppendSeparator();

    _abortRenderItem = _fractalMenu->Append(ID_ABORT_RENDER, _("Abort") + '\t' + "P");
    _abortRenderItem->Enable(false);
    _fractalMenu->Append(ID_REDRAW, _("Redraw") + '\t' + "F5");
    _fractalMenu->Append(ID_RESET, _("Reset"));
    _rendererMenu->Append(ID_PALETTE, _("Renderer options"));

    // Help menu.
    _helpMenu->Append(ID_USER_MANUAL, _("User manual"));
    _keyboardGuide = new wxMenuItem(_helpMenu, ID_KEYBOARD_GUIDE, _("Keyboard guide"), wxEmptyString, wxITEM_CHECK);
    _helpMenu->Append(_keyboardGuide);
    _helpMenu->Append(ID_WELCOME_DIALOG, _("Open welcome guide"));
    _helpMenu->Append(ID_ABOUT, _("About"));


    _menubar->Append(_fileMenu, _("File"));
    _menubar->Append(_viewMenu, _("View"));
    _menubar->Append(_fractalMenu, _("Fractal"));
    _menubar->Append(_iterationsMenu, _("Iterations"));
    _menubar->Append(_rendererMenu, _("Renderer"));
    _menubar->Append(_toolMenu, _("Tools"));
    _menubar->Append(_helpMenu, _("Help"));
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

    _fractalOptionsPanel = new FractalOptionsPanel(_optionPanel);
    _fractalOptionsPanel->SetApplyHandler([this]()
    {
        _fractalCanvas->SetFocus();
        _fractalCanvas->GetFractalPresenter()->Redraw();
    });
    _optionSizer->Add(_fractalOptionsPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    _optionPanel->SetSizer(_optionSizer);
    _optionPanel->Layout();
    _optionSizer->Fit(_optionPanel);
    panelSizer->Add(_optionPanel, 1, wxEXPAND | wxALL, 1);

    _sizer->Add(panelSizer, 2, wxEXPAND, 5);
    this->SetSizer(_sizer);
    this->Layout();
    this->Centre(wxVERTICAL);

    _size = GetClientSize();
    _size.SetWidth(std::max(1, _size.GetWidth() - _interactionToolbar->GetBestSize().GetWidth() - 220));
    _size.SetHeight(std::max(1, _size.GetHeight() - 40));

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
    _fractalCanvas->GetFractalPresenter()->SetAntiAliasingScale(_appConfig.antiAliasingScale);

    SetAutomaticIterations(_appConfig.automaticIterations);
    _fractalCanvas->GetFractalPresenter()->SetExteriorColorMode(_appConfig.colorFractal);
    _fractalCanvas->GetFractalPresenter()->SetFractalSetColorMode(_appConfig.colorSet);
    _fractalCanvas->GetFractalPresenter()->SetZoomOptions(_appConfig.zoomStepPercent, _appConfig.zoomInertiaMilliseconds);
    _fractalSizer->Add(_fractalCanvas, 1, wxEXPAND | wxALL, 0);
    CreateStatusBarControls();
    this->Layout();
    LayoutStatusBarControls();
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
    SettingsFrame dialog(this, _appConfig, [this](const AppConfig& config) { ApplyAppConfig(config); });
    dialog.ShowModal();
}
void MainFrame::ApplyAppConfig(const AppConfig& config)
{
    const bool appearanceChanged = config.appearance != _appConfig.appearance;
    const bool languageChanged = config.language != _appConfig.language;
    _appConfig = config;

    if (appearanceChanged || languageChanged)
    {
        wxMessageBox(
            _("You need to restart wxChaos for appearance and language changes to take effect."),
            _("Restart required"), wxOK | wxICON_INFORMATION, this);
    }

    wxGradient gradient;
    gradient.SetMin(0);
    gradient.SetMax(config.paletteSize);
    gradient.FromString(wxString::FromUTF8(config.colorStyleGrad.c_str()));
    _fractalCanvas->GetFractalPresenter()->SetColorPalette(config.colorStyle);
    _fractalCanvas->GetFractalPresenter()->SetGradient(gradient);
    _fractalCanvas->GetFractalPresenter()->SetColorCycleLength(config.colorCycleLength);
    _fractalCanvas->GetFractalPresenter()->SetAntiAliasingScale(config.antiAliasingScale);
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
void MainFrame::ApplyAutomaticIterationsSetting() const
{
    SetAutomaticIterations(_appConfig.automaticIterations);
}
void MainFrame::CloseAll()
{
    if (_renderStatusWidget != nullptr)
        _renderStatusWidget->SetPresenter(nullptr);

    if (_fractalCanvas != nullptr)
        _fractalCanvas->PrepareForClose();

    if (_commandConsole != nullptr)
    {
        _commandConsole->Destroy();
        _commandConsole = nullptr;
    }
    if (_informationViewer != nullptr)
    {
        _informationViewer->Destroy();
        _informationViewer = nullptr;
        _informationFrameIsActive = false;
    }
    DestroyJuliaMode(true);
    DestroyDimensionFrame();
    _fractalCanvas = nullptr;
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
    if (_fractalCanvas != nullptr)
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
    ImageExportSizeDialog* sizeDialogSave;

    if (_fractalType == FractalType::ScriptFractal && _selectedScriptIndex.has_value())
        sizeDialogSave = new ImageExportSizeDialog(_fractalCanvas, _fractalType, _fractalCanvas->GetFractal(),
                                                   this, _loadedScripts[*_selectedScriptIndex].file);
    else
        sizeDialogSave = new ImageExportSizeDialog(_fractalCanvas, _fractalType, _fractalCanvas->GetFractal(), this);

    sizeDialogSave->ShowModal();
    sizeDialogSave->Destroy();
}

void MainFrame::OpenJuliaPreviewInMainWindow(const FractalType fractalType, const Options& options,
                                             const ColorPaletteTypes colorPalette, const Rect& view,
                                             const bool automaticIterations)
{
    if (_fractalCanvas == nullptr)
        return;

    _selectedScriptIndex.reset();
    _juliaPreviewFrame = nullptr;
    _fractalCanvas->SetJuliaMode(false);

    if (_fractalCanvas->GetFractal()->IsRendering())
        _fractalCanvas->GetFractal()->StopRender();

    _fractalCanvas->ChangeType(fractalType);
    _fractalType = fractalType;

    Fractal* fractal = _fractalCanvas->GetFractal();
    fractal->SetOptions(options);

    FractalPresenter* presenter = _fractalCanvas->GetFractalPresenter();
    presenter->SetColorPalette(colorPalette);
    presenter->SetView(view);
    SetAutomaticIterations(automaticIterations);

    ResetColorRotationTool();
    UpdateMenu();
    _juliaMode->Check(false);
    _juliaMode->Enable(false);
}

void MainFrame::OnPalette(wxCommandEvent&)
{
    OpenRendererOptions();
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
    _fractalCanvas->GetFractalPresenter()->SetAntiAliasingScale(_appConfig.antiAliasingScale);
    this->UpdateMenu();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnIncreaseIterations(wxCommandEvent&)
{
    _fractalCanvas->GetFractalPresenter()->IncreaseIterations();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnDecreaseIterations(wxCommandEvent&)
{
    _fractalCanvas->GetFractalPresenter()->DecreaseIterations();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnShowOrbit(wxCommandEvent&)
{
    const bool mode = _showOrbit->IsChecked();
    _fractalCanvas->SetOrbitMode(mode);
    _showOrbit->Check(mode);
}
void MainFrame::OnManualJuliaConstant(wxCommandEvent&)
{
    // Manual constant.
    if (!_manualJuliaConstantActive)
    {
        _juliaConstantDialog = new JuliaConstantDialog(&_manualJuliaConstantActive, _fractalCanvas->GetFractalPresenter(), this);
        _juliaConstantDialog->Show(true);
        _manualJuliaConstantActive = true;
    }
    else
    {
        _juliaConstantDialog->Show(false);
        _manualJuliaConstantActive = false;
        delete _juliaConstantDialog;
    }
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnSliderJuliaConstant(wxCommandEvent&)
{
    const bool mode = _sliderJuliaConstant->IsChecked();
    _fractalCanvas->SetSliderMode(mode);
    _sliderJuliaConstant->Check(mode);
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
    _statusBar->SetStatusText(event.GetString(), 0);
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
// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void MainFrame::OnToolbarVisibility(wxCommandEvent& event)
{
    if (_interactionToolbar == nullptr)
        return;

    const bool showToolbar = event.IsChecked();
    _interactionToolbar->Show(showToolbar);
    if (_toolbarItem != nullptr)
        _toolbarItem->Check(showToolbar);

    this->GetSizer()->Layout();
    LayoutStatusBarControls();
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
        HideOptionsPanel(false);
    }
}
// ReSharper disable once CppMemberFunctionMayBeConst
void MainFrame::OnApplyPanelOpt(wxCommandEvent&)
{
    _fractalOptionsPanel->Apply();
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
            _("To use the zoom recorder you need to first perform a zoom on the main window."),
            _("Zoom recorder"),
            wxOK | wxICON_INFORMATION,
            this);
        return;
    }

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
void MainFrame::ChangeLogisticMap(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::LogisticMap, false);
}
void MainFrame::ChangeHenonMap(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::HenonMap, false);
}
void MainFrame::ChangeDPendulum(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::DoublePendulum, false);
}
void MainFrame::ChangeUserDefinedEscapeTime(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::UserDefinedEscapeTime, true);
}
void MainFrame::ChangeUserDefinedFixedPoint(wxCommandEvent&)
{
    this->ChangeFractal(FractalType::UserDefinedFixedPoint, false);
}
void MainFrame::ChangeUserDefinedNewton(wxCommandEvent&)
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
        _fractalCanvas->GetFractalPresenter()->SetAntiAliasingScale(_appConfig.antiAliasingScale);
        ApplyAutomaticIterationsSetting();
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
    _fractalCanvas->GetFractalPresenter()->SetAntiAliasingScale(_appConfig.antiAliasingScale);
    ApplyAutomaticIterationsSetting();

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
    if (PanelOptions* panelOptions = _fractalCanvas->GetFractal()->GetOptionsPanel(); panelOptions->GetElementsSize() > 0)
    {
        _fractalOptionsItem->Enable(true);
        _fractalOptionsPanel->SetTarget(_fractalCanvas->GetFractal());

        if (panelOptions->GetForceShow())
        {
            _fractalOptionsItem->Check(true);
            if (!_showOptionsPanel)
            {
                _optionPanel->Show();
                const wxSize windowSize = this->GetSize();
                if (!this->IsMaximized())
                    this->SetSize(windowSize.GetWidth() + 175, windowSize.GetHeight());

                this->GetSizer()->Layout();
                _showOptionsPanel = true;
            }
        }
        else
            HideOptionsPanel(false);

        _optionSizer->Layout();
        _optionPanel->SetScrollbars(20, 20, 0, 50);
    }
    else
    {
        _fractalOptionsItem->Check(false);
        _fractalOptionsItem->Enable(false);
        this->DeleteOptPanel();
    }
}
void MainFrame::DeleteOptPanel()
{
    HideOptionsPanel(true);
}

void MainFrame::HideOptionsPanel(const bool clearTarget)
{
    if (clearTarget)
        _fractalOptionsPanel->ClearTarget();

    _fractalOptionsItem->Check(false);
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
    if (_manualJuliaConstantActive)
    {
        _juliaConstantDialog->Show(false);
        _manualJuliaConstantActive = false;
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

    _abortRenderItem->SetItemLabel(_("Abort") + '\t' + "P");
    _abortRenderItem->Enable(false);

    // If Julia mode is opened, closes it.
    DestroyJuliaMode(true);
    this->UpdateOptionsPanel();
    UpdateInformationTool();
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
        case FractalType::UserDefinedEscapeTime: juliaType = FractalType::UserDefinedEscapeTime; break;
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
