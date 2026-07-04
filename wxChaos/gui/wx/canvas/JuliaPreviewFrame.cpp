// ReSharper disable CppDFAMemoryLeak
#include "canvas/JuliaPreviewFrame.h"
#include "AppPaths.h"
#include "docs/DocumentViewer.h"
#include "docs/DocumentationLinkAction.h"
#include "docs/FractalDocumentation.h"
#include "main/MainFrame.h"

wxDEFINE_EVENT(wxEVT_JULIA_MODE_CLOSED, wxCommandEvent);

JuliaPreviewFrame::JuliaPreviewFrame(wxWindow* parent, FractalCanvas* target, const FractalType fractalType,
                                     const Options& juliaOpt, const int zoomStepPercent, const int zoomInertiaMilliseconds,
                                     const wxSize& size)
                                     : wxFrame(parent, wxID_ANY, "Julia mode", wxDefaultPosition, wxDefaultSize),
                                       _target(target),
                                       _constantSyncTimer(this)
{
    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    SetIcon(icon);
    CreateMenuBar();

    _toolbar = new FractalToolbar(this);
    _toolbar->SetToolChangedHandler([this](const FractalInteractionTool tool)
    {
        if (_previewCanvas != nullptr)
            _previewCanvas->SetInteractionTool(tool);
    });
    _toolbar->SetColorRotationHandler([this]
    {
        if (_previewCanvas == nullptr || _previewCanvas->GetFractal()->IsRendering())
            return false;

        _previewCanvas->GetFractalPresenter()->ToggleColorRotation();
        return true;
    });
    _toolbar->SetInformationHandler([this] { OpenFractalInformation(); });

    _previewCanvas = new FractalCanvas(fractalType, this, wxID_ANY, wxDefaultPosition, size);
    _previewCanvas->SetMainCanvasOverlaysVisible(false);
    _previewCanvas->GetFractal()->SetOptions(juliaOpt, true);
    _previewCanvas->GetFractal()->SetJuliaMode(true);
    _previewCanvas->GetFractalPresenter()->SetZoomOptions(zoomStepPercent, zoomInertiaMilliseconds);
    ApplyRendererOptions(juliaOpt);

    const auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(_toolbar, 0, wxEXPAND);
    sizer->Add(_previewCanvas, 1, wxEXPAND);
    SetSizer(sizer);
    CreateStatusBarControls();
    SetClientSize(wxSize(size.GetWidth() + _toolbar->GetBestSize().GetWidth(), size.GetHeight()));
    SetSizeHints(wxSize(500, 300), wxDefaultSize);
    wxTopLevelWindowBase::Layout();
    LayoutStatusBarControls();
    UpdateInformationTool();

    if (parent != nullptr)
    {
        const wxPoint parentPos = parent->GetPosition();
        const wxSize parentSize = parent->GetSize();
        Move(parentPos.x + parentSize.GetWidth() + 5, parentPos.y);
    }

    Bind(wxEVT_CLOSE_WINDOW, &JuliaPreviewFrame::OnClose, this);
    Bind(wxEVT_TIMER, &JuliaPreviewFrame::OnConstantSyncTimer, this);
    Bind(wxEVT_FRACTAL_CANVAS_STATUS_TEXT, &JuliaPreviewFrame::OnCanvasStatusText, this);
    Bind(wxEVT_SIZE, &JuliaPreviewFrame::OnResize, this);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnCloseMenu, this, wxID_CLOSE);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnToolbarVisibility, this, ID_JULIA_PREVIEW_VIEW_TOOLBAR);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnOpenInMainWindow, this, ID_JULIA_PREVIEW_OPEN_IN_MAIN);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnAbortRender, this, ID_JULIA_PREVIEW_ABORT_RENDER);
    Bind(wxEVT_UPDATE_UI, &JuliaPreviewFrame::OnUpdateAbortRender, this, ID_JULIA_PREVIEW_ABORT_RENDER);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnRedraw, this, ID_JULIA_PREVIEW_REDRAW);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnReset, this, ID_JULIA_PREVIEW_RESET);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnSetIterations, this, ID_JULIA_PREVIEW_SET_ITERATIONS);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnIncreaseIterations, this, ID_JULIA_PREVIEW_INCREASE_ITERATIONS);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnDecreaseIterations, this, ID_JULIA_PREVIEW_DECREASE_ITERATIONS);
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnAutomaticIterations, this, ID_JULIA_PREVIEW_AUTOMATIC_ITERATIONS);
    Bind(wxEVT_UPDATE_UI, &JuliaPreviewFrame::OnUpdateAutomaticIterations, this, ID_JULIA_PREVIEW_AUTOMATIC_ITERATIONS);

    _constantSyncTimer.Start(16);
}

