/** 
* @file IterDialog.h 
* @brief Defines a dialog to change iterations.
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/19/2012
*/

#pragma once
#ifndef __Iter
#define __Iter

#include <wx/wx.h>
#include "FractalClasses.h"

/**
* @class IterDialog
* @brief A dialog that lets the user to manually change the iterations.
*/
class IterDialog : public wxFrame 
{
    wxPanel* panel;
    wxTextCtrl* textCtrl;
    wxButton* plusButton;
    wxButton* minusButton;
    wxButton* acceptButton;
    wxButton* applyButton;

    unsigned int number;
    Fractal *target;
    wxString text;
    bool *active;
    
public:
    IterDialog(bool *Active, Fractal *_target, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, 
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(314,124), 
               long style = wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL);

    ~IterDialog();

    void OnPlus(wxCommandEvent& event);
    void OnMinus(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void SetTarget(Fractal* _target);
};

#endif
