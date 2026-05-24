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
int PanelOptions::GetElementsSize() const { return type.size(); }
LinkTo PanelOptions::GetLinkType(const int index) const { return linkTo.at(index); }
wxString PanelOptions::GetLabelElement(const int index) { return label.at(index); }
int* PanelOptions::GetIntElement(const int index) const { return intTarget.at(index); }
double* PanelOptions::GetDoubleElement(const int index) const { return dblTarget.at(index); }
bool* PanelOptions::GetBoolElement(const int index) const { return boolTarget.at(index); }
wxString PanelOptions::GetDefault(const int index) { return defaults.at(index); }
PanelOptionType PanelOptions::GetPanelOptType(const int index) const { return type.at(index); }
void PanelOptions::SetForceShow(const bool mode) { forceShow = mode; }
bool PanelOptions::GetForceShow() const { return forceShow; }
