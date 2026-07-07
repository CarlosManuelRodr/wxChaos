#pragma once
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/bmpbndl.h>
#include "canvas/FractalCanvas.h"

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
    wxStaticText* _colorRotateSpeedText;
    wxSpinCtrlDouble* _colorSpeedCtrl;

    FractalCanvas* _fractalCanvasPtr;
    FractalFactory _fractalFactory;
    int _previewWidth;
    int _previewHeight;
    int _recordingWidth;
    int _recordingHeight;

    PreciseRect _outermostZoom, _innermostZoom;

    void OnScrollPreview(wxScrollEvent& event);
    void OnSaveVideo(wxCommandEvent&);
    void OnCancel(wxCommandEvent&);
    void OnUpdateTotalFrames(wxSpinEvent&);
    void OnColorRotate(wxCommandEvent&);
    void OnChangeSpeedDbl(wxSpinDoubleEvent&);

    void CreateFractalFactory();
    void InitializeRenderSizes();
    void RenderPreview(int zoom, double colorSpeed = -1.0) const;
    void RenderPreview();
    void UpdateTotalFrames();
    [[nodiscard]] int GetTotalFrames() const;
    [[nodiscard]] static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                                      const wxString& darkIcon);
    [[nodiscard]] static wxBitmapBundle CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                                         const wxSize& size);
    void SetButtonIcon(wxButton* button, const wxString& lightIcon, const wxString& darkIcon) const;
    static void CreateFractalInstance(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, int width, int height);
    static PreciseRect CreateRecordingFractal(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, int width, int height);
public:
    ZoomRecorder(FractalCanvas* fractalCanvas, wxWindow* parent, wxWindowID id = wxID_ANY,
                 const wxString& title = wxTRANSLATE("Zoom recorder"),
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(900, 420),
                 long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ZoomRecorder() override;

    static Rect GetDefaultView(FractalCanvas* fractalCanvas, int width, int height);
};
