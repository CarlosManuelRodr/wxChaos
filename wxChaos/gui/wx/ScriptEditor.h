#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/stc/stc.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/statbmp.h>
#include <wx/collpane.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <vector>
#include "AngelscriptBindings.h"

class ScriptNameDialog : public wxDialog
{
    wxStaticText* scriptNameText;
    wxTextCtrl* scriptNameCtrl;
    wxStdDialogButtonSizer* buttonsSizer;
    wxButton* buttonsSizerOK;
    wxButton* buttonsSizerCancel;

    void OnCancel(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
public:
    explicit ScriptNameDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 220), long style = wxDEFAULT_DIALOG_STYLE);
    ~ScriptNameDialog() override;
    wxString GetScriptName() const;
};

class ScriptEditor : public wxFrame
{
    wxPanel* mainPanel;
    wxPanel* scriptPanel;
    wxListBox* scriptsListBox;
    wxButton* saveChangesButton;
    wxButton* newButton;
    wxButton* removeButton;
    wxButton* closeButton;
    wxStyledTextCtrl* codeEditor;
    wxGenericCollapsiblePane* debugCollapsiblePane;
    wxPanel* debugPanel;
    wxButton* validateButton;
    wxButton* runButton;
    wxRichTextCtrl* console;
    wxStaticBitmap* renderPreviewBitmap;
    bool* isActive;

    std::vector<ScriptData> loadedScripts;
    int currentScriptIndex;
    int debugCollapsiblePaneBestHeight;

    void SetUpLexer() const;
    void FetchUserScripts();
    void LoadScript(unsigned index);
    void ConsoleSetText(const wxString& text) const;
    void ConsoleSetWelcomeText() const;
    void ConsolePrepareInput(const wxString& command) const;
    void ConsolePrepareOutput() const;
    void SetBlackPreview() const;
    int GetScriptIndex(const wxString& scriptName) const;

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
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1400, 900),
                 long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ScriptEditor() override;
};
