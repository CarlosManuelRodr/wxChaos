/**
* @file MainWindow.h
* @brief This header file contains the main frame of the program.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/18/2012
*/

#pragma once
#ifndef _mainWindow
#define _mainWindow

#include "JuliaMode.h"
#include "About.h"
#include "ColorFrame.h"
#include "ConstDialog.h"
#include "SizeDialogSave.h"
#include "IterDialog.h"
#include "ConfigParser.h"
#include "FormulaDialog.h"
#include "StringFuncs.h"

#ifdef _WIN32
#include <WTypes.h>
#endif
#ifdef linux
#include<unistd.h>
#include<X11/Xlib.h>
#include<X11/extensions/Xrandr.h>
#endif

/**
* @enum IDS
* @brief Enum used to connect events.
*/
enum IDS
{
    ID_SAVE = 1,
    ID_PALETA,
    ID_MANDELBROT,
    ID_MANDELBROT_ZN,
    ID_JULIA,
    ID_JULIA_ZN,
    ID_NEWTON,
    ID_SINOIDAL,
    ID_MAGNET,
    ID_MEDUSA,
    ID_MANOWAR,
    ID_MANOWAR_JULIA,
    ID_SIERP_TRIANGLE,
    ID_FIXEDPOINT1,
    ID_FIXEDPOINT2,
    ID_FIXEDPOINT3,
    ID_FIXEDPOINT4,
    ID_TRICORN,
    ID_BURNING_SHIP,
    ID_BURNING_SHIP_JULIA,
    ID_FRACTORY,
    ID_CELL,
    ID_LOGISTIC,
    ID_HENON_MAP,
    ID_DPENDULUM,
    ID_USER_DEFINED,
    ID_FPUSER_DEFINED,
    ID_JULIA_MODE,
    ID_ABOUT,
    ID_KEYBGUIDE,
    ID_PAUSE_CONTINUE,
    ID_REDRAW,
    ID_RESET,
    ID_INCREASE_IT,
    ID_DECREASE_IT,
    ID_SHOW_ORBIT,
    ID_ENTER_MAN_CONSTANT,
    ID_ENTER_SLD_CONSTANT,
    ID_IT_MANUAL,
    ID_FORMULA_DIALOG,
    ID_OPTPANEL,
    ID_OPEN_SCRIPT_FOLDER,
    ID_USER_MANUAL,
    ID_WELCOME_DIALOG
};

/**
* @struct ConfigParserOpt
* @brief Struct used to hold information parsed from the "config.ini" file.
*/

struct ConfigParserOpt
{
    ColorMode mode;
    FractalType type;
    int maxIterations;
    int paletteSize;
    string colorStyleGrad;
    GaussianColorStyles colorStyleGaussian;
    bool constantWindow, commandConsole, juliaMode;
    bool colorPaletteWindow, colorFractal, colorSet;
    bool firstUse;
};


/**
* @class MainFrame
* @brief Main frame of the program.
*
* Contains all the wxWidgets menu items. The fractal is rendered in the fractalCanvas object,
* which is a binding of wxWidgets and SFML.
*/
class MainFrame : public wxFrame
{
    FractalCanvas *fractalCanvas;
    JuliaMode *ptr;
    ConstDialog *diag;
    IterDialog *iterDiag;
    ColorFrame *pal;
    FormulaDialog *formDialog;
    bool changeJuliaMode;
    bool changeKeybGuide;
    MainWindowStatus statusData;

    // WX
    wxMenuBar *menubar;
    wxMenu *fileMenu;
    wxMenu *fractalMenu;
    wxMenu *iterationsMenu;
    wxMenu *toolMenu;
    wxMenu *colorMenu;
    wxMenu *helpMenu;
    wxMenu* formula;
    wxMenuItem* juliaMode;
    wxMenuItem* keyboardGuide;
    wxMenuItem* showOrbit;
    wxMenuItem* slider;
    wxMenuItem* manual;
    wxMenuItem* itManual;
    wxMenuItem* MoreIter;
    wxMenuItem* lessIter;
    wxMenuItem* fractOptItem;
    wxMenu* introConstant;
    wxMenu *typeComplex, *typeNumMet, *typePhysics, *typeOther;
    wxScrolledWindow* optionPanel;
    wxStaticBitmap* propBitmap;
    wxBoxSizer* fractalSizer;
    wxBoxSizer* optionSizer;
    PauseContinueButton pauseBtn;

    // Menu items from user scripts.
    vector<wxMenuItem*> scriptItems;
    int selectedScriptIndex;

    // Elements of the option panel.
    wxButton* panelButton;
    vector<int> foundLabels, foundTextControls;
    vector<int> foundSpinControls, foundCheckBoxes;
    vector<wxStaticText*> labels;
    vector<wxTextCtrl*> textControls;
    vector<wxSpinCtrl*> spinControls;
    vector<wxCheckBox*> checkBoxes;

