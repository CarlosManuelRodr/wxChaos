#include "canvas/FractalTutorialController.h"
#include <utility>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/sound.h>
#include <wx/window.h>
#include "AppPaths.h"
#include "canvas/FractalCanvas.h"
#include "canvas/FractalToolbar.h"
#include "canvas/FractalTutorialOverlay.h"
#include "sfml/FractalPresenter.h"
#include "Fractal.h"

FractalTutorialController::FractalTutorialController(
    FractalCanvas* canvas, FractalToolbar* toolbar,
    std::function<void(TutorialStatus)> statusChanged)
    : _canvas(canvas),
      _toolbar(toolbar),
      _overlay(std::make_unique<FractalTutorialOverlay>(canvas)),
      _statusChanged(std::move(statusChanged)),
      _timer(this)
{
    _overlay->SetSkipStepHandler([this]
    {
        HandleAction(FractalTutorialAction::SkipPointPicker);
    });
    Bind(wxEVT_TIMER, &FractalTutorialController::OnTimer, this);
    _canvas->Bind(wxEVT_SIZE, &FractalTutorialController::OnCanvasSize, this);
    _topLevelWindow = wxGetTopLevelParent(_canvas);
    if (_topLevelWindow != nullptr)
        _topLevelWindow->Bind(wxEVT_MOVE, &FractalTutorialController::OnFrameMove, this);
}

FractalTutorialController::~FractalTutorialController()
{
    _timer.Stop();
    if (_canvas != nullptr)
        _canvas->Unbind(wxEVT_SIZE, &FractalTutorialController::OnCanvasSize, this);
    if (_topLevelWindow != nullptr)
        _topLevelWindow->Unbind(wxEVT_MOVE, &FractalTutorialController::OnFrameMove, this);
}

void FractalTutorialController::Start(const bool automaticRun)
{
    _timer.Stop();
    _model.Reset();
    _automaticRun = automaticRun;
    _pauseAfterTransition = false;
    _documentationClosedDuringTransition = false;
    _phaseElapsedMilliseconds = 0;
    _phase = Phase::WaitingForRender;
    _currentTool = FractalInteractionTool::Cursor;
    _overlay->HideAll();

    _canvas->SetInteractionTool(FractalInteractionTool::Cursor);
    _toolbar->SetInteractionTool(FractalInteractionTool::Cursor);
    if (_canvas->GetFractal()->IsGradientAnimating())
        _canvas->GetFractalPresenter()->ToggleColorRotation();
    _toolbar->ResetColorRotationTool();
    _canvas->SetFocus();
    _timer.Start(100);
}

void FractalTutorialController::HandleAction(const FractalTutorialAction action)
{
    if (_phase != Phase::Active)
        return;

    if ((action == FractalTutorialAction::SelectionZoom
         || action == FractalTutorialAction::ZoomBack
         || action == FractalTutorialAction::MiddleMousePan
         || action == FractalTutorialAction::KeyboardPan)
        && _currentTool != FractalInteractionTool::Cursor)
        return;

    if (!_model.HandleAction(action))
        return;

    if (action == FractalTutorialAction::FractalInformationOpened)
        _pauseAfterTransition = true;

    if (_model.IsCompleted())
        PersistStatus(TutorialStatus::Completed);

    BeginSuccessTransition();
}

void FractalTutorialController::HandleToolSelected(const FractalInteractionTool tool)
{
    _currentTool = tool;
    if (tool == FractalInteractionTool::PointPicker)
        HandleAction(FractalTutorialAction::PointPickerSelected);
}

void FractalTutorialController::NotifyDocumentationClosed()
{
    if (_phase == Phase::SuccessTransition && _pauseAfterTransition)
    {
        _documentationClosedDuringTransition = true;
        return;
    }

    if (_phase != Phase::PausedForDocumentation)
        return;

    _phase = Phase::Active;
    _overlay->ShowStep(_model.GetStep());
    _canvas->SetFocus();
}

bool FractalTutorialController::HandleEscape()
{
    if (!IsActive())
        return false;

    _timer.Stop();
    const TutorialStatus abortStatus =
        FractalTutorialModel::GetStatusAfterAbort(TutorialStatus::Pending, _automaticRun);
    if (_automaticRun)
        PersistStatus(abortStatus);

    _phase = Phase::SkippedMessage;
    _phaseElapsedMilliseconds = 0;
    _overlay->ShowSkipped();
    _timer.Start(50);
    return true;
}

bool FractalTutorialController::IsActive() const
{
    return _phase != Phase::Inactive;
}

void FractalTutorialController::PersistStatus(const TutorialStatus status) const
{
    if (_statusChanged)
        _statusChanged(status);
}

void FractalTutorialController::PlaySuccessSound() const
{
    const wxString path = AppPaths::ResourceFile({"success_audio.wav"});
    if (!wxFileExists(path) || !wxSound::Play(path, wxSOUND_ASYNC))
        wxLogWarning("Could not play tutorial success sound: %s", path);
}

void FractalTutorialController::BeginSuccessTransition()
{
    PlaySuccessSound();
    _overlay->ShowSuccess();
    _phase = Phase::SuccessTransition;
    _timer.StartOnce(650);
}

void FractalTutorialController::OnTimer(wxTimerEvent&)
{
    switch (_phase)
    {
        case Phase::WaitingForRender:
            if (!_canvas->GetFractal()->IsRendered())
                return;
            _timer.Stop();
            _phase = Phase::Active;
            _overlay->ShowStep(_model.GetStep());
            break;
        case Phase::SuccessTransition:
            _overlay->HideSuccess();
            if (_model.IsCompleted())
            {
                _phase = Phase::FinalMessage;
                _overlay->ShowFinal();
                _timer.StartOnce(2500);
            }
            else if (_pauseAfterTransition)
            {
                _pauseAfterTransition = false;
                if (_documentationClosedDuringTransition)
                {
                    _documentationClosedDuringTransition = false;
                    _phase = Phase::Active;
                    _overlay->ShowStep(_model.GetStep());
                }
                else
                {
                    _phase = Phase::PausedForDocumentation;
                    _overlay->HideAll();
                }
            }
            else
            {
                _phase = Phase::Active;
                _overlay->ShowStep(_model.GetStep());
            }
            break;
        case Phase::FinalMessage:
            _overlay->HideAll();
            _phase = Phase::Inactive;
            break;
        case Phase::SkippedMessage:
            _phaseElapsedMilliseconds += 50;
            if (_phaseElapsedMilliseconds >= 1500)
            {
                const double fade =
                    static_cast<double>(_phaseElapsedMilliseconds - 1500) / 1000.0;
                _overlay->SetSkippedFade(fade);
            }
            if (_phaseElapsedMilliseconds >= 2500)
            {
                _timer.Stop();
                _overlay->HideAll();
                _phase = Phase::Inactive;
            }
            break;
        case Phase::Inactive:
        case Phase::Active:
        case Phase::PausedForDocumentation:
            break;
    }
}

void FractalTutorialController::OnCanvasSize(wxSizeEvent& event)
{
    _overlay->Reposition();
    event.Skip();
}

void FractalTutorialController::OnFrameMove(wxMoveEvent& event)
{
    _overlay->Reposition();
    event.Skip();
}
