/**
* @file JuliaPreviewWindow.h
* @brief Defines the Julia mode option.
*
* @author Carlos Manuel Rodriguez Martinez
*
* @date 7/19/2012
*/

#pragma once
#include <atomic>
#include <mutex>
#include <SFML/System.hpp>
#include "FractalCanvas.h"
#include "FractalTypes.h"

wxDECLARE_EVENT(wxEVT_JULIA_MODE_CLOSED, wxCommandEvent);

/**
* @class JuliaPreviewWindow
* @brief Owns the separate SFML window that previews the Julia variant.
*
* JuliaPreviewWindow runs its own render loop thread, keeps a Julia fractal synchronized
* with selected renderer options and constants from the main canvas, and emits a
* close event back to the wxWidgets parent.
*/
class JuliaPreviewWindow
{
    sf::RenderWindow* _window;
    FractalCanvas* _target;
    FractalPresenter* _fractalPresenter;
    SelectionRect* _selection;
    ToggleButton* _play;
    wxWindow* _parent;

    FractalFactory _juliaFractal;
    Options _myJuliaOpt;
    FractalType _type;
    wxSize _size;

    sf::Event _event;
    sf::Clock _movementClock;
    sf::Thread m_thread; // Thread for the rendering loop
    std::atomic_bool _closeRequested;
    std::mutex _rendererOptionsMutex;
    Options _pendingRendererOptions;
    bool _rendererOptionsPending;
    double _pendingKReal;
    double _pendingKImaginary;
    bool _constantPending;
    bool _mouseWheelPanning;
    sf::Vector2i _lastMouseWheelPanPosition;

    ///@brief Handles the window's events.
    void HandleEvent();

    ///@brief Zooms into the preview around a pixel position.
    void ZoomAtMousePosition(const sf::Vector2i& position) const;

    ///@brief The main loop for the Julia window thread.
    void Run();

    ///@brief Applies renderer settings without replacing Julia-specific state.
    void ApplyRendererOptions(const Options& options) const;

public:
    ///@brief Constructor
    ///@param parent Parent wxWidget window.
    ///@param ptr Pointer to the fractalCanvas.
    ///@param fractalType Type of Julia fractal to be created.
    ///@param juliaOpt Options to copy from the parent fractal.
    ///@param size
    JuliaPreviewWindow(wxWindow* parent, FractalCanvas* ptr, FractalType fractalType, const Options& juliaOpt,
                       const wxSize& size = wxSize(640, 480));
    ~JuliaPreviewWindow();

    ///@brief Launches the thread.
    void Launch();

    ///@brief Waits for the thread to finish.
    void Wait();

    ///@brief Signals the window to close.
    void Close();

    ///@brief Queues renderer settings to be applied by the Julia window thread.
    void SetRendererOptions(const Options& options);

    ///@brief Queues a Julia constant to be applied by the Julia window thread.
    void SetConstant(double real, double imaginary);
};
