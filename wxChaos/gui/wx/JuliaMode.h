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
* @brief Creates and manages a window for displaying a Julia fractal variant.
*/
class JuliaMode
{
private:
    sf::RenderWindow* window;
    FractalCanvas* target;
    FractalHandler juliaFractal;
    SelectRect* selection;
    Options myJuliaOpt;
    ButtonChange* play;
    FractalType type;
    wxWindow* parent;

    sf::Event event;

    sf::Thread m_thread; // Thread for the rendering loop

    ///@brief Handles the window's events.
    void Handle_Event();

    ///@brief The main loop for the Julia window thread.
    void Run();

public:
    ///@brief Constructor
    ///@param ptr Pointer to the fractalCanvas.
    ///@param fractalType Type of Julia fractal to be created.
    ///@param juliaOpt Options to copy from the parent fractal.
    ///@param _parent Parent wxWidget window.
    JuliaMode(FractalCanvas* ptr, FractalType fractalType, Options juliaOpt, wxWindow* _parent = nullptr);
    ~JuliaMode();

    ///@brief Launches the thread.
    void Launch();

    ///@brief Waits for the thread to finish.
    void Wait();

    ///@brief Forcibly terminates the thread.
    void Terminate();

    ///@brief Signals the window to close.
    void Close();
};