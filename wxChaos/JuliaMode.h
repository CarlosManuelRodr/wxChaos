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
    sf::RenderWindow* window;
    FractalCanvas* target;
    FractalHandler juliaFractal;
    SelectRect* selection;
    Options myJuliaOpt;
    ButtonChange* play;
    FractalType type;
    wxWindow* parent;

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
    JuliaMode(FractalCanvas* ptr, FractalType fractalType, Options juliaOpt, wxWindow* _parent = nullptr);
    ~JuliaMode();
    void Close();

    ///@brief Start the thread execution.
    virtual void Run();
};