void JuliaPreviewFrame::CreateMenuBar()
{
    _menubar = new wxMenuBar();
    _fileMenu = new wxMenu();
    _viewMenu = new wxMenu();
    _fractalMenu = new wxMenu();
    _iterationsMenu = new wxMenu();

    _fileMenu->Append(wxID_CLOSE, "Close");

    _toolbarItem = new wxMenuItem(_viewMenu, ID_JULIA_PREVIEW_VIEW_TOOLBAR, "Toolbar", wxEmptyString, wxITEM_CHECK);
    _viewMenu->Append(_toolbarItem);
    _toolbarItem->Check(true);

    _fractalMenu->Append(ID_JULIA_PREVIEW_OPEN_IN_MAIN, "Open this in main window");
    _fractalMenu->AppendSeparator();
    _abortRenderItem = _fractalMenu->Append(ID_JULIA_PREVIEW_ABORT_RENDER, wxString("Abort") + '\t' + "P");
    _abortRenderItem->Enable(false);
    _fractalMenu->Append(ID_JULIA_PREVIEW_REDRAW, wxString("Redraw") + '\t' + "F5");
    _fractalMenu->Append(ID_JULIA_PREVIEW_RESET, "Reset");

    _iterationsMenu->Append(ID_JULIA_PREVIEW_SET_ITERATIONS, "Set iterations");
    _iterationsMenu->Append(ID_JULIA_PREVIEW_INCREASE_ITERATIONS, wxString("Increase iterations") + '\t' + "L");
    _iterationsMenu->Append(ID_JULIA_PREVIEW_DECREASE_ITERATIONS, wxString("Decrease iterations") + '\t' + "K");
    _iterationsMenu->AppendSeparator();
    _automaticIterationsItem = new wxMenuItem(
        _iterationsMenu,
        ID_JULIA_PREVIEW_AUTOMATIC_ITERATIONS,
        "Automatic iterations",
        wxEmptyString,
        wxITEM_CHECK);
    _iterationsMenu->Append(_automaticIterationsItem);

    _menubar->Append(_fileMenu, "File");
    _menubar->Append(_viewMenu, "View");
    _menubar->Append(_fractalMenu, "Fractal");
    _menubar->Append(_iterationsMenu, "Iterations");
    SetMenuBar(_menubar);
}

void JuliaPreviewFrame::CreateStatusBarControls()
{
    _statusBar = CreateStatusBar(2, wxST_SIZEGRIP, wxID_ANY);
    constexpr int widths[] = {-1, 180};
    _statusBar->SetStatusWidths(2, widths);
    _statusBar->SetStatusText(wxEmptyString, 0);
    _statusBar->SetStatusText(wxEmptyString, 1);

    _renderStatusWidget = new RenderStatusWidget(
        _statusBar,
        _previewCanvas->GetFractalPresenter(),
        [this] { OpenIterationsDialog(); });
    _statusBar->Bind(wxEVT_SIZE, [this](wxSizeEvent& event)
    {
        LayoutStatusBarControls();
        event.Skip();
    });
    LayoutStatusBarControls();
}

void JuliaPreviewFrame::LayoutStatusBarControls() const
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

void JuliaPreviewFrame::OpenIterationsDialog()
{
    if (!_iterationsDialogIsActive)
    {
        _iterationsDialog = new IterationsDialog(
            &_iterationsDialogIsActive,
            _previewCanvas->GetFractalPresenter(),
            this,
            _previewCanvas);
        _iterationsDialog->Show(true);
        _iterationsDialogIsActive = true;
    }
    else
    {
        _iterationsDialog->Raise();
        _iterationsDialog->SetFocus();
    }
}

