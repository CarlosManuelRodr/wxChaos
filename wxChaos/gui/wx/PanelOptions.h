#pragma once
#include <vector>
#include <wx/string.h>

enum class PanelOptionType { Label, TextCtrl, Spin, CheckBox };
enum class LinkTo { ToInt, ToDouble, ToBool };

class PanelOptions
{
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
    void LinkInt(PanelOptionType pType, const wxString& labelTxt, int* linkInt, const wxString& defaultVal);
    void LinkDbl(PanelOptionType pType, const wxString& labelTxt, double* linkDbl, const wxString& defaultVal);
    void LinkBool(PanelOptionType pType, const wxString& labelTxt, bool* linkBool, const wxString& defaultVal);
    [[nodiscard]] size_t GetElementsSize() const;
    [[nodiscard]] LinkTo GetLinkType(unsigned int index) const;
    wxString GetLabelElement(unsigned int index);
    [[nodiscard]] int* GetIntElement(unsigned int index) const;
    [[nodiscard]] double* GetDoubleElement(unsigned int index) const;
    [[nodiscard]] bool* GetBoolElement(unsigned int index) const;
    wxString GetDefault(unsigned int index);
    [[nodiscard]] PanelOptionType GetPanelOptType(unsigned int index) const;
    void SetForceShow(bool mode);
    [[nodiscard]] bool GetForceShow() const;
};
