/** 
* @file JuliaMode.h 
* @brief Defines the Julia mode option.
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/19/2012
*/

#pragma once
#include <SFML/System.hpp>
#include "FractalCanvas.h"
#include "FractalTypes.h"

extern bool juliaModeState;

/**
* @class JuliaMode
* @brief Creates a thread that handles the execution of a Julia fractal.
*/
class JuliaMode : public sf::Thread
{
	sf::RenderWindow *window;
	FractalCanvas *target;
	FractalHandler juliaFractal;
	SelectRect *selection;
	Options myJuliaOpt;
	ButtonChange *play;
	FRACTAL_TYPE type;
	wxWindow *parent;

	sf::Event event;
	bool resizing;

	///@brief Handles it's own events.
	void Handle_Event();

public:
	///@brief Constructor
	///@param ptr Pointer to the fractalCanvas.
	///@param fractalType Type of Julia fractal to be created.
	///@param juliaOpt Options to copy from the parent fractal.
	///@param _parent Parent wxWidget window.
	JuliaMode( FractalCanvas *ptr, FRACTAL_TYPE fractalType, Options juliaOpt, wxWindow *_parent = NULL );
	~JuliaMode();
	void Close();

	///@brief Start the thread execution.
	virtual void Run();
};