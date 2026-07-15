#pragma once

#include <wx/panel.h>
#include "analysis/BoxCountMap.h"

/**
 * @class ImagePanel
 * @brief Displays the fractal occupancy map and its box-counting grid.
 *
 * Occupied boxes are highlighted in blue, the grid is drawn in black, and the
 * current occupied-box count is shown over the preview.
 */
class ImagePanel : public wxPanel
{
    BoxCountMap _map;   ///< Local copy of the occupancy map displayed by the panel.
    int _size;          ///< Fixed square preview size in pixels.
    int _div;           ///< Number of box-grid divisions drawn along each axis.

public:
    /**
     * @brief Creates a square box-count preview panel.
     * @param parent Parent wxWidgets window.
     * @param id Window identifier.
     * @param size Width and height of the preview in pixels.
     */
    ImagePanel(wxWindow* parent, int id, int size);

    /** @brief Paints occupied boxes, grid lines, fractal pixels, and the box count. */
    void OnPaintEvent(wxPaintEvent&);

    /**
     * @brief Replaces the displayed occupancy data and grid resolution.
     * @param map Completed occupancy map to copy.
     * @param div Number of divisions along each axis.
     */
    void SetMap(const BoxCountMap& map, int div);
};
