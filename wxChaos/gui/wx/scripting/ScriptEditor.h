#pragma once
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/stc/stc.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/statbmp.h>
#include <wx/frame.h>
#include <vector>
#include "AngelscriptBindings.h"

wxDECLARE_EVENT(wxEVT_SCRIPT_EDITOR_CLOSED, wxCommandEvent);

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
    wxButton* _newFromSampleButton;
    wxButton* _removeButton;
    wxButton* _closeButton;
    wxButton* _openScriptsDirectoryButton;
    wxButton* _documentationButton; ///< Opens the bundled AngelScript scripting guide.
    wxStyledTextCtrl* _codeEditor;
    wxPanel* _debugPanel;
    wxButton* _validateButton;
    wxButton* _previewButton;
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
    [[nodiscard]] static wxString ConsoleTimestamp();
    [[nodiscard]] static wxString FormatConsoleMessage(wxString message);
    [[nodiscard]] static wxColour ConsoleTextColor();
    [[nodiscard]] static wxColour ConsoleMutedColor();
    [[nodiscard]] static wxColour ConsoleActionColor();
    [[nodiscard]] static wxColour ConsoleSuccessColor();
    [[nodiscard]] static wxColour ConsoleErrorColor();
    void SetBlackPreview() const;
    [[nodiscard]] int GetScriptIndex(const wxString& scriptName) const;
    static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                        const wxString& darkIcon);
    static void SetButtonIcon(wxButton* button, const wxString& lightIcon, const wxString& darkIcon);

    void OnSelectScript(wxCommandEvent& event);
    void OnSaveChanges(wxCommandEvent& event);
    void OnNewScript(wxCommandEvent& event);
    void OnNewFromSample(wxCommandEvent& event);
    void OnDeleteScript(wxCommandEvent& event);
    void OnCloseButton(wxCommandEvent& event);
    void OnOpenScriptsDirectory(wxCommandEvent& event); ///< Opens the user scripts directory in the system file manager.
    void OnDocumentation(wxCommandEvent& event);        ///< Opens the scripting tutorial in a modeless documentation viewer.
    void OnClose(wxCloseEvent& event);
    void OnCodeChange(wxKeyEvent& event);
    void OnValidateScript(wxCommandEvent& event);
    void OnPreviewScript(wxCommandEvent& event);
    void OnRunScript(wxCommandEvent& event);
    void OnClearConsole(wxCommandEvent& event);
public:
    explicit ScriptEditor(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxTRANSLATE("Script editor"),
                          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1400, 1000),
                          long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~ScriptEditor() override;
};
