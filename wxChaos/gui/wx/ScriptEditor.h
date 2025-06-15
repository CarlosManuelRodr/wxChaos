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
#include <wx/stattext.h>
#include <wx/stc/stc.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/statbmp.h>
#include <wx/collpane.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <vector>
#include "AngelscriptEngine.h"

class ScriptNameDialog : public wxDialog
{
private:
    wxStaticText* scriptNameText;
    wxTextCtrl* scriptNameCtrl;
    wxStdDialogButtonSizer* buttonsSizer;
    wxButton* buttonsSizerOK;
    wxButton* buttonsSizerCancel;

    void OnCancel(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
public:
    ScriptNameDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, 
                     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(494, 158), long style = wxDEFAULT_DIALOG_STYLE);
    ~ScriptNameDialog();
    wxString GetScriptName();
};

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
    wxRichTextCtrl* console;
    wxStaticBitmap* renderPreviewBitmap;
    bool* isActive;

    std::vector<ScriptData> loadedScripts;
    int currentScriptIndex;

    void SetUpLexer();
    void FetchUserScripts();
    void LoadScript(unsigned index);
    void ConsoleSetText(wxString text);
    void ConsoleSetWelcomeText();
    void ConsolePrepareInput(wxString command);
    void ConsolePrepareOutput();
    void SetBlackPreview();
    int GetScriptIndex(wxString scriptName);

    void OnSelectScript(wxCommandEvent& event);
    void OnSaveChanges(wxCommandEvent& event);
    void OnNewScript(wxCommandEvent& event);
    void OnDeleteScript(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnCodeChange(wxKeyEvent& event);
    void OnValidateScript(wxCommandEvent& event);
    void OnRunScript(wxCommandEvent& event);
    void OnDebugPanel(wxCollapsiblePaneEvent& event);
public:
    ScriptEditor(bool* active, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Script editor"),
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1069, 600),
                 long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ScriptEditor();
};