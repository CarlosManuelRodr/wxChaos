#pragma once

#include <vector>
#include <wx/frame.h>
#include <mathplot.h>
#include "analysis/LinePlotter.h"

/**
 * @class PlotWindow
 * @brief Window that plots box-count samples and, optionally, their fitted line.
 */
class PlotWindow : public wxFrame
{
    mpWindow* _plot;    ///< MathPlot canvas owned by this wxWidgets window.
    wxWindowID _id{};  ///< Identifier assigned to the plot window.

public:
    /**
     * @brief Creates a scatter plot from paired x and y samples.
     * @param xList Horizontal coordinates.
     * @param yList Vertical coordinates paired with xList.
     * @param parent Parent wxWidgets window.
     * @param id Window identifier.
     * @param title Window title.
     * @param pos Initial window position.
     * @param size Initial window size.
     * @param style wxWidgets frame style flags.
     */
    PlotWindow(const std::vector<double> &xList, const std::vector<double> &yList, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Plot",
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(720, 640), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

    /**
     * @brief Creates a plot containing paired samples and a fitted line.
     * @param params Slope and intercept of the fitted line.
     * @param xList Horizontal sample coordinates and fitted-line domain.
     * @param yList Vertical sample coordinates paired with xList.
     * @param parent Parent wxWidgets window.
     * @param id Window identifier.
     * @param title Window title.
     * @param pos Initial window position.
     * @param size Initial window size.
     * @param style wxWidgets frame style flags.
     */
    PlotWindow(LineParams params, const std::vector<double> &xList, const std::vector<double> &yList, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Plot",
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(720, 640), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

    /** @brief Destroys the plot window and its child plotting canvas. */
    ~PlotWindow() override;
};
