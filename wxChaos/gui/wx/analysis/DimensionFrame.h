/**
* @file DimensionFrame.h
* @brief Defines a frame to calculate the fractal dimension.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 9/22/2012
*/

#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <SFML/System.hpp>

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/notebook.h>
#include <wx/bmpbuttn.h>
#include <wx/bmpbndl.h>
#include <wx/spinctrl.h>
#include <wx/timer.h>

#include "analysis/BoxCountWorker.h"
#include "analysis/BoxCountMap.h"
#include "analysis/ImagePanel.h"
#include "analysis/PlotWindow.h"
#include "common/FractalOptionsPanel.h"
#include "FractalFactory.h"
#include "FormulaOptions.h"
#include "Options.h"
#include "ScriptData.h"

/** @brief Event posted to the parent when the dimension-calculator window closes. */
wxDECLARE_EVENT(wxEVT_DIMENSION_FRAME_CLOSED, wxCommandEvent);

#define DimensionFrameSize wxSize(1300, 960)

/**
* @class DimensionFrame
* @brief Tool window for estimating fractal dimension by box counting.
*
* DimensionFrame renders a square preview of the selected fractal, counts boxes
* across the requested grid divisions, logs the samples, and can plot either the
* collected values or a fitted line.
*/
class DimensionFrame : public wxFrame
{
    /** @brief Associates one built-in fractal with known-good box-counting inputs. */
    struct BuiltInDimensionPreset
    {
        FractalType fractalType{};            ///< Fractal to which this preset applies.
        DimensionCalculatorPreset preset;     ///< Values copied into the calculator controls.
    };

    wxScrolledWindow* _mainPanel;
    wxChoice* _fractalChoice{};
    wxSpinCtrlDouble* _minXCtrl{};
    wxSpinCtrlDouble* _maxXCtrl{};
    wxSpinCtrlDouble* _minYCtrl{};
    wxSpinCtrlDouble* _maxYCtrl{};
    wxSpinCtrl* _iterCtrl{};
    wxSpinCtrl* _sizeCtrl{};
    wxButton* _fractalOptionsButton{};
    wxButton* _formulaButton{};
    wxStaticText* _nDivTxt{};
    wxSpinCtrl* _numberOfDivisionsSpinCtrl{};
    wxStaticText* _divTxt{};
    wxNotebook* _divNotebook{};
    wxPanel* _byFunctionPanel{};
    wxStaticText* _funcTxt{};
    wxStaticText* _fDeclTxt{};
    wxTextCtrl* _funcCtrl{};
    wxStaticText* _goesFromTxt{};
    wxSpinCtrl* _xMinSpin{};
    wxStaticText* _goesToTxt{};
    wxSpinCtrl* _xMaxSpin{};
    wxPanel* _byListPanel{};
    wxTextCtrl* _listCtrl{};
    wxButton* _calcButton{};
    wxButton* _closeButton{};
    wxButton* _clearButton{};
    wxStaticLine* _outLine{};
    wxRichTextCtrl* _logCtrl{};
    wxGauge* _progressBar{};
    wxStaticText* _progressTxt{};
    wxButton* _savePreviewButton{};
    wxCheckBox* _dataCheck{};
    wxCheckBox* _dataFitCheck{};
    wxStaticText* _resolutionWarning{};
    wxBitmapButton* _helpButton{};

    wxDialog* _fractalOptionsDialog{};                      ///< Dialog that hosts the selected fractal's options.
    FractalOptionsPanel* _fractalOptionsPanel{};            ///< Reusable panel bound to the selected fractal.
    Fractal* _target;                                       ///< Fractal currently rendered and measured by the calculator.
    FractalFactory _fractalFactory;                         ///< Owns and creates the currently selected fractal.
    ImagePanel* _previewImage{};                              ///< Displays occupied pixels and the selected box grid.
    Options _myOpt;                                         ///< Last options read from the dimension controls.
    FormulaOptions _userFormula;                            ///< User-defined escape-time formula used by this tool.
    int _threadNumber;                                      ///< Number of parallel box-counting workers.
    std::vector<BoxCountWorker> _dimensionCalculator;       ///< One box-counting worker per thread.
    BoxCountMap _boxCountMap;                               ///< Unified occupancy map for raster and vector fractals.
    std::vector<std::unique_ptr<sf::Thread>> _dimThreads;   ///< Threads executing the box-counting workers.
    std::vector<int> _div;                                  ///< Grid-division counts requested for the current calculation.
    std::vector<double> _epsilon;                           ///< Pixel widths of the sampled boxes.
    std::vector<int> _boxCount;                             ///< Occupied-box count for each sampled epsilon.
    std::vector<ScriptData> _loadedScripts;                 ///< Discovered user-script metadata.
    std::vector<FractalType> _builtInFractalList;           ///< Built-in types in the same order as their choice entries.
    std::vector<unsigned int> _scriptList;                  ///< Indices of scripts eligible for dimension calculation.
    int _divIndex{};                                        ///< Index of the grid division currently being counted.
    bool _scriptSelected;                                   ///< true when the active choice represents a user script.
    bool _firstRender;                                      ///< true until the full-size target has rendered once.

