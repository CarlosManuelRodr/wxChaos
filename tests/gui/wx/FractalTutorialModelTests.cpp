#include <doctest/doctest.h>
#include "canvas/FractalTutorialModel.h"

TEST_CASE("Fractal tutorial accepts only the action required by each step")
{
    FractalTutorialModel model;
    CHECK(model.GetStep() == FractalTutorialStep::CursorZoomIn);
    CHECK_FALSE(model.HandleAction(FractalTutorialAction::ZoomBack));
    CHECK(model.HandleAction(FractalTutorialAction::SelectionZoom));
    CHECK(model.GetStep() == FractalTutorialStep::CursorZoomBack);
    CHECK(model.HandleAction(FractalTutorialAction::ZoomBack));
    CHECK(model.GetStep() == FractalTutorialStep::CursorPan);
}

TEST_CASE("Fractal tutorial accepts either cursor panning method")
{
    FractalTutorialModel mouseModel;
    mouseModel.HandleAction(FractalTutorialAction::SelectionZoom);
    mouseModel.HandleAction(FractalTutorialAction::ZoomBack);
    CHECK(mouseModel.HandleAction(FractalTutorialAction::MiddleMousePan));

    FractalTutorialModel keyboardModel;
    keyboardModel.HandleAction(FractalTutorialAction::SelectionZoom);
    keyboardModel.HandleAction(FractalTutorialAction::ZoomBack);
    CHECK(keyboardModel.HandleAction(FractalTutorialAction::KeyboardPan));
}

TEST_CASE("Fractal tutorial requires Point Picker selection before hover")
{
    FractalTutorialModel model;
    model.HandleAction(FractalTutorialAction::SelectionZoom);
    model.HandleAction(FractalTutorialAction::ZoomBack);
    model.HandleAction(FractalTutorialAction::KeyboardPan);
    model.HandleAction(FractalTutorialAction::HandPan);
    model.HandleAction(FractalTutorialAction::ZoomToolDrag);

    CHECK(model.GetStep() == FractalTutorialStep::PointPicker);
    CHECK_FALSE(model.HandleAction(FractalTutorialAction::PointPickerHovered));
    CHECK_FALSE(model.HandleAction(FractalTutorialAction::PointPickerSelected));
    CHECK(model.HandleAction(FractalTutorialAction::PointPickerHovered));
}

TEST_CASE("Fractal tutorial optional Point Picker step can be skipped")
{
    FractalTutorialModel model;
    model.HandleAction(FractalTutorialAction::SelectionZoom);
    model.HandleAction(FractalTutorialAction::ZoomBack);
    model.HandleAction(FractalTutorialAction::MiddleMousePan);
    model.HandleAction(FractalTutorialAction::HandPan);
    model.HandleAction(FractalTutorialAction::ZoomToolDrag);

    CHECK(model.HandleAction(FractalTutorialAction::SkipPointPicker));
    CHECK(model.GetStep() == FractalTutorialStep::ColorAnimation);
}

TEST_CASE("Fractal tutorial completes its full ordered sequence")
{
    FractalTutorialModel model;
    CHECK(model.HandleAction(FractalTutorialAction::SelectionZoom));
    CHECK(model.HandleAction(FractalTutorialAction::ZoomBack));
    CHECK(model.HandleAction(FractalTutorialAction::KeyboardPan));
    CHECK(model.HandleAction(FractalTutorialAction::HandPan));
    CHECK(model.HandleAction(FractalTutorialAction::ZoomToolDrag));
    CHECK(model.HandleAction(FractalTutorialAction::SkipPointPicker));
    CHECK(model.HandleAction(FractalTutorialAction::ColorAnimationToggled));
    CHECK_FALSE(model.HandleAction(FractalTutorialAction::ColorAnimationToggled));
    CHECK(model.HandleAction(FractalTutorialAction::FractalInformationOpened));
    CHECK(model.IsCompleted());
    CHECK_FALSE(model.HandleAction(FractalTutorialAction::FractalInformationOpened));
}

TEST_CASE("Fractal tutorial startup and abort lifecycle follows saved preferences")
{
    CHECK(FractalTutorialModel::GetStartupAction(TutorialStatus::Pending, true)
          == FractalTutorialStartupAction::OpenWelcomeThenTutorial);
    CHECK(FractalTutorialModel::GetStartupAction(TutorialStatus::Pending, false)
          == FractalTutorialStartupAction::StartTutorial);
    CHECK(FractalTutorialModel::GetStartupAction(TutorialStatus::Completed, true)
          == FractalTutorialStartupAction::OpenWelcomeThenSummary);
    CHECK(FractalTutorialModel::GetStartupAction(TutorialStatus::Completed, false)
          == FractalTutorialStartupAction::None);
    CHECK(FractalTutorialModel::GetStartupAction(TutorialStatus::Dismissed, true)
          == FractalTutorialStartupAction::OpenWelcome);

    CHECK(FractalTutorialModel::GetStatusAfterAbort(TutorialStatus::Pending, true)
          == TutorialStatus::Dismissed);
    CHECK(FractalTutorialModel::GetStatusAfterAbort(TutorialStatus::Completed, false)
          == TutorialStatus::Completed);
}
