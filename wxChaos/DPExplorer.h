/**
* @file DPExplorer.h
* @brief This header file contains the frames and classes related to the DPExplorer dialog.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 9/26/2012
*/

#pragma once
#ifndef _DPExplorer
#define _DPExplorer

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/statusbr.h>
#include <cstdlib>


#include "muParserX/mpParser.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "global.h"
using namespace std;

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#endif

extern bool dpExplorerState;

/**
* @struct DPSimOpt
* @brief Holds the simulation parameters.
*/
struct DPSimOpt
{
	bool th1Bailout;
	bool th2Bailout;
	double th1NumBailout;
	double th2NumBailout;

	double th1, th2;
	double vth1, vth2;
	double m1, m2;
	double l, g, dt;
};

/**
* @class DPSim
* @brief Launches a simulation of the double pendulum.
*/

class DPSim : public sf::Thread
{
	double vth1, vth2;
	double m1, m2;
	double l, g, dt;

	bool th1Bailout;
	bool th2Bailout;
	double th1NumBailout;
	double th2NumBailout;

	double ecMov1, ecMov2;
	bool isRunning;

	sf::Clock clock;

public:
	DPSim(DPSimOpt myOpt);
	~DPSim();
	virtual void Run();
	void Stop();

	bool eventReached;
	double th1, th2;
};


/**
* @class DPSimFrame
* @brief Frame to hold the simulation on Linux.
*/
class DPSimFrame : public wxFrame
{
    DPSim* dpSim;
    double l;

public:
    DPSimFrame(DPSimOpt myOpt, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT(dpSimTxt),
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 450,450 ), long style = wxCAPTION|wxCLOSE_BOX|wxMINIMIZE|wxTAB_TRAVERSAL );
    ~DPSimFrame();

    void OnUpdateUI( wxUpdateUIEvent& event );
};


/**
* @enum IDS_MW
* @brief Enum used to connect events in the DPExplorer frame.
*/
enum IDS_MW
{
	ID_DPOPEN = 1,
	ID_RUN,
	ID_RENDER,
	ID_PLOT,
	ID_HELP,
	ID_DATTOCLIP,
	ID_GET_INFO,
	ID_PLOTEPHASE
};

/**
* @struct CoordDP
* @brief Stores coordinates. Used by the DPExplorer frame.
*/
struct CoordDP
{
public:
	int coordX, coordY;
	CoordDP()
	{
		coordX = 0;
		coordY = 0;
	}
	CoordDP(int x, int y)
	{
		coordX = x;
		coordY = y;
	}
};

/**
* @struct Color
* @brief Stores color. Used by the DPExplorer frame.
*/
class Color
{
public:
	short r, g, b;
	Color(int mR, int mG, int mB)
	{
		r = mR;
		g = mG;
		b = mB;
	}
};

/**
* @enum PLOT_TYPE
* @brief Type of functional relation.
*/
enum PLOT_TYPE
{
	TH1ONTH2,
	TH2ONTH1
};

/**
* @struct FormData
* @brief Data used to plot the formula.
*/
struct FormData
{
	PLOT_TYPE type;
	wxString formText;
};

extern std::vector<FormData> formulas;


#ifdef _WIN32
#define EditFormDialogSize wxSize( 359,147 )
#elif __linux__
#define EditFormDialogSize wxSize( 430,147 )
#endif

/**
* @class EditFormDialog
* @brief A dialog to edit formulas.
*/
class EditFormDialog : public wxDialog
{
	// WX
	wxPanel* mainPanel;
	wxStaticText* funcLabel;
	wxTextCtrl* ctrl;
	wxStaticLine* staticLine;
	wxButton* acceptButton;
	wxButton* cancelButton;
	wxRadioButton* th1on2btn;
	wxRadioButton* th2on1btn;
	FormData* mData;

