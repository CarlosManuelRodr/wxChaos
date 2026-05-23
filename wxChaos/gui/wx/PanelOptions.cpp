#include "PanelOptions.h"

PanelOptions::PanelOptions() : forceShow(false) {}

void PanelOptions::LinkInt(PanelOptionType pType, wxString labelTxt, int* linkInt, wxString defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    defaults.push_back(defaultVal);
    linkTo.push_back(LinkTo::ToInt);
    intTarget.push_back(linkInt);
}
void PanelOptions::LinkDbl(PanelOptionType pType, wxString labelTxt, double* linkDbl, wxString defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    defaults.push_back(defaultVal);
    linkTo.push_back(LinkTo::ToDouble);
    dblTarget.push_back(linkDbl);
}
void PanelOptions::LinkBool(PanelOptionType pType, wxString labelTxt, bool* linkBool, wxString defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    defaults.push_back(defaultVal);
    linkTo.push_back(LinkTo::ToBool);
    boolTarget.push_back(linkBool);
}
int PanelOptions::GetElementsSize() const { return type.size(); }
LinkTo PanelOptions::GetLinkType(int index) const { return linkTo.at(index); }
wxString PanelOptions::GetLabelElement(int index) { return label.at(index); }
int* PanelOptions::GetIntElement(int index) const { return intTarget.at(index); }
double* PanelOptions::GetDoubleElement(int index) const { return dblTarget.at(index); }
bool* PanelOptions::GetBoolElement(int index) const { return boolTarget.at(index); }
wxString PanelOptions::GetDefault(int index) { return defaults.at(index); }
PanelOptionType PanelOptions::GetPanelOptType(int index) const { return type.at(index); }
void PanelOptions::SetForceShow(bool mode) { forceShow = mode; }
bool PanelOptions::GetForceShow() const { return forceShow; }
