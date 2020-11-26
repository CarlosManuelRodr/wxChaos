/** 
* @file CommandFrame.h 
* @brief This header file contains the command console frame.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/18/2012
*/

#pragma once
#ifndef __infoFrame
#define __infoFrame

#include <wx/richtext/richtextctrl.h>
#include <wx/statline.h>
#include <wx/frame.h>
#include <wx/wx.h>
#include "FractalClasses.h"
#include "global.h"

extern bool consoleState;
extern std::string consoleText;
extern bool thereIsConsoleText;

void SetConsoleText(std::string msg);
void CallConsole(Fractal* _target);

/**
* @class CommandFrame
* @brief Frame that shows a command console.
*/
class CommandFrame : public wxFrame
{
    bool *active;
    vector<wxString> commandHistory;
    vector<wxString> commandList;
    unsigned comHistIndex;
    Fractal* target;
    wxPanel* mainPanel;
    wxRichTextCtrl* infoText;
    wxStaticText* commandText;
    wxTextCtrl* commandCtrl;
    wxButton* commandButton;
        
    bool ConsoleCommand(wxString command);        // Command that is processed by the console.
    void OnCommand();                             // Process command.
    void SetWelcomeText();
    void SetCommandList();
    void OnClose( wxCloseEvent& event );
    void OnCommandClick( wxCommandEvent& event );
    void OnKey( wxKeyEvent& event );
        
    public:
        
    CommandFrame( bool* Active, Fractal* _target, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT( "Console" ), 
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 607,450 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    ~CommandFrame();

    ///@brief Sets text in the console.
    ///@param text Text to show.
    void SetText( wxString text );

    ///@brief Sets the target fractal.
    ///@param _target Pointer to target fractal.
    void SetTarget( Fractal* _target );

    ///@brief Prepares console to receive output from script.
    void PrepareOutput();
};

extern CommandFrame* commandFramePtr;

#endif
