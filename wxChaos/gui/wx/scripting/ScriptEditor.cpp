#include <chrono>
#include <fstream>
#include <wx/bmpbndl.h>
#include <wx/datetime.h>
#include "common/AppTheme.h"
#include "docs/DocumentViewer.h"
#include "AppPaths.h"
#include "scripting/ScriptEditor.h"
#include "FractalTypes.h"
#include "global.h"
#include "TextUtils.h"
#include "AngelscriptConfigurationEngine.h"
using namespace std;

wxDEFINE_EVENT(wxEVT_SCRIPT_EDITOR_CLOSED, wxCommandEvent);

wxPanel* ScriptEditor::CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                           const wxString& darkIcon)
{
    const auto header = new wxPanel(parent, wxID_ANY);
    header->SetBackgroundColour(AppTheme::ControlBackground());

    const auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    const wxSize iconSize(24, 24);
    const wxString icon = AppTheme::IsDark() ? darkIcon : lightIcon;
    const wxBitmapBundle iconBundle = wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), iconSize);
    const auto iconBitmap = new wxStaticBitmap(header, wxID_ANY, iconBundle.GetBitmap(iconSize));
    iconBitmap->SetBackgroundColour(AppTheme::ControlBackground());
    headerSizer->Add(iconBitmap, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP | wxBOTTOM, 12);

    const auto title = new wxStaticText(header, wxID_ANY, text);
    wxFont titleFont = title->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleFont.SetPointSize(titleFont.GetPointSize() + 1);
    title->SetFont(titleFont);
    title->SetBackgroundColour(AppTheme::ControlBackground());
    title->SetForegroundColour(AppTheme::Foreground());
    headerSizer->Add(title, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

    header->SetSizer(headerSizer);
    header->SetMinSize(wxSize(-1, 52));
    return header;
}

void ScriptEditor::SetButtonIcon(wxButton* button, const wxString& lightIcon, const wxString& darkIcon)
{
    const wxSize iconSize(20, 20);
    const wxString icon = AppTheme::IsDark() ? darkIcon : lightIcon;
    button->SetBitmap(wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), iconSize));
}

const string newScriptTemplate = R""""(void Configure()
{
    SetFractalName("New script");
    SetCategory("Other");
    // AddDoubleOption("scale", "Scale: ", 1.0);
}

void Render()
{
    // const double scale = GetDoubleOption("scale");
    // Example: Draw color gradient
    int color;
    if(threadIndex == 0)
    {
        for(int y=0; y<screenHeight; y++)
        {
            color = (float(y)/screenHeight)*paletteSize;
            for(int x=0; x<screenWidth; x++)
                SetPoint(x, y, false, color);
        }
}

void DrawOrbit()
{
    // Draw the selected point's orbit with DrawLine(..., red, green, blue).
}
)"""";

ScriptNameDialog::ScriptNameDialog(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
                                   const wxSize& size, const long style) : wxDialog(parent, id, wxGetTranslation(title), pos, size, style)
{
    this->SetSizeHints(wxSize(600, 220), wxDefaultSize);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);

    const auto scriptNameSizer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Set a name for the script")), wxHORIZONTAL);

    _scriptNameText = new wxStaticText(scriptNameSizer->GetStaticBox(), wxID_ANY, _("Script name: "), wxDefaultPosition, wxDefaultSize, 0);
    _scriptNameText->Wrap(-1);
    scriptNameSizer->Add(_scriptNameText, 0, wxALL, 5);

    _scriptNameCtrl = new wxTextCtrl(scriptNameSizer->GetStaticBox(), wxID_ANY, "MyNewScript", wxDefaultPosition, wxDefaultSize, 0);
    scriptNameSizer->Add(_scriptNameCtrl, 1, wxALL, 5);

    mainSizer->Add(scriptNameSizer, 0, wxEXPAND, 5);

    _buttonsSizer = new wxStdDialogButtonSizer();
    _buttonsSizerOk = new wxButton(this, wxID_OK);
    _buttonsSizer->AddButton(_buttonsSizerOk);
    _buttonsSizerCancel = new wxButton(this, wxID_CANCEL);
    _buttonsSizer->AddButton(_buttonsSizerCancel);
    _buttonsSizer->Realize();

    mainSizer->Add(_buttonsSizer, 0, wxEXPAND, 5);


    this->SetSizer(mainSizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    // Connect Events
    _buttonsSizerCancel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnCancel, this);
    _buttonsSizerOk->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnOk, this);
}

ScriptNameDialog::~ScriptNameDialog()
{
    // Disconnect Events
    _buttonsSizerCancel->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnCancel, this);
    _buttonsSizerOk->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnOk, this);
}

void ScriptNameDialog::OnCancel(wxCommandEvent&)
{
    this->EndModal(0);
}
void ScriptNameDialog::OnOk(wxCommandEvent&)
{
    this->EndModal(1);
}
wxString ScriptNameDialog::GetScriptName() const
{
    return _scriptNameCtrl->GetValue();
}


ScriptEditor::ScriptEditor(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
                           const wxSize& size, const long style) : wxFrame(parent, id, wxGetTranslation(title), pos, size, style)
{
    _currentScriptIndex = -1;

    this->SetSizeHints(wxSize(1200, 760), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);

    _mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto mainPanelSizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(CreateSectionHeader(_mainPanel, _("User script editor"),
                                            "code_editor_light.svg", "code_editor_dark.svg"),
                        0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _scriptPanel = new wxPanel(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxHORIZONTAL);

    const auto scriptListSizer = new wxStaticBoxSizer(new wxStaticBox(_scriptPanel, wxID_ANY, _("Scripts")), wxVERTICAL);

    scriptListSizer->SetMinSize(wxSize(250, -1));
    _scriptsListBox = new wxListBox(scriptListSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_ALWAYS_SB | wxLB_HSCROLL);
    scriptListSizer->Add(_scriptsListBox, 1, wxALL | wxEXPAND, 5);

    _saveChangesButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, _("Save Changes"), wxDefaultPosition, wxDefaultSize, 0);

    SetButtonIcon(_saveChangesButton, "save_light.svg", "save_dark.svg");
    _saveChangesButton->Enable(false);

    scriptListSizer->Add(_saveChangesButton, 0, wxALL | wxEXPAND, 5);

    _newButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, _("New script"), wxDefaultPosition, wxDefaultSize, 0);

    SetButtonIcon(_newButton, "new_light.svg", "new_dark.svg");
    scriptListSizer->Add(_newButton, 0, wxALL | wxEXPAND, 5);

    _removeButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, _("Delete script"), wxDefaultPosition, wxDefaultSize, 0);

    SetButtonIcon(_removeButton, "delete_light.svg", "delete_dark.svg");
    scriptListSizer->Add(_removeButton, 0, wxALL | wxEXPAND, 5);

    _closeButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, _("Close and reload"), wxDefaultPosition, wxDefaultSize, 0);

    SetButtonIcon(_closeButton, "close_light.svg", "close_dark.svg");
    scriptListSizer->Add(_closeButton, 0, wxALL | wxEXPAND, 5);

    _documentationButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, _("Documentation"),
                                        wxDefaultPosition, wxDefaultSize, 0);
    SetButtonIcon(_documentationButton, "help_light.svg", "help_dark.svg");
    scriptListSizer->Add(_documentationButton, 0, wxALL | wxEXPAND, 5);
    panelSizer->Add(scriptListSizer, 0, wxEXPAND, 5);

    const auto codeSizer = new wxStaticBoxSizer(new wxStaticBox(_scriptPanel, wxID_ANY, _("Source")), wxVERTICAL);

    _codeEditor = new wxStyledTextCtrl(codeSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString);
    _codeEditor->SetUseTabs(false);
    _codeEditor->SetTabWidth(4);
    _codeEditor->SetIndent(4);
    _codeEditor->SetTabIndents(true);
    _codeEditor->SetBackSpaceUnIndents(true);
    _codeEditor->SetViewEOL(false);
    _codeEditor->SetViewWhiteSpace(false);
    _codeEditor->SetMarginWidth(2, 0);
    _codeEditor->SetIndentationGuides(true);
    _codeEditor->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
    _codeEditor->SetMarginMask(1, wxSTC_MASK_FOLDERS);
    _codeEditor->SetMarginWidth(1, 16);
    _codeEditor->SetMarginSensitive(1, true);
    _codeEditor->SetProperty("fold", "1");
    _codeEditor->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
    _codeEditor->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    _codeEditor->SetMarginWidth(0, _codeEditor->TextWidth(wxSTC_STYLE_LINENUMBER, "_99999"));
    _codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
    _codeEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDER, wxColour("BLACK"));
    _codeEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDER, wxColour("WHITE"));
    _codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
    _codeEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, wxColour("BLACK"));
    _codeEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, wxColour("WHITE"));
    _codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
    _codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS);
    _codeEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, wxColour("BLACK"));
    _codeEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, wxColour("WHITE"));
    _codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS);
    _codeEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour("BLACK"));
    _codeEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour("WHITE"));
    _codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
    _codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
    _codeEditor->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    _codeEditor->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

    const auto font = wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");
    _codeEditor->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    this->SetUpLexer();
    codeSizer->Add(_codeEditor, 1, wxEXPAND | wxALL, 5);
    panelSizer->Add(codeSizer, 1, wxEXPAND, 5);


    _scriptPanel->SetSizer(panelSizer);
    _scriptPanel->Layout();
    panelSizer->Fit(_scriptPanel);
    mainPanelSizer->Add(_scriptPanel, 1, wxEXPAND | wxALL, 5);

    _debugPanel = new wxPanel(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto debugSizer = new wxBoxSizer(wxVERTICAL);
    debugSizer->Add(CreateSectionHeader(_debugPanel, _("Debugger"), "debugger_light.svg", "debugger_dark.svg"),
                    0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    const auto debugElementsSizer = new wxBoxSizer(wxHORIZONTAL);

    const auto debugButtonsSizer = new wxStaticBoxSizer(new wxStaticBox(_debugPanel, wxID_ANY, _("Actions")), wxVERTICAL);

    _validateButton = new wxButton(debugButtonsSizer->GetStaticBox(), wxID_ANY, _("Validate"), wxDefaultPosition, wxDefaultSize, 0);

    SetButtonIcon(_validateButton, "validate_light.svg", "validate_dark.svg");
    debugButtonsSizer->Add(_validateButton, 0, wxALL | wxEXPAND, 5);

    _runButton = new wxButton(debugButtonsSizer->GetStaticBox(), wxID_ANY, _("Run"), wxDefaultPosition, wxDefaultSize, 0);

    SetButtonIcon(_runButton, "run_light.svg", "run_dark.svg");
    debugButtonsSizer->Add(_runButton, 0, wxALL | wxEXPAND, 5);

    _clearConsoleButton = new wxButton(debugButtonsSizer->GetStaticBox(), wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0);
    SetButtonIcon(_clearConsoleButton, "erase_light.svg", "erase_dark.svg");
    debugButtonsSizer->Add(_clearConsoleButton, 0, wxALL | wxEXPAND, 5);
    debugElementsSizer->Add(debugButtonsSizer, 0, wxEXPAND, 5);

    const auto consoleSizer = new wxStaticBoxSizer(new wxStaticBox(_debugPanel, wxID_ANY, _("Console")), wxVERTICAL);
    consoleSizer->SetMinSize(wxSize(300, -1));

    _console = new wxRichTextCtrl(consoleSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
                                  wxTE_READONLY | wxVSCROLL | wxHSCROLL | wxNO_BORDER | wxWANTS_CHARS);
    _console->SetMinSize(wxSize(300, -1));
    _console->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas"));
    _console->SetBackgroundColour(AppTheme::IsDark() ? wxColour(17, 24, 39) : wxColour(248, 250, 252));
    _console->SetForegroundColour(ConsoleTextColor());
    this->ConsoleSetWelcomeText();

    consoleSizer->Add(_console, 1, wxEXPAND | wxALL, 5);
    debugElementsSizer->Add(consoleSizer, 1, wxEXPAND, 5);

    const auto previewSizer = new wxStaticBoxSizer(new wxStaticBox(_debugPanel, wxID_ANY, _("Preview")), wxVERTICAL);

    _renderPreviewBitmap = new wxStaticBitmap(previewSizer->GetStaticBox(), wxID_ANY, 
                                             wxBitmap(AppPaths::ResourceFile({"fractal_thumbnail.png"}), wxBITMAP_TYPE_ANY),
                                             wxDefaultPosition, wxDefaultSize, 0);
    previewSizer->Add(_renderPreviewBitmap, 0, wxALL, 5);
    debugElementsSizer->Add(previewSizer, 0, wxEXPAND, 5);

    debugSizer->Add(debugElementsSizer, 1, wxEXPAND | wxALL, 5);
    _debugPanel->SetSizer(debugSizer);
    _debugPanel->Layout();
    debugSizer->Fit(_debugPanel);
    mainPanelSizer->Add(_debugPanel, 0, wxALL | wxEXPAND, 5);


    _mainPanel->SetSizer(mainPanelSizer);
    _mainPanel->Layout();
    mainPanelSizer->Fit(_mainPanel);
    mainSizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainSizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    this->FetchUserScripts();
    this->SetBlackPreview();

    // Connect Events
    _scriptsListBox->Bind(wxEVT_COMMAND_LISTBOX_SELECTED, &ScriptEditor::OnSelectScript, this);
    _saveChangesButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnSaveChanges, this);
    _newButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnNewScript, this);
    _removeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnDeleteScript, this);
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnCloseButton, this);
    _documentationButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnDocumentation, this);
    this->Bind(wxEVT_CLOSE_WINDOW, &ScriptEditor::OnClose, this);
    _codeEditor->Bind(wxEVT_KEY_DOWN, &ScriptEditor::OnCodeChange, this);
    _validateButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnValidateScript, this);
    _runButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnRunScript, this);
    _clearConsoleButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnClearConsole, this);
}

ScriptEditor::~ScriptEditor()
{
    // Disconnect Events
    _scriptsListBox->Unbind(wxEVT_COMMAND_LISTBOX_SELECTED, &ScriptEditor::OnSelectScript, this);
    _saveChangesButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnSaveChanges, this);
    _newButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnNewScript, this);
    _removeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnDeleteScript, this);
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnCloseButton, this);
    _documentationButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnDocumentation, this);
    _codeEditor->Unbind(wxEVT_KEY_DOWN, &ScriptEditor::OnCodeChange, this);
    _validateButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnValidateScript, this);
    _runButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnRunScript, this);
    _clearConsoleButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptEditor::OnClearConsole, this);
}

void ScriptEditor::SetUpLexer() const
{
    _codeEditor->SetLexer(wxSTC_LEX_CPP);

    // Set the color to use for various elements
    _codeEditor->StyleSetForeground(wxSTC_C_COMMENT, wxColor(60, 162, 2));
    _codeEditor->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColor(60, 162, 2));
    _codeEditor->StyleSetForeground(wxSTC_C_NUMBER, wxColor(88, 45, 117));
    _codeEditor->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColor(0, 0, 255));
    _codeEditor->StyleSetForeground(wxSTC_C_STRING, wxColor(255, 60, 10));
    _codeEditor->StyleSetForeground(wxSTC_C_WORD, wxColor(0, 104, 201));
    _codeEditor->StyleSetForeground(wxSTC_C_IDENTIFIER, wxColor(71, 89, 36));

    // Give a list of keywords. They will be given the style specified for
    // wxSTC_C_WORD items.
    _codeEditor->SetKeyWords(0, "void bool int char complex float double");
}
void ScriptEditor::FetchUserScripts()
{
    _scriptsListBox->Clear();
    _loadedScripts = GetAllUserScripts();

    for (const ScriptData& d : _loadedScripts)
        _scriptsListBox->Append(AppPaths::BaseName(wxString::FromUTF8(d.file.c_str())));

    if (!_loadedScripts.empty())
    {
        this->LoadScript(0);
        _scriptsListBox->SetSelection(0);
        _currentScriptIndex = 0;
    }
}
void ScriptEditor::LoadScript(const unsigned index)
{
    if (!_loadedScripts.empty() && index < _loadedScripts.size())
    {
        _codeEditor->LoadFile(_loadedScripts[index].file);
        _currentScriptIndex = static_cast<int>(index);
    }
}
void ScriptEditor::SetBlackPreview() const
{
    wxBitmap black(250, 166);
    wxMemoryDC dc(black);
    dc.SetBackground(*wxBLACK_BRUSH);
    _renderPreviewBitmap->SetBitmap(black);
}
int ScriptEditor::GetScriptIndex(const wxString& scriptName) const
{
    for (int i = 0; i < _scriptsListBox->GetCount(); i++)
    {
        if (_scriptsListBox->GetString(i) == AppPaths::BaseName(scriptName))
            return i;
    }
    return -1;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ScriptEditor::OnSelectScript(wxCommandEvent& event)
{
    const int selection = event.GetSelection();
    this->LoadScript(selection);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ScriptEditor::OnSaveChanges(wxCommandEvent&)
{
    _saveChangesButton->Enable(false);
    int saveIndex = _scriptsListBox->GetSelection();
    _codeEditor->SaveFile(_loadedScripts[saveIndex].file);
}
void ScriptEditor::OnNewScript(wxCommandEvent&)
{
    ScriptNameDialog nameDialog(this);

    if (nameDialog.ShowModal())
    {
        const string scriptFileName = nameDialog.GetScriptName().ToStdString() + ".as";
        AppPaths::EnsureDirectory(AppPaths::ScriptsDir());
        const string newFilePath = AppPaths::ScriptFileStd(wxString::FromUTF8(scriptFileName.c_str()));
        ofstream ofs(newFilePath, std::ofstream::out);
        ofs << newScriptTemplate;
        ofs.close();

        this->FetchUserScripts();
        this->SetBlackPreview();
        const int newScriptIndex = this->GetScriptIndex(scriptFileName);
        this->LoadScript(newScriptIndex);
        _scriptsListBox->SetSelection(newScriptIndex);
    }
}
void ScriptEditor::OnDeleteScript(wxCommandEvent&)
{
    wxMessageDialog messageDialog(this, _("This operation cannot be undone. Are you sure you want to continue?"),
                                  wxMessageBoxCaptionStr, wxYES | wxNO);
    if (messageDialog.ShowModal() == wxID_YES)
    {
        int deleteIndex = _scriptsListBox->GetSelection();
        AppPaths::RemoveFile(_loadedScripts[deleteIndex].file);
        this->FetchUserScripts();
    }
}
void ScriptEditor::OnCloseButton(wxCommandEvent&)
{
    this->Close(true);
}
void ScriptEditor::OnDocumentation(wxCommandEvent&)
{
    const auto viewer = new DocumentViewer(AppPaths::ResourceFile({"Documents", "scripting.html"}),
                                           this, wxID_ANY, _("Scripting guide"),
                                           wxDefaultPosition, wxSize(1500, 960));
    viewer->Show(true);
}
void ScriptEditor::OnClose(wxCloseEvent&)
{
    wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_SCRIPT_EDITOR_CLOSED));
    this->Destroy();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ScriptEditor::OnCodeChange(wxKeyEvent& event)
{
    if (!_saveChangesButton->IsThisEnabled())
        _saveChangesButton->Enable(true);
    event.Skip();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ScriptEditor::OnValidateScript(wxCommandEvent&)
{
    if (_currentScriptIndex < 0 || _currentScriptIndex >= static_cast<int>(_loadedScripts.size()))
    {
        this->ConsoleAppendEntry(_("Validate"), _("No script selected."), false);
        return;
    }

    AngelscriptConfigurationEngine configEngine;
    if (!configEngine.CompileFromPath(_loadedScripts[_currentScriptIndex].file))
    {
        this->ConsoleAppendEntry(_("Validate"), _("Compile error: ") + configEngine.GetErrorInfo(), false);
        return;
    }

    if (!configEngine.Execute())
    {
        this->ConsoleAppendEntry(_("Validate"), _("Execution error: ") + configEngine.GetErrorInfo(), false);
        return;
    }

    this->ConsoleAppendEntry(_("Validate"), _("No errors found."), configEngine.GetStatus() == EngineStatus::Ok);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ScriptEditor::OnRunScript(wxCommandEvent&)
{
    if (_currentScriptIndex < 0 || _currentScriptIndex >= static_cast<int>(_loadedScripts.size()))
    {
        this->ConsoleAppendEntry(_("Run"), _("No script selected."), false);
        return;
    }

    // Start timer
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    ScriptFractal scriptFractal(250, 166, _loadedScripts[_currentScriptIndex], 1);
    const wxBitmap fractalBitmap = scriptFractal.GetRenderedWxBitmap();

    // Stop timer
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    _renderPreviewBitmap->SetBitmap(fractalBitmap);

    if (scriptFractal.IsThereError())
    {
        this->ConsoleAppendEntry(_("Run"), scriptFractal.GetErrorInfo(), false);
        scriptFractal.ClearErrorInfo();
    }
    else
        this->ConsoleAppendEntry(_("Run"), _("Rendered preview in ") + TextUtils::ToWxString(elapsed.count()) + _(" ms."), true);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ScriptEditor::OnClearConsole(wxCommandEvent&)
{
    ConsoleClear();
}

void ScriptEditor::ConsoleWriteText(const wxString& text, const wxColour& color) const
{
    _console->MoveEnd();
    _console->BeginTextColour(color);
    _console->WriteText(text);
    _console->ShowPosition(_console->GetCaretPosition());
    _console->EndTextColour();
}

void ScriptEditor::ConsoleSetWelcomeText() const
{
    ConsoleWriteText("wxChaos " + wxString::FromUTF8(APP_VERSION) + _(" script console\n"), ConsoleActionColor());
    ConsoleWriteText(_("Validate or run a script to inspect results.\n"), ConsoleMutedColor());
    ConsoleWriteText(_("Ready.\n"), ConsoleTextColor());
}

void ScriptEditor::ConsoleClear() const
{
    _console->Clear();
    ConsoleSetWelcomeText();
}

void ScriptEditor::ConsoleAppendEntry(const wxString& action, const wxString& message, const bool success) const
{
    ConsoleWriteText("\n[" + ConsoleTimestamp() + "] ", ConsoleMutedColor());
    ConsoleWriteText(action + "\n", ConsoleActionColor());
    ConsoleWriteText(success ? _("  OK     ") : _("  ERROR  "), success ? ConsoleSuccessColor() : ConsoleErrorColor());
    ConsoleWriteText(FormatConsoleMessage(message) + "\n", ConsoleTextColor());
    _console->ShowPosition(_console->GetLastPosition());
}

wxString ScriptEditor::ConsoleTimestamp()
{
    return wxDateTime::Now().Format("%H:%M:%S");
}

wxString ScriptEditor::FormatConsoleMessage(wxString message)
{
    message.Replace("\r\n", "\n");
    message.Trim(true);
    message.Trim(false);
    message.Replace("\n", "\n         ");
    return message;
}

wxColour ScriptEditor::ConsoleTextColor()
{
    return AppTheme::IsDark() ? wxColour(226, 232, 240) : wxColour(31, 41, 55);
}

wxColour ScriptEditor::ConsoleMutedColor()
{
    return AppTheme::IsDark() ? wxColour(148, 163, 184) : wxColour(100, 116, 139);
}

wxColour ScriptEditor::ConsoleActionColor()
{
    return AppTheme::IsDark() ? wxColour(96, 165, 250) : wxColour(29, 78, 216);
}

wxColour ScriptEditor::ConsoleSuccessColor()
{
    return AppTheme::IsDark() ? wxColour(52, 211, 153) : wxColour(4, 120, 87);
}

wxColour ScriptEditor::ConsoleErrorColor()
{
    return AppTheme::IsDark() ? wxColour(248, 113, 113) : wxColour(185, 28, 28);
}
