#include "PanelOptions.h"

PanelOptions::PanelOptions() : forceShow(false) {}

void PanelOptions::LinkInt(PanelOptionType pType, wxString labelTxt, int* linkInt, wxString defaultVal) {}
void PanelOptions::LinkDbl(PanelOptionType pType, wxString labelTxt, double* linkDbl, wxString defaultVal) {}
void PanelOptions::LinkBool(PanelOptionType pType, wxString labelTxt, bool* linkBool, wxString defaultVal) {}
int PanelOptions::GetElementsSize() { return type.size(); }
LinkTo PanelOptions::GetLinkType(int index) { return linkTo.at(index); }
wxString PanelOptions::GetLabelElement(int index) { return label.at(index); }
int* PanelOptions::GetIntElement(int index) { return intTarget.at(index); }
double* PanelOptions::GetDoubleElement(int index) { return dblTarget.at(index); }
bool* PanelOptions::GetBoolElement(int index) { return boolTarget.at(index); }
wxString PanelOptions::GetDefault(int index) { return defaults.at(index); }
PanelOptionType PanelOptions::GetPanelOptType(int index) { return type.at(index); }
void PanelOptions::SetForceShow(bool mode) { forceShow = mode; }
bool PanelOptions::GetForceShow() { return forceShow; }
