#pragma once

#include <string>
#include <vector>
#include <wx/string.h>

namespace TextUtils
{
    std::string ToString(int value);
    std::string ToString(double value);
    wxString ToWxString(int value);
    wxString ToWxString(long long value);
    wxString ToWxString(double value);
    double ToDouble(const wxString& value);
    double ToDouble(const std::string& value);
    int ToInt(const wxString& value);
    int ToInt(const std::string& value);
    std::vector<int> ParseIntList(const wxString& value);
    std::string Trim(const std::string& value);
}