	void OnAccept(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnTh1(wxCommandEvent& event);
	void OnTh2(wxCommandEvent& event);

public:
	EditFormDialog(wxWindow* parent, FormData* data, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
					const wxPoint& pos = wxDefaultPosition, const wxSize& size = EditFormDialogSize, long style = wxDEFAULT_DIALOG_STYLE);
	~EditFormDialog();
};

/**
* @class PlotDialog
* @brief A dialog to add or delete formulas.
*/
class PlotDialog : public wxDialog
{
	// WX
	wxPanel* mainPanel;
	wxListBox* list;
	wxStaticLine* staticLine;
	wxButton* okButton;
	wxButton* addButton;
	wxButton* editButton;
	wxButton* eraseButton;

	void OnOk(wxCommandEvent& event);
	void OnAdd(wxCommandEvent& event);
	void OnEdit(wxCommandEvent& event);
	void OnErase(wxCommandEvent& event);

	void RedrawList();		///< When a new formula is added, deleted or edited redraws the list.
public:
	PlotDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT(editFormTxt),
					const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 359,247 ), long style = wxDEFAULT_DIALOG_STYLE);
	~PlotDialog();
};

/**
* @class DPImagePanel
* @brief Holds a BMP image and plots.
*/
class DPImagePanel : public wxPanel
{
	wxBitmap bmp;						///< Bitmap to draw the image.
	int coordIndex;						///< Number of points drawn on the bitmap.
	int plotIndex;						///< Number of plots on the bitmap.
	vector<CoordDP> coordVector;
	vector<Color> colorVector;
	vector<wxString> plots;
	vector<PLOT_TYPE> plotType;
	vector<mup::ParserX> parser;
	bool plotError;
	double th1Fact, th2Fact;
	int myRefTh1, myRefTh2;

public:
	DPImagePanel(wxPanel *parent, int id, wxBitmap image);
	void SetBitmap(wxBitmap image);		///< Sets the bitmap image.
	int SetPoint(CoordDP coord, Color color);		///< Sets a point to be drawn on top of the bitmap.
	void RemovePoint(int index = -1);		///< Deletes the point on the given index.
	void OnPaint(wxPaintEvent& event);
	void AddPlot(wxString plot, double th1Factor, double th2Factor, PLOT_TYPE type, int refTh1, int refTh2);	///< Draws a plot on top of the bitmap.
	void RemovePlots();		///< Remove all plots.
};

/**
* @class DPFrame
* @brief The DPExplorer frame.
*/
class DPFrame : public wxFrame
{
	// WX
	wxStatusBar* mStatus;
	wxToolBar* mToolbar;
	wxScrolledWindow* scrollPanel;
	DPImagePanel* image;
	DPSim *sim;

	double th1Factor;
	double th2Factor;
	wxString text;
	double minTh2;
	double maxTh2;
	double minTh1;
	double maxTh1;
	double th1;
	double th2;
	double dt;
	bool th1Bailout;
	bool th2Bailout;
	double th1NumBailout;
	double th2NumBailout;
	bool locked;
	wxString Event;
	double m1, m2, l, g, maxIter;
	bool allOk;
	CoordDP selected;		///< The clicked coordinates.

	void WriteStatusBar(CoordDP pos = CoordDP(-1, -1));
	bool LoadFile(string filePath);		///< Load the bitmap file.

public:
	DPFrame( wxString filePath, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT(dpExpTxt), const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize( 681,445 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	~DPFrame();
	void OnOpen(wxCommandEvent &event);
	void OnRun(wxCommandEvent &event);
	void OnPlot(wxCommandEvent &event);
	void OnHelp(wxCommandEvent &event);
	void OnMoveMouse(wxMouseEvent &event);
	void OnClick(wxMouseEvent& event);
	void OnLeaveTool(wxMouseEvent& event);
	void OnContextMenu(wxMouseEvent& event);
	void OnDataToClipboard(wxCommandEvent &event);
	void OnGetInfo(wxCommandEvent &event);
};

#endif
