/**
* @file JuliaMode.h
* @brief Defines the Julia mode option.
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/19/2012
*/

#pragma once
#include <atomic>
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
    SelectionRect* _selection;
    Options _myJuliaOpt;
    ToggleButton* _play;
    FractalType _type;
    wxWindow* _parent;
    wxSize _size;

    sf::Event _event;
    sf::Thread m_thread; // Thread for the rendering loop
    std::atomic_bool _closeRequested;

    ///@brief Handles the window's events.
    void HandleEvent();

    ///@brief The main loop for the Julia window thread.
    void Run();

public:
    ///@brief Constructor
    ///@param parent Parent wxWidget window.
    ///@param ptr Pointer to the fractalCanvas.
    ///@param fractalType Type of Julia fractal to be created.
    ///@param juliaOpt Options to copy from the parent fractal.
    ///@param size
    JuliaMode(wxWindow* parent, FractalCanvas* ptr, FractalType fractalType, const Options& juliaOpt,
              const wxSize& size = wxSize(640, 480));
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
