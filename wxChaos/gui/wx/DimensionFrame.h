/**
* @file DimensionFrame.h
* @brief Defines a frame to calculate the fractal dimension.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 9/22/2012
*/

#pragma once
#ifndef _dimFrame
#define _dimFrame

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/bmpbuttn.h>

#include <mathplot.h>
#include "FractalHandler.h"

extern bool dimensionFrameState;
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
    FL_SINOIDAL,
    FL_MEDUSA,
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
* @class DimensionCalculator
* @brief A multithread fractal dimension calculator.
*/

class DimensionCalculator
{
    int _ho, _hf;      ///< Work area of the thread.
    bool** _map;       ///< Fractal target.
    int _size, _div;
    bool _running;
    int _boxCountN;    ///< Number of counted boxes.

public:
    DimensionCalculator();    ///< Constructor.

    ///@brief Sets the map of the fractal target.
    ///@param map Pointer to a fractal map.
    ///@param size Size of the map (it will always be a square map).
    ///@param ho Upper limit of the work area.
    ///@param hf Lower limit of the work area.
    void SetMap(bool** map, int size, int ho, int hf);

    void SetDiv(int div);     ///< Set the number of grid divisions.
    void Run();               ///< The worker function for the thread.
    int GetBoxCount() const;
    bool IsRunning() const;
    void Terminate(); // To signal the thread to stop
};

/**
* @class ImagePanel
* @brief A panel to show a preview of the dimension calculator.
*/

class ImagePanel : public wxPanel
{
    bool** _map;        ///< Fractal target.
    int _size;
    int _div;

public:
    ImagePanel(wxWindow* parent, int id, int size);
    ~ImagePanel() override;
    void OnPaintEvent(wxPaintEvent&);

    ///@brief Sets the map of the fractal target.
    ///@param map Pointer to a fractal map.
    ///@param div Number of grid divisions to draw.
    void SetMap(bool** map, int div);
};

/**
* @class ConfigFractalOptionsDialog
* @brief A fractal options dialog.
*
* This behaves just as the Fractal options on the main frame.
*/
class ConfigFractalOptionsDialog : public wxDialog
{
    wxScrolledWindow* _mainScroll;
    wxBoxSizer* _optionsBox;
    wxStaticText* _kRealLabel;
    wxTextCtrl* _kRealCtrl;
    wxStaticText* _kImaginaryLabel;
    wxTextCtrl* _kImaginaryCtrl;
    wxStaticLine* _staticLine;
    wxButton* _okButton;
    wxButton* _applyButton;
    Fractal* _target;

    // Elements of the option panel.
    std::vector<int> _foundLabels, _foundTextControls;
    std::vector<int> _foundSpinControls, _foundCheckBoxes;
    std::vector<wxStaticText*> _labels;
    std::vector<wxTextCtrl*> _textControls;
    std::vector<wxSpinCtrl*> _spinControls;
    std::vector<wxCheckBox*> _checkBoxes;

    void OnOk(wxCommandEvent&);
    void OnApply(wxCommandEvent&);
    void AdjustOptPanel();        ///< Adjust the option panel when a new fractal type is selected.
    void DeleteOptPanel();        ///< Deletes all the elements in the option panel.

public:
    ConfigFractalOptionsDialog(Fractal* target, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Fractal options"),
                               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(420, 560), long style = wxCAPTION);
    ~ConfigFractalOptionsDialog() override;

    ///@brief Set a new fractal target.
    ///@param target Pointer to new fractal.
    void SetNewTarget(Fractal* target);
};

/**
* @struct LineParams
* @brief Parameters of a line (to be used on the LinePlotter).
*/
struct LineParams
{
    double m, b;
};

/**
* @class LinePlotter
* @brief Class to plot a line on the PlotWindow.
*/
class LinePlotter : public mpFX
{
    LineParams _params;
public:
    explicit LinePlotter(LineParams params);
    double GetY(double x) override;
};


/**
* @class PlotWindow
* @brief Holds a wxMathPlot control.
*/
class PlotWindow : public wxFrame
{
    mpWindow* _plot;
    wxWindowID _id;

public:
    PlotWindow(const std::vector<double> &xList, const std::vector<double> &yList, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Plot"),
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(720, 640), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    PlotWindow(LineParams params, const std::vector<double> &xList, const std::vector<double> &yList, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Plot"),
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(720, 640), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~PlotWindow() override;
};

#define DimensionFrameSize wxSize(1200, 1260)

/**
* @class DimensionFrame
* @brief Frame to calculate the dimension of a fractal.
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
    wxSpinCtrl* _nDivSpin;
    wxButton* _previewButton;
    wxStaticText* _divTxt;
    wxNotebook* _divNotebook;
    wxPanel* _byFunctionPanel;
    wxStaticText* _funcTxt;
    wxStaticText* _fDeclTxt;
    wxTextCtrl* _funcCtrl;
    wxStaticLine* _funcLine;
    wxStaticText* _goesFromTxt;
    wxSpinCtrl* _xMinSpin;
    wxStaticText* _goesToTxt;
    wxSpinCtrl* _xMaxSpin;
    wxPanel* _byListPanel;
    wxTextCtrl* _listCtrl;
    wxCheckBox* _dumpCheck;
    wxTextCtrl* _filePathCtrl;
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

    ConfigFractalOptionsDialog* _confFractOptDialog; ///< Fractal options dialog.
    Fractal* _target;                                ///< The fractal target.
    FractalHandler _fractalHandler;                  ///< The fractal handler.
    ImagePanel* _previewImage;                       ///< Panel to show a preview of the dimension calculator.
    Options _myOpt;                                  ///< Fractal options.
    DimensionCalculator* _dimensionCalculator;       ///< An array of DimCalculator.
    sf::Thread** _dimThreads;                        ///< An array of sf::Thread pointers.
    std::vector<int> _div;                           ///< Vector to hold the number of divisions.
    std::vector<double> _epsilon;                    ///< Vector to hold the epsilon values.
    std::vector<int> _boxCount;                      ///< Vector to hold the box counting.
    std::vector<ScriptData> _loadedScripts;          ///< Parameters and location of user scripts.
    std::vector<int> _scriptList;                    ///< List of script fractals.
    int _divIndex;                                   ///< Division index.
    int _threadNumber;                               ///< Number of render threads.
    bool _scriptSelected;
    bool _firstRender;

    int _previewSize;
    int _size;

    bool _renderingPreview, _calculatingDimension;
    int _progress;
    sf::Clock _clock;

    void OnChangeFractal(wxCommandEvent&);
    void OnRenderPreview(wxCommandEvent&);
    void OnCalculate(wxCommandEvent&);
    void OnUpdateUI(wxUpdateUIEvent&);
    void OnChangeDump(wxCommandEvent&);
    void OnManualMaxY(wxCommandEvent&);
    void OnClose(wxCommandEvent&);
    void OnDestroy(wxCloseEvent&);
    void OnFractalOpt(wxCommandEvent&);
    void OnSavePreview(wxCommandEvent& );
    void OnHelp(wxCommandEvent&);

    void CreateFractal(int size);
    void GetScriptFractals();                   ///< Creates the menu elements corresponding to the script fractals.
    void WriteText(const wxString &txt) const;  ///< Writes text to the output panel.
public:
    explicit DimensionFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Calculate Dimension"),
                            const wxPoint& pos = wxDefaultPosition, const wxSize& size = DimensionFrameSize,
                            long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~DimensionFrame() override;
};

#endif