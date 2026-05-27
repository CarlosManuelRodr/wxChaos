/** 
* @file wxGradientDialog.h
* @brief Defines a gradient.
*
* @author Evan Cordell
*
* @date 7/19/2012
*/

#pragma once
#ifndef WX_GRADIENT_DLG_H
#define WX_GRADIENT_DLG_H

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include "wxGradient.h"

/**
* @class wxGradientDialog
* @brief A dialog that lets the user select a gradient.
*/
class wxGradientDialog : wxDialog
{
    void OnStopsAreaClick(wxMouseEvent& event);
    void OnEditColor(wxCommandEvent& event);
    void OnDeleteColor(wxCommandEvent& event);
    void OnWindowClose(wxCloseEvent& event);

    void CreateWidgets();
    void paintGradient();
    void paintStops();
    wxBoxSizer *topSizer;
    wxPanel *topPanel;
    wxSizer *buttonSizer;
    wxStaticBoxSizer *gradientSizer;
    wxStaticBitmap *gradientStatBmp;
    wxBitmap *gradientBmp;
    wxBoxSizer *stopAreaSizer;
    wxStaticBitmap *stopsStatBmp;
    wxBitmap *stopsBmp;
    wxStaticBoxSizer *stopEditSizer;
    wxBoxSizer *colorSizer;
    wxStaticText *colorTxt;
    wxStaticBitmap *colorStatBmp;
    wxButton *colorEditBtn;
    wxButton *colorDeleteBtn;

    int selectedColorStop;
    int gradientSize;
    std::vector<wxColor> m_displayedStops;
    wxGradient* m_gradient;
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxGradientDialog)

    enum
    {
        ID_STOPS_AREA = wxID_HIGHEST + 1,
    };

public:
    wxGradientDialog();
    explicit wxGradientDialog(wxWindow *parent, const wxGradient& grad = wxGradient(std::vector<wxColor>(2, *wxBLACK), 0, 100));
    ~wxGradientDialog() override;
    bool Create(wxWindow *parent, const wxGradient& grad = wxGradient(std::vector<wxColor>(2, *wxBLACK), 0, 100));
    wxGradient GetGradient() const;
    int ShowModal() override;
};

#endif