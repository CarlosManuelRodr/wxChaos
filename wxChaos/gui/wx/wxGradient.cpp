#include "wxGradient.h"

wxGradient::wxGradient()
{
    _min = _max = 0;
}

wxGradient::wxGradient(const std::vector<wxColour>& stops, const unsigned int min, const unsigned int max)
{
    _stops = stops;
    _min = min;
    _max = max;
}

wxGradient::~wxGradient() = default;

wxColour wxGradient::GetColorAt(const double value) const
{
    if (_stops.empty())
        return *wxBLACK;
    if (_stops.size() == 1 || _max <= _min)
        return _stops.front();
    if (value < _min)
        return _stops.front();
    if (value >= _max)
        return _stops.back();

    const unsigned int range = _max - _min;
    const double v = value - _min;
    const double step = range / static_cast<double>(_stops.size() - 1);
    const int bin = static_cast<int>(v / step);
    const double normalized_v = (v - bin*step) / step;
    return Lerp(_stops[bin], _stops[bin+1], normalized_v);
}

void wxGradient::AddColorStop(const wxColour& col)
{
    _stops.push_back(col);
}

void wxGradient::InsertColorStop(const unsigned int index, const wxColour& col)
{
    if (index > static_cast<int>(_stops.size()))
        AddColorStop(col);
    else
        _stops.insert(_stops.begin() + index, col);
}

void wxGradient::RemoveColorStop(const unsigned int index)
{
    _stops.erase(_stops.begin() + index);
}

void wxGradient::EditColorStop(const unsigned int index, const wxColour& col)
{
    _stops[index] = col;
}

void wxGradient::SetMin(const unsigned int min)
{
    _min = min;
}

unsigned int wxGradient::GetMin() const
{
    return _min;
}

void wxGradient::SetMax(const unsigned int max)
{
    _max = max;
}

unsigned int wxGradient::GetMax() const
{
    return _max;
}

void wxGradient::SetStops(const std::vector<wxColour>& stops)
{
    _stops = stops;
}

std::vector<wxColour> wxGradient::GetStops()
{
    return _stops;
}

void wxGradient::FromString(wxString str)
{
    if (str.length()<=1)
    {
        AddColorStop(wxColour(0,0,0));
        AddColorStop(wxColour(255,255,255));
    }
    while(str.length()>1)
    {
        AddColorStop(wxColour(str.BeforeFirst(';')));
        str = str.AfterFirst(';');
    }
}

wxString wxGradient::ToString()
{
    wxString str = "";
    for (auto itr = _stops.begin(); itr!=_stops.end(); ++itr)
    {
        str.Append(itr->GetAsString(wxC2S_CSS_SYNTAX));
        str.Append(';');
    }
    return str;
}

wxColour wxGradient::Lerp(const wxColour& c1, const wxColour& c2, const double value)
{
    const unsigned char R = static_cast<unsigned char>((1.0 - value)*c1.Red() + value*c2.Red());
    const unsigned char G = static_cast<unsigned char>((1.0 - value)*c1.Green() + value*c2.Green());
    const unsigned char B = static_cast<unsigned char>((1.0 - value)*c1.Blue() + value*c2.Blue());
    return {R,G,B};
}
