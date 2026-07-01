#pragma once

#include <vector>
#include <wx/frame.h>
#include <mathplot.h>
#include "analysis/LinePlotter.h"

class PlotWindow : public wxFrame
{
    mpWindow* _plot;
    wxWindowID _id{};

public:
    PlotWindow(const std::vector<double> &xList, const std::vector<double> &yList, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Plot",
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(720, 640), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    PlotWindow(LineParams params, const std::vector<double> &xList, const std::vector<double> &yList, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Plot",
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(720, 640), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~PlotWindow() override;
};