    wxBoxSizer *sizer;
    wxSize size;
    wxStatusBar* status;
    bool colorFrameActive;
    bool introConstActive;
    bool iterDiagActive;
    bool infoFrameActive;
    bool formDiagActive;
    bool pause;
    bool showOptPanel;
    GaussianColorStyles colorStyle;
    FractalType fractalType;
    ConfigParserOpt opt;

    void SetUpGUI();                      ///< Create the main window.
    void UpdateMenu();                    ///< Adjust menu items when a new fractal type is selected.
    void UpdateOptPanel();                ///< Adjust the option panel when a new fractal type is selected.
    void UpdateJuliaMode();               ///< Closes the Julia window when a new fractal is selected.
    ///@brief Changes the fractal type.
    ///@param fType Type of the fractal.
    ///@param enableJulia Enables a Julia version of this type.
    void ChangeFractal(FractalType fType, bool enableJulia);
    void GetParserOpt();                  ///< Gets parameters from the config.ini file.
    void DeleteOptPanel();                ///< Deletes all the elements in the option panel.
    void GetScriptFractals();             ///< Creates the menu elements corresponding to the script fractals.
    void ConnectEvents();
    void CloseAll();
    void ShowFirstUseDialog();

public :
    MainFrame();
    void OnSave(wxCommandEvent &event);                    ///< Saves a picture of the viewing area.
    void OnJuliaMode(wxCommandEvent &event);               ///< Opens a windows with the Julia version of the selected fractal.
    void OnPalette(wxCommandEvent &event);                 ///< Opens a ColorFrame.
    void OnWelcomeDialog(wxCommandEvent& event);           ///< Shows the welcome dialog.
    void OnAbout(wxCommandEvent &event);                   ///< Opens the About frame.
    void OnClose(wxCloseEvent& event);                     ///< Closes the frame.
    void OnQuit(wxCommandEvent &event);
    void OnResize(wxSizeEvent &event);                     ///< Resizes the frame.
    void JuliaHandle(wxUpdateUIEvent &event);              ///< Keeps track of the Julia window.
    void ChangeMandelbrot(wxCommandEvent &event);
    void ChangeMandelbrotZN(wxCommandEvent &event);
    void ChangeJulia(wxCommandEvent &event);
    void ChangeJuliaZN(wxCommandEvent &event);
    void ChangeNewton(wxCommandEvent &event);
    void ChangeSinoidal(wxCommandEvent &event);
    void ChangeMagnet(wxCommandEvent &event);
    void ChangeMedusa(wxCommandEvent &event);
    void ChangeManowar(wxCommandEvent &event);
    void ChangeManowarJulia(wxCommandEvent &event);
    void ChangeSierpTriangle(wxCommandEvent &event);
    void ChangeFixedPoint1(wxCommandEvent &event);
    void ChangeFixedPoint2(wxCommandEvent &event);
    void ChangeFixedPoint3(wxCommandEvent &event);
    void ChangeFixedPoint4(wxCommandEvent &event);
    void ChangeTricorn(wxCommandEvent &event);
    void ChangeBurningShip(wxCommandEvent &event);
    void ChangeBurningShipJulia(wxCommandEvent &event);
    void ChangeFractory(wxCommandEvent &event);
    void ChangeCell(wxCommandEvent &event);
    void ChangeLogistic(wxCommandEvent &event);
    void ChangeHenonMap(wxCommandEvent &event);
    void ChangeDPendulum(wxCommandEvent &event);
    void ChangeUserDefined(wxCommandEvent &event);
    void ChangeFPUserDefined(wxCommandEvent &event);
    void ChangeScriptItem(wxCommandEvent &event);
    void OnKeybGuide(wxCommandEvent &event);
    void OnPauseContinue(wxCommandEvent &event);
    void OnRedraw(wxCommandEvent &event);
    void OnReset(wxCommandEvent &event);
    void OnMoreIt(wxCommandEvent &event);
    void OnLessIt(wxCommandEvent &event);
    void OnShowOrbit(wxCommandEvent &event);
    void OnManIntroConst(wxCommandEvent &event);
    void OnSldIntroConst(wxCommandEvent &event);
    void OnItManual(wxCommandEvent &event);
    void OnFormulaDialog(wxCommandEvent &event);
    void OnFractalOptions(wxCommandEvent &event);
    void OnApplyPanelOpt(wxCommandEvent& event);
    void OnOpenScriptFolder(wxCommandEvent& event);
    void OnUserManual(wxCommandEvent &event);

    void ReloadScripts();  ///< Search again for script fractals.
};

extern MainFrame* mainFramePtr;  ///< Pointer to the MainFrame. Used by the command console.

#endif