#include "CommandFrame.h"
#include "StringFuncs.h"
#include "MainWindow.h"
using namespace std;

bool consoleState = false;
CommandFrame* commandFramePtr = NULL;

void CallConsole(Fractal* _target)
{
    if(!consoleState)
    {
        new CommandFrame(&consoleState, _target, mainFramePtr);
        fractalCanvasPtr->SetCommandConsole(commandFramePtr);
        commandFramePtr->Show(true);
    }
    else commandFramePtr->SetFocus();
}
void SetConsoleText(string msg)
{
    if(consoleText.size() == 0)
        consoleText = msg;
    else
    {
        consoleText += "\n";
        consoleText += msg;
    }
    thereIsConsoleText = true;
}
void ClearConsoleText()
{
    consoleText.clear();
    thereIsConsoleText = false;
}

CommandFrame::CommandFrame(bool* Active, Fractal* _target, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    commandFramePtr = this;

    // Crea ventana WX.
    active = Active;
    target = _target;
    comHistIndex = 0;
    consoleState = true;
    this->SetSizeHints(wxSize(607,386), wxDefaultSize);

#ifdef __linux__
    wxIcon icon(GetWxAbsPath("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
#elif _WIN32
    wxIcon icon(GetWxAbsPath("Resources\\icon.ico"), wxBITMAP_TYPE_ICO);
#endif
    this->SetIcon(icon);

    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer(wxVERTICAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelBoxxy;
    panelBoxxy = new wxBoxSizer(wxVERTICAL);

    infoText = new wxRichTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxSIMPLE_BORDER);
    infoText->SetFont(wxFont(10, 74, 90, 90, false, wxT("Consolas")));

    infoText->SetBackgroundColour( wxColour( 37, 52, 80 ) );
    panelBoxxy->Add(infoText, 7, wxEXPAND | wxALL, 5);

    wxBoxSizer* commandBoxxy;
    commandBoxxy = new wxBoxSizer( wxHORIZONTAL );

    commandText = new wxStaticText( mainPanel, wxID_ANY, wxT("Command:"), wxDefaultPosition, wxDefaultSize, 0 );
    commandText->Wrap( -1 );
    commandBoxxy->Add( commandText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    commandCtrl = new wxTextCtrl( mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);

    commandCtrl->SetFont(wxFont(10, 74, 90, 90, false, wxT("Consolas")));

    commandBoxxy->Add( commandCtrl, 1, wxALL|wxEXPAND, 5 );

    commandButton = new wxButton( mainPanel, wxID_ANY, wxT("Enter"), wxDefaultPosition, wxDefaultSize, 0 );
    commandBoxxy->Add( commandButton, 0, wxALL|wxEXPAND, 5 );

    panelBoxxy->Add( commandBoxxy, 0, wxEXPAND, 5 );

    mainPanel->SetSizer(panelBoxxy);
    mainPanel->Layout();
    panelBoxxy->Fit(mainPanel);
    mainBoxxy->Add(mainPanel, 1, wxALL|wxEXPAND, 0);

    this->SetSizer(mainBoxxy);
    this->Layout();

    this->Centre(wxBOTH);
    this->SetWelcomeText();
    this->SetCommandList();

    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CommandFrame::OnClose));
    commandCtrl->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler(CommandFrame::OnKey ), NULL, this );
    commandButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CommandFrame::OnCommandClick ), NULL, this );
}

