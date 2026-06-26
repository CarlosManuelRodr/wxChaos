/**
* @file FormulaDialog.h
* @brief This header file contains the user formula-related dialogs.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/19/2012
*/

#pragma once

#include <wx/wx.h>
#include "FractalCanvas.h"

/**
* @class FunctionsHelpDialog
* @brief Dialog that shows the available functions.
*/
class FunctionsHelpDialog : public wxDialog
{
    wxPanel* _mainPanel;
    wxTextCtrl* _text;
    wxButton* _closeButton;

    void OnClose(wxCommandEvent& event);

public:
    explicit FunctionsHelpDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(460, 250),
                                 long style = wxDEFAULT_DIALOG_STYLE);
    ~FunctionsHelpDialog() override;
};

#define FormulaDialogSize wxSize(520, 320)

/**
* @class FormulaDialog
* @brief Dialog that lets the user introduce custom formulas.
*/
class FormulaDialog : public wxDialog
{
    wxPanel* _mainPanel;
    wxTextCtrl* _formulaCtrl;
    wxStaticText* _bailText;
    wxStaticText* _typeText;
    wxTextCtrl* _bailCtrl;
    wxCheckBox* _juliaCheck;
    wxButton* _acceptButton;
    wxButton* _applyButton;
    wxButton* _funcButton;
    wxChoice* _typeChoice;
    FractalCanvas* _fCanvas;
    wxWindow* _parent;

    wxMenuItem* _slider;
    wxMenuItem* _manual;
    bool* _active;
    int _userDefinedId, _fpUserDefinedId, _newtonUserDefinedId;

    void OnAccept(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnFunc(wxCommandEvent& event);
    void OnChoice(wxCommandEvent& event);

public:
    FormulaDialog(int userDefinedId, int fPUserDefinedId, int newtonUserDefinedId, wxMenuItem* juliaSlider,
                  wxMenuItem* juliaManual, bool* active, FractalCanvas* fCanvas, wxWindow* parent,
                  wxWindowID id = wxID_ANY, const wxString& title = "User formula", const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = FormulaDialogSize, long style = wxDEFAULT_DIALOG_STYLE);
    ~FormulaDialog() override;
};
