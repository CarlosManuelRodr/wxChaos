#pragma once
#include <wx/xrc/xmlres.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include "FractalCanvas.h"

/**
* @class ZoomRecorder
* @brief Configures and previews a zoom animation export.
*
* ZoomRecorder samples the current canvas zoom range, renders preview frames for
* the selected timing, rotation, and color-cycling options, then starts video
* generation when the user saves.
*/
class ZoomRecorder : public wxDialog
{
    wxPanel* _panel;
    wxStaticBitmap* _previewBitmap;
    wxStaticText* _previewFrameText;
    wxSlider* _previewSlider;
    wxButton* _saveButton;
    wxButton* _cancelButton;
    wxStaticText* _videoDurationText;
    wxSpinCtrl* _minutesSpinCtrl;
    wxStaticText* _minutesText;
    wxSpinCtrl* _secondsSpinCtrl;
    wxStaticText* _secondsText;
    wxStaticText* _framerateText;
    wxSpinCtrl* _framerateSpinCtrl;
    wxStaticText* _framesPerSecondText;
    wxCheckBox* _rotateCheckbox;
    wxStaticText* _zoomSpeedText;
    wxSpinCtrl* _zoomSpeedCtrl;
    wxStaticText* _colorRotateSpeedText;
    wxSpinCtrlDouble* _colorSpeedCtrl;

    FractalCanvas* _fractalCanvasPtr;
    FractalFactory _fractalFactory;

    Rect _outermostZoom, _innermostZoom;

    void OnScrollPreview(wxScrollEvent& event);
    void OnSaveVideo(wxCommandEvent&);
    void OnCancel(wxCommandEvent&);
    void OnUpdateTotalFrames(wxSpinEvent&);
    void OnColorRotate(wxCommandEvent&);
    void OnChangeSpeed(wxSpinEvent&);
    void OnChangeSpeedDbl(wxSpinDoubleEvent&);

    void CreateFractalFactory();
    void RenderPreview(int zoom, int zoomSpeed = 1, double colorSpeed = -1.0) const;
    void RenderPreview();
    void UpdateTotalFrames();
    [[nodiscard]] int GetTotalFrames() const;
public:
    ZoomRecorder(FractalCanvas* fractalCanvas, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Zoom recorder",
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(960, 720),
                 long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ZoomRecorder() override;
};