void JuliaPreviewFrame::OpenFractalInformation()
{
    if (_previewCanvas == nullptr)
        return;

    const wxString documentFile = _previewCanvas->GetFractal()->GetFractalInformationFile();
    if (documentFile.empty())
        return;

    const auto viewer = new DocumentViewer(
        documentFile,
        this,
        wxID_ANY,
        _previewCanvas->GetFractal()->GetName(),
        wxDefaultPosition,
        wxSize(1500, 960),
        wxDEFAULT_FRAME_STYLE,
        [this](const wxString& url) { return HandleDocumentationLink(url); });
    viewer->Show(true);
}

void JuliaPreviewFrame::UpdateInformationTool() const
{
    if (_toolbar != nullptr && _previewCanvas != nullptr)
        _toolbar->SetInformationEnabled(_previewCanvas->GetFractal()->HasFractalInformation());
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void JuliaPreviewFrame::ResetColorRotationTool() const
{
    if (_toolbar != nullptr)
        _toolbar->ResetColorRotationTool();
}

bool JuliaPreviewFrame::HandleDocumentationLink(const wxString& url)
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse(url);
    if (action.GetType() == DocumentationLinkAction::Type::ToggleTool &&
        action.GetTarget() == "julia-constant-slider")
        return FocusMainFrameFromDocumentation();

    return false;
}

bool JuliaPreviewFrame::FocusMainFrameFromDocumentation() const
{
    wxWindow* mainFrame = GetParent();
    if (mainFrame == nullptr && _target != nullptr)
        mainFrame = wxGetTopLevelParent(_target);
    if (mainFrame == nullptr)
        return false;

    mainFrame->Raise();
    mainFrame->SetFocus();
    return true;
}

void JuliaPreviewFrame::SetRendererOptions(const Options& options) const
{
    ApplyRendererOptions(options);
}

void JuliaPreviewFrame::SetConstant(const double real, const double imaginary) const
{
    if (_previewCanvas != nullptr)
        _previewCanvas->GetFractalPresenter()->SetK(real, imaginary);
}

void JuliaPreviewFrame::SetZoomOptions(const int zoomStepPercent, const int zoomInertiaMilliseconds) const
{
    if (_previewCanvas != nullptr)
        _previewCanvas->GetFractalPresenter()->SetZoomOptions(zoomStepPercent, zoomInertiaMilliseconds);
}

