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
#include "Fractal.h"
#include "sfml/SFMLFractal.h"

/**
* @class IterDialog
* @brief A dialog that lets the user manually change the iterations.
*/
class IterDialog : public wxFrame 
{
    wxPanel* _panel;
    wxTextCtrl* _textCtrl;
    wxButton* _plusButton;
    wxButton* _minusButton;
    wxButton* _acceptButton;
    wxButton* _applyButton;

    unsigned int _number;
    SFMLFractal* _presenter;
    Fractal* _target;
    wxString _text;
    bool* _active;
    
public:
    IterDialog(bool* Active, SFMLFractal* presenter, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(420, 180), 
               long style = wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL);

    ~IterDialog() override;

    void OnPlus(wxCommandEvent& event);
    void OnMinus(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void SetTarget(SFMLFractal* presenter);
};

#endif
