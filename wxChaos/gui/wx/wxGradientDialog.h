/** 
* @file wxGradientDialog.h
* @brief Defines a gradient.
*
* @author Evan Cordell
*
* @date 7/19/2012
*/

#pragma once

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
class wxGradientDialog : public wxDialog
{
    enum
    {
        ID_STOPS_AREA = wxID_HIGHEST + 1,
    };

    wxBoxSizer* _topSizer;
    wxPanel* _topPanel;
    wxSizer* _buttonSizer;
    wxStaticBoxSizer* _gradientSizer;
    wxStaticBitmap* _gradientStatBmp;
    wxBitmap* _gradientBmp;
    wxBoxSizer* _stopAreaSizer;
    wxStaticBitmap* _stopsStatBmp;
    wxBitmap* _stopsBmp;
    wxStaticBoxSizer* _stopEditSizer;
    wxBoxSizer* _colorSizer;
    wxStaticText* _colorTxt;
    wxStaticBitmap* _colorStatBmp;
    wxButton* _colorEditButton;
    wxButton* _colorDeleteButton;

    int _selectedColorStop;
    int _gradientSize;
    std::vector<wxColour> _displayedStops;
    wxGradient* _gradient;

    void OnStopsAreaClick(wxMouseEvent& event);
    void OnEditColor(wxCommandEvent& event);
    void OnDeleteColor(wxCommandEvent& event);
    void OnWindowClose(wxCloseEvent& event);

    void CreateWidgets();
    void paintGradient();
    void paintStops();

public:
    wxGradientDialog();
    explicit wxGradientDialog(wxWindow* parent, const wxGradient& gradient = wxGradient(std::vector(2, *wxBLACK), 0, 100));
    ~wxGradientDialog() override;
    bool Create(wxWindow *parent, const wxGradient& grad = wxGradient(std::vector(2, *wxBLACK), 0, 100));
    [[nodiscard]] wxGradient GetGradient() const;
    int ShowModal() override;

    wxDECLARE_DYNAMIC_CLASS(wxGradientDialog);
};
