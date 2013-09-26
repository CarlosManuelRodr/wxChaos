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

#include "wxMathPlot/mathplot.h"
#include "FractalTypes.h"
#include "global.h"

extern bool dimFrameState;
void GetDesktopResolution(int& width, int& height);

/**
* @enum FRACTAL_TYPE
* @brief Enumerates the fractals available for selection on the DimensionFrame.
*/

enum FractalList
{
	FL_HENON_MAP = 0,
	FL_MANDELBROT,
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
	FL_LOGISTIC_MAP,
	COUNT
};

/**
* @class DimCalculator
* @brief A multithread fractal dimension calculator.
*/

class DimCalculator : public sf::Thread
{
	int ho, hf;			///< Work area of the thread.
	bool **map;			///< Fractal target.
	int size, div;
	bool running;
	int N;				///< Number of counted boxes.

public:
	DimCalculator();	///< Constructor.

	///@brief Sets the map of the fractal target.
	///@param _map Pointer to fractal map.
	///@param _size Size of the map (it will always be a square map).
	///@param _ho Upper limit of the workarea.
	///@param _hf Lower limit of the workarea.
	void SetMap(bool **_map, int _size, int _ho, int _hf);

	void SetDiv(int _div);  ///< Set number of grid divisions.
	void Run();				///< Start DimCalculator thread.
	int GetBoxCount();
	bool IsRunning();
};

/**
* @class ImagePanel
* @brief A panel to show a preview of the dimension calculator.
*/

class ImagePanel : public wxPanel
{
	bool **map;		///< Fractal target.
	int size;
	int div;

public:
	ImagePanel(wxWindow* parent, int id, int _size);
	~ImagePanel();
	void OnPaint(wxPaintEvent& event);

	///@brief Sets the map of the fractal target.
	///@param _map Pointer to fractal map.
	///@param _div Number of grid divisions to draw.
	void SetMap(bool **_map, int _div);
};

/**
* @class ConfFractOptDialog
* @brief A fractal options dialog.
*
* This works just as the Fractal options on the main frame.
*/
class ConfFractOptDialog : public wxDialog
{
	wxScrolledWindow* mainScroll;
	wxBoxSizer* optionsBoxxy;
	wxStaticText* kRealLabel;
	wxTextCtrl* kRealCtrl;
	wxStaticText* kImaginaryLabel;
	wxTextCtrl* kImaginaryCtrl;
	wxStaticLine* staticLine;
	wxButton* okButton;
	wxButton* applyButton;
	Fractal* target;

	// Elements of the option panel.
	vector<int> foundLabels, foundTextControls;
	vector<int> foundSpinControls, foundCheckBoxes;
	vector<wxStaticText*> labels;
	vector<wxTextCtrl*> textControls;
	vector<wxSpinCtrl*> spinControls;
	vector<wxCheckBox*> checkBoxes;

	void OnOk( wxCommandEvent& event );
	void OnApply( wxCommandEvent& event );
	void AdjustOptPanel();		///< Adjust the option panel when a new fractal type is selected.
	void DeleteOptPanel();		///< Deletes all the elements in the option panel.

public:
	ConfFractOptDialog( Fractal* _target, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT(menuFractalOptTxt),
						const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 213,361 ), long style = wxCAPTION );
	~ConfFractOptDialog();

	///@brief Set new fractal target.
	///@param _target Pointer to new fractal.
	void SetNewTarget(Fractal* _target);
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
	LineParams params;
public:
	LinePlotter(LineParams _params);
	virtual double GetY(double x);
};


