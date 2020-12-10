#include "ScriptEditor.h"
#include "Filesystem.h"

ScriptEditor::ScriptEditor(bool* active, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, 
                           const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    isActive = active;
    this->SetSizeHints(wxSize(1069, 600), wxDefaultSize);

    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer(wxVERTICAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* mainPanelSizer;
    mainPanelSizer = new wxBoxSizer(wxVERTICAL);

    scriptPanel = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelSizer;
    panelSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBoxSizer* scriptListSizer;
    scriptListSizer = new wxStaticBoxSizer(new wxStaticBox(scriptPanel, wxID_ANY, wxT("Scripts")), wxVERTICAL);

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

    closeButton = new wxButton(scriptListSizer->GetStaticBox(), wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0);

    closeButton->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE, wxART_TOOLBAR));
    scriptListSizer->Add(closeButton, 0, wxALL | wxEXPAND, 5);


    panelSizer->Add(scriptListSizer, 0, wxEXPAND, 5);

    wxStaticBoxSizer* codeSizer;
    codeSizer = new wxStaticBoxSizer(new wxStaticBox(scriptPanel, wxID_ANY, wxT("Code editor")), wxVERTICAL);

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

    console = new wxRichTextCtrl(consoleSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxVSCROLL | wxHSCROLL | wxNO_BORDER | wxWANTS_CHARS);
    console->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas")));
    console->SetMinSize(wxSize(300, -1));

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

    // Connect Events
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
    saveChangesButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnSaveChanges), NULL, this);
    newButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnNewScript), NULL, this);
    removeButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnDeleteScript), NULL, this);
    closeButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnClose), NULL, this);
    codeEditor->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(ScriptEditor::OnCodeChange), NULL, this);
    validateButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnValidateScript), NULL, this);
    runButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnRunScript), NULL, this);
    helpButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScriptEditor::OnHelp), NULL, this);
}

void ScriptEditor::GetUserScripts()
{

}

void ScriptEditor::OnSaveChanges(wxCommandEvent& event)
{

}
void ScriptEditor::OnNewScript(wxCommandEvent& event)
{

}
void ScriptEditor::OnDeleteScript(wxCommandEvent& event)
{

}
void ScriptEditor::OnClose(wxCommandEvent& event)
{

}
void ScriptEditor::OnCodeChange(wxKeyEvent& event)
{
    event.Skip();
}
void ScriptEditor::OnValidateScript(wxCommandEvent& event)
{

}
void ScriptEditor::OnRunScript(wxCommandEvent& event)
{

}
void ScriptEditor::OnHelp(wxCommandEvent& event)
{

}