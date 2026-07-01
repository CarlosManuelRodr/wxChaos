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
#include <wx/bmpbndl.h>
#include <wx/statline.h>
#include "canvas/FractalCanvas.h"

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
    wxStaticText* _widthText;
    wxSpinCtrl* _widthSpin;
    wxStaticText* _heightText;
    wxSpinCtrl* _heightSpin;
    wxStaticText* _iterationsText;
    wxSpinCtrl* _iterationsSpin;
    wxStaticText* _largeImageWarningText;
    wxButton* _saveButton;
    wxButton* _cancelButton;

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
    void UpdateLargeImageWarning();
    void OnSave(wxCommandEvent& event);           ///< Selects the target path and saves image.
    void OnCancel(wxCommandEvent& event);         ///< Closes the dialog without saving.
    [[nodiscard]] static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                                      const wxString& darkIcon);
    [[nodiscard]] static wxBitmapBundle CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                                         const wxSize& size);
    void SetButtonIcon(wxButton* button, const wxString& lightIcon, const wxString& darkIcon) const;

public:
    ImageExportSizeDialog(FractalCanvas* fractalCanvas, FractalType type, const Fractal* target,
                          wxWindow* parent, const std::string& scriptPath = "",
                          wxWindowID id = wxID_ANY, const wxString& title = "Select size",
                          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(480, 280),
                          long style = wxDEFAULT_DIALOG_STYLE);
    ~ImageExportSizeDialog() override;
};
