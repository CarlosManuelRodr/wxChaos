#pragma once
#include <vector>
#include <wx/wx.h>

enum class PanelOptionType { Label, TextCtrl, Spin, CheckBox };
enum class LinkTo { ToInt, ToDouble, ToBool };

class PanelOptions {
    std::vector<PanelOptionType> type;
    std::vector<wxString> label;
    std::vector<wxString> defaults;
    std::vector<LinkTo> linkTo;
    std::vector<int*> intTarget;
    std::vector<double*> dblTarget;
    std::vector<bool*> boolTarget;
    bool forceShow;
public:
    PanelOptions();
    void LinkInt(PanelOptionType pType, wxString labelTxt, int* linkInt, wxString defaultVal);
    void LinkDbl(PanelOptionType pType, wxString labelTxt, double* linkDbl, wxString defaultVal);
    void LinkBool(PanelOptionType pType, wxString labelTxt, bool* linkBool, wxString defaultVal);
    int GetElementsSize();
    LinkTo GetLinkType(int index);
    wxString GetLabelElement(int index);
    int* GetIntElement(int index);
    double* GetDoubleElement(int index);
    bool* GetBoolElement(int index);
    wxString GetDefault(int index);
    PanelOptionType GetPanelOptType(int index);
    void SetForceShow(bool mode);
    bool GetForceShow();
};
