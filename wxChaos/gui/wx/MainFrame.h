/**
* @file MainFrame.h
* @brief This header file contains the main frame of the program.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/18/2012
*/

#pragma once

#include <optional>
#include "FractalCanvas.h"
#include "config/AppConfigStore.h"
#include "JuliaPreviewFrame.h"
#include "About.h"
#include "RendererOptionsFrame.h"
#include "JuliaConstantDialog.h"
#include "IterationsDialog.h"
#include "FormulaDialog.h"
#include "ScriptEditor.h"
#include "ZoomRecorder.h"
#include "DimensionFrame.h"
#include "CommandConsole.h"
#include "SettingsFrame.h"
#include "FractalToolbar.h"
#include "RenderStatusWidget.h"

/**
* @enum IDS
* @brief Enum used to connect events.
*/
enum IDS
{
    ID_SAVE = 1,
    ID_PALETTE,
    ID_MANDELBROT,
    ID_MANDELBROT_ZN,
    ID_JULIA,
    ID_JULIA_ZN,
    ID_NEWTON,
    ID_SINUSOIDAL,
    ID_MAGNET,
    ID_JELLYFISH,
    ID_MANOWAR,
    ID_MANOWAR_JULIA,
    ID_SIERPINSKI_TRIANGLE,
    ID_FIXEDPOINT1,
    ID_FIXEDPOINT2,
    ID_FIXEDPOINT3,
    ID_FIXEDPOINT4,
    ID_TRICORN,
    ID_BURNING_SHIP,
    ID_BURNING_SHIP_JULIA,
    ID_FRACTORY,
    ID_CELL,
    ID_DOUBLE_PENDULUM,
    ID_USER_DEFINED,
    ID_FIXED_POINT_USER_DEFINED,
    ID_NEWTON_USER_DEFINED,
    ID_JULIA_MODE,
    ID_ABOUT,
    ID_KEYBOARD_GUIDE,
    ID_ABORT_RENDER,
    ID_REDRAW,
    ID_RESET,
    ID_INCREASE_IT,
    ID_DECREASE_IT,
    ID_SHOW_ORBIT,
    ID_ENTER_MAN_CONSTANT,
    ID_ENTER_SLD_CONSTANT,
    ID_IT_MANUAL,
    ID_FORMULA_DIALOG,
    ID_OPTION_PANEL,
    ID_USER_MANUAL,
    ID_WELCOME_DIALOG,
    ID_SCRIPT_EDITOR,
    ID_ZOOM_RECORDER,
    ID_DIMENSION_CALCULATOR,
    ID_COMMAND_CONSOLE,
    ID_SETTINGS,
    ID_AUTOMATIC_ITERATIONS,
    ID_VIEW_TOOLBAR
};

/**
* @class MainFrame
* @brief Main wxWidgets frame that coordinates application-level UI.
*
* MainFrame builds the menu bar, status bar, option panel, and auxiliary tool
* windows. It routes menu commands to FractalCanvas, keeps Julia, renderer,
* script, dimension, console, and settings windows synchronized with the active
* fractal, and applies persisted configuration during startup and runtime.
*/
class MainFrame : public wxFrame
{
    FractalCanvas* _fractalCanvas{};
    FormulaDialog* _formulaDialog{};
    JuliaPreviewFrame* _juliaPreviewFrame;
    JuliaConstantDialog* _juliaConstantDialog;
    IterationsDialog* _iterationsDialog{};
    RendererOptionsFrame* _rendererOptions;
    ScriptEditor* _scriptEditor{};
    DimensionFrame* _dimensionCalculator;
    CommandConsole* _commandConsole{};
    SettingsFrame* _settingsFrame{};              ///< Active application settings frame.
    
    bool _changeKeyboardGuide;
    bool _introConstActive;
    bool _iterationsDialogIsActive;
    bool _informationFrameIsActive;
    bool _formulaDialogIsActive;
    bool _showOptionsPanel{};

