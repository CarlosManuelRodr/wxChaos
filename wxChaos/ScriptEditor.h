#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stc/stc.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/statbmp.h>
#include <wx/collpane.h>
#include <wx/frame.h>


class ScriptEditor : public wxFrame
{
private:
    wxPanel* mainPanel;
    wxPanel* scriptPanel;
    wxListBox* scriptsListBox;
    wxButton* saveChangesButton;
    wxButton* newButton;
    wxButton* removeButton;
    wxButton* closeButton;
    wxStyledTextCtrl* codeEditor;
    wxCollapsiblePane* debugCollapsiblePane;
    wxPanel* debugPanel;
    wxButton* validateButton;
    wxButton* runButton;
    wxButton* helpButton;
    wxRichTextCtrl* console;
    wxStaticBitmap* renderPreviewBitmap;
    bool* isActive;

    void GetUserScripts();

    void OnSaveChanges(wxCommandEvent& event);
    void OnNewScript(wxCommandEvent& event);
    void OnDeleteScript(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnCodeChange(wxKeyEvent& event);
    void OnValidateScript(wxCommandEvent& event);
    void OnRunScript(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
public:

    ScriptEditor(bool* active, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Script editor"),
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1069, 600),
                 long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ScriptEditor();
};