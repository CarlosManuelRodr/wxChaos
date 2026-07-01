#include "common/PanelOptions.h"

PanelOptions::PanelOptions() : _forceShow(false) {}

void PanelOptions::LinkInt(const PanelOptionType type, const wxString& labelText, int* linkInt, const wxString& defaultValue)
{
    _type.push_back(type);
    _label.push_back(labelText);
    _defaults.push_back(defaultValue);
    _linkTo.push_back(LinkTo::ToInt);
    _intTarget.push_back(linkInt);
}
void PanelOptions::LinkDbl(const PanelOptionType type, const wxString& labelText, double* linkDbl, const wxString& defaultValue)
{
    _type.push_back(type);
    _label.push_back(labelText);
    _defaults.push_back(defaultValue);
    _linkTo.push_back(LinkTo::ToDouble);
    _dblTarget.push_back(linkDbl);
}
void PanelOptions::LinkBool(const PanelOptionType type, const wxString& labelText, bool* linkBool, const wxString& defaultValue)
{
    _type.push_back(type);
    _label.push_back(labelText);
    _defaults.push_back(defaultValue);
    _linkTo.push_back(LinkTo::ToBool);
    _boolTarget.push_back(linkBool);
}
size_t PanelOptions::GetElementsSize() const { return _type.size(); }
LinkTo PanelOptions::GetLinkType(const unsigned int index) const { return _linkTo.at(index); }
wxString PanelOptions::GetLabelElement(const unsigned int index) { return _label.at(index); }
int* PanelOptions::GetIntElement(const unsigned int index) const { return _intTarget.at(index); }
double* PanelOptions::GetDoubleElement(const unsigned int index) const { return _dblTarget.at(index); }
bool* PanelOptions::GetBoolElement(const unsigned int index) const { return _boolTarget.at(index); }
wxString PanelOptions::GetDefault(const unsigned int index) { return _defaults.at(index); }
PanelOptionType PanelOptions::GetPanelOptType(const unsigned int index) const { return _type.at(index); }
void PanelOptions::SetForceShow(const bool mode) { _forceShow = mode; }
bool PanelOptions::GetForceShow() const { return _forceShow; }
