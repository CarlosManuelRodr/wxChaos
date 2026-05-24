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
    sf::RenderWindow* _window;
    FractalCanvas* _target;
    FractalHandler _juliaFractal;
    SFMLFractal _sfmlFractal;
    SelectRect* _selection;
    Options _myJuliaOpt;
    ButtonChange* _play;
    FractalType _type;
    wxWindow* _parent;

    sf::Event _event;
    sf::Thread m_thread; // Thread for the rendering loop

    ///@brief Handles the window's events.
    void HandleEvent();

    ///@brief The main loop for the Julia window thread.
    void Run();

public:
    ///@brief Constructor
    ///@param ptr Pointer to the fractalCanvas.
    ///@param fractalType Type of Julia fractal to be created.
    ///@param juliaOpt Options to copy from the parent fractal.
    ///@param _parent Parent wxWidget window.
    JuliaMode(FractalCanvas* ptr, FractalType fractalType, const Options& juliaOpt, wxWindow* parent = nullptr);
    ~JuliaMode();

    ///@brief Launches the thread.
    void Launch();

    ///@brief Waits for the thread to finish.
    void Wait();

    ///@brief Forcibly terminates the thread.
    void Terminate();

    ///@brief Signals the window to close.
    void Close() const;
};
