#include "PanelOptions.h"

PanelOptions::PanelOptions() : forceShow(false) {}

void PanelOptions::LinkInt(const PanelOptionType pType, const wxString& labelTxt, int* linkInt, const wxString& defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    defaults.push_back(defaultVal);
    linkTo.push_back(LinkTo::ToInt);
    intTarget.push_back(linkInt);
}
void PanelOptions::LinkDbl(const PanelOptionType pType, const wxString& labelTxt, double* linkDbl, const wxString& defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    defaults.push_back(defaultVal);
    linkTo.push_back(LinkTo::ToDouble);
    dblTarget.push_back(linkDbl);
}
void PanelOptions::LinkBool(const PanelOptionType pType, const wxString& labelTxt, bool* linkBool, const wxString& defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    defaults.push_back(defaultVal);
    linkTo.push_back(LinkTo::ToBool);
    boolTarget.push_back(linkBool);
}
size_t PanelOptions::GetElementsSize() const { return type.size(); }
LinkTo PanelOptions::GetLinkType(const unsigned int index) const { return linkTo.at(index); }
wxString PanelOptions::GetLabelElement(const unsigned int index) { return label.at(index); }
int* PanelOptions::GetIntElement(const unsigned int index) const { return intTarget.at(index); }
double* PanelOptions::GetDoubleElement(const unsigned int index) const { return dblTarget.at(index); }
bool* PanelOptions::GetBoolElement(const unsigned int index) const { return boolTarget.at(index); }
wxString PanelOptions::GetDefault(const unsigned int index) { return defaults.at(index); }
PanelOptionType PanelOptions::GetPanelOptType(const unsigned int index) const { return type.at(index); }
void PanelOptions::SetForceShow(const bool mode) { forceShow = mode; }
bool PanelOptions::GetForceShow() const { return forceShow; }
