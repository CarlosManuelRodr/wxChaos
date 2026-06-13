#pragma once

#include <functional>
#include <vector>
#include <wx/listbook.h>
#include <wx/spinctrl.h>
#include <wx/wx.h>
#include "config/AppConfigStore.h"
#include "wxGradient.h"

wxDECLARE_EVENT(wxEVT_SETTINGS_FRAME_CLOSED, wxCommandEvent);

class SettingsFrame final : public wxFrame
{
public:
    SettingsFrame(wxWindow* parent, const AppConfig& config, std::function<void(const AppConfig&)> configChanged);

private:
    wxPanel* CreateGeneralPage();
    wxPanel* CreatePresetsPage();
    wxPanel* CreateRenderingPage();
    void LoadControls(const AppConfig& config);
    AppConfig ReadControls();
    void UpdateGradientPreview();
    void SaveSettings(bool closeAfterSave);
    void OnEditGradient(wxCommandEvent& event);
    void OnRestoreDefaults(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    wxListbook* _pages{};
    wxCheckBox* _constantWindow{};
    wxCheckBox* _commandConsole{};
    wxCheckBox* _juliaMode{};
    wxCheckBox* _colorPaletteWindow{};
    wxCheckBox* _firstUse{};
    wxChoice* _fractalType{};
    wxSpinCtrl* _maxIterations{};
    wxSpinCtrl* _paletteSize{};
    wxCheckBox* _colorFractal{};
    wxCheckBox* _colorSet{};
    wxStaticBitmap* _gradientPreview{};
    wxGradient _gradient;
    std::vector<FractalType> _fractalTypes;
    std::function<void(const AppConfig&)> _configChanged;
    bool _closing{};
};