    int _previewSize;                                       ///< Fixed width and height of the interactive preview.
    int _size{};                                            ///< Width and height used for the full dimension calculation.

    bool _renderingPreview;                                 ///< true while the asynchronous preview render is active.
    bool _calculatingDimension;                             ///< true from Calculate until counting and fitting finish.
    bool _suppressPreviewUpdate{};                          ///< Prevents programmatic control changes from scheduling renders.
    bool _previewRenderQueued{};                            ///< Requests a fresh preview after cancellation completes.
    bool _hasPreviewMap{};                                  ///< true when _boxCountMap contains the current preview.
    int _progress{};                                        ///< Last percentage displayed by the progress controls.
    sf::Clock _clock;                                       ///< Limits polling and UI updates to a reasonable frequency.
    wxTimer _previewTimer;                                  ///< Debounces rapid preview-parameter changes.

    /** @brief Recreates the target when the fractal selector changes. */
    void OnChangeFractal(wxCommandEvent&);

    /** @brief Starts or stops the complete render-and-box-count calculation. */
    void OnCalculate(wxCommandEvent&);

    /** @brief Polls render and worker progress and advances the calculation state machine. */
    void OnUpdateUI(wxUpdateUIEvent&);

    /** @brief Starts a preview after the debounce timer expires. */
    void OnPreviewTimer(wxTimerEvent&);

    /** @brief Schedules a preview after an integer render parameter changes. */
    void OnPreviewParameterChanged(wxCommandEvent&);

    /** @brief Schedules a preview after a viewport coordinate changes. */
    void OnPreviewDoubleParameterChanged(wxSpinDoubleEvent&);

    /** @brief Repaints the existing preview with a different box grid. */
    void OnPreviewGridChanged(wxCommandEvent&);

    /** @brief Presents the current occupancy map and paints it immediately. */
    void DisplayPreviewMap() const;

    /** @brief Refreshes the image-resolution warning after a division input changes. */
    void OnDivisionDefinitionChanged(wxCommandEvent&);

    /** @brief Refreshes the image-resolution warning after switching division modes. */
    void OnDivisionModeChanged(wxBookCtrlEvent& event);

    /** @brief Closes the dimension-calculator window. */
    void OnClose(wxCommandEvent&);

    /** @brief Clears the dimension-calculation log. */
    void OnClear(wxCommandEvent&);

    /** @brief Cancels outstanding work and destroys the frame safely. */
    void OnDestroy(wxCloseEvent&);

    /** @brief Opens the options dialog for the currently selected fractal. */
    void OnFractalOptions(wxCommandEvent&);

    /** @brief Opens the formula editor for the user-defined escape-time fractal. */
    void OnFormula(wxCommandEvent& event);

    /** @brief Renders and saves the preview with occupied-box highlighting. */
    void OnSavePreview(wxCommandEvent&);

    /** @brief Opens the box-counting dimension documentation. */
    void OnHelp(wxCommandEvent&);

    /** @brief Appends one built-in fractal and records the type represented by its choice index. */
    void AddBuiltInFractalChoice(const wxString& label, FractalType type);

    /** @brief Rebuilds the selector from supported built-in fractals and user scripts. */
    void PopulateFractalChoices();

    /** @brief Selects the built-in fractal presented when the calculator opens. */
    void SelectDefaultFractal();

    /** @brief Returns the configured calculator preset for a fractal, when one exists. */
    [[nodiscard]] static const DimensionCalculatorPreset* FindDimensionPreset(FractalType fractalType);

