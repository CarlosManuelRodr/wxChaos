#include <cmath>
#include <wx/dcclient.h>
#include "TextUtils.h"
#include "analysis/ImagePanel.h"

ImagePanel::ImagePanel(wxWindow* parent, const int id, const int size)
                       : wxPanel(parent, id, wxDefaultPosition, wxSize(size, size))
{
    _size = size;
    _div = 20;

    this->wxWindowBase::SetMinSize(wxSize(_size, _size));
    this->SetInitialSize(wxSize(_size, _size));
    this->Bind(wxEVT_PAINT, &ImagePanel::OnPaintEvent, this);
}

void ImagePanel::OnPaintEvent(wxPaintEvent&)
{
    wxPaintDC dc(this);
    dc.SetBrush(wxBrush(wxColour(255, 255, 255)));
    dc.SetPen(wxPen(wxColour(255, 255, 255)));
    dc.DrawRectangle(0, 0, _size, _size);

    if (_map.GetSize() == _size)
    {
        const double epsilon = static_cast<double>(_size) / static_cast<double>(_div);
        int boxes = 0;

        dc.SetPen(wxColour(50, 50, 255));
        dc.SetBrush(wxBrush(wxColour(50, 50, 255)));

        for (int ey = 0; ey < _div; ey++)
        {
            for (int ex = 0; ex < _div; ex++)
            {
                bool found = false;

                for (int w = static_cast<int>(ex * epsilon); w < (ex + 1) * epsilon && !found; w++)
                {
                    for (int h = static_cast<int>(ey * epsilon); h < (ey + 1) * epsilon; h++)
                    {
                        if (w < _size && h < _size)
                        {
                            if (_map.IsOccupied(w, h))
                            {
                                found = true;
                                boxes++;
                                const auto xPosition = static_cast<wxCoord>(ex * epsilon);
                                const auto yPosition = static_cast<wxCoord>(ey * epsilon);
                                dc.DrawRectangle(xPosition, yPosition, ceil(epsilon), ceil(epsilon));
                                break;
                            }
                        }
                        else break;
                    }
                }
            }
        }

        dc.SetPen(wxColour(0, 0, 0));
        for (int ey = 0; ey < _div; ey++)
        {
            const int y = static_cast<int>(ey * epsilon);
            dc.DrawLine(0, y, _size, y);
        }
        dc.DrawLine(0, _size - 1, _size, _size - 1);

        for (int ex = 0; ex < _div; ex++)
        {
            const int x = static_cast<int>(ex * epsilon);
            dc.DrawLine(x, 0, x, _size);
        }
        dc.DrawLine(_size - 1, 0, _size - 1, _size);

        for (int i = 0; i < _size; i++)
        {
            for (int j = 0; j < _size; j++)
            {
                if (_map.IsOccupied(i, j))
                {
                    dc.DrawPoint(i, j);
                }
            }
        }

        wxString outText = _("N = ");
        outText += TextUtils::ToWxString(boxes);
        int textWidth;
        int textHeight;
        dc.GetTextExtent(outText, &textWidth, &textHeight);

        constexpr int textPadding = 5;
        constexpr int bottomMargin = 2;
        const int labelHeight = textHeight + textPadding * 2;
        const int labelTop = _size - labelHeight - bottomMargin;

        dc.SetBrush(wxBrush(wxColour(0, 0, 0, 100)));
        dc.SetTextForeground(wxColour(255, 255, 255));
        dc.DrawRectangle(0, labelTop, textWidth + textPadding * 2, labelHeight);
        dc.DrawText(outText, textPadding, labelTop + textPadding);
    }
}

void ImagePanel::SetMap(const BoxCountMap& map, const int div)
{
    _map = map;
    _div = div;
}
