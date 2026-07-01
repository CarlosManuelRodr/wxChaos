/**
* @file DimensionFrame.h
* @brief Defines a frame to calculate the fractal dimension.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 9/22/2012
*/

#pragma once

#include <memory>
#include <vector>

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/bmpbndl.h>

#include "analysis/BoxCountWorker.h"
#include "analysis/ConfigFractalOptionsDialog.h"
#include "analysis/ImagePanel.h"
#include "analysis/PlotWindow.h"
#include "FractalFactory.h"

wxDECLARE_EVENT(wxEVT_DIMENSION_FRAME_CLOSED, wxCommandEvent);
void GetDesktopResolution(int& width, int& height);

/**
* @enum FractalList
* @brief Lists the fractals available for selection on the DimensionFrame.
*/

enum FractalList
{
    FL_MANDELBROT = 0,
    FL_MANDELBROT_ZN,
    FL_JULIA,
    FL_JULIA_ZN,
    FL_SINUSOIDAL,
    FL_JELLYFISH,
    FL_MANOWAR,
    FL_MANOWAR_JULIA,
    FL_TRICORN,
    FL_BURNING_SHIP,
    FL_BURNING_SHIP_JULIA,
    FL_FRACTORY,
    FL_CELL,
    FL_MAGNET,
    FL_DOUBLE_PENDULUM,
    COUNT
};
/**
* @class DimensionFrame
* @brief Tool window for estimating fractal dimension by box counting.
*
* DimensionFrame renders a square preview of the selected fractal, counts boxes
* across the requested grid divisions, logs the samples, and can plot either the
* collected values or a fitted line.
*/
class DimensionFrame : public wxFrame
{
    wxScrolledWindow* _mainPanel;
    wxChoice* _fractalChoice;
    wxStaticText* _minXTxt;
    wxTextCtrl* _minXCtrl;
    wxStaticText* _maxXTxt;
    wxTextCtrl* _maxXCtrl;
    wxStaticText* _minYTxt;
    wxTextCtrl* _minYCtrl;
    wxCheckBox* _manualMaxYChk;
    wxStaticText* _maxYTxt;
    wxTextCtrl* _maxYCtrl;
    wxStaticText* _iterTxt;
    wxTextCtrl* _iterCtrl;
    wxStaticText* _sizeTxt;
    wxTextCtrl* _sizeCtrl;
    wxButton* _fractalOptionsButton;
    wxStaticText* _nDivTxt;
    wxSpinCtrl* _numberOfDivisionsSpinCtrl;
    wxButton* _previewButton;
    wxStaticText* _divTxt;
    wxNotebook* _divNotebook;
    wxPanel* _byFunctionPanel;
    wxStaticText* _funcTxt;
    wxStaticText* _fDeclTxt;
    wxTextCtrl* _funcCtrl;
    wxStaticText* _goesFromTxt;
    wxSpinCtrl* _xMinSpin;
    wxStaticText* _goesToTxt;
    wxSpinCtrl* _xMaxSpin;
    wxPanel* _byListPanel;
    wxTextCtrl* _listCtrl;
    wxButton* _calcButton;
    wxButton* _closeButton;
    wxStaticLine* _outLine;
    wxRichTextCtrl* _logCtrl;
    wxGauge* _progressBar;
    wxStaticText* _progressTxt;
    wxButton* _savePreviewButton;
    wxCheckBox* _dataCheck;
    wxCheckBox* _dataFitCheck;
    wxBitmapButton* _helpButton;

    ConfigFractalOptionsDialog* _confFractOptDialog;        ///< Fractal options dialog.
    Fractal* _target;                                       ///< The fractal target.
    FractalFactory _fractalFactory;                         ///< The fractal factory.
    ImagePanel* _previewImage;                              ///< Panel to show a preview of the dimension calculator.
    Options _myOpt;                                         ///< Fractal options.
    int _threadNumber;                                      ///< Number of dimension worker threads.
    std::vector<BoxCountWorker> _dimensionCalculator;       ///< Dimension workers, one per thread.
    std::vector<std::unique_ptr<sf::Thread>> _dimThreads;   ///< Owned dimension worker threads.
    std::vector<int> _div;                                  ///< Vector to hold the number of divisions.
    std::vector<double> _epsilon;                           ///< Vector to hold the epsilon values.
    std::vector<int> _boxCount;                             ///< Vector to hold the box counting.
    std::vector<ScriptData> _loadedScripts;                 ///< Parameters and location of user scripts.
    std::vector<unsigned int> _scriptList;                  ///< List of script fractals.
    int _divIndex{};                                        ///< Division index.
    bool _scriptSelected;
    bool _firstRender;

    int _previewSize;
    int _size{};

    bool _renderingPreview, _calculatingDimension;
    int _progress{};
    sf::Clock _clock;

    void OnChangeFractal(wxCommandEvent&);
    void OnRenderPreview(wxCommandEvent&);
    void OnCalculate(wxCommandEvent&);
    void OnUpdateUI(wxUpdateUIEvent&);
    void OnManualMaxY(wxCommandEvent&);
    void OnClose(wxCommandEvent&);
    void OnDestroy(wxCloseEvent&);
    void OnFractalOpt(wxCommandEvent&);
    void OnSavePreview(wxCommandEvent& );
    void OnHelp(wxCommandEvent&);

    static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                        const wxString& darkIcon);
    [[nodiscard]] static wxBitmapBundle CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon, const wxSize& size);
    void CreateFractal(int size);
    /**
     * @brief Waits for all dimension worker threads and releases their resources.
     */
    void JoinDimensionThreads();
    /**
     * @brief Requests all dimension workers to stop, then joins and releases their threads.
     */
    void StopDimensionThreads();
    void GetScriptFractals();                   ///< Creates the menu elements corresponding to the script fractals.
    void WriteText(const wxString &txt) const;  ///< Writes text to the output panel.
public:
    explicit DimensionFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Calculate Dimension",
                            const wxPoint& pos = wxDefaultPosition, const wxSize& size = DimensionFrameSize,
                            long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~DimensionFrame() override;
};
