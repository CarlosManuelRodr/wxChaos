#pragma once

#include <wx/panel.h>
#include "analysis/BoxCountMap.h"

class ImagePanel : public wxPanel
{
    BoxCountMap _map;
    int _size;
    int _div;

public:
    ImagePanel(wxWindow* parent, int id, int size);
    void OnPaintEvent(wxPaintEvent&);
    void SetMap(const BoxCountMap& map, int div);
};
