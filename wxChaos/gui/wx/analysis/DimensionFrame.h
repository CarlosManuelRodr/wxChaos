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
#include <wx/bmpbuttn.h>
#include <wx/bmpbndl.h>
#include <wx/spinctrl.h>
#include <wx/timer.h>

#include "analysis/BoxCountWorker.h"
#include "analysis/BoxCountMap.h"
#include "analysis/ImagePanel.h"
#include "analysis/PlotWindow.h"
#include "common/FractalOptionsPanel.h"
#include "FractalFactory.h"

wxDECLARE_EVENT(wxEVT_DIMENSION_FRAME_CLOSED, wxCommandEvent);
void GetDesktopResolution(int& width, int& height);

#define DimensionFrameSize wxSize(1200, 1260)

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
    wxSpinCtrlDouble* _minXCtrl;
    wxSpinCtrlDouble* _maxXCtrl;
    wxSpinCtrlDouble* _minYCtrl;
    wxSpinCtrlDouble* _maxYCtrl;
    wxSpinCtrl* _iterCtrl;
    wxSpinCtrl* _sizeCtrl;
    wxButton* _fractalOptionsButton;
    wxButton* _formulaButton;
    wxStaticText* _nDivTxt;
    wxSpinCtrl* _numberOfDivisionsSpinCtrl;
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

    wxDialog* _fractalOptionsDialog{};                      ///< Fractal options dialog.
    FractalOptionsPanel* _fractalOptionsPanel{};            ///< Reusable fractal options panel.
    Fractal* _target;                                       ///< The fractal target.
    FractalFactory _fractalFactory;                         ///< The fractal factory.
    ImagePanel* _previewImage;                              ///< Panel to show a preview of the dimension calculator.
    Options _myOpt;                                         ///< Fractal options.
    FormulaOptions _userFormula;                            ///< User-defined escape-time formula used by this tool.
    int _threadNumber;                                      ///< Number of dimension worker threads.
    std::vector<BoxCountWorker> _dimensionCalculator;       ///< Dimension workers, one per thread.
    BoxCountMap _boxCountMap;                               ///< Unified occupancy map for raster and vector fractals.
    std::vector<std::unique_ptr<sf::Thread>> _dimThreads;   ///< Owned dimension worker threads.
    std::vector<int> _div;                                  ///< Vector to hold the number of divisions.
    std::vector<double> _epsilon;                           ///< Vector to hold the epsilon values.
    std::vector<int> _boxCount;                             ///< Vector to hold the box counting.
    std::vector<ScriptData> _loadedScripts;                 ///< Parameters and location of user scripts.
    std::vector<FractalType> _builtInFractalList;           ///< Built-in fractals shown before user scripts.
    std::vector<unsigned int> _scriptList;                  ///< List of script fractals.
    int _divIndex{};                                        ///< Division index.
    bool _scriptSelected;
    bool _firstRender;

    int _previewSize;
    int _size{};

    bool _renderingPreview, _calculatingDimension;
    bool _suppressPreviewUpdate{};
    bool _previewRenderQueued{};
    bool _hasPreviewMap{};
    int _progress{};
    sf::Clock _clock;
    wxTimer _previewTimer;

    void OnChangeFractal(wxCommandEvent&);
    void OnCalculate(wxCommandEvent&);
    void OnUpdateUI(wxUpdateUIEvent&);
    void OnPreviewTimer(wxTimerEvent&);
    void OnPreviewParameterChanged(wxCommandEvent&);
    void OnPreviewDoubleParameterChanged(wxSpinDoubleEvent&);
    void OnPreviewGridChanged(wxCommandEvent&);
    void OnClose(wxCommandEvent&);
    void OnDestroy(wxCloseEvent&);
    void OnFractalOpt(wxCommandEvent&);
    void OnFormula(wxCommandEvent& event);
    void OnSavePreview(wxCommandEvent& );
    void OnHelp(wxCommandEvent&);
    void AddBuiltInFractalChoice(const wxString& label, FractalType type);
    void PopulateFractalChoices();
    [[nodiscard]] bool IsUserDefinedEscapeTimeSelected() const;
    void UpdateFormulaButtonVisibility() const;

    static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                        const wxString& darkIcon);
    [[nodiscard]] static wxBitmapBundle CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon, const wxSize& size);
    wxSizer* CreateFractalParameterRow(const wxString& label, wxWindow* control) const;
    [[nodiscard]] wxSpinCtrlDouble* CreateCoordinateSpin(const wxString& value) const;
    void CreateFractal(int size);
    [[nodiscard]] Options ReadDimensionOptions();
    void UpdateDerivedMaxY() const;
    void SetControlsFromOptions(const Options& options);
    void SchedulePreviewRender();
    void StartPreviewRender();
    void StopPreviewRender() const;
    void RefreshPreviewOverlayOnly();
    void UpdateBoxCountMap();
    void ConfigureDimensionWorkers();

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
    explicit DimensionFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxTRANSLATE("Calculate Dimension"),
                            const wxPoint& pos = wxDefaultPosition, const wxSize& size = DimensionFrameSize,
                            long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~DimensionFrame() override;
};
