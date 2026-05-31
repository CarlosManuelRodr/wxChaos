#include "wxGradient.h"

wxGradient::wxGradient()
{
    m_min = m_max = 0;
}

wxGradient::wxGradient(const std::vector<wxColour>& stops, const int min, const int max)
{
    m_stops = stops;
    m_min = min;
    m_max = max;
}

wxGradient::~wxGradient() = default;

wxColour wxGradient::GetColorAt(const int value) const
{
    if (value < m_min)
    {
        return m_min;
    }
    if (value > m_max)
    {
        return m_max;
    }

    const int range = m_max - m_min;
    const int v = value - m_min;
    const double step = range / static_cast<double>(m_stops.size() - 1);
    const int bin = static_cast<int>(v / step);
    const double normalized_v = (v - bin*step) / step;
    return Lerp(m_stops[bin], m_stops[bin+1], normalized_v);
}

void wxGradient::AddColorStop(const wxColour& col)
{
    m_stops.push_back(col);
}

void wxGradient::InsertColorStop(const int index, const wxColour& col)
{
    if (index > static_cast<int>(m_stops.size()))
        AddColorStop(col);
    else
        m_stops.insert(m_stops.begin() + index, col);
}

void wxGradient::RemoveColorStop(const int index)
{
    m_stops.erase(m_stops.begin() + index);
}

void wxGradient::EditColorStop(const int index, const wxColour& col)
{
    m_stops[index] = col;
}

void wxGradient::SetMin(const int min)
{
    m_min = min;
}

int wxGradient::GetMin() const
{
    return m_min;
}

void wxGradient::SetMax(const int max)
{
    m_max = max;
}

int wxGradient::GetMax() const
{
    return m_max;
}

void wxGradient::SetStops(const std::vector<wxColour>& stops)
{
    m_stops = stops;
}

std::vector<wxColour> wxGradient::GetStops()
{
    return m_stops;
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
    wxString str = wxT("");
    for (auto itr = m_stops.begin(); itr!=m_stops.end(); ++itr)
    {
        str.Append(itr->GetAsString(wxC2S_CSS_SYNTAX));
        str.Append(';');
    }
    return str;
}

wxColour wxGradient::Lerp(const wxColour& c1, const wxColour& c2, const double value)
{
    const unsigned char R = static_cast<char>((1.0 - value)*c1.Red() + value*c2.Red());
    const unsigned char G = static_cast<char>((1.0 - value)*c1.Green() + value*c2.Green());
    const unsigned char B = static_cast<char>((1.0 - value)*c1.Blue() + value*c2.Blue());
    return {R,G,B};
}