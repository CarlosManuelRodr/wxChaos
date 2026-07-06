#include "common/PanelOptions.h"

PanelOptions::PanelOptions() : _forceShow(false) {}

void PanelOptions::LinkInteger(const PanelOptionType type, const wxString& labelText, int* linkInteger,
                               const wxString& defaultValue)
{
    _type.push_back(type);
    _label.push_back(labelText);
    _defaults.push_back(defaultValue);
    _linkTo.push_back(LinkTo::ToInt);
    _integerTarget.push_back(linkInteger);
    _increments.push_back(1.0);
}
void PanelOptions::LinkDouble(const PanelOptionType type, const wxString& labelText, double* linkDouble,
                              const wxString& defaultValue, double increment)
{
    _type.push_back(type);
    _label.push_back(labelText);
    _defaults.push_back(defaultValue);
    _linkTo.push_back(LinkTo::ToDouble);
    _doubleTarget.push_back(linkDouble);
    _increments.push_back(increment);
}
void PanelOptions::LinkBool(const PanelOptionType type, const wxString& labelText, bool* linkBool,
                            const wxString& defaultValue)
{
    _type.push_back(type);
    _label.push_back(labelText);
    _defaults.push_back(defaultValue);
    _linkTo.push_back(LinkTo::ToBool);
    _boolTarget.push_back(linkBool);
    _increments.push_back(1.0);
}
size_t PanelOptions::GetElementsSize() const { return _type.size(); }
LinkTo PanelOptions::GetLinkType(const unsigned int index) const { return _linkTo.at(index); }
wxString PanelOptions::GetLabelValue(const unsigned int index) { return _label.at(index); }
int* PanelOptions::GetIntegerValue(const unsigned int index) const { return _integerTarget.at(index); }
double* PanelOptions::GetDoubleValue(const unsigned int index) const { return _doubleTarget.at(index); }
bool* PanelOptions::GetBoolValue(const unsigned int index) const { return _boolTarget.at(index); }
double PanelOptions::GetIncrement(const unsigned int index) const { return _increments.at(index); }
wxString PanelOptions::GetDefault(const unsigned int index) { return _defaults.at(index); }
PanelOptionType PanelOptions::GetPanelOptionType(const unsigned int index) const { return _type.at(index); }
void PanelOptions::CopyValuesFrom(const PanelOptions& source) const
{
    if (_linkTo.size() != source._linkTo.size())
        return;

    for (size_t i = 0; i < _linkTo.size(); ++i)
    {
        if (_linkTo[i] != source._linkTo[i])
            return;
    }

    size_t intIndex = 0;
    size_t doubleIndex = 0;
    size_t boolIndex = 0;
    for (const auto i : _linkTo)
    {
        switch (i)
        {
            case LinkTo::ToInt:
                *_integerTarget.at(intIndex) = *source._integerTarget.at(intIndex);
                ++intIndex;
                break;
            case LinkTo::ToDouble:
                *_doubleTarget.at(doubleIndex) = *source._doubleTarget.at(doubleIndex);
                ++doubleIndex;
                break;
            case LinkTo::ToBool:
                *_boolTarget.at(boolIndex) = *source._boolTarget.at(boolIndex);
                ++boolIndex;
                break;
        }
    }
}
void PanelOptions::SetForceShow(const bool mode) { _forceShow = mode; }
bool PanelOptions::GetForceShow() const { return _forceShow; }
