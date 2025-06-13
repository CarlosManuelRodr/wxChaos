#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/slider.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include "FractalClasses.h"
#include "FractalCanvas.h"

/**
* @class ZoomRecorder
* @brief Creates a dialog to preview and select the zoom recorder parameters.
*/
class ZoomRecorder : public wxDialog
{
private:
    wxPanel* panel;
    wxStaticBitmap* previewBitmap;
    wxStaticText* previewFrameText;
    wxSlider* previewSlider;
    wxButton* saveButton;
    wxButton* cancelButton;
    wxStaticText* videoDurationText;
    wxSpinCtrl* minutesSpinCtrl;
    wxStaticText* minutesText;
    wxSpinCtrl* secondsSpinCtrl;
    wxStaticText* secondsText;
    wxStaticText* framerateText;
    wxSpinCtrl* framerateSpinCtrl;
    wxStaticText* framesPerSecondText;
    wxCheckBox* rotateCheckbox;
    wxStaticText* zoomSpeedText;
    wxSpinCtrl* zoomSpeedCtrl;
    wxStaticText* colorRotateSpeedText;
    wxSpinCtrlDouble* colorSpeedCtrl;

    FractalCanvas* fractalCanvasPtr;
    FractalHandler fractalHandler;

    Rect outermostZoom, innermostZoom;

    void OnScrollPreview(wxScrollEvent& event);
    void OnSaveVideo(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnUpdateTotalFrames(wxSpinEvent& event);
    void OnColorRotate(wxCommandEvent& event);
    void OnChangeSpeed(wxSpinEvent& event);
    void OnChangeSpeedDbl(wxSpinDoubleEvent& event);

    void CreateFractalHandler();
    void RenderPreview(int zoom, int zoomSpeed = 1, double colorSpeed = -1.0);
    void RenderPreview();
    void UpdateTotalFrames();
    int CalculateTotalFrames();
public:
    ZoomRecorder(FractalCanvas* mFCanvas, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Zoom recorder"),
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 400),
                 long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ZoomRecorder();
};