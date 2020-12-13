#include <chrono>
#include "ScriptEditor.h"
#include "Filesystem.h"
#include "FractalTypes.h"
#include "global.h"
using namespace std;

ScriptNameDialog::ScriptNameDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxSize(494, 158), wxDefaultSize);

    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* scriptNameSizer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Set a name for the script")), wxHORIZONTAL);

    scriptNameText = new wxStaticText(scriptNameSizer->GetStaticBox(), wxID_ANY, wxT("Script name: "), wxDefaultPosition, wxDefaultSize, 0);
    scriptNameText->Wrap(-1);
    scriptNameSizer->Add(scriptNameText, 0, wxALL, 5);

    scriptNameCtrl = new wxTextCtrl(scriptNameSizer->GetStaticBox(), wxID_ANY, wxT("MyNewScript"), wxDefaultPosition, wxDefaultSize, 0);
    scriptNameSizer->Add(scriptNameCtrl, 1, wxALL, 5);

    mainSizer->Add(scriptNameSizer, 0, wxEXPAND, 5);

    buttonsSizer = new wxStdDialogButtonSizer();
    buttonsSizerOK = new wxButton(this, wxID_OK);
    buttonsSizer->AddButton(buttonsSizerOK);
    buttonsSizerCancel = new wxButton(this, wxID_CANCEL);
    buttonsSizer->AddButton(buttonsSizerCancel);
    buttonsSizer->Realize();

    mainSizer->Add(buttonsSizer, 0, wxEXPAND, 5);


    this->SetSizer(mainSizer);
    this->Layout();
    this->Centre(wxBOTH);

    // Connect Events
    buttonsSizerCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptNameDialog::OnCancel), NULL, this);
    buttonsSizerOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptNameDialog::OnOk), NULL, this);
}

ScriptNameDialog::~ScriptNameDialog()
{
    // Disconnect Events
    buttonsSizerCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptNameDialog::OnCancel), NULL, this);
    buttonsSizerOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptNameDialog::OnOk), NULL, this);

}

void ScriptNameDialog::OnCancel(wxCommandEvent& event)
{
    this->EndModal(0);
}
void ScriptNameDialog::OnOk(wxCommandEvent& event)
{
    this->EndModal(1);
}
wxString ScriptNameDialog::GetScriptName()
{
    return scriptNameCtrl->GetValue();
}


