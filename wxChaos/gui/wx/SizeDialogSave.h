/**
* @file SizeDialogSave.h
* @brief A dialog to select the image size and show the rendering progress.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/19/2012
*/

#pragma once
#ifndef _sizeDialog
#define _sizeDialog

#include <wx/wx.h>
#include <wx/statline.h>
#include "FractalCanvas.h"

/**
* @class SaveProgressDiag
* @brief Creates a dialog that shows the user the amount of rendering progress and allows the user to pause it and save progress.
*/
class SaveProgressDiag : public wxDialog
{
    wxGauge* _progress;
    wxStaticText* _progressLabel;
    wxStaticLine* _staticLine;
    wxButton* _cancelButton;
    Fractal* _myFractal;
    FractalType _myType;

    bool _finished;
    bool _saveProgressAvailable;
    sf::Clock _clock;

    void CalcProgress(wxUpdateUIEvent& event);  ///< When the window is updates ask the threads to calculate their progress.
    void OnCancel(wxCommandEvent& event);       ///< Stop threads and close the window.

public:
    SaveProgressDiag(Fractal* targetFractal, wxWindow* parent, bool saveProgressAvailable = true, wxWindowID id = wxID_ANY,
        const wxString& title = wxT("Saving..."), const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(480, 180), long style = wxDEFAULT_DIALOG_STYLE);
    ~SaveProgressDiag() override;

    ///@brief Inform if the fractal has finished.
    ///@return true if has finished, false if not.
    bool IsFinished() const;
};

/**
* @class SizeDialogSave
* @brief Creates a dialog to select the fractal size and number of iterations.
*/
class SizeDialogSave : public wxDialog
{
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
    std::string path;
    std::string myScriptPath;
    double screenRatio;
    int extension;

    void ChangeWidth(wxSpinEvent& event);         ///< Calculates new height and change value.
    void ChangeHeight(wxSpinEvent& event);        ///< Calculates new width and change value.
    void OnOk(wxCommandEvent& event);             ///< Creates fractal with the parameters from the dialog and saves image.

public:
    SizeDialogSave(FractalCanvas* mFCanvas, const std::string& filePath, int ext, FractalType type, const Fractal* target, wxWindow* parent,
                   const std::string& scriptPath = "", wxWindowID id = wxID_ANY, const wxString& title = wxT("Select size"),
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(420, 300),
                   long style = wxDEFAULT_DIALOG_STYLE);
    ~SizeDialogSave() override;
};

#endif