CommandFrame::~CommandFrame()
{
    consoleState = false;
}
void CommandFrame::SetCommandList()
{
    commandList.push_back(wxT("AskInfo("));
    commandList.push_back(wxT("SetBoundaries("));
    commandList.push_back(wxT("Redraw()"));
    commandList.push_back(wxT("Abort()"));
    commandList.push_back(wxT("SaveOrbit("));
    commandList.push_back(wxT("Help()"));
    commandList.push_back(wxT("SetThreadNumber("));
    commandList.push_back(wxT("GetThreadNumber()"));
    commandList.push_back(wxT("ReloadScripts()"));
    commandList.push_back(wxT("DrawLine("));
    commandList.push_back(wxT("DrawCircle("));
    commandList.push_back(wxT("DeleteFigures()"));
    commandList.push_back(wxT("Clc()"));
}
void CommandFrame::SetText(wxString text)
{
    // Puts text into the console.
    infoText->MoveEnd();
    infoText->BeginTextColour(wxColour(255, 255, 255));
    infoText->WriteText(text.c_str());
    infoText->ShowPosition(infoText->GetCaretPosition());
    infoText->EndTextColour();
}
void CommandFrame::SetWelcomeText()
{
    infoText->BeginTextColour(wxColour(255, 255, 255));
    infoText->WriteText(wxT("wxChaos "));
    infoText->WriteText(wxString::FromUTF8(APP_VERSION));
    infoText->WriteText(wxT(" Console\nFor a list of commands type \"Help()\"\n"));
    infoText->WriteText(target->AskInfo());
    infoText->EndTextColour();
}
void CommandFrame::PrepareOutput()
{
    infoText->BeginTextColour(wxColour(172, 181, 15));
    infoText->WriteText(wxT("\nOutput >> \n"));
    infoText->EndTextColour();
}
void CommandFrame::SetTarget(Fractal* _target)
{
    target = _target;
}
void CommandFrame::OnClose(wxCloseEvent& event)
{
    this->Show(false);
    consoleState = false;
    this->Destroy();
}
void CommandFrame::OnCommandClick(wxCommandEvent& event)
{
    this->OnCommand();
}
void CommandFrame::OnCommand()
{
    // Process a command.
    infoText->MoveEnd();
    wxString command = commandCtrl->GetValue();
    commandHistory.push_back(command);
    comHistIndex = commandHistory.size();
    commandCtrl->Clear();
    infoText->BeginTextColour(wxColour(172, 181, 15));
    infoText->WriteText(wxT("\n<< "));
    infoText->WriteText(command);
    infoText->EndTextColour();
    infoText->BeginTextColour(wxColour(255, 255, 255));
    infoText->WriteText(wxT("\n>> "));
    if(!this->ConsoleCommand(command))
    {
        infoText->WriteText(target->Command(command));
        infoText->WriteText(wxT("\n"));
    }
    infoText->ShowPosition(infoText->GetCaretPosition());
    infoText->EndTextColour();
    infoText->MoveToLineEnd();
}
bool CommandFrame::ConsoleCommand(wxString command)
{
    if(command == wxT("Clc()"))
    {
        infoText->SetValue(wxT(""));
        return true;
    }
    if(command == wxT("ReloadScripts()"))
    {
        mainFramePtr->ReloadScripts();
        infoText->WriteText(wxT("Done"));
        return true;
    }
    if(command == wxT("Help()"))
    {
        wxString out;
        out = wxT("\nAskInfo(realNum, imagNum, maxIter)\n");
        out += wxT("SetBoundaries(minX, maxX, minY, maxY)\n");
        out += wxT("Redraw()\n");
        out += wxT("Abort()\n");
        out += wxT("SaveOrbit(realNum, imagNum, maxIter, filepath)\n");
        out += wxT("SetThreadNumber(threadNumber)\n");
        out += wxT("GetThreadNumber()\n");
        out += wxT("ReloadScripts()\n");
        out += wxT("DrawLine(x1, y1, x2, y2, radius)\n");
        out += wxT("DrawCircle(x, y, radius)\n");
        out += wxT("DeleteFigures()\n");
        out += wxT("Clc()\n");
        infoText->WriteText(out);
        return true;
    }
    return false;
}
void CommandFrame::OnKey(wxKeyEvent& event)
{
    // Keyboard event method.
    if(event.GetKeyCode() == WXK_RETURN)
    {
        this->OnCommand();
    }
    else if(event.GetKeyCode() == WXK_UP)
    {
        if(commandHistory.size() > 0 && comHistIndex >= 1)
        {
            commandCtrl->SetValue(commandHistory[--comHistIndex]);
            commandCtrl->SetInsertionPointEnd();
        }
    }
    else if(event.GetKeyCode() == WXK_DOWN)
    {
        if(commandHistory.size() > 0 && comHistIndex >= 0)
        {
            if(comHistIndex < commandHistory.size()-1)
            {
                commandCtrl->SetValue(commandHistory[++comHistIndex]);
            }
            else
            {
                if(commandHistory.size() != comHistIndex)
                {
                    commandCtrl->SetValue(wxT(""));
                    comHistIndex++;
                }
            }
        }
    }
    else if(event.GetKeyCode() == WXK_TAB)
    {
        wxString commandTxt = commandCtrl->GetValue();
        for(unsigned int i=0; i<commandList.size(); i++)
        {
            if(is_there_substr(commandList[i], commandTxt))
            {
                commandCtrl->SetValue(commandList[i]);
                commandCtrl->SetInsertionPointEnd();
                break;
            }
        }
    }
    else event.Skip();
}
