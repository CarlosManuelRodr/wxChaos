#include "analysis/ConfigFractalOptionsDialog.h"

#include "TextUtils.h"

ConfigFractalOptionsDialog::ConfigFractalOptionsDialog(Fractal* target, wxWindow* parent, const wxWindowID id,
                                                       const wxString& title, const wxPoint& pos, const wxSize& size,
                                                       const long style) : wxDialog(parent, id, title, pos, size, style)
{
    _target = target;
    this->SetSizeHints(DimensionFrameSize, wxDefaultSize);

    const auto mainBoxSizer = new wxBoxSizer(wxVERTICAL);

    _mainScroll = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    _mainScroll->SetScrollRate(5, 5);
    _optionsBox = new wxBoxSizer(wxVERTICAL);

    _kRealLabel = new wxStaticText(_mainScroll, wxID_ANY, "K real", wxDefaultPosition, wxDefaultSize, 0);
    _kRealLabel->Wrap(-1);
    _optionsBox->Add(_kRealLabel, 0, wxALL, 5);

    _kRealCtrl = new wxTextCtrl(_mainScroll, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, 0);
    _optionsBox->Add(_kRealCtrl, 0, wxALL | wxEXPAND, 5);

    _kImaginaryLabel = new wxStaticText(_mainScroll, wxID_ANY, "K imaginary", wxDefaultPosition, wxDefaultSize, 0);
    _kImaginaryLabel->Wrap(-1);
    _optionsBox->Add(_kImaginaryLabel, 0, wxALL, 5);

    _kImaginaryCtrl = new wxTextCtrl(_mainScroll, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, 0);
    _optionsBox->Add(_kImaginaryCtrl, 0, wxALL | wxEXPAND, 5);

    _mainScroll->SetSizer(_optionsBox);
    _mainScroll->Layout();
    _optionsBox->Fit(_mainScroll);
    mainBoxSizer->Add(_mainScroll, 7, wxEXPAND | wxALL, 1);

    _staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainBoxSizer->Add(_staticLine, 0, wxEXPAND | wxALL, 5);

    const auto buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _okButton = new wxButton(this, wxID_ANY, "Ok", wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_okButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    _applyButton = new wxButton(this, wxID_ANY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_applyButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    mainBoxSizer->Add(buttonBoxSizer, 0, wxEXPAND, 5);

    this->SetSizer(mainBoxSizer);
    this->wxTopLevelWindowBase::Layout();

    this->Centre(wxBOTH);

    if (!_target->IsJuliaVariety())
    {
        _kRealCtrl->Enable(false);
        _kImaginaryCtrl->Enable(false);
    }
    this->AdjustOptPanel();

    _okButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ConfigFractalOptionsDialog::OnOk, this);
    _applyButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ConfigFractalOptionsDialog::OnApply, this);
}

ConfigFractalOptionsDialog::~ConfigFractalOptionsDialog()
{
    _okButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ConfigFractalOptionsDialog::OnOk, this);
    _applyButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ConfigFractalOptionsDialog::OnApply, this);
}

