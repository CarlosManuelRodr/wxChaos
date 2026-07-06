#include "common/FractalOptionsPanel.h"
#include "common/AppTheme.h"
#include "common/UnrestrictedSpinDoubleCtrl.h"
#include "AppPaths.h"
#include "TextUtils.h"

FractalOptionsPanel::FractalOptionsPanel(wxWindow* parent, const bool showJuliaConstants)
                                         : wxPanel(parent, wxID_ANY), _showJuliaConstants(showJuliaConstants)
{
    _sizer = new wxBoxSizer(wxVERTICAL);
    _sizer->Add(CreateFractalOptionsHeader(), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    SetSizer(_sizer);
}

FractalOptionsPanel::~FractalOptionsPanel()
{
    if (_applyButton != nullptr)
        _applyButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &FractalOptionsPanel::OnApply, this);
}

void FractalOptionsPanel::SetTarget(Fractal* target)
{
    _target = target;
    Build();
}

void FractalOptionsPanel::ClearTarget()
{
    _target = nullptr;
    ClearDynamicControls();
    Layout();
}

void FractalOptionsPanel::SetApplyHandler(std::function<void()> handler)
{
    _applyHandler = std::move(handler);
}

bool FractalOptionsPanel::HasVisibleOptions() const
{
    return _target != nullptr && (_showJuliaConstants || _target->GetOptPanel()->GetElementsSize() > 0);
}

wxPanel* FractalOptionsPanel::CreateFractalOptionsHeader()
{
    const auto header = new wxPanel(this, wxID_ANY);
    header->SetBackgroundColour(AppTheme::ControlBackground());

    const auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    const wxString icon = AppTheme::IsDark() ? "cog_dark.svg" : "cog_light.svg";
    const wxSize iconSize(24, 24);
    const wxBitmapBundle iconBundle = wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), iconSize);
    const auto iconBitmap = new wxStaticBitmap(header, wxID_ANY, iconBundle.GetBitmap(iconSize));
    iconBitmap->SetBackgroundColour(AppTheme::ControlBackground());
    headerSizer->Add(iconBitmap, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP | wxBOTTOM, 12);

    const auto title = new wxStaticText(header, wxID_ANY, "Fractal options");
    wxFont titleFont = title->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleFont.SetPointSize(titleFont.GetPointSize() + 1);
    title->SetFont(titleFont);
    title->SetBackgroundColour(AppTheme::ControlBackground());
    title->SetForegroundColour(AppTheme::Foreground());
    headerSizer->Add(title, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

    header->SetSizer(headerSizer);
    header->SetMinSize(wxSize(-1, 52));
    return header;
}

