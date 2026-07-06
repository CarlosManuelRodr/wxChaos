#pragma once

#include <functional>
#include <vector>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include "Fractal.h"

class UnrestrictedSpinDoubleCtrl;

class FractalOptionsPanel : public wxPanel
{
    wxBoxSizer* _sizer{};
    wxStaticText* _kRealLabel{};
    wxTextCtrl* _kRealCtrl{};
    wxStaticText* _kImaginaryLabel{};
    wxTextCtrl* _kImaginaryCtrl{};
    wxButton* _applyButton{};
    Fractal* _target{};
    bool _showJuliaConstants{};

    std::vector<int> _foundLabels;
    std::vector<int> _foundTextControls;
    std::vector<int> _foundSpinControls;
    std::vector<int> _foundSpinDoubleControls;
    std::vector<int> _foundCheckBoxes;
    std::vector<wxWindow*> _dynamicControls;
    std::vector<wxTextCtrl*> _textControls;
    std::vector<wxSpinCtrl*> _spinControls;
    std::vector<UnrestrictedSpinDoubleCtrl*> _spinDoubleControls;
    std::vector<wxCheckBox*> _checkBoxes;
    std::function<void()> _applyHandler;

    wxPanel* CreateFractalOptionsHeader();
    void Build();
    void ClearDynamicControls();
    void OnApply(wxCommandEvent& event);

public:
    explicit FractalOptionsPanel(wxWindow* parent, bool showJuliaConstants = false);
    ~FractalOptionsPanel() override;

    void SetTarget(Fractal* target);
    void ClearTarget();
    void Apply() const;
    void SetApplyHandler(std::function<void()> handler);
    [[nodiscard]] bool HasVisibleOptions() const;
};