ScriptEditor::ScriptEditor(bool* active, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, 
                           const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    isActive = active;
    currentScriptIndex = -1;

    this->SetSizeHints(wxSize(1069, 600), wxDefaultSize);

    wxIcon icon(GetWxAbsPath({ "Resources", "icon.ico" }), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);

    scriptPanel = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBoxSizer* scriptListSizer = new wxStaticBoxSizer(new wxStaticBox(scriptPanel, wxID_ANY, wxT("Scripts")), wxVERTICAL);

    scriptListSizer->SetMinSize(wxSize(250, -1));
    scriptsListBox = new wxListBox(scriptListSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_ALWAYS_SB | wxLB_HSCROLL);
    scriptListSizer->Add(scriptsListBox, 1, wxALL | wxEXPAND, 5);

    saveChangesButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, wxT("Save Changes"), wxDefaultPosition, wxDefaultSize, 0);

    saveChangesButton->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR));
    saveChangesButton->Enable(false);

    scriptListSizer->Add(saveChangesButton, 0, wxALL | wxEXPAND, 5);

    newButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, wxT("New script"), wxDefaultPosition, wxDefaultSize, 0);

    newButton->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR));
    scriptListSizer->Add(newButton, 0, wxALL | wxEXPAND, 5);

    removeButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, wxT("Delete script"), wxDefaultPosition, wxDefaultSize, 0);

    removeButton->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_TOOLBAR));
    scriptListSizer->Add(removeButton, 0, wxALL | wxEXPAND, 5);

    closeButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, wxT("Close and reload"), wxDefaultPosition, wxDefaultSize, 0);

    closeButton->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE, wxART_TOOLBAR));
    scriptListSizer->Add(closeButton, 0, wxALL | wxEXPAND, 5);
    panelSizer->Add(scriptListSizer, 0, wxEXPAND, 5);

    wxStaticBoxSizer* codeSizer = new wxStaticBoxSizer(new wxStaticBox(scriptPanel, wxID_ANY, wxT("Code editor")), wxVERTICAL);

    codeEditor = new wxStyledTextCtrl(codeSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString);
    codeEditor->SetUseTabs(false);
    codeEditor->SetTabWidth(4);
    codeEditor->SetIndent(4);
    codeEditor->SetTabIndents(true);
    codeEditor->SetBackSpaceUnIndents(true);
    codeEditor->SetViewEOL(false);
    codeEditor->SetViewWhiteSpace(false);
    codeEditor->SetMarginWidth(2, 0);
    codeEditor->SetIndentationGuides(true);
    codeEditor->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
    codeEditor->SetMarginMask(1, wxSTC_MASK_FOLDERS);
    codeEditor->SetMarginWidth(1, 16);
    codeEditor->SetMarginSensitive(1, true);
    codeEditor->SetProperty(wxT("fold"), wxT("1"));
    codeEditor->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
    codeEditor->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    codeEditor->SetMarginWidth(0, codeEditor->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));
    codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
    codeEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDER, wxColour(wxT("BLACK")));
    codeEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDER, wxColour(wxT("WHITE")));
    codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
    codeEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(wxT("BLACK")));
    codeEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(wxT("WHITE")));
    codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
    codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS);
    codeEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, wxColour(wxT("BLACK")));
    codeEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, wxColour(wxT("WHITE")));
    codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS);
    codeEditor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(wxT("BLACK")));
    codeEditor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(wxT("WHITE")));
    codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
    codeEditor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
    codeEditor->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    codeEditor->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

    wxFont font = wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
    codeEditor->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    this->SetUpLexer();
    codeSizer->Add(codeEditor, 1, wxEXPAND | wxALL, 5);
    panelSizer->Add(codeSizer, 1, wxEXPAND, 5);


    scriptPanel->SetSizer(panelSizer);
    scriptPanel->Layout();
    panelSizer->Fit(scriptPanel);
    mainPanelSizer->Add(scriptPanel, 1, wxEXPAND | wxALL, 5);

    debugCollapsiblePane = new wxCollapsiblePane(mainPanel, wxID_ANY, wxT("Debugger"), wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);
    debugCollapsiblePane->Collapse(true);
    debugCollapsiblePane->SetMinSize(wxSize(750, -1));

    wxBoxSizer* debugSizer = new wxBoxSizer(wxHORIZONTAL);

    debugPanel = new wxPanel(debugCollapsiblePane->GetPane(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* debugElementsSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBoxSizer* debugButtonsSizer = new wxStaticBoxSizer(new wxStaticBox(debugPanel, wxID_ANY, wxT("Actions")), wxVERTICAL);

    validateButton = new wxButton(debugButtonsSizer->GetStaticBox(), wxID_ANY, wxT("Validate"), wxDefaultPosition, wxDefaultSize, 0);

    validateButton->SetBitmap(wxArtProvider::GetBitmap(wxART_WARNING, wxART_TOOLBAR));
    debugButtonsSizer->Add(validateButton, 0, wxALL | wxEXPAND, 5);

    runButton = new wxButton(debugButtonsSizer->GetStaticBox(), wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0);

    runButton->SetBitmap(wxBitmap(GetWxAbsPath({ "Resources", "play.png" }), wxBITMAP_TYPE_ANY));
    debugButtonsSizer->Add(runButton, 0, wxALL | wxEXPAND, 5);

    helpButton = new wxButton(debugButtonsSizer->GetStaticBox(), wxID_ANY, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0);

    helpButton->SetBitmap(wxArtProvider::GetBitmap(wxART_HELP_FOLDER, wxART_TOOLBAR));
    debugButtonsSizer->Add(helpButton, 0, wxALL | wxEXPAND, 5);
    debugElementsSizer->Add(debugButtonsSizer, 0, wxEXPAND, 5);

    wxStaticBoxSizer* consoleSizer = new wxStaticBoxSizer(new wxStaticBox(debugPanel, wxID_ANY, wxT("Console")), wxVERTICAL);
    consoleSizer->SetMinSize(wxSize(300, -1));

    console = new wxRichTextCtrl(consoleSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
                                 wxTE_READONLY | wxVSCROLL | wxHSCROLL | wxNO_BORDER | wxWANTS_CHARS);
    console->SetMinSize(wxSize(300, -1));
    console->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));
    console->SetBackgroundColour(wxColour(37, 52, 80));
    this->ConsoleSetWelcomeText();

    consoleSizer->Add(console, 1, wxEXPAND | wxALL, 5);
    debugElementsSizer->Add(consoleSizer, 1, wxEXPAND, 5);

    wxStaticBoxSizer* previewSizer = new wxStaticBoxSizer(new wxStaticBox(debugPanel, wxID_ANY, wxT("Preview")), wxVERTICAL);

    renderPreviewBitmap = new wxStaticBitmap(previewSizer->GetStaticBox(), wxID_ANY, 
                                             wxBitmap(GetWxAbsPath({ "Resources", "fractal_thumbnail.png" }), wxBITMAP_TYPE_ANY), 
                                             wxDefaultPosition, wxDefaultSize, 0);
    previewSizer->Add(renderPreviewBitmap, 0, wxALL, 5);
    debugElementsSizer->Add(previewSizer, 0, wxEXPAND, 5);

    debugPanel->SetSizer(debugElementsSizer);
    debugPanel->Layout();
    debugElementsSizer->Fit(debugPanel);
    debugSizer->Add(debugPanel, 1, wxEXPAND | wxALL, 5);


    debugCollapsiblePane->GetPane()->SetSizer(debugSizer);
    debugCollapsiblePane->GetPane()->Layout();
    debugSizer->Fit(debugCollapsiblePane->GetPane());
    mainPanelSizer->Add(debugCollapsiblePane, 0, wxALL | wxEXPAND, 5);


    mainPanel->SetSizer(mainPanelSizer);
    mainPanel->Layout();
    mainPanelSizer->Fit(mainPanel);
    mainSizer->Add(mainPanel, 1, wxEXPAND | wxALL, 1);


    this->SetSizer(mainSizer);
    this->Layout();
    this->Centre(wxBOTH);

    this->LocateUserScripts();
    this->SetBlackPreview();

    // Connect Events
    scriptsListBox->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(ScriptEditor::OnSelectScript), NULL, this);
    saveChangesButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnSaveChanges), NULL, this);
    newButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnNewScript), NULL, this);
    removeButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnDeleteScript), NULL, this);
    closeButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnClose), NULL, this);
    codeEditor->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(ScriptEditor::OnCodeChange), NULL, this);
    validateButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnValidateScript), NULL, this);
    runButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnRunScript), NULL, this);
    helpButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnHelp), NULL, this);
}