    /** @brief Applies the selected fractal's known-good calculator values to the controls. */
    void ApplySelectedFractalPreset();

    /** @brief Copies one built-in or scripted preset into every related calculator control. */
    void ApplyDimensionPreset(const DimensionCalculatorPreset& preset);

    /** @brief Returns true when the selector points to the editable user formula. */
    [[nodiscard]] bool IsUserDefinedEscapeTimeSelected() const;

    /** @brief Shows the formula button only for the user-defined escape-time choice. */
    void UpdateFormulaButtonVisibility() const;

    /**
     * @brief Returns the upper division count implied by the active function or list.
     * @return Positive division count, or no value when the active input cannot be evaluated.
     */
    [[nodiscard]] std::optional<int> GetUpperDivisionCount() const;

    /** @brief Shows or hides the warning that the finest boxes approach pixel resolution. */
    void UpdateResolutionWarning();

    /** @brief Creates the left column containing parameters and action buttons. */
    [[nodiscard]] wxSizer* CreateParameterColumn();

    /** @brief Creates the right column containing the preview, log, and progress controls. */
    [[nodiscard]] wxSizer* CreateOutputColumn();

    /** @brief Connects control and frame events after the complete layout exists. */
    void BindEvents();

    /** @brief Disconnects control and frame events before destruction. */
    void UnbindEvents();

    /**
     * @brief Creates a themed heading row used by sections of the frame.
     * @return Newly allocated panel owned by the supplied parent.
     */
    static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                        const wxString& darkIcon);

    /**
     * @brief Loads the light- or dark-theme SVG icon at the requested size.
     * @return Bitmap bundle suitable for a wxWidgets control.
     */
    [[nodiscard]] static wxBitmapBundle CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon, const wxSize& size);

    /**
     * @brief Creates a vertically labelled row for one fractal parameter control.
     * @return Sizer owned by the caller's containing layout.
     */
    wxSizer* CreateFractalParameterRow(const wxString& label, wxWindow* control) const;

    /** @brief Creates a consistently configured floating-point coordinate control. */
    [[nodiscard]] wxSpinCtrlDouble* CreateCoordinateSpin(const wxString& value) const;

    /** @brief Creates the selected built-in or script fractal at a square resolution. */
    void CreateFractal(int size);

    /** @brief Reads viewport, iteration, and image-size controls into an Options value. */
    [[nodiscard]] Options ReadDimensionOptions();

    /** @brief Derives the maximum Y coordinate so the measured viewport remains square. */
    void UpdateDerivedMaxY() const;

    /** @brief Copies fractal options into the editable viewport and iteration controls. */
    void SetControlsFromOptions(const Options& options);

    /** @brief Debounces or queues a preview render after an option changes. */
    void SchedulePreviewRender();

    /** @brief Configures and launches the asynchronous square preview render. */
    void StartPreviewRender();

    /** @brief Stops the active preview render and restores the preview controls. */
    void StopPreviewRender() const;

    /** @brief Repaints only the box grid using the already-built occupancy map. */
    void RefreshPreviewOverlayOnly();

    /** @brief Rebuilds the unified occupancy map from the completed target render. */
    void UpdateBoxCountMap();

    /** @brief Assigns disjoint horizontal slices of the occupancy map to worker objects. */
    void ConfigureDimensionWorkers();

    /** @brief Waits for all dimension worker threads and releases their resources. */
    void JoinDimensionThreads();

    /** @brief Requests all dimension workers to stop, then joins and releases their threads. */
    void StopDimensionThreads();

    /** @brief Discovers eligible script fractals and appends them to the selector. */
    void GetScriptFractals();

    /** @brief Appends text to the dimension-calculation log. */
    void WriteText(const wxString& txt) const;

public:
    /**
     * @brief Creates the interactive box-counting dimension calculator.
     * @param parent Parent wxWidgets window.
     * @param id Window identifier.
     * @param title Localizable frame title.
     * @param pos Initial window position.
     * @param size Initial window size.
     * @param style wxWidgets frame style flags.
     */
    explicit DimensionFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxTRANSLATE("Calculate Dimension"),
                            const wxPoint& pos = wxDefaultPosition, const wxSize& size = DimensionFrameSize,
                            long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

    /** @brief Stops outstanding renders and workers before releasing the frame. */
    ~DimensionFrame() override;
};
