#pragma once

#include <functional>
#include <wx/popupwin.h>
#include "canvas/FractalTutorialModel.h"

class wxButton;
class wxStaticBitmap;
class wxStaticText;

/**
* @class FractalTutorialOverlay
* @brief Lightweight tutorial cards positioned over a FractalCanvas.
*/
class FractalTutorialOverlay
{
    wxWindow* _canvas;
    wxPopupWindow* _instructionPopup;
    wxPopupWindow* _skipPopup;
    wxPopupWindow* _checkPopup;
    wxStaticText* _stepText;
    wxStaticText* _titleText;
    wxStaticText* _bodyText;
    wxStaticBitmap* _toolImage;
    wxStaticBitmap* _gestureImage;
    wxStaticBitmap* _secondaryGestureImage;
    wxButton* _skipStepButton;
    wxStaticText* _skipText;
    std::function<void()> _skipStepHandler;

    wxBitmap LoadBitmap(const wxString& filename, const wxSize& maximumSize) const;
    void SetImages(const wxString& tool, const wxString& gesture = wxEmptyString,
                   const wxString& secondaryGesture = wxEmptyString);
    void SetCard(const wxString& step, const wxString& title, const wxString& body);

public:
    explicit FractalTutorialOverlay(wxWindow* canvas);
    ~FractalTutorialOverlay();

    void SetSkipStepHandler(std::function<void()> handler);
    void ShowStep(FractalTutorialStep step);
    void ShowSuccess();
    void HideSuccess();
    void ShowFinal();
    void ShowSkipped();
    void SetSkippedFade(double progress);
    void HideAll();
    void Reposition() const;
};
