#include "TextUtils.h"

#include <sstream>

namespace TextUtils
{
    static std::string ToStdString(const wxString& value)
    {
        const wxScopedCharBuffer buffer = value.ToUTF8();
        return buffer ? std::string(buffer.data()) : std::string();
    }

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
        return wxString::Format("%d", value);
    }

    wxString ToWxString(const long long value)
    {
        return wxString::Format("%d", value);
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

        if (!value.empty() && value.Last() == ',')
            numbers.push_back(0);

        return numbers;
    }

    std::string Trim(const std::string& value)
    {
        auto begin = value.begin();
        while (begin != value.end() && std::isspace(static_cast<unsigned char>(*begin)))
            ++begin;

        auto end = value.end();
        while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1))))
            --end;

        return {begin, end};
    }
}