/**
* @class PlotWindow
* @brief Holds a wxMathPlot control.
*/
class PlotWindow : public wxFrame
{
	mpWindow *m_plot;

public:
	PlotWindow(vector<double> xList, vector<double> yList, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Plot"),
				const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 390,390 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	PlotWindow(LineParams params, vector<double> xList, vector<double> yList, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Plot"),
				const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 390,390 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	~PlotWindow();
};

#ifdef _WIN32
#define DimensionFrameSize wxSize( 890,674 )
#elif __linux__
#define DimensionFrameSize wxSize( 890,710 )
#endif

/**
* @class DimensionFrame
* @brief Frame to calculate the dimension of a fractal.
*/
class DimensionFrame : public wxFrame
{
	wxScrolledWindow* mainPanel;
	wxChoice* fractalChoice;
	wxStaticText* minXTxt;
	wxTextCtrl* minXCtrl;
	wxStaticText* maxXTxt;
	wxTextCtrl* maxXCtrl;
	wxStaticText* minYTxt;
	wxTextCtrl* minYCtrl;
	wxCheckBox* manualMaxYChk;
	wxStaticText* maxYTxt;
	wxTextCtrl* maxYCtrl;
	wxStaticText* iterTxt;
	wxTextCtrl* iterCtrl;
	wxStaticText* sizeTxt;
	wxTextCtrl* sizeCtrl;
	wxButton* fOptButton;
	wxStaticText* nDivTxt;
	wxSpinCtrl* nDivSpin;
	wxButton* previewButton;
	wxStaticText* divTxt;
	wxNotebook* divNotebook;
	wxPanel* byFunctionPanel;
	wxStaticText* funcTxt;
	wxStaticText* fDeclTxt;
	wxTextCtrl* funcCtrl;
	wxStaticLine* funcLine;
	wxStaticText* goesFromTxt;
	wxSpinCtrl* xMinSpin;
	wxStaticText* goesToTxt;
	wxSpinCtrl* xMaxSpin;
	wxPanel* byListPanel;
	wxTextCtrl* listCtrl;
	wxCheckBox* dumpCheck;
	wxTextCtrl* filePathCtrl;
	wxButton* calcButton;
	wxButton* closeButton;
	wxStaticLine* outLine;
	wxRichTextCtrl* logCtrl;
	wxGauge* progressBar;
	wxStaticText* progressTxt;
	wxButton* savePreviewButton;
	wxCheckBox* dataCheck;
	wxCheckBox* dataFitCheck;
	wxBitmapButton* helpButton;

	ConfFractOptDialog *confFractOptDialog;		///< Fractal options dialog.
	Fractal* target;							///< The fractal target.
	FractalHandler fractalHandler;				///< The fractal handler.
	ImagePanel* previewImage;					///< Panel to show a preview of the dimension calculator.
	Options myOpt;								///< Fractal options.
	DimCalculator *dimCalculator;				///< An array of DimCalculator.
	vector<int> div;							///< Vector to hold the number of divisions.
	vector<double> epsilon;						///< Vector to hold the epsilon values.
	vector<int> boxCount;						///< Vector to hold the box counting.
	int divIndex;						        ///< Division index.
	int threadNumber;
	vector<int> scriptList;						///< List of script fractals.
	bool scriptSelected;
	bool firstRender;

	int previewSize;
	int size;

	bool renderingPreview, calculatingDimension;
	int progress;
	sf::Clock clock;

	void OnChangeFractal( wxCommandEvent& event );
	void OnRenderPreview( wxCommandEvent& event );
	void OnCalculate( wxCommandEvent& event );
	void OnUpdateUI( wxUpdateUIEvent& event );
	void OnChangeDump( wxCommandEvent& event );
	void OnManualMaxY( wxCommandEvent& event );
	void OnClose( wxCommandEvent& event );
	void OnDestroy( wxCloseEvent& event );
	void OnFractalOpt( wxCommandEvent& event );
	void OnSavePreview( wxCommandEvent& event );
	void OnHelp( wxCommandEvent& event );

	void CreateFractal(int size);
	void GetScriptFractals();		///< Creates the menu elements corresponding to the script fractals.
	void WriteText(wxString txt);	///< Writes text to the output panel.

public:

	DimensionFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT(calcDimTxt),
					const wxPoint& pos = wxDefaultPosition, const wxSize& size = DimensionFrameSize, long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	~DimensionFrame();

};

#endif