void ConfigFractalOptionsDialog::AdjustOptPanel()
{
    this->DeleteOptPanel();

    if (PanelOptions* pOptions = _target->GetOptPanel(); pOptions->GetElementsSize() > 0)
    {
        size_t labelIndex;
        size_t index;

        for (int i = 0; i < pOptions->GetElementsSize(); i++)
        {
            switch (pOptions->GetPanelOptType(i))
            {
                case PanelOptionType::Label:
                {
                    _labels.push_back(new wxStaticText(_mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                    labelIndex = _labels.size() - 1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionsBox->Add(_labels[labelIndex], 0, wxALL, 5);
                    _foundLabels.push_back(i);
                }
                break;
                case PanelOptionType::TextCtrl:
                {
                    _labels.push_back(new wxStaticText(_mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                    labelIndex = _labels.size() - 1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionsBox->Add(_labels[labelIndex], 0, wxALL, 5);

                    _textControls.push_back(new wxTextCtrl(_mainScroll, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0));
                    index = _textControls.size() - 1;
                    _optionsBox->Add(_textControls[index], 0, wxALL | wxEXPAND, 5);
                    _foundTextControls.push_back(i);
                }
                break;
                case PanelOptionType::Spin:
                {
                    _labels.push_back(new wxStaticText(_mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                    labelIndex = _labels.size() - 1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionsBox->Add(_labels[labelIndex], 0, wxALL, 5);

                    _spinControls.push_back(new wxSpinCtrl(_mainScroll, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0));
                    index = _spinControls.size() - 1;
                    _optionsBox->Add(_spinControls[index], 0, wxALL | wxEXPAND, 5);
                    _foundSpinControls.push_back(i);
                }
                break;
                case PanelOptionType::CheckBox:
                {
                    _checkBoxes.push_back(new wxCheckBox(_mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                    index = _checkBoxes.size() - 1;
                    if (pOptions->GetDefault(i) == "true")
                        _checkBoxes[index]->SetValue(true);
                    else
                        _checkBoxes[index]->SetValue(false);
                    _optionsBox->Add(_checkBoxes[index], 0, wxALL | wxEXPAND, 5);
                    _foundCheckBoxes.push_back(i);
                }
                break;
            };
        }

        _optionsBox->Layout();
        _mainScroll->SetScrollbars(20, 20, 0, 50);
    }
}

void ConfigFractalOptionsDialog::DeleteOptPanel()
{
    for (const auto & _label : _labels)
        _label->Destroy();

    _labels.clear();
    _foundLabels.clear();
    for (const auto & _textControl : _textControls)
        _textControl->Destroy();

    _textControls.clear();
    _foundTextControls.clear();
    for (const auto & _spinControl : _spinControls)
        _spinControl->Destroy();

    _spinControls.clear();
    _foundSpinControls.clear();
    for (const auto & _checkBoxe : _checkBoxes)
        _checkBoxe->Destroy();

    _checkBoxes.clear();
    _foundCheckBoxes.clear();
}

void ConfigFractalOptionsDialog::SetNewTarget(Fractal* target)
{
    _target = target;
    if (_target->IsJuliaVariety())
    {
        _kRealCtrl->Enable(true);
        _kRealCtrl->SetValue(TextUtils::ToWxString(target->GetKReal()));
        _kImaginaryCtrl->Enable(true);
        _kImaginaryCtrl->SetValue(TextUtils::ToWxString(target->GetKImaginary()));
    }
    else
    {
        _kRealCtrl->Enable(false);
        _kImaginaryCtrl->Enable(false);
    }
    this->AdjustOptPanel();
}

void ConfigFractalOptionsDialog::OnOk(wxCommandEvent&)
{
    this->Show(false);
}

void ConfigFractalOptionsDialog::OnApply(wxCommandEvent&)
{
    const PanelOptions* pOptions = _target->GetOptPanel();
    for (unsigned int i = 0; i < _foundTextControls.size(); i++)
    {
        *pOptions->GetDoubleElement(i) = TextUtils::ToDouble(_textControls[i]->GetValue());
    }
    for (unsigned int i = 0; i < _foundSpinControls.size(); i++)
    {
        *pOptions->GetIntElement(i) = _spinControls[i]->GetValue();
    }
    for (unsigned int i = 0; i < _foundCheckBoxes.size(); i++)
    {
        if (_checkBoxes[i]->GetValue())
            *pOptions->GetBoolElement(i) = true;
        else
            *pOptions->GetBoolElement(i) = false;
    }
    if (_target->IsJuliaVariety())
        _target->SetK(TextUtils::ToDouble(_kRealCtrl->GetValue()), TextUtils::ToDouble(_kImaginaryCtrl->GetValue()));
    _target->SetFractalPropChanged();
}
