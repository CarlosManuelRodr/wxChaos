#include "canvas/FractalTutorialOverlay.h"
#include <algorithm>
#include <utility>
#include <wx/button.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include "AppPaths.h"
#include "common/AppTheme.h"

FractalTutorialOverlay::FractalTutorialOverlay(wxWindow* canvas)
    : _canvas(canvas),
      _instructionPopup(new wxPopupWindow(canvas, wxBORDER_SIMPLE | wxPU_CONTAINS_CONTROLS)),
      _skipPopup(new wxPopupWindow(canvas, wxBORDER_SIMPLE)),
      _checkPopup(new wxPopupWindow(canvas, wxBORDER_NONE))
{
    const wxColour background = AppTheme::IsDark() ? wxColour(31, 34, 40) : wxColour(250, 250, 252);
    const wxColour foreground = AppTheme::Foreground();
    _instructionPopup->SetBackgroundColour(background);
    _skipPopup->SetBackgroundColour(background);
    _checkPopup->SetBackgroundColour(background);

    const auto cardSizer = new wxBoxSizer(wxVERTICAL);
    _stepText = new wxStaticText(_instructionPopup, wxID_ANY, wxEmptyString);
    _stepText->SetForegroundColour(foreground);
    cardSizer->Add(_stepText, 0, wxLEFT | wxRIGHT | wxTOP, 12);

    const auto headingSizer = new wxBoxSizer(wxHORIZONTAL);
    _toolImage = new wxStaticBitmap(_instructionPopup, wxID_ANY, wxNullBitmap);
    _titleText = new wxStaticText(_instructionPopup, wxID_ANY, wxEmptyString);
    wxFont titleFont = _titleText->GetFont();
    titleFont.MakeBold();
    titleFont.Scale(1.15);
    _titleText->SetFont(titleFont);
    _titleText->SetForegroundColour(foreground);
    headingSizer->Add(_toolImage, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    headingSizer->Add(_titleText, 1, wxALIGN_CENTER_VERTICAL);
    cardSizer->Add(headingSizer, 0, wxEXPAND | wxALL, 12);

    _bodyText = new wxStaticText(_instructionPopup, wxID_ANY, wxEmptyString);
    _bodyText->SetForegroundColour(foreground);
    _bodyText->Wrap(370);
    cardSizer->Add(_bodyText, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);

    const auto gestureSizer = new wxBoxSizer(wxHORIZONTAL);
    _gestureImage = new wxStaticBitmap(_instructionPopup, wxID_ANY, wxNullBitmap);
    _secondaryGestureImage = new wxStaticBitmap(_instructionPopup, wxID_ANY, wxNullBitmap);
    gestureSizer->Add(_gestureImage, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    gestureSizer->Add(_secondaryGestureImage, 0, wxALIGN_CENTER_VERTICAL);
    cardSizer->Add(gestureSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxBOTTOM, 12);

    _actionHeadingText = new wxStaticText(_instructionPopup, wxID_ANY, _("To continue:"));
    wxFont actionHeadingFont = _actionHeadingText->GetFont();
    actionHeadingFont.MakeBold();
    _actionHeadingText->SetFont(actionHeadingFont);
    _actionHeadingText->SetForegroundColour(foreground);
    cardSizer->Add(_actionHeadingText, 0, wxLEFT | wxRIGHT, 12);

    _actionText = new wxStaticText(_instructionPopup, wxID_ANY, wxEmptyString);
    _actionText->SetForegroundColour(foreground);
    _actionText->Wrap(370);
    cardSizer->Add(_actionText, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 12);

    _skipStepButton = new wxButton(_instructionPopup, wxID_ANY, _("Skip this step"));
    _skipStepButton->Hide();
    cardSizer->Add(_skipStepButton, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    _instructionPopup->SetSizer(cardSizer);
    _skipStepButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        if (_skipStepHandler)
            _skipStepHandler();
    });

    const auto skipSizer = new wxBoxSizer(wxVERTICAL);
    _skipText = new wxStaticText(
        _skipPopup, wxID_ANY,
        _("Press Esc to skip. You can play the tutorial again from the Help menu."));
    _skipText->SetForegroundColour(foreground);
    skipSizer->Add(_skipText, 0, wxALL, 8);
    _skipPopup->SetSizerAndFit(skipSizer);

    const auto checkSizer = new wxBoxSizer(wxVERTICAL);
    const auto checkImage = new wxStaticBitmap(
        _checkPopup, wxID_ANY, LoadBitmap("checkmark.png", _canvas->FromDIP(wxSize(72, 72))));
    checkSizer->Add(checkImage);
    _checkPopup->SetSizerAndFit(checkSizer);
}

FractalTutorialOverlay::~FractalTutorialOverlay()
{
    _instructionPopup->Destroy();
    _skipPopup->Destroy();
    _checkPopup->Destroy();
}

wxBitmap FractalTutorialOverlay::LoadBitmap(const wxString& filename, const wxSize& maximumSize) const
{
    const wxString path = AppPaths::ResourceFile({filename});
    wxImage image(path);
    if (!image.IsOk())
    {
        wxLogWarning("Could not load tutorial image: %s", path);
        return wxNullBitmap;
    }

    const double scale = std::min({
        1.0,
        static_cast<double>(maximumSize.GetWidth()) / image.GetWidth(),
        static_cast<double>(maximumSize.GetHeight()) / image.GetHeight()
    });
    return wxBitmap(image.Scale(
        std::max(1, static_cast<int>(image.GetWidth() * scale)),
        std::max(1, static_cast<int>(image.GetHeight() * scale)),
        wxIMAGE_QUALITY_HIGH));
}

void FractalTutorialOverlay::SetSkipStepHandler(std::function<void()> handler)
{
    _skipStepHandler = std::move(handler);
}

void FractalTutorialOverlay::SetImages(const wxString& tool, const wxString& gesture,
                                       const wxString& secondaryGesture)
{
    _toolImage->SetBitmap(LoadBitmap(tool, _canvas->FromDIP(wxSize(64, 64))));
    _gestureImage->SetBitmap(
        gesture.empty() ? wxNullBitmap : LoadBitmap(gesture, _canvas->FromDIP(wxSize(180, 125))));
    _secondaryGestureImage->SetBitmap(
        secondaryGesture.empty()
            ? wxNullBitmap
            : LoadBitmap(secondaryGesture, _canvas->FromDIP(wxSize(150, 115))));
    _gestureImage->Show(!gesture.empty());
    _secondaryGestureImage->Show(!secondaryGesture.empty());
}

void FractalTutorialOverlay::SetCard(const wxString& step, const wxString& title, const wxString& body,
                                     const wxString& action)
{
    _stepText->SetLabel(step);
    _titleText->SetLabel(title);
    _bodyText->SetLabel(body);
    _bodyText->Wrap(_canvas->FromDIP(370));
    _actionText->SetLabel(action);
    _actionText->Wrap(_canvas->FromDIP(370));
    _actionHeadingText->Show(!action.empty());
    _actionText->Show(!action.empty());
}

void FractalTutorialOverlay::ShowStep(const FractalTutorialStep step)
{
    _titleText->SetForegroundColour(AppTheme::Foreground());
    _bodyText->SetForegroundColour(AppTheme::Foreground());
    _actionHeadingText->SetForegroundColour(AppTheme::Foreground());
    _actionText->SetForegroundColour(AppTheme::Foreground());
    const int stepNumber = static_cast<int>(step) + 1;
    const wxString progress = wxString::Format(_("Step %d of 8"), stepNumber);
    _skipStepButton->Show(step == FractalTutorialStep::PointPicker);

    switch (step)
    {
        case FractalTutorialStep::CursorZoomIn:
            SetCard(progress, _("Cursor: zoom in"),
                    _("The Cursor is the default tool. Hold the left mouse button and drag a rectangle around "
                      "the area you want to explore."),
                    _("Left-click and drag a selection rectangle on the fractal."));
            SetImages("cursor_button.png", "ClickDragSelection.png");
            break;
        case FractalTutorialStep::CursorZoomBack:
            SetCard(progress, _("Cursor: zoom back"),
                    _("Right-clicking returns the fractal to the previous view."),
                    _("Right-click the fractal to zoom back."));
            SetImages("cursor_button.png", "mouse.png");
            break;
        case FractalTutorialStep::CursorPan:
            SetCard(progress, _("Cursor: move around"),
                    _("You can move around the fractal with the middle mouse button, the Arrow keys, or WASD."),
                    _("Pan the view using a middle-button drag, an Arrow key, or a WASD key."));
            SetImages("cursor_button.png", "keyboard.png", "mouse_middle_click.png");
            break;
        case FractalTutorialStep::HandPan:
            SetCard(progress, _("Hand tool"),
                    _("The Hand is an optional convenience for panning. Select it, then hold the left mouse button "
                      "and drag the fractal."),
                    _("Select the Hand tool, then left-click and drag the fractal."));
            SetImages("hand_button.png");
            break;
        case FractalTutorialStep::ZoomTool:
            SetCard(progress, _("Zoom tool"),
                    _("The Zoom tool provides a smooth way to zoom in or out by dragging vertically."),
                    _("Select the Zoom tool, then left-click and drag up or down."));
            SetImages("zoom_button.png");
            break;
        case FractalTutorialStep::PointPicker:
            SetCard(progress, _("Point Picker"),
                    _("Select the Point Picker and move over the canvas to inspect a point's trajectory and result. "
                      "It is useful when exploring the mathematics, but you may skip this step."),
                    _("Select the Point Picker and move the pointer over the canvas, or choose Skip this step."));
            SetImages("picker_button.png");
            break;
        case FractalTutorialStep::ColorAnimation:
            SetCard(progress, _("Color animation"),
                    _("Color animation is an optional toy for a trippy viewing experience."),
                    _("Click the Play button to start the color animation."));
            SetImages("play_button.png");
            break;
        case FractalTutorialStep::FractalInformation:
            SetCard(progress, _("Fractal information"),
                    _("Every supported fractal has its own interesting museum page."),
                    _("Click the Information button to open this fractal's museum page and finish the tutorial."));
            SetImages("help_button.png");
            break;
        case FractalTutorialStep::Completed:
            ShowFinal();
            return;
    }

    _instructionPopup->Fit();
    _instructionPopup->Show();
    _skipPopup->Show();
    Reposition();
}

void FractalTutorialOverlay::ShowSuccess()
{
    _checkPopup->Show();
    Reposition();
}

void FractalTutorialOverlay::HideSuccess()
{
    _checkPopup->Hide();
}

void FractalTutorialOverlay::ShowFinal()
{
    _skipStepButton->Hide();
    SetCard(wxEmptyString, _("You're all set"), _("Have fun exploring wxChaos!"));
    SetImages("checkmark.png");
    _instructionPopup->Fit();
    _instructionPopup->Show();
    _skipPopup->Hide();
    _checkPopup->Hide();
    Reposition();
}

void FractalTutorialOverlay::ShowSkipped()
{
    _skipStepButton->Hide();
    SetCard(wxEmptyString, _("Tutorial skipped"),
            _("You can play it again at any time from the Help menu."));
    SetImages("cursor_button.png");
    _instructionPopup->Fit();
    _instructionPopup->Show();
    _skipPopup->Hide();
    _checkPopup->Hide();
    Reposition();
}

void FractalTutorialOverlay::SetSkippedFade(const double progress)
{
    const wxColour background = _instructionPopup->GetBackgroundColour();
    const wxColour foreground = AppTheme::Foreground();
    const double visible = std::clamp(1.0 - progress, 0.0, 1.0);
    const auto blend = [visible](const unsigned char front, const unsigned char back)
    {
        return static_cast<unsigned char>(back + (front - back) * visible);
    };
    const wxColour color(
        blend(foreground.Red(), background.Red()),
        blend(foreground.Green(), background.Green()),
        blend(foreground.Blue(), background.Blue()));
    _titleText->SetForegroundColour(color);
    _bodyText->SetForegroundColour(color);
    _instructionPopup->Refresh();
}

void FractalTutorialOverlay::HideAll()
{
    _instructionPopup->Hide();
    _skipPopup->Hide();
    _checkPopup->Hide();
}

void FractalTutorialOverlay::Reposition() const
{
    if (_canvas == nullptr || !_instructionPopup->IsShown() && !_skipPopup->IsShown() && !_checkPopup->IsShown())
        return;

    const wxSize canvasSize = _canvas->GetClientSize();
    const int margin = _canvas->FromDIP(12);
    const wxPoint origin = _canvas->ClientToScreen(wxPoint(0, 0));
    _instructionPopup->Move(origin + wxPoint(margin, margin));
    _skipPopup->Move(origin + wxPoint(
        margin, std::max(margin, canvasSize.GetHeight() - _skipPopup->GetSize().GetHeight() - margin)));
    _checkPopup->Move(origin + wxPoint(
        std::max(margin, canvasSize.GetWidth() - _checkPopup->GetSize().GetWidth() - margin),
        std::max(margin, canvasSize.GetHeight() - _checkPopup->GetSize().GetHeight() - margin)));
}
