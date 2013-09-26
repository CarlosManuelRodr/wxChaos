/**
* @file ColorFrame.h
* @brief This header contains the ColorFrame.
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/19/2012
*/

#pragma once
#ifndef _color
#define _color

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/notebook.h>
#include <SFML/Graphics/Color.hpp>
#include "FractalClasses.h"
#include "Styles.h"
#include "gradientdlg.h"

#ifdef _WIN32
#define ColorFrameSize wxSize( 672,555 )
#elif __linux__
#define ColorFrameSize wxSize( 690,600 )
#endif

/**
* @class ColorFrame
* @brief Frame that allows the user to change several color and algorithm parameters.
*/
class ColorFrame : public wxFrame
{
	wxScrolledWindow* mPanel;
	wxStaticBitmap* colorOptBitmap;
	wxStaticText* algorithmText;
	wxChoice* algorithmChoice;
	wxStaticText* optionsText;
	wxCheckBox* relativeCheck;
	wxCheckBox* colorFractal;
	wxCheckBox* colorSet;
	wxCheckBox* orbitTrap;
	wxCheckBox* smoothRender;
	wxStaticText* redSetText;
	wxSlider* redSetSld;
	wxStaticText* greenSetText;
	wxSlider* greenSetSld;
	wxStaticText* blueSetText;
	wxSlider* blueSetSld;
	wxButton* okButton;
	wxNotebook* typeNotebook;
	wxPanel* gradientLabel;
	wxStaticText* gradStylesLabel;
	wxChoice* gradStylesChoice;
	wxButton* gradButton;
	wxPanel* stdPanel;
	wxStaticText* estilosLabel;
	wxChoice* ESTStylesChoice;
	wxStaticText* pSizeText;
	wxSpinCtrl* pSizeSpin;
	wxStaticText* redIntText;
	wxSlider* redIntSld;
	wxStaticText* redPosText;
	wxSlider* redPosSld;
	wxStaticText* redDesText;
	wxSlider* redDesSld;
	wxStaticText* greenIntText;
	wxSlider* greenIntSld;
	wxStaticText* greenPosText;
	wxSlider* greenPosSld;
	wxStaticText* greenDesText;
	wxSlider* greenDesSld;
	wxStaticText* blueIntText;
	wxSlider* blueIntSld;
	wxStaticText* bluePosText;
	wxSlider* bluePosSld;
	wxStaticText* blueDesText;
	wxSlider* blueDesSld;
	wxStaticBitmap* gradientMap;
	wxStaticText* gradPalText;
	wxSpinCtrl* gradPalSize;
	wxStaticText* colorVarText;
	wxSlider* colorVarSlider;

	Fractal *target;						///< Target fractal.
	bool *active;							///< Used to communicate with the MainFrame.
	sf::Color setColor;						///< Color of the fractal set.
	ESTFractalColor estFractalColor;		///< Color in EST color mode.
	GradFractalColor gradFractalColor;		///< Color in Grad color mode.
	int escapeTimeIndex, gaussIntIndex, buddhabrotIndex;
	int escapeAngleIndex, triangleIneqIndex, chaoticMapIndex;
	int lyapunovIndex, convergenceTestIndex;

	void OnClose( wxCloseEvent& event );
	void OnChangeAlgorithm( wxCommandEvent& event );
	void OnRelativeColor( wxCommandEvent& event );
	void OnPageChange( wxNotebookEvent& event );
	void OnColorFractal( wxCommandEvent& event );
	void OnColorSet( wxCommandEvent& event );
	void OnOrbitTrap( wxCommandEvent& event );
	void OnSmoothRender( wxCommandEvent& event );
	void OnSetRed( wxScrollEvent& event );
	void OnSetGreen( wxScrollEvent& event );
	void OnSetBlue( wxScrollEvent& event );
	void OnOk( wxCommandEvent& event );
	void OnGrad( wxCommandEvent& event );
	void ESTChangeSelection( wxCommandEvent& event );
	void OnPaletteSize( wxSpinEvent& event );
	void ChangeIntRed( wxScrollEvent& event );
	void ChangePosRed( wxScrollEvent& event );
	void ChangeDesRed( wxScrollEvent& event );
	void ChangeIntGreen( wxScrollEvent& event );
	void ChangePosGreen( wxScrollEvent& event );
	void ChangeDesGreen( wxScrollEvent& event );
	void ChangeIntBlue( wxScrollEvent& event );
	void ChangePosBlue( wxScrollEvent& event );
	void ChangeDesBlue( wxScrollEvent& event );
	void OnGradPaletteSize( wxSpinEvent& event );
	void GradChangeSelection( wxCommandEvent& event );
	void OnColorVar( wxScrollEvent& event );

	void ConnectEvents();
	void SetAlgorithmChoices();		///< Search for the algorithms available in the target fractal and constructs choice widget.
	wxBitmap PaintGradient();		///< Paints the gradient widget.


public:
	///@brief Constructor.
	///@param _active Used to communicate with the MainFrame.
	///@param _target Pointer to the target fractal.
	///@param parent Parent wxWindow.
	ColorFrame(bool *_active, Fractal *_target, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT( "Color options" ),
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = ColorFrameSize, long windowStyle = wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL|wxRESIZE_BORDER );
	~ColorFrame();

	///@brief Sets the target fractal.
	///@param _target Pointer to target fractal.
	void SetTarget( Fractal*_target );

};

#endif //_color
