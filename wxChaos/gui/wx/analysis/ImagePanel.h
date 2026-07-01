#pragma once

#include <wx/panel.h>

class ImagePanel : public wxPanel
{
    bool** _map;
    int _size;
    int _div;

public:
    ImagePanel(wxWindow* parent, int id, int size);
    ~ImagePanel() override;
    void OnPaintEvent(wxPaintEvent&);
    void SetMap(bool** map, int div);
};
