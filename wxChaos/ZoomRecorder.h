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


class ZoomRecorder : public wxFrame
{
private:
    wxPanel* panel;
    wxStaticBitmap* previewBitmap;
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
    wxStaticText* resolutionText;
    wxChoice* resolutionChoice;
    wxCheckBox* rotateCheckbox;
    bool* isActive;

    void OnScrollPreview(wxScrollEvent& event);
    void OnSaveVideo(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
public:
    ZoomRecorder(bool* active, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Zoom recorder"),
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(707, 427), 
                 long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ZoomRecorder();
};