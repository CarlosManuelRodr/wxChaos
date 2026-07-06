#pragma once
#include <vector>
#include <wx/string.h>

enum class PanelOptionType { Label, TextCtrl, Spin, CheckBox };
enum class LinkTo { ToInt, ToDouble, ToBool };

class PanelOptions
{
    std::vector<PanelOptionType> _type;
    std::vector<wxString> _label;
    std::vector<wxString> _defaults;
    std::vector<LinkTo> _linkTo;
    std::vector<int*> _integerTarget;
    std::vector<double*> _doubleTarget;
    std::vector<bool*> _boolTarget;
    bool _forceShow;

public:
    PanelOptions();
    void LinkInteger(PanelOptionType type, const wxString& labelText, int* linkInt, const wxString& defaultValue);
    void LinkDouble(PanelOptionType type, const wxString& labelText, double* linkDbl, const wxString& defaultValue);
    void LinkBool(PanelOptionType type, const wxString& labelText, bool* linkBool, const wxString& defaultValue);
    [[nodiscard]] size_t GetElementsSize() const;
    [[nodiscard]] LinkTo GetLinkType(unsigned int index) const;
    wxString GetLabelValue(unsigned int index);
    [[nodiscard]] int* GetIntegerValue(unsigned int index) const;
    [[nodiscard]] double* GetDoubleValue(unsigned int index) const;
    [[nodiscard]] bool* GetBoolValue(unsigned int index) const;
    wxString GetDefault(unsigned int index);
    [[nodiscard]] PanelOptionType GetPanelOptionType(unsigned int index) const;
    void CopyValuesFrom(const PanelOptions& source);
    void SetForceShow(bool mode);
    [[nodiscard]] bool GetForceShow() const;
};