    // WX
    wxMenuBar* _menubar{};
    wxMenu* _fileMenu{};
    wxMenu* _fractalMenu{};
    wxMenu* _iterationsMenu{};
    wxMenu* _toolMenu{};
    wxMenu* _rendererMenu{};
    wxMenu* _viewMenu{};
    wxMenu* _helpMenu{};
    wxMenu* _formula{};
    wxMenuItem* _juliaMode{};
    wxMenuItem* _keyboardGuide{};
    wxMenuItem* _showOrbit{};
    wxMenuItem* _sliderJuliaConstant{};
    wxMenuItem* _manualJuliaConstant{};
    wxMenuItem* _setIterations{};
    wxMenuItem* _moreIterations{};
    wxMenuItem* _lessIterations{};
    wxMenuItem* _automaticIterations{};
    wxMenuItem* _fractalOptionsItem{};
    wxMenuItem* _toolbarItem{};
    wxMenu* _introConstant{};
    wxMenu *_typeComplex{}, *_typeNumericalMethod{}, *_typePhysics{}, *_typeOther{};
    wxScrolledWindow* _optionPanel{};
    wxStaticBitmap* _fractalOptionsBitmap{};
    wxBoxSizer* _fractalSizer{};
    wxBoxSizer* _optionSizer{};
    wxMenuItem* _abortRenderItem{};
    wxBoxSizer* _sizer{};
    FractalToolbar* _interactionToolbar{};
    wxSize _size;
    wxStatusBar* _statusBar{};
    RenderStatusWidget* _renderStatusWidget{};

    // Menu items from user scripts.
    std::vector<ScriptData> _loadedScripts;
    std::vector<wxMenuItem*> _scriptItems;
    std::optional<unsigned int> _selectedScriptIndex;

    // Elements of the option panel.
    wxButton* _panelButton{};
    std::vector<int> _foundLabels, _foundTextControls;
    std::vector<int> _foundSpinControls, _foundCheckBoxes;
    std::vector<wxStaticText*> _labels;
    std::vector<wxTextCtrl*> _textControls;
    std::vector<wxSpinCtrl*> _spinControls;
    std::vector<wxCheckBox*> _checkBoxes;

    // Configuration.
    FractalType _fractalType{};
    AppConfig _appConfig;

    void SetUpGUI();                      ///< Create the main window.
    void UpdateMenu();                    ///< Adjust menu items when a new fractal type is selected.
    void UpdateOptionsPanel();            ///< Adjust the option panel when a new fractal type is selected.
    void UpdateJuliaMode();               ///< Closes the Julia window when a new fractal is selected.
    void UpdateJuliaRendererOptions(const Options& options) const;
    ///@brief Changes the fractal type.
    ///@param type Type of the fractal.
    ///@param enableJulia Enables a Julia version of this type.
    void ChangeFractal(FractalType type, bool enableJulia);
    void GetParserOpt();                  ///< Gets parameters from the config.ini file.
    void DeleteOptPanel();                ///< Deletes all the elements in the option panel.
    void GetScriptFractals();             ///< Creates the menu elements corresponding to the script fractals.
    void ConnectEvents();
    void CloseAll();
    void DestroyJuliaMode(bool requestClose);
    bool OpenJuliaModeAt(double real, double imaginary);
    void DestroyDimensionFrame();
    void ShowCommandConsole();
    void ShowFirstUseDialog();
    /// @brief Applies saved settings that can safely change during the current session.
    /// @param config Newly saved application configuration.
    void ApplyAppConfig(const AppConfig& config);
    void SetAutomaticIterations(bool mode) const;
    void AddScriptMenuElement(const ScriptData& scriptData, unsigned int index);
    void RemoveScriptMenuElements();
    void CreateInteractionToolbar();
    void CreateStatusBarControls();
    void LayoutStatusBarControls() const;
    void OpenIterationsDialog();
    void OpenFractalInformation();
    void UpdateInformationTool() const;
    void ResetColorRotationTool() const;

public :
    MainFrame();
    /// @brief Replaces the main canvas with the fractal currently shown by a Julia preview.
    /// @param fractalType Fractal type to install in the main canvas.
    /// @param options Renderer and fractal options copied from the preview.
    /// @param colorPalette Color palette copied from the preview.
    /// @param view World-coordinate viewport copied from the preview.
    /// @param automaticIterations True when the preview uses automatic iterations.
    void OpenJuliaPreviewInMainWindow(FractalType fractalType, const Options& options, ColorPaletteTypes colorPalette,
                                      const Rect& view, bool automaticIterations);