void JuliaPreviewFrame::ApplyRendererOptions(const Options& options) const
{
    if (_previewCanvas == nullptr)
        return;

    FractalPresenter* presenter = _previewCanvas->GetFractalPresenter();
    presenter->SetGradient(options.gradient);
    presenter->SetGradientSize(options.gradPaletteSize);
    presenter->SetColorVariationOffset(options.colorVariationOffset);
    presenter->SetColorRotationSpeed(options.colorRotationSpeed);
    presenter->SetAlgorithm(options.alg);
    presenter->SetRenderingPrecisionMode(options.renderingPrecisionMode);
    presenter->SetRelativeColor(options.relativeColor);
    presenter->SetExteriorColorMode(options.colorMode);
    presenter->SetFractalSetColorMode(options.colorSet);
    presenter->SetFractalSetColor(options.fSetColor);
    presenter->SetOrbitTrapMode(options.orbitTrapMode);
    presenter->SetSmoothRender(options.smoothRender);
    presenter->SetK(options.kReal, options.kImaginary);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void JuliaPreviewFrame::SyncConstantFromTarget() const
{
    if (_target != nullptr && _target->ChangeInCoordinateSelector())
        SetConstant(_target->GetKReal(), _target->GetKImaginary());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnConstantSyncTimer(wxTimerEvent&)
{
    SyncConstantFromTarget();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void JuliaPreviewFrame::OnCanvasStatusText(wxCommandEvent& event)
{
    _statusBar->SetStatusText(event.GetString(), 0);
}

void JuliaPreviewFrame::OnResize(wxSizeEvent& event)
{
    Layout();
    LayoutStatusBarControls();
    event.Skip();
}

void JuliaPreviewFrame::OnCloseMenu(wxCommandEvent&)
{
    Close();
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void JuliaPreviewFrame::OnToolbarVisibility(wxCommandEvent& event)
{
    if (_toolbar == nullptr)
        return;

    const bool showToolbar = event.IsChecked();
    _toolbar->Show(showToolbar);
    if (_toolbarItem != nullptr)
        _toolbarItem->Check(showToolbar);

    Layout();
    LayoutStatusBarControls();
}

void JuliaPreviewFrame::OnOpenInMainWindow(wxCommandEvent&)
{
    auto* mainFrame = dynamic_cast<MainFrame*>(GetParent());
    if (mainFrame == nullptr || _previewCanvas == nullptr)
        return;

    FractalPresenter* presenter = _previewCanvas->GetFractalPresenter();
    Fractal* fractal = _previewCanvas->GetFractal();
    mainFrame->OpenJuliaPreviewInMainWindow(
        _previewCanvas->GetFractalType(),
        fractal->GetOptions(),
        fractal->GetColorPalette(),
        presenter->GetCurrentZoom(),
        presenter->AutomaticIterationsEnabled());
    Close();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnAbortRender(wxCommandEvent&)
{
    if (_previewCanvas != nullptr)
        _previewCanvas->AbortRender();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnRedraw(wxCommandEvent&)
{
    if (_previewCanvas != nullptr)
        _previewCanvas->GetFractalPresenter()->Redraw();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnReset(wxCommandEvent&)
{
    if (_previewCanvas == nullptr)
        return;

    FractalPresenter* presenter = _previewCanvas->GetFractalPresenter();
    Fractal* fractal = _previewCanvas->GetFractal();
    const Options options = fractal->GetOptions();
    const ColorPaletteTypes colorPalette = fractal->GetColorPalette();
    const bool automaticIterations = presenter->AutomaticIterationsEnabled();

    _previewCanvas->Reset();
    _previewCanvas->GetFractal()->SetJuliaMode(true);
    _previewCanvas->GetFractal()->SetOptions(options, true);
    presenter->SetColorPalette(colorPalette);
    presenter->SetAutomaticIterations(automaticIterations);
    ResetColorRotationTool();
}

void JuliaPreviewFrame::OnSetIterations(wxCommandEvent&)
{
    OpenIterationsDialog();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnIncreaseIterations(wxCommandEvent&)
{
    if (_previewCanvas != nullptr)
        _previewCanvas->GetFractalPresenter()->IncreaseIterations();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnDecreaseIterations(wxCommandEvent&)
{
    if (_previewCanvas != nullptr)
        _previewCanvas->GetFractalPresenter()->DecreaseIterations();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnAutomaticIterations(wxCommandEvent&)
{
    if (_previewCanvas == nullptr)
        return;

    const bool automaticIterations = _automaticIterationsItem != nullptr && _automaticIterationsItem->IsChecked();
    _previewCanvas->GetFractalPresenter()->SetAutomaticIterations(automaticIterations);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnUpdateAbortRender(wxUpdateUIEvent& event)
{
    event.Enable(_previewCanvas != nullptr && _previewCanvas->CanAbortRender());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnUpdateAutomaticIterations(wxUpdateUIEvent& event)
{
    event.Check(_previewCanvas != nullptr && _previewCanvas->GetFractalPresenter()->AutomaticIterationsEnabled());
}

void JuliaPreviewFrame::OnClose(wxCloseEvent&)
{
    _constantSyncTimer.Stop();

    if (_renderStatusWidget != nullptr)
        _renderStatusWidget->SetPresenter(nullptr);

    if (_iterationsDialogIsActive && _iterationsDialog != nullptr)
        _iterationsDialog->Destroy();

    if (_previewCanvas != nullptr)
        _previewCanvas->PrepareForClose();

    if (GetParent() != nullptr)
        wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_JULIA_MODE_CLOSED));

    Destroy();
}
