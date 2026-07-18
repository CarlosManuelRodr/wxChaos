#include <wx/sizer.h>
#include "AppPaths.h"
#include "analysis/PlotWindow.h"

using namespace std;

PlotWindow::PlotWindow(const vector<double> &xList, const vector<double> &yList, wxWindow* parent, const wxWindowID id,
                       const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                       : wxFrame(parent, id, title, pos, size, style)
{
    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.ico"}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxFont graphFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    _plot = new mpWindow(this, -1, wxPoint(0, 0), wxSize(500, 500), wxBORDER_NONE);
    auto* xAxis = new mpScaleX(_("Epsilon"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    const auto yAxis = new mpScaleY(_("N"), mpALIGN_LEFT, true);
    xAxis->SetDrawOutsideMargins(false);
    yAxis->SetDrawOutsideMargins(false);
    xAxis->SetFont(graphFont);
    yAxis->SetFont(graphFont);

    _plot->SetMargins(30, 30, 50, 100);
    _plot->AddLayer(xAxis);
    _plot->AddLayer(yAxis);
    const auto vectorLayer = new mpFXYVector(_("Data"));
    vectorLayer->SetData(xList, yList);
    vectorLayer->SetPen(wxPen(*wxBLUE, 3, wxPENSTYLE_SOLID));
    _plot->AddLayer(vectorLayer);
    _plot->Fit();

    const auto topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(_plot, 1, wxEXPAND);
    SetAutoLayout(TRUE);
    SetSizer(topSizer);
}

PlotWindow::PlotWindow(const LineParams params, const vector<double> &xList, const vector<double> &yList, wxWindow* parent,
                       const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                       : wxFrame(parent, id, title, pos, size, style), _id(id)
{
    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.ico"}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxFont graphFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    _plot = new mpWindow(this, -1, wxPoint(0, 0), wxSize(500, 500), wxBORDER_NONE);
    const auto xAxis = new mpScaleX(_("Log(1/Epsilon)"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    const auto yAxis = new mpScaleY(_("Log(N)"), mpALIGN_LEFT, true);
    xAxis->SetDrawOutsideMargins(false);
    yAxis->SetDrawOutsideMargins(false);
    xAxis->SetFont(graphFont);
    yAxis->SetFont(graphFont);

    _plot->SetMargins(30, 30, 50, 100);
    _plot->AddLayer(xAxis);
    _plot->AddLayer(yAxis);
    const auto vectorLayer = new mpFXYVector(_("Data"));
    vectorLayer->SetData(xList, yList);
    vectorLayer->SetPen(wxPen(*wxBLUE, 3, wxPENSTYLE_SOLID));
    _plot->AddLayer(new LinePlotter(params));
    _plot->AddLayer(vectorLayer);
    _plot->Fit();

    auto* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(_plot, 1, wxEXPAND);
    SetAutoLayout(TRUE);
    SetSizer(topSizer);
}

PlotWindow::~PlotWindow() = default;
