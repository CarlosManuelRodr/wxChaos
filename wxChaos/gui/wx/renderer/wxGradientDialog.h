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
#include "common/wxGradient.h"

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

    wxBoxSizer* _topSizer{};                 ///< Top-level dialog layout owned by the dialog.
    wxPanel* _topPanel{};                    ///< Optional top panel owned by the dialog.
    wxSizer* _buttonSizer{};                 ///< Standard button layout owned by the top-level sizer.
    wxStaticBoxSizer* _gradientSizer{};      ///< Layout containing gradient editing controls.
    wxStaticBitmap* _gradientStatBmp{};      ///< Control displaying the rendered gradient.
    wxBitmap _gradientBmp;                   ///< Rendered gradient bitmap.
    wxBoxSizer* _stopAreaSizer{};            ///< Layout containing the color-stop bitmap.
    wxStaticBitmap* _stopsStatBmp{};         ///< Control displaying gradient color stops.
    wxBitmap _stopsBmp;                      ///< Rendered color-stop bitmap.
    wxStaticBoxSizer* _stopEditSizer{};      ///< Layout containing stop editing controls.
    wxBoxSizer* _colorSizer{};               ///< Layout containing the selected color controls.
    wxStaticText* _colorTxt{};               ///< Label for the selected color.
    wxStaticBitmap* _colorStatBmp{};         ///< Preview of the selected color.
    wxButton* _colorEditButton{};            ///< Opens the selected color editor.
    wxButton* _colorDeleteButton{};          ///< Deletes the selected color stop.

    int _selectedColorStop{};                ///< Index of the selected color stop.
    int _gradientSize{};                     ///< Number of positions in the editable gradient.
    std::vector<wxColour> _displayedStops;   ///< Color stops currently displayed by the dialog.
    wxGradient _gradient;                    ///< Gradient being edited.

    void OnStopsAreaClick(wxMouseEvent& event);
    void OnEditColor(wxCommandEvent& event);
    void OnDeleteColor(wxCommandEvent& event);
    void OnWindowClose(wxCloseEvent& event);

    void CreateWidgets();
    void paintGradient();
    void paintStops();

public:
    /// @brief Constructs an uninitialized dialog for wxWidgets dynamic creation.
    wxGradientDialog();

    /// @brief Creates a gradient editor initialized from an existing gradient.
    /// @param parent Window that owns the dialog.
    /// @param gradient Initial gradient value.
    explicit wxGradientDialog(wxWindow* parent, const wxGradient& gradient = wxGradient(std::vector(2, *wxBLACK), 0, 100));

    /// @brief Destroys the dialog and its wxWidgets-owned child controls.
    ~wxGradientDialog() override;

    /// @brief Creates the native dialog and initializes its controls.
    /// @param parent Window that owns the dialog.
    /// @param grad Initial gradient value.
    /// @return True when the dialog was created successfully.
    bool Create(wxWindow *parent, const wxGradient& grad = wxGradient(std::vector(2, *wxBLACK), 0, 100));

    /// @brief Returns the gradient currently represented by the controls.
    /// @return Edited gradient value.
    [[nodiscard]] wxGradient GetGradient() const;

    /// @brief Shows the editor as a modal dialog.
    /// @return Modal result code.
    int ShowModal() override;

    wxDECLARE_DYNAMIC_CLASS(wxGradientDialog);
};