    void OnSave(wxCommandEvent& event);                    ///< Saves a picture of the viewing area.
    void OnJuliaMode(wxCommandEvent& event);               ///< Opens a window with the Julia version of the selected fractal.
    void OnPalette(wxCommandEvent& event);                 ///< Opens a ColorFrame.
    void OnWelcomeDialog(wxCommandEvent& event);           ///< Shows the welcome dialog.
    void OnAbout(wxCommandEvent& event);                   ///< Opens the About frame.
    void OnClose(wxCloseEvent& event);                     ///< Closes the frame.
    void OnQuit(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);                 ///< Opens or focuses the settings frame.
    void OnSettingsFrameClosed(wxCommandEvent& event);      ///< Clears the settings frame reference after closure.
    void OnResize(wxSizeEvent& event);                      ///< Resizes the frame.
    void OnJuliaModeClosed(wxCommandEvent& event);          ///< Handles the Julia window closed event.
    void OnRendererOptionsClosed(wxCommandEvent& event);    ///< Handles the renderer options window closed event.
    void OnScriptEditorClosed(wxCommandEvent& event);       ///< Handles the script editor window closed event.
    void OnDimensionFrameClosed(wxCommandEvent& event);     ///< Handles the dimension window closed event.
    void OnCommandConsoleClosed(wxCommandEvent& event);     ///< Handles the command console closed event.
    void ChangeMandelbrot(wxCommandEvent& event);
    void ChangeMandelbrotZN(wxCommandEvent& event);
    void ChangeJulia(wxCommandEvent& event);
    void ChangeJuliaZN(wxCommandEvent& event);
    void ChangeNewton(wxCommandEvent& event);
    void ChangeSinusoidal(wxCommandEvent& event);
    void ChangeMagnet(wxCommandEvent& event);
    void ChangeJellyfish(wxCommandEvent& event);
    void ChangeManowar(wxCommandEvent& event);
    void ChangeManowarJulia(wxCommandEvent& event);
    void ChangeSierpinskiTriangle(wxCommandEvent& event);
    void ChangeFixedPoint1(wxCommandEvent& event);
    void ChangeFixedPoint2(wxCommandEvent& event);
    void ChangeFixedPoint3(wxCommandEvent& event);
    void ChangeFixedPoint4(wxCommandEvent& event);
    void ChangeTricorn(wxCommandEvent& event);
    void ChangeBurningShip(wxCommandEvent& event);
    void ChangeBurningShipJulia(wxCommandEvent& event);
    void ChangeFractory(wxCommandEvent& event);
    void ChangeCell(wxCommandEvent& event);
    void ChangeDPendulum(wxCommandEvent& event);
    void ChangeUserDefined(wxCommandEvent& event);
    void ChangeFPUserDefined(wxCommandEvent& event);
    void ChangeNewtonUserDefined(wxCommandEvent& event);
    void ChangeScriptItem(wxCommandEvent& event);
    void OnKeyboardGuide(wxCommandEvent& event);
    void OnCanvasStatusText(wxCommandEvent& event);
    void OnAbortRender(wxCommandEvent& event);
    void OnUpdateAbortRender(wxUpdateUIEvent& event);
    void OnUpdateShowOrbit(wxUpdateUIEvent& event);
    void OnUpdateSliderMode(wxUpdateUIEvent& event);
    void OnUpdateManualIterations(wxUpdateUIEvent& event);
    void OnUpdateAutomaticIterations(wxUpdateUIEvent& event);
    void OnRedraw(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnMoreIt(wxCommandEvent& event);
    void OnLessIt(wxCommandEvent& event);
    void OnShowOrbit(wxCommandEvent& event);
    void OnManIntroConst(wxCommandEvent& event);
    void OnSldIntroConst(wxCommandEvent& event);
    void OnSetIterations(wxCommandEvent& event);
    void OnAutomaticIterations(wxCommandEvent& event);
    void OnFormulaDialog(wxCommandEvent& event);
    void OnToolbarVisibility(wxCommandEvent& event);        ///< Shows or hides the interaction toolbar.
    void OnFractalOptions(wxCommandEvent& event);
    void OnApplyPanelOpt(wxCommandEvent& event);
    void OnUserManual(wxCommandEvent& event);
    void OnScriptEditor(wxCommandEvent& event);
    void OnZoomRecorder(wxCommandEvent& event);
    void OnDimensionCalculator(wxCommandEvent& event);
    void OnCommandConsole(wxCommandEvent& event);

    void ReloadScripts();  ///< Search again for script fractals.
};
