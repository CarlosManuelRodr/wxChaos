#include "canvas/FractalTutorialModel.h"

void FractalTutorialModel::Reset()
{
    _step = FractalTutorialStep::CursorZoomIn;
    _pointPickerSelected = false;
}

FractalTutorialStep FractalTutorialModel::GetStep() const
{
    return _step;
}

bool FractalTutorialModel::IsCompleted() const
{
    return _step == FractalTutorialStep::Completed;
}

bool FractalTutorialModel::HandleAction(const FractalTutorialAction action)
{
    bool completedStep = false;
    switch (_step)
    {
        case FractalTutorialStep::CursorZoomIn:
            completedStep = action == FractalTutorialAction::SelectionZoom;
            break;
        case FractalTutorialStep::CursorZoomBack:
            completedStep = action == FractalTutorialAction::ZoomBack;
            break;
        case FractalTutorialStep::CursorPan:
            completedStep = action == FractalTutorialAction::MiddleMousePan
                || action == FractalTutorialAction::KeyboardPan;
            break;
        case FractalTutorialStep::HandPan:
            completedStep = action == FractalTutorialAction::HandPan;
            break;
        case FractalTutorialStep::ZoomTool:
            completedStep = action == FractalTutorialAction::ZoomToolDrag;
            break;
        case FractalTutorialStep::PointPicker:
            if (action == FractalTutorialAction::PointPickerSelected)
            {
                _pointPickerSelected = true;
                return false;
            }
            completedStep = action == FractalTutorialAction::SkipPointPicker
                || (_pointPickerSelected && action == FractalTutorialAction::PointPickerHovered);
            break;
        case FractalTutorialStep::ColorAnimation:
            completedStep = action == FractalTutorialAction::ColorAnimationToggled;
            break;
        case FractalTutorialStep::FractalInformation:
            completedStep = action == FractalTutorialAction::FractalInformationOpened;
            break;
        case FractalTutorialStep::Completed:
            return false;
    }

    if (!completedStep)
        return false;

    _step = static_cast<FractalTutorialStep>(static_cast<int>(_step) + 1);
    _pointPickerSelected = false;
    return true;
}

FractalTutorialStartupAction FractalTutorialModel::GetStartupAction(
    const TutorialStatus status, const bool showWelcomeOnStartup)
{
    if (status == TutorialStatus::Pending)
    {
        return showWelcomeOnStartup
            ? FractalTutorialStartupAction::OpenWelcomeThenTutorial
            : FractalTutorialStartupAction::StartTutorial;
    }
    if (status == TutorialStatus::Completed && showWelcomeOnStartup)
        return FractalTutorialStartupAction::OpenWelcomeThenSummary;
    if (status == TutorialStatus::Dismissed && showWelcomeOnStartup)
        return FractalTutorialStartupAction::OpenWelcome;
    return FractalTutorialStartupAction::None;
}

TutorialStatus FractalTutorialModel::GetStatusAfterAbort(
    const TutorialStatus savedStatus, const bool automaticRun)
{
    return automaticRun ? TutorialStatus::Dismissed : savedStatus;
}