void FractalOptionsPanel::Build()
{
    ClearDynamicControls();

    if (_target == nullptr)
    {
        Layout();
        return;
    }

    if (_showJuliaConstants)
    {
        _kRealLabel = new wxStaticText(this, wxID_ANY, "K real", wxDefaultPosition, wxDefaultSize, 0);
        _dynamicControls.push_back(_kRealLabel);
        _sizer->Add(_kRealLabel, 0, wxALL, 5);

        _kRealCtrl = new wxTextCtrl(this, wxID_ANY, TextUtils::ToWxString(_target->GetKReal()), wxDefaultPosition, wxDefaultSize, 0);
        _dynamicControls.push_back(_kRealCtrl);
        _sizer->Add(_kRealCtrl, 0, wxALL | wxEXPAND, 5);

        _kImaginaryLabel = new wxStaticText(this, wxID_ANY, "K imaginary", wxDefaultPosition, wxDefaultSize, 0);
        _dynamicControls.push_back(_kImaginaryLabel);
        _sizer->Add(_kImaginaryLabel, 0, wxALL, 5);

        _kImaginaryCtrl = new wxTextCtrl(this, wxID_ANY, TextUtils::ToWxString(_target->GetKImaginary()), wxDefaultPosition, wxDefaultSize, 0);
        _dynamicControls.push_back(_kImaginaryCtrl);
        _sizer->Add(_kImaginaryCtrl, 0, wxALL | wxEXPAND, 5);

        const bool juliaOptionsEnabled = _target->IsJuliaVariety();
        _kRealCtrl->Enable(juliaOptionsEnabled);
        _kImaginaryCtrl->Enable(juliaOptionsEnabled);
    }

    if (PanelOptions* panelOptions = _target->GetOptPanel(); panelOptions->GetElementsSize() > 0)
    {
        size_t labelIndex;
        size_t index;
        int textControlTargetIndex = 0;
        int spinControlTargetIndex = 0;
        int spinDoubleControlTargetIndex = 0;
        int checkBoxTargetIndex = 0;

        for (int i = 0; i < panelOptions->GetElementsSize(); i++)
        {
            switch (panelOptions->GetPanelOptionType(i))
            {
                case PanelOptionType::Label:
                {
                    auto* label = new wxStaticText(this, wxID_ANY, wxString(panelOptions->GetLabelValue(i)), wxDefaultPosition, wxDefaultSize, 0);
                    _dynamicControls.push_back(label);
                    labelIndex = _dynamicControls.size() - 1;
                    dynamic_cast<wxStaticText*>(_dynamicControls[labelIndex])->Wrap(-1);
                    _sizer->Add(_dynamicControls[labelIndex], 0, wxALL, 5);
                    _foundLabels.push_back(i);
                    break;
                }
                case PanelOptionType::TextCtrl:
                {
                    auto* label = new wxStaticText(this, wxID_ANY, wxString(panelOptions->GetLabelValue(i)), wxDefaultPosition, wxDefaultSize, 0);
                    _dynamicControls.push_back(label);
                    labelIndex = _dynamicControls.size() - 1;
                    dynamic_cast<wxStaticText*>(_dynamicControls[labelIndex])->Wrap(-1);
                    _sizer->Add(_dynamicControls[labelIndex], 0, wxALL, 5);

                    _textControls.push_back(new wxTextCtrl(this, wxID_ANY, wxString(panelOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0));
                    index = _textControls.size() - 1;
                    _dynamicControls.push_back(_textControls[index]);
                    _sizer->Add(_textControls[index], 0, wxALL | wxEXPAND, 5);
                    _foundTextControls.push_back(textControlTargetIndex);
                    ++textControlTargetIndex;
                    break;
                }
                case PanelOptionType::Spin:
                {
                    auto* label = new wxStaticText(this, wxID_ANY, wxString(panelOptions->GetLabelValue(i)), wxDefaultPosition, wxDefaultSize, 0);
                    _dynamicControls.push_back(label);
                    labelIndex = _dynamicControls.size() - 1;
                    dynamic_cast<wxStaticText*>(_dynamicControls[labelIndex])->Wrap(-1);
                    _sizer->Add(_dynamicControls[labelIndex], 0, wxALL, 5);

                    _spinControls.push_back(new wxSpinCtrl(this, wxID_ANY, wxString(panelOptions->GetDefault(i)), wxDefaultPosition,
                                                           wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0));
                    index = _spinControls.size() - 1;
                    _dynamicControls.push_back(_spinControls[index]);
                    _sizer->Add(_spinControls[index], 0, wxALL | wxEXPAND, 5);
                    _foundSpinControls.push_back(spinControlTargetIndex);
                    ++spinControlTargetIndex;
                    break;
                }
                case PanelOptionType::SpinDouble:
                {
                    auto* label = new wxStaticText(this, wxID_ANY, wxString(panelOptions->GetLabelValue(i)), wxDefaultPosition, wxDefaultSize, 0);
                    _dynamicControls.push_back(label);
                    labelIndex = _dynamicControls.size() - 1;
                    dynamic_cast<wxStaticText*>(_dynamicControls[labelIndex])->Wrap(-1);
                    _sizer->Add(_dynamicControls[labelIndex], 0, wxALL, 5);

                    _spinDoubleControls.push_back(new UnrestrictedSpinDoubleCtrl(this, panelOptions->GetDefault(i),
                                                                                 panelOptions->GetIncrement(i)));
                    index = _spinDoubleControls.size() - 1;
                    _dynamicControls.push_back(_spinDoubleControls[index]);
                    _sizer->Add(_spinDoubleControls[index], 0, wxALL | wxEXPAND, 5);
                    _foundSpinDoubleControls.push_back(spinDoubleControlTargetIndex);
                    ++spinDoubleControlTargetIndex;
                    break;
                }
                case PanelOptionType::CheckBox:
                {
                    _checkBoxes.push_back(new wxCheckBox(this, wxID_ANY, wxString(panelOptions->GetLabelValue(i)), wxDefaultPosition, wxDefaultSize, 0));
                    index = _checkBoxes.size() - 1;
                    _checkBoxes[index]->SetValue(panelOptions->GetDefault(i) == "true");
                    _dynamicControls.push_back(_checkBoxes[index]);
                    _sizer->Add(_checkBoxes[index], 0, wxALL | wxEXPAND, 5);
                    _foundCheckBoxes.push_back(checkBoxTargetIndex);
                    ++checkBoxTargetIndex;
                    break;
                }
            }
        }
    }

    _applyButton = new wxButton(this, wxID_ANY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
    _dynamicControls.push_back(_applyButton);
    _sizer->Add(_applyButton, 0, wxALL, 5);
    _applyButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FractalOptionsPanel::OnApply, this);

    _sizer->Layout();
    Layout();
}

void FractalOptionsPanel::ClearDynamicControls()
{
    if (_applyButton != nullptr)
        _applyButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &FractalOptionsPanel::OnApply, this);

    for (auto* control : _dynamicControls)
        control->Destroy();

    _dynamicControls.clear();
    _foundLabels.clear();
    _foundTextControls.clear();
    _foundSpinControls.clear();
    _foundSpinDoubleControls.clear();
    _foundCheckBoxes.clear();
    _textControls.clear();
    _spinControls.clear();
    _spinDoubleControls.clear();
    _checkBoxes.clear();
    _kRealLabel = nullptr;
    _kRealCtrl = nullptr;
    _kImaginaryLabel = nullptr;
    _kImaginaryCtrl = nullptr;
    _applyButton = nullptr;
}