ScriptEditor::~ScriptEditor()
{
    *isActive = false;

    // Disconnect Events
    scriptsListBox->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(ScriptEditor::OnSelectScript), NULL, this);
    saveChangesButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnSaveChanges), NULL, this);
    newButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnNewScript), NULL, this);
    removeButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnDeleteScript), NULL, this);
    closeButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnClose), NULL, this);
    codeEditor->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(ScriptEditor::OnCodeChange), NULL, this);
    validateButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnValidateScript), NULL, this);
    runButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnRunScript), NULL, this);
    helpButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnHelp), NULL, this);
}

void ScriptEditor::SetUpLexer()
{
    codeEditor->SetLexer(wxSTC_LEX_CPP);

    // Set the color to use for various elements
    codeEditor->StyleSetForeground(wxSTC_C_COMMENT, wxColor(60, 162, 2));
    codeEditor->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColor(60, 162, 2));
    codeEditor->StyleSetForeground(wxSTC_C_NUMBER, wxColor(88, 45, 117));
    codeEditor->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColor(0, 0, 255));
    codeEditor->StyleSetForeground(wxSTC_C_STRING, wxColor(255, 60, 10));
    codeEditor->StyleSetForeground(wxSTC_C_WORD, wxColor(0, 104, 201));
    codeEditor->StyleSetForeground(wxSTC_C_IDENTIFIER, wxColor(71, 89, 36));

    // Give a list of keywords. They will be given the style specified for
    // wxSTC_C_WORD items.
    codeEditor->SetKeyWords(0, wxT("void bool int char complex float double"));
}
void ScriptEditor::LocateUserScripts()
{
    scriptsListBox->Clear();
    loadedScripts = GetAllUserScripts();

    for (ScriptData d : loadedScripts)
        scriptsListBox->Append(GetFileBaseName(d.file));

    if (!loadedScripts.empty())
    {
        this->LoadScript(0);
        scriptsListBox->SetSelection(0);
        currentScriptIndex = 0;
    }
}
void ScriptEditor::LoadScript(unsigned index)
{
    if (loadedScripts.size() > 0 && index < loadedScripts.size())
    {
        codeEditor->LoadFile(loadedScripts[index].file);
        currentScriptIndex = index;
    }
}
void ScriptEditor::ConsoleSetText(wxString text)
{
    console->MoveEnd();
    console->BeginTextColour(wxColour(255, 255, 255));
    console->WriteText(text);
    console->ShowPosition(console->GetCaretPosition());
    console->EndTextColour();
}
void ScriptEditor::ConsoleSetWelcomeText()
{
    console->BeginTextColour(wxColour(255, 255, 255));
    console->WriteText(wxT("wxChaos "));
    console->WriteText(wxString::FromUTF8(APP_VERSION));
    console->WriteText(wxT(" Console\n=====================\n"));
    console->EndTextColour();
}
void ScriptEditor::ConsolePrepareInput(wxString command)
{
    console->BeginTextColour(wxColour(15, 181, 57));
    console->WriteText(wxString("\nInput << ") << command << wxT("\n"));
    console->EndTextColour();
}
void ScriptEditor::ConsolePrepareOutput()
{
    console->BeginTextColour(wxColour(172, 181, 15));
    console->WriteText(wxT("Output >> \n"));
    console->EndTextColour();
}
void ScriptEditor::SetBlackPreview()
{
    wxBitmap black(250, 166);
    wxMemoryDC dc(black);
    dc.SetBackground(*wxBLACK_BRUSH);
    renderPreviewBitmap->SetBitmap(black);
}

