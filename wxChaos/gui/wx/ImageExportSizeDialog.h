/**
* @file ImageExportSizeDialog.h
* @brief A dialog to select the image size and show the rendering progress.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/19/2012
*/

#pragma once

#include <wx/wx.h>
#include <wx/statline.h>
#include "FractalCanvas.h"

/**
* @class ImageExportProgressDialog
* @brief Tracks progress while a high-resolution image render is being saved.
*
* The dialog polls the target fractal render progress, allows cancellation when
* supported, and closes once the save render finishes.
*/
class ImageExportProgressDialog : public wxDialog
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
    ImageExportProgressDialog(Fractal* targetFractal, wxWindow* parent, bool saveProgressAvailable = true, wxWindowID id = wxID_ANY,
                              const wxString& title = "Saving...", const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxSize(480, 180), long style = wxDEFAULT_DIALOG_STYLE);
    ~ImageExportProgressDialog() override;

    ///@brief Inform if the fractal has finished.
    ///@return true if it has finished, false if not.
    [[nodiscard]] bool IsFinished() const;
};
/**
* @class ImageExportSizeDialog
* @brief Collects export dimensions and starts an off-screen image save render.
*
* ImageExportSizeDialog preserves the active canvas aspect ratio while the user changes
* width or height, copies the current fractal options, renders the requested
* size through FractalFactory, and writes the selected image format.
*/
class ImageExportSizeDialog : public wxDialog
{
    wxPanel* _mainPanel;
    wxStaticText* _selectText;
    wxStaticText* _widthText;
    wxSpinCtrl* _widthSpin;
    wxStaticText* _dumbText;
    wxStaticText* _heightText;
    wxSpinCtrl* _heightSpin;
    wxStaticText* _iterationsText;
    wxSpinCtrl* _iterationsSpin;
    wxButton* _okButton;

    FractalFactory _fractalFactory;
    FractalCanvas* _fractalCanvas;
    Options _options;
    FractalType _fractalType;
    std::string _path;
    std::string _myScriptPath;
    double _screenRatio;
    int _extension;

    void ChangeWidth(wxSpinEvent& event);         ///< Calculates new height and change value.
    void ChangeHeight(wxSpinEvent& event);        ///< Calculates new width and change value.
    void OnOk(wxCommandEvent& event);             ///< Creates fractal with the parameters from the dialog and saves image.

public:
    ImageExportSizeDialog(FractalCanvas* fractalCanvas, const std::string& filePath, int ext, FractalType type,
                          const Fractal* target, wxWindow* parent, const std::string& scriptPath = "",
                          wxWindowID id = wxID_ANY, const wxString& title = "Select size",
                          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(420, 300),
                          long style = wxDEFAULT_DIALOG_STYLE);
    ~ImageExportSizeDialog() override;
};