void FractalOptionsPanel::Apply() const
{
    if (_target == nullptr)
        return;

    const PanelOptions* panelOptions = _target->GetOptPanel();
    for (unsigned int i = 0; i < _foundTextControls.size(); i++)
        *panelOptions->GetDoubleValue(_foundTextControls[i]) = TextUtils::ToDouble(_textControls[i]->GetValue());

    for (unsigned int i = 0; i < _foundSpinControls.size(); i++)
        *panelOptions->GetIntegerValue(_foundSpinControls[i]) = _spinControls[i]->GetValue();

    for (unsigned int i = 0; i < _foundSpinDoubleControls.size(); i++)
        *panelOptions->GetDoubleValue(_foundSpinDoubleControls[i]) = _spinDoubleControls[i]->GetValue();

    for (unsigned int i = 0; i < _foundCheckBoxes.size(); i++)
        *panelOptions->GetBoolValue(_foundCheckBoxes[i]) = _checkBoxes[i]->GetValue();

    if (_showJuliaConstants && _target->IsJuliaVariety())
        _target->SetK(TextUtils::ToDouble(_kRealCtrl->GetValue()), TextUtils::ToDouble(_kImaginaryCtrl->GetValue()));

    _target->SetFractalPropChanged();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void FractalOptionsPanel::OnApply(wxCommandEvent&)
{
    Apply();
    if (_applyHandler)
        _applyHandler();
}
