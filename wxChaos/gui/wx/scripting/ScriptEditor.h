#pragma once
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/stc/stc.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/statbmp.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <vector>
#include "AngelscriptBindings.h"

wxDECLARE_EVENT(wxEVT_SCRIPT_EDITOR_CLOSED, wxCommandEvent);

/**
* @class ScriptNameDialog
* @brief Prompts for the name of a new user script.
*/
class ScriptNameDialog : public wxDialog
{
    wxStaticText* _scriptNameText;
    wxTextCtrl* _scriptNameCtrl;
    wxStdDialogButtonSizer* _buttonsSizer;
    wxButton* _buttonsSizerOk;
    wxButton* _buttonsSizerCancel;

    void OnCancel(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
public:
    explicit ScriptNameDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 220), long style = wxDEFAULT_DIALOG_STYLE);
    ~ScriptNameDialog() override;
    [[nodiscard]] wxString GetScriptName() const;
};

/**
* @class ScriptEditor
* @brief Editor and validation tool for user AngelScript fractals.
*
* ScriptEditor lists saved user scripts, edits scripts with a styled code
* control, validates configuration/render entry points, and shows console and
* preview output for debugging script fractals.
*/
class ScriptEditor : public wxFrame
{
    wxPanel* _mainPanel;
    wxPanel* _scriptPanel;
    wxListBox* _scriptsListBox;
    wxButton* _saveChangesButton;
    wxButton* _newButton;
    wxButton* _removeButton;
    wxButton* _closeButton;
    wxStyledTextCtrl* _codeEditor;
    wxPanel* _debugPanel;
    wxButton* _validateButton;
    wxButton* _runButton;
    wxButton* _clearConsoleButton;
    wxRichTextCtrl* _console;
    wxStaticBitmap* _renderPreviewBitmap;

    std::vector<ScriptData> _loadedScripts;
    int _currentScriptIndex;

    void SetUpLexer() const;
    void FetchUserScripts();
    void LoadScript(unsigned index);
    void ConsoleWriteText(const wxString& text, const wxColour& color) const;
    void ConsoleSetWelcomeText() const;
    void ConsoleClear() const;
    void ConsoleAppendEntry(const wxString& action, const wxString& message, bool success) const;
    [[nodiscard]] wxString ConsoleTimestamp() const;
    [[nodiscard]] wxString FormatConsoleMessage(wxString message) const;
    [[nodiscard]] wxColour ConsoleTextColor() const;
    [[nodiscard]] wxColour ConsoleMutedColor() const;
    [[nodiscard]] wxColour ConsoleActionColor() const;
    [[nodiscard]] wxColour ConsoleSuccessColor() const;
    [[nodiscard]] wxColour ConsoleErrorColor() const;
    void SetBlackPreview() const;
    [[nodiscard]] int GetScriptIndex(const wxString& scriptName) const;
    wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                 const wxString& darkIcon) const;
    void SetButtonIcon(wxButton* button, const wxString& lightIcon, const wxString& darkIcon) const;

    void OnSelectScript(wxCommandEvent& event);
    void OnSaveChanges(wxCommandEvent& event);
    void OnNewScript(wxCommandEvent& event);
    void OnDeleteScript(wxCommandEvent& event);
    void OnCloseButton(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnCodeChange(wxKeyEvent& event);
    void OnValidateScript(wxCommandEvent& event);
    void OnRunScript(wxCommandEvent& event);
    void OnClearConsole(wxCommandEvent& event);
public:
    explicit ScriptEditor(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxTRANSLATE("Script editor"),
                          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1400, 900),
                          long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ScriptEditor() override;
};
