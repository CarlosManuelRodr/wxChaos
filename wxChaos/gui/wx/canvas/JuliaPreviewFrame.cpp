// ReSharper disable CppDFAMemoryLeak
#include "canvas/JuliaPreviewFrame.h"
#include "AppPaths.h"
#include "docs/DocumentViewer.h"
#include "docs/FractalDocumentation.h"
#include "main/MainFrame.h"

wxDEFINE_EVENT(wxEVT_JULIA_MODE_CLOSED, wxCommandEvent);

JuliaPreviewFrame::JuliaPreviewFrame(wxWindow* parent, FractalCanvas* target, const FractalType fractalType,
                                     const Options& juliaOpt, const int zoomStepPercent, const int zoomInertiaMilliseconds,
                                     const wxSize& size)
                                     : wxFrame(parent, wxID_ANY, _("Julia mode"), wxDefaultPosition, wxDefaultSize),
                                       _parentFractalCanvas(target),
                                       _constantSyncTimer(this)
{
    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    SetIcon(icon);
    CreateMenuBar();

    _toolbar = new FractalToolbar(this);
    _toolbar->SetToolChangedHandler([this](const FractalInteractionTool tool)
    {
        if (_previewFractalCanvas != nullptr)
            _previewFractalCanvas->SetInteractionTool(tool);
    });
    _toolbar->SetColorRotationHandler([this]
    {
        if (_previewFractalCanvas == nullptr || !_previewFractalCanvas->GetFractal()->SupportsColorRotation()
            || _previewFractalCanvas->GetFractal()->IsRendering())
            return false;

        _previewFractalCanvas->GetFractalPresenter()->ToggleColorRotation();
        return true;
    });
    _toolbar->SetInformationHandler([this] { OpenFractalInformation(); });

    _previewFractalCanvas = new FractalCanvas(fractalType, this, wxID_ANY, wxDefaultPosition, size);
    if (_parentFractalCanvas != nullptr && fractalType == FractalType::UserDefinedEscapeTime)
    {
        FormulaOptions previewFormula = _parentFractalCanvas->GetFormula();
        previewFormula.julia = true;
        _previewFractalCanvas->SetUserFormula(previewFormula);
        _previewFractalCanvas->GetFractal()->SetFormula(previewFormula);
    }
    _previewFractalCanvas->SetMainCanvasOverlaysVisible(false);
    _previewFractalCanvas->GetFractal()->SetOptions(juliaOpt, true);
    _previewFractalCanvas->GetFractal()->SetJuliaMode(true);
    _previewFractalCanvas->GetFractalPresenter()->SetZoomOptions(zoomStepPercent, zoomInertiaMilliseconds);
    ApplyRendererOptions(juliaOpt);

    const auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(_toolbar, 0, wxEXPAND);
    sizer->Add(_previewFractalCanvas, 1, wxEXPAND);
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
    Bind(wxEVT_MENU, &JuliaPreviewFrame::OnShowOrbit, this, ID_JULIA_PREVIEW_SHOW_ORBIT);
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

    _fileMenu->Append(wxID_CLOSE, _("Close"));


    _toolbarItem = new wxMenuItem(_viewMenu, ID_JULIA_PREVIEW_VIEW_TOOLBAR, _("Toolbar"), wxEmptyString, wxITEM_CHECK);
    _viewMenu->Append(_toolbarItem);
    _toolbarItem->Check(true);

    _fractalMenu->Append(ID_JULIA_PREVIEW_OPEN_IN_MAIN, _("Open this in main window"));

    _showOrbit = new wxMenuItem(_fractalMenu, ID_JULIA_PREVIEW_SHOW_ORBIT, _("Show orbit") + '\t' + "F2", wxEmptyString, wxITEM_CHECK);
    _fractalMenu->Append(_showOrbit);
    _fractalMenu->AppendSeparator();
    _abortRenderItem = _fractalMenu->Append(ID_JULIA_PREVIEW_ABORT_RENDER, _("Abort") + '\t' + "P");
    _abortRenderItem->Enable(false);
    _fractalMenu->Append(ID_JULIA_PREVIEW_REDRAW, _("Redraw") + '\t' + "F5");
    _fractalMenu->Append(ID_JULIA_PREVIEW_RESET, _("Reset"));

    _iterationsMenu->Append(ID_JULIA_PREVIEW_SET_ITERATIONS, _("Set iterations"));
    _iterationsMenu->Append(ID_JULIA_PREVIEW_INCREASE_ITERATIONS, _("Increase iterations") + '\t' + "L");
    _iterationsMenu->Append(ID_JULIA_PREVIEW_DECREASE_ITERATIONS, _("Decrease iterations") + '\t' + "K");
    _iterationsMenu->AppendSeparator();
    _automaticIterationsItem = new wxMenuItem(
        _iterationsMenu,
        ID_JULIA_PREVIEW_AUTOMATIC_ITERATIONS,
        _("Automatic iterations"),
        wxEmptyString,
        wxITEM_CHECK);
    _iterationsMenu->Append(_automaticIterationsItem);

    _menubar->Append(_fileMenu, _("File"));
    _menubar->Append(_viewMenu, _("View"));
    _menubar->Append(_fractalMenu, _("Fractal"));
    _menubar->Append(_iterationsMenu, _("Iterations"));
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
        _previewFractalCanvas->GetFractalPresenter(),
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
            _previewFractalCanvas->GetFractalPresenter(),
            this,
            _previewFractalCanvas);
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
    if (_previewFractalCanvas == nullptr)
        return;

    const wxString documentFile = _previewFractalCanvas->GetFractal()->GetFractalInformationFile();
    if (documentFile.empty())
        return;

    const auto viewer = new DocumentViewer(
        documentFile,
        this,
        wxID_ANY,
        _previewFractalCanvas->GetFractal()->GetName(),
        wxDefaultPosition,
        wxSize(1500, 960),
        wxDEFAULT_FRAME_STYLE,
        [this](const wxString& url) { return HandleDocumentationLink(url); });
    viewer->Show(true);
}

void JuliaPreviewFrame::UpdateInformationTool() const
{
    if (_toolbar != nullptr && _previewFractalCanvas != nullptr)
    {
        _toolbar->SetInformationEnabled(_previewFractalCanvas->GetFractal()->HasFractalInformation());
        _toolbar->SetColorRotationEnabled(_previewFractalCanvas->GetFractal()->SupportsColorRotation());
    }
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
    if (action.GetType() == DocumentationLinkAction::Type::SetRendering)
        return SetDocumentationRenderingFromDocumentation(action.GetRenderingMethod());

    return false;
}

bool JuliaPreviewFrame::FocusMainFrameFromDocumentation() const
{
    wxWindow* mainFrame = GetParent();
    if (mainFrame == nullptr && _parentFractalCanvas != nullptr)
        mainFrame = wxGetTopLevelParent(_parentFractalCanvas);
    if (mainFrame == nullptr)
        return false;

    mainFrame->Raise();
    mainFrame->SetFocus();
    return true;
}

bool JuliaPreviewFrame::SetDocumentationRenderingFromDocumentation(
    const DocumentationLinkAction::RenderingMethod& method) const
{
    if (_previewFractalCanvas == nullptr || method.fractalType == FractalType::Undefined)
        return false;

    Fractal* fractal = _previewFractalCanvas->GetFractal();
    if (fractal == nullptr || fractal->GetType() != method.fractalType)
        return false;

    MainFrame* mainFrame = dynamic_cast<MainFrame*>(GetParent());
    if (mainFrame == nullptr)
        return false;

    return mainFrame->SetDocumentationRenderingFromJuliaPreview(method);
}

void JuliaPreviewFrame::SetRendererOptions(const Options& options) const
{
    ApplyRendererOptions(options);
}

void JuliaPreviewFrame::SetConstant(const double real, const double imaginary) const
{
    if (_previewFractalCanvas != nullptr)
        _previewFractalCanvas->GetFractalPresenter()->SetK(real, imaginary);
}

void JuliaPreviewFrame::SetZoomOptions(const int zoomStepPercent, const int zoomInertiaMilliseconds) const
{
    if (_previewFractalCanvas != nullptr)
        _previewFractalCanvas->GetFractalPresenter()->SetZoomOptions(zoomStepPercent, zoomInertiaMilliseconds);
}

void JuliaPreviewFrame::ApplyRendererOptions(const Options& options) const
{
    if (_previewFractalCanvas == nullptr)
        return;

    FractalPresenter* presenter = _previewFractalCanvas->GetFractalPresenter();
    presenter->SetGradient(options.gradient);
    presenter->SetGradientSize(options.gradPaletteSize);
    presenter->SetColorVariationOffset(options.colorVariationOffset);
    presenter->SetColorRotationSpeed(options.colorRotationSpeed);
    presenter->SetAlgorithm(options.alg);
    presenter->SetRenderingPrecisionMode(options.renderingPrecisionMode);
    presenter->SetAntiAliasingScale(options.antiAliasingScale);
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
    if (_parentFractalCanvas != nullptr && _parentFractalCanvas->ChangeInCoordinateSelector())
        SetConstant(_parentFractalCanvas->GetKReal(), _parentFractalCanvas->GetKImaginary());
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
    if (mainFrame == nullptr || _previewFractalCanvas == nullptr)
        return;

    FractalPresenter* presenter = _previewFractalCanvas->GetFractalPresenter();
    Fractal* fractal = _previewFractalCanvas->GetFractal();
    mainFrame->OpenJuliaPreviewInMainWindow(
        _previewFractalCanvas->GetFractalType(),
        fractal->GetOptions(),
        fractal->GetColorPalette(),
        presenter->GetCurrentZoom(),
        presenter->AutomaticIterationsEnabled());
    Close();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnAbortRender(wxCommandEvent&)
{
    if (_previewFractalCanvas != nullptr)
        _previewFractalCanvas->AbortRender();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnRedraw(wxCommandEvent&)
{
    if (_previewFractalCanvas != nullptr)
        _previewFractalCanvas->GetFractalPresenter()->Redraw();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnReset(wxCommandEvent&)
{
    if (_previewFractalCanvas == nullptr)
        return;

    FractalPresenter* presenter = _previewFractalCanvas->GetFractalPresenter();
    Fractal* fractal = _previewFractalCanvas->GetFractal();
    const Options options = fractal->GetOptions();
    const ColorPaletteTypes colorPalette = fractal->GetColorPalette();
    const bool automaticIterations = presenter->AutomaticIterationsEnabled();

    _previewFractalCanvas->Reset();
    _previewFractalCanvas->GetFractal()->SetJuliaMode(true);
    _previewFractalCanvas->GetFractal()->SetOptions(options, true);
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
    if (_previewFractalCanvas != nullptr)
        _previewFractalCanvas->GetFractalPresenter()->IncreaseIterations();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnDecreaseIterations(wxCommandEvent&)
{
    if (_previewFractalCanvas != nullptr)
        _previewFractalCanvas->GetFractalPresenter()->DecreaseIterations();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnShowOrbit(wxCommandEvent&)
{
    const bool mode = _showOrbit->IsChecked();
    _previewFractalCanvas->SetOrbitMode(mode);
    _showOrbit->Check(mode);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnAutomaticIterations(wxCommandEvent&)
{
    if (_previewFractalCanvas == nullptr)
        return;

    const bool automaticIterations = _automaticIterationsItem != nullptr && _automaticIterationsItem->IsChecked();
    _previewFractalCanvas->GetFractalPresenter()->SetAutomaticIterations(automaticIterations);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnUpdateAbortRender(wxUpdateUIEvent& event)
{
    event.Enable(_previewFractalCanvas != nullptr && _previewFractalCanvas->CanAbortRender());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void JuliaPreviewFrame::OnUpdateAutomaticIterations(wxUpdateUIEvent& event)
{
    event.Check(_previewFractalCanvas != nullptr && _previewFractalCanvas->GetFractalPresenter()->AutomaticIterationsEnabled());
}

void JuliaPreviewFrame::OnClose(wxCloseEvent&)
{
    _constantSyncTimer.Stop();

    if (_renderStatusWidget != nullptr)
        _renderStatusWidget->SetPresenter(nullptr);

    if (_iterationsDialogIsActive && _iterationsDialog != nullptr)
        _iterationsDialog->Destroy();

    if (_previewFractalCanvas != nullptr)
        _previewFractalCanvas->PrepareForClose();

    if (GetParent() != nullptr)
        wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_JULIA_MODE_CLOSED));

    Destroy();
}
