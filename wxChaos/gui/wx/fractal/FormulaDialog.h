/**
* @file FormulaDialog.h
* @brief This header file contains the user formula dialog.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/19/2012
*/

#pragma once

#include <functional>
#include <wx/wx.h>
#include <wx/bmpbndl.h>
#include "FormulaOptions.h"
#include "types/FormulaType.h"
#include "canvas/FractalCanvas.h"

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
    FractalCanvas* _fractalCanvas;
    wxWindow* _parent;

    wxMenuItem* _slider;
    wxMenuItem* _manual;
    bool* _active;
    int _userDefinedId, _fpUserDefinedId, _newtonUserDefinedId;
    FormulaOptions _formulaOptions;
    std::function<void(const FormulaOptions&)> _applyHandler;

    void OnAccept(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnFunc(wxCommandEvent& event);
    void OnChoice(wxCommandEvent& event);
    [[nodiscard]] FormulaOptions GetInitialFormulaOptions(const FormulaOptions* formulaOptions) const;
    [[nodiscard]] FormulaOptions ReadFormulaOptions() const;
    void SetFormulaTypeSelection(FormulaType type, bool updateFormulaText) const;
    [[nodiscard]] static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                                      const wxString& darkIcon);
    [[nodiscard]] static wxBitmapBundle CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                                         const wxSize& size);

public:
    FormulaDialog(int userDefinedId, int fixedPointUserDefinedId, int newtonUserDefinedId, wxMenuItem* juliaSlider,
                  wxMenuItem* juliaManual, bool* active, FractalCanvas* fractalCanvas, wxWindow* parent,
                  wxWindowID id = wxID_ANY, const wxString& title = wxTRANSLATE("User formula"), const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = FormulaDialogSize, long style = wxDEFAULT_DIALOG_STYLE,
                  const FormulaOptions* formulaOptions = nullptr,
                  std::function<void(const FormulaOptions&)> applyHandler = {});
    FormulaDialog(const FormulaOptions& formulaOptions, std::function<void(const FormulaOptions&)> applyHandler,
                  wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxTRANSLATE("User formula"),
                  const wxPoint& pos = wxDefaultPosition, const wxSize& size = FormulaDialogSize,
                  long style = wxDEFAULT_DIALOG_STYLE);
    ~FormulaDialog() override;
    void SelectFormulaType(FormulaType type) const;
};
