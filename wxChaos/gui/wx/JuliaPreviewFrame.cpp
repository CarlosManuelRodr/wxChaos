// ReSharper disable CppDFAMemoryLeak
#include "JuliaPreviewFrame.h"
#include "AppPaths.h"

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

    _toolbar = new FractalToolbar(this);
    _toolbar->SetToolChangedHandler([this](const FractalInteractionTool tool)
    {
        _previewCanvas->SetInteractionTool(tool);
    });
    _toolbar->SetColorRotationHandler([this]
    {
        if (_previewCanvas == nullptr || _previewCanvas->GetFractal()->IsRendering())
            return false;

        _previewCanvas->GetFractalPresenter()->ToggleColorRotation();
        return true;
    });

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
    SetClientSize(wxSize(size.GetWidth() + _toolbar->GetBestSize().GetWidth(), size.GetHeight()));
    SetSizeHints(wxSize(500, 300), wxDefaultSize);

    if (parent != nullptr)
    {
        const wxPoint parentPos = parent->GetPosition();
        const wxSize parentSize = parent->GetSize();
        Move(parentPos.x + parentSize.GetWidth() + 5, parentPos.y);
    }

    Bind(wxEVT_CLOSE_WINDOW, &JuliaPreviewFrame::OnClose, this);
    Bind(wxEVT_TIMER, &JuliaPreviewFrame::OnConstantSyncTimer, this);
    _constantSyncTimer.Start(16);
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

void JuliaPreviewFrame::OnClose(wxCloseEvent&)
{
    _constantSyncTimer.Stop();

    if (_previewCanvas != nullptr && _previewCanvas->GetFractal()->IsRendering())
        _previewCanvas->GetFractal()->StopRender();

    if (GetParent() != nullptr)
        wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_JULIA_MODE_CLOSED));

    Destroy();
}