void ScriptEditor::OnSelectScript(wxCommandEvent& event)
{
    const int selection = event.GetSelection();
    this->LoadScript(selection);
}
void ScriptEditor::OnSaveChanges(wxCommandEvent& event)
{
    saveChangesButton->Enable(false);
}
void ScriptEditor::OnNewScript(wxCommandEvent& event)
{
    ScriptNameDialog* nameDialog = new ScriptNameDialog(this);

    if (nameDialog->ShowModal())
    {

    }
}
void ScriptEditor::OnDeleteScript(wxCommandEvent& event)
{

}
void ScriptEditor::OnClose(wxCommandEvent& event)
{
    this->Show(false);
    *isActive = false;
    this->Destroy();
}
void ScriptEditor::OnCodeChange(wxKeyEvent& event)
{
    if (!saveChangesButton->IsThisEnabled())
        saveChangesButton->Enable(true);
    event.Skip();
}
void ScriptEditor::OnValidateScript(wxCommandEvent& event)
{
    this->ConsolePrepareInput(wxT("Validate script"));
    this->ConsolePrepareOutput();

    AngelscriptConfigurationEngine configEngine;
    if (!configEngine.CompileFromPath(loadedScripts[currentScriptIndex].file))
        this->ConsoleSetText(wxString("Compile error: ") << configEngine.GetErrorInfo());

    if (!configEngine.Execute())
        this->ConsoleSetText(wxString("Execution error: ") << configEngine.GetErrorInfo());

    if (configEngine.GetStatus() == EngineStatus::Ok)
        this->ConsoleSetText(wxString("No errors found."));
}
void ScriptEditor::OnRunScript(wxCommandEvent& event)
{
    this->ConsolePrepareInput(wxT("Run script"));

    // Start timer
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    ScriptFractal scriptFractal(250, 166, loadedScripts[currentScriptIndex], 1);
    wxBitmap fractalBitmap = scriptFractal.GetRenderedWxBitmap();

    // Stop timer
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    renderPreviewBitmap->SetBitmap(fractalBitmap);

    this->ConsolePrepareOutput();
    if (scriptFractal.IsThereError())
    {
        this->ConsoleSetText(scriptFractal.GetErrorInfo());
        scriptFractal.ClearErrorInfo();
    }
    else
        this->ConsoleSetText(wxString("Time ellapsed: ") << elapsed.count() << wxT(" milliseconds"));
}
void ScriptEditor::OnHelp(wxCommandEvent& event)
{

}