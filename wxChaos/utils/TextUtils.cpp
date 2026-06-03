#include "TextUtils.h"

#include <sstream>

namespace
{
    std::string ToStdString(const wxString& value)
    {
        const wxScopedCharBuffer buffer = value.ToUTF8();
        return buffer ? std::string(buffer.data()) : std::string();
    }
}

namespace TextUtils
{
    std::string ToString(const int value)
    {
        return std::to_string(value);
    }

    std::string ToString(const double value)
    {
        std::ostringstream output;
        output << value;
        return output.str();
    }

    wxString ToWxString(const int value)
    {
        return wxString::Format(wxT("%d"), value);
    }

    wxString ToWxString(const double value)
    {
        return wxString::FromUTF8(ToString(value).c_str());
    }

    double ToDouble(const wxString& value)
    {
        return ToDouble(ToStdString(value));
    }

    double ToDouble(const std::string& value)
    {
        std::istringstream input(value);
        double number = 0.0;
        return input >> number ? number : 0.0;
    }

    int ToInt(const wxString& value)
    {
        return ToInt(ToStdString(value));
    }

    int ToInt(const std::string& value)
    {
        return static_cast<int>(ToDouble(value));
    }

    std::vector<int> ParseIntList(const wxString& value)
    {
        std::vector<int> numbers;
        std::stringstream input(ToStdString(value));
        std::string token;

        while (std::getline(input, token, ','))
            numbers.push_back(ToInt(token));

        if (!value.empty() && value.Last() == wxT(','))
            numbers.push_back(0);

        return numbers;
    }
}
