/**
* @file FormulaDialog.h
* @brief This header file contains the user formula related dialogs.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/19/2012
*/

#pragma once
#ifndef __formulaDialog
#define __formulaDialog

#include <wx/wx.h>
#include "FractalCanvas.h"
#include "global.h"


/**
* @class FuncDialog
* @brief Dialog that shows the available functions.
*/
class FuncDialog : public wxDialog
{
private:
    wxPanel* mainPanel;
    wxTextCtrl* text;
    wxButton* closeButton;

    void OnClose(wxCommandEvent& event);

public:
    FuncDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, 
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(249, 173), 
               long style = wxDEFAULT_DIALOG_STYLE);
    ~FuncDialog();
};

#ifdef _WIN32
#define FormulaDialogSize wxSize(320, 249)
#elif __linux__
#define FormulaDialogSize wxSize(340, 260)
#endif

/**
* @class FormulaDialog
* @brief Dialog that lets the user to introduce custom formulas.
*/
class FormulaDialog : public wxDialog
{
private:
    wxPanel* mainPanel;
    wxTextCtrl* formulaCtrl;
    wxStaticText* bailText;
    wxStaticText* typeText;
    wxTextCtrl* bailCtrl;
    wxCheckBox* juliaCheck;
    wxButton* acceptButton;
    wxButton* applyButton;
    wxButton* funcButton;
    wxChoice* typeChoice;
    FractalCanvas* fCanvas;
    GaussianColorStyles* colorStyle;
    wxWindow* parent;

    wxMenuItem* slider;
    wxMenuItem* manual;
    bool* active;
    int userDefinedID, FPuserDefinedID;

    void OnAccept(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnFunc(wxCommandEvent& event);
    void OnChoice(wxCommandEvent& event);

public:
    FormulaDialog(int _userDefinedID, int _FPuserDefinedID, GaussianColorStyles* mColorStyle, wxMenuItem* juliaSlider, 
                  wxMenuItem* juliaManual, bool* Active, FractalCanvas* _fCanvas, wxWindow* _parent,
                  wxWindowID id = wxID_ANY, const wxString& title = wxT(userFormTxt), const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = FormulaDialogSize, long style = wxDEFAULT_DIALOG_STYLE);
    ~FormulaDialog();
};

#endif