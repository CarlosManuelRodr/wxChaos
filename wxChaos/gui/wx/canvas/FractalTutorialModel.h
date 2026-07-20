#pragma once
#include "config/TutorialStatus.h"

enum class FractalTutorialStep
{
    CursorZoomIn,
    CursorZoomBack,
    CursorPan,
    HandPan,
    ZoomTool,
    PointPicker,
    ColorAnimation,
    FractalInformation,
    Completed
};

enum class FractalTutorialAction
{
    SelectionZoom,
    ZoomBack,
    MiddleMousePan,
    KeyboardPan,
    HandPan,
    ZoomToolDrag,
    PointPickerSelected,
    PointPickerHovered,
    SkipPointPicker,
    FractalInformationOpened,
    ColorAnimationToggled
};

enum class FractalTutorialStartupAction
{
    None,
    OpenWelcome,
    OpenWelcomeThenTutorial,
    StartTutorial,
    OpenWelcomeThenSummary
};

/**
* @class FractalTutorialModel
* @brief UI-independent state machine for the guided fractal tutorial.
*/
class FractalTutorialModel
{
    FractalTutorialStep _step{FractalTutorialStep::CursorZoomIn};
    bool _pointPickerSelected{};

public:
    void Reset();
    [[nodiscard]] FractalTutorialStep GetStep() const;
    [[nodiscard]] bool IsCompleted() const;
    bool HandleAction(FractalTutorialAction action);

    [[nodiscard]] static FractalTutorialStartupAction GetStartupAction(
        TutorialStatus status, bool showWelcomeOnStartup);
    [[nodiscard]] static TutorialStatus GetStatusAfterAbort(
        TutorialStatus savedStatus, bool automaticRun);
};
