/**
* @file SizeDialogSave.h
* @brief A dialog to select the image size and show the rendering progress.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/19/2012
*/

#pragma once
#ifndef __sizeDiag
#define __sizeDiag
#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <SFML/System.hpp>
#include "FractalClasses.h"
#include "FractalCanvas.h"
#include "global.h"

/**
* @class SaveProgressDiag
* @brief Creates a dialog that shows the user the amount of rendering progress and allows the user to pause it and save progress.
*/
class SaveProgressDiag : public wxDialog
{
private:
    wxGauge* progress;
    wxStaticText* progressLabel;
    wxStaticLine* staticLine;
    wxButton* cancelButton;
    Fractal* myFractal;
    FractalType myType;

    bool finished;
    bool saveProgressAvailable;
    sf::Clock clock;

    void CalcProgress(wxUpdateUIEvent& event);  ///< When the window is updates ask the threads to calculate their progress.
    void OnCancel(wxCommandEvent& event);       ///< Stop threads and close window.

public:
    SaveProgressDiag(Fractal* targetFractal, wxWindow* parent, bool _saveProgressAvailable = true, wxWindowID id = wxID_ANY, 
                     const wxString& title = wxT("Saving..."), const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxSize(366, 150), long style = wxDEFAULT_DIALOG_STYLE);
    ~SaveProgressDiag();

    ///@brief Inform if the fractal has finished.
    ///@return true if has finished, false if not.
    bool IsFinished();
};

/**
* @class SizeDialogSave
* @brief Creates a dialog to select the fractal size and number of iterations.
*/
class SizeDialogSave : public wxDialog
{
private:
    wxPanel* mainPanel;
    wxStaticText* selectText;
    wxStaticText* widthText;
    wxSpinCtrl* widthSpin;
    wxStaticText* dumbText;
    wxStaticText* heightText;
    wxSpinCtrl* heightSpin;
    wxStaticText* iterationsText;
    wxSpinCtrl* iterationsSpin;
    wxButton* okButton;

    FractalHandler fractalHandler;
    FractalCanvas* fCanvas;
    Options opt;
    FractalType fractalType;
    string path;
    string myScriptPath;
    double screenRatio;
    int extension;

    void ChangeWidth(wxSpinEvent& event);         ///< Calculates new height and change value.
    void ChangeHeight(wxSpinEvent& event);        ///< Calculates new width and change value.
    void OnOk(wxCommandEvent& event);             ///< Creates fractal with the parameters from the dialog and saves image.

public:
    SizeDialogSave(FractalCanvas* mFCanvas, string filePath, int ext, FractalType type, Fractal* target, wxWindow* parent,
                   string scriptPath = "", wxWindowID id = wxID_ANY, const wxString& title = wxT("Select size"),
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(283, 251), 
                   long style = wxDEFAULT_DIALOG_STYLE);
    ~SizeDialogSave();
};

#endif