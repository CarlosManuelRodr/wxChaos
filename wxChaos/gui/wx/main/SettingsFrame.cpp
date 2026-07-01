// ReSharper disable CppDFAUnreachableFunctionCall
// ReSharper disable CppDFAMemoryLeak
#include <wx/dcbuffer.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <utility>
#include "main/SettingsFrame.h"
#include "AppPaths.h"
#include "common/AppTheme.h"
#include "coloring/ColorPalette.h"
#include "renderer/wxGradientDialog.h"

wxDEFINE_EVENT(wxEVT_SETTINGS_FRAME_CLOSED, wxCommandEvent);

SettingsFrame::SettingsFrame(wxWindow* parent, const AppConfig& config, std::function<void(const AppConfig&)> configChanged)
                             : wxFrame(parent, wxID_ANY, "Settings", wxDefaultPosition, wxSize(700, 520),
                                  wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX),
                             _configChanged(std::move(configChanged))
{
    wxTopLevelWindowBase::SetMinSize(wxSize(620, 440));

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    _pages = new wxListbook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_LEFT);
    _pages->AddPage(CreateGeneralPage(), "General", true);
    _pages->AddPage(CreatePresetsPage(), "Presets");
    _pages->AddPage(CreateRenderingPage(), "Rendering");
    _pages->AddPage(CreateZoomPage(), "Zoom");
#ifdef __WXMSW__
    // wxMSW uses a no-header report view for left-side listbook labels.
    // Autosize its text column so page titles are not occluded.
    _pages->GetListView()->SetColumnWidth(0, wxLIST_AUTOSIZE);
#endif
    mainSizer->Add(_pages, 1, wxEXPAND | wxALL, 12);

    const auto buttons = new wxBoxSizer(wxHORIZONTAL);
    const auto defaultsButton = new wxButton(this, wxID_REVERT_TO_SAVED, "Restore defaults");
    const auto okButton = new wxButton(this, wxID_OK);
    const auto cancelButton = new wxButton(this, wxID_CANCEL);
    const auto applyButton = new wxButton(this, wxID_APPLY);
    buttons->Add(defaultsButton, 0, wxRIGHT, 8);
    buttons->AddStretchSpacer();
    buttons->Add(okButton, 0, wxRIGHT, 8);
    buttons->Add(cancelButton, 0, wxRIGHT, 8);
    buttons->Add(applyButton);
    mainSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);

    SetSizer(mainSizer);
    LoadControls(config);
    CentreOnParent();

    defaultsButton->Bind(wxEVT_BUTTON, &SettingsFrame::OnRestoreDefaults, this);
    okButton->Bind(wxEVT_BUTTON, &SettingsFrame::OnOk, this);
    cancelButton->Bind(wxEVT_BUTTON, &SettingsFrame::OnCancel, this);
    applyButton->Bind(wxEVT_BUTTON, &SettingsFrame::OnApply, this);
    Bind(wxEVT_CLOSE_WINDOW, &SettingsFrame::OnClose, this);
}

wxPanel* SettingsFrame::CreateGeneralPage()
{
    const auto page = new wxPanel(_pages);
    const auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(page, wxID_ANY, "Startup"), 0, wxBOTTOM, 8);

    _constantWindow = new wxCheckBox(page, wxID_ANY, "Open the Julia constant window");
    _commandConsole = new wxCheckBox(page, wxID_ANY, "Open the command console");
    _juliaMode = new wxCheckBox(page, wxID_ANY, "Open Julia mode");
    _colorPaletteWindow = new wxCheckBox(page, wxID_ANY, "Open renderer options");
    _firstUse = new wxCheckBox(page, wxID_ANY, "Show the welcome guide on next launch");
    const wxString themeChoices[] = {"System", "Light", "Dark"};
    _theme = new wxChoice(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, std::size(themeChoices), themeChoices);

    sizer->Add(_constantWindow, 0, wxBOTTOM, 8);
    sizer->Add(_commandConsole, 0, wxBOTTOM, 8);
    sizer->Add(_juliaMode, 0, wxBOTTOM, 8);
    sizer->Add(_colorPaletteWindow, 0, wxBOTTOM, 8);
    sizer->Add(_firstUse, 0, wxBOTTOM, 8);
    const auto themeRow = new wxBoxSizer(wxHORIZONTAL);
    themeRow->Add(new wxStaticText(page, wxID_ANY, "Appearance:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    themeRow->Add(_theme, 0);
    sizer->Add(themeRow, 0, wxTOP | wxBOTTOM, 12);
    sizer->Add(new wxStaticText(page, wxID_ANY,
        "Startup window changes take effect the next time wxChaos starts."), 0, wxTOP, 12);
    page->SetSizer(sizer);
    return page;
}

wxPanel* SettingsFrame::CreatePresetsPage()
{
    const auto page = new wxPanel(_pages);
    const auto sizer = new wxFlexGridSizer(2, 12, 12);
    sizer->AddGrowableCol(1, 1);

    const wxString names[] = {
        "Mandelbrot", "Mandelbrot ZN", "Julia", "Julia ZN", "Newton",
        "Sine", "Magnet", "Jellyfish", "Manowar", "Manowar Julia",
        "Sierpinski Triangle", "Fixed Point: sin(z)", "Fixed Point: cos(z)",
        "Fixed Point: tan(z)", "Fixed Point: z^2", "Tricorn", "Burning Ship",
        "Burning Ship Julia", "Fractory", "Cell", "Double Pendulum",
        "User Defined", "User Defined Fixed Point", "User Defined Newton-Raphson"
    };
    _fractalTypes = {
        FractalType::Mandelbrot, FractalType::MandelbrotZN, FractalType::Julia, FractalType::JuliaZN,
        FractalType::NewtonRaphsonMethod, FractalType::Sinusoidal, FractalType::Magnetic, FractalType::Jellyfish,
        FractalType::Manowar, FractalType::ManowarJulia, FractalType::SierpinskiTriangle, FractalType::FixedPoint1,
        FractalType::FixedPoint2, FractalType::FixedPoint3, FractalType::FixedPoint4, FractalType::Tricorn,
        FractalType::BurningShip, FractalType::BurningShipJulia, FractalType::Fractory, FractalType::Cell,
        FractalType::DoublePendulum, FractalType::UserDefinedEscapeTime, FractalType::UserDefinedFixedPoint,
        FractalType::UserDefinedNewtonRaphson
    };

    _fractalType = new wxChoice(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, std::size(names), names);
    _maxIterations = new wxSpinCtrl(page, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 1, 20000000, 100);
    _automaticIterations = new wxCheckBox(page, wxID_ANY, "Automatic iterations");
    sizer->Add(new wxStaticText(page, wxID_ANY, "Default fractal:"), 0, wxALIGN_CENTER_VERTICAL);
    sizer->Add(_fractalType, 1, wxEXPAND);
    sizer->Add(new wxStaticText(page, wxID_ANY, "Default iterations:"), 0, wxALIGN_CENTER_VERTICAL);
    sizer->Add(_maxIterations, 1, wxEXPAND);
    sizer->AddSpacer(0);
    sizer->Add(_automaticIterations, 0, wxEXPAND);
    page->SetSizer(sizer);
    return page;
}

wxPanel* SettingsFrame::CreateRenderingPage()
{
    const auto page = new wxPanel(_pages);
    const auto sizer = new wxBoxSizer(wxVERTICAL);

    const auto paletteRow = new wxBoxSizer(wxHORIZONTAL);
    paletteRow->Add(new wxStaticText(page, wxID_ANY, "Palette size:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    _paletteSize = new wxSpinCtrl(page, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 1, 20000, 300);
    paletteRow->Add(_paletteSize, 0);
    sizer->Add(paletteRow, 0, wxBOTTOM, 16);

    const auto cycleRow = new wxBoxSizer(wxHORIZONTAL);
    cycleRow->Add(new wxStaticText(page, wxID_ANY, "Color cycle length:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    _colorCycleLength = new wxSpinCtrl(page, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 1, 20000, 72);
    cycleRow->Add(_colorCycleLength, 0);
    sizer->Add(cycleRow, 0, wxBOTTOM, 16);

    const wxString colorStyleNames[] = {
        "System",
        "Retro",
        "Hakim",
        "Aquamarine",
        "Pastel Dream",
        "Rose Gold",
        "Gunmetal",
        "Sunset Drive",
        "Aurora Borealis",
        "Vaporwave",
        "Deep Ocean",
        "Ember",
        "Rainbow Fire",
        "Classic Mandelbrot",
        "Custom"
    };
    _colorStyles = {
        System,
        Retro,
        Hakim,
        Aquamarine,
        PastelDream,
        RoseGold,
        Gunmetal,
        SunsetDrive,
        AuroraBorealis,
        Vaporwave,
        DeepOcean,
        Ember,
        RainbowFire,
        ClassicMandelbrot,
        CustomGradient
    };
    const auto styleRow = new wxBoxSizer(wxHORIZONTAL);
    styleRow->Add(new wxStaticText(page, wxID_ANY, "Color style:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    _colorStyle = new wxChoice(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, std::size(colorStyleNames), colorStyleNames);
    styleRow->Add(_colorStyle, 0);
    sizer->Add(styleRow, 0, wxBOTTOM, 12);

    sizer->Add(new wxStaticText(page, wxID_ANY, "Gradient preview:"), 0, wxBOTTOM, 6);
    const auto gradientRow = new wxBoxSizer(wxHORIZONTAL);
    _gradientPreview = new wxStaticBitmap(page, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(320, 44), wxBORDER_SIMPLE);
    const auto editGradient = new wxButton(page, wxID_EDIT, "Edit...");
    gradientRow->Add(_gradientPreview, 1, wxEXPAND | wxRIGHT, 12);
    gradientRow->Add(editGradient, 0, wxALIGN_CENTER_VERTICAL);
    sizer->Add(gradientRow, 0, wxEXPAND | wxBOTTOM, 18);

    _colorFractal = new wxCheckBox(page, wxID_ANY, "Color points outside the fractal set");
    _colorSet = new wxCheckBox(page, wxID_ANY, "Color points inside the fractal set");
    sizer->Add(_colorFractal, 0, wxBOTTOM, 8);
    sizer->Add(_colorSet, 0, wxBOTTOM, 8);
    page->SetSizer(sizer);

    _colorStyle->Bind(wxEVT_CHOICE, &SettingsFrame::OnColorStyleChanged, this);
    editGradient->Bind(wxEVT_BUTTON, &SettingsFrame::OnEditGradient, this);
    return page;
}

wxPanel* SettingsFrame::CreateZoomPage()
{
    const auto page = new wxPanel(_pages);
    const auto sizer = new wxFlexGridSizer(2, 12, 12);
    sizer->AddGrowableCol(1, 1);

    _zoomStepPercent = new wxSpinCtrl(page, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 1, 95, 25);
    _zoomInertiaMilliseconds = new wxSpinCtrl(page, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 0, 1000, 180);

    sizer->Add(new wxStaticText(page, wxID_ANY, "Wheel zoom step (%):"), 0, wxALIGN_CENTER_VERTICAL);
    sizer->Add(_zoomStepPercent, 0);
    sizer->Add(new wxStaticText(page, wxID_ANY, "Preview inertia (ms):"), 0, wxALIGN_CENTER_VERTICAL);
    sizer->Add(_zoomInertiaMilliseconds, 0);
    page->SetSizer(sizer);
    return page;
}

void SettingsFrame::LoadControls(const AppConfig& config)
{
    _constantWindow->SetValue(config.constantWindow);
    _commandConsole->SetValue(config.commandConsole);
    _juliaMode->SetValue(config.juliaMode);
    _colorPaletteWindow->SetValue(config.colorPaletteWindow);
    _firstUse->SetValue(config.firstUse);
    switch (config.appearance)
    {
        case AppAppearance::Light:
            _theme->SetSelection(1);
            break;
        case AppAppearance::Dark:
            _theme->SetSelection(2);
            break;
        case AppAppearance::System:
            _theme->SetSelection(0);
            break;
    }
    _maxIterations->SetValue(config.maxIterations);
    _automaticIterations->SetValue(config.automaticIterations);
    _paletteSize->SetValue(config.paletteSize);
    _colorCycleLength->SetValue(config.colorCycleLength);
    _colorFractal->SetValue(config.colorFractal);
    _colorSet->SetValue(config.colorSet);
    _zoomStepPercent->SetValue(config.zoomStepPercent);
    _zoomInertiaMilliseconds->SetValue(config.zoomInertiaMilliseconds);
    _colorStyle->SetSelection(0);
    for (size_t i = 0; i < _colorStyles.size(); ++i)
    {
        if (_colorStyles[i] == config.colorStyle)
        {
            _colorStyle->SetSelection(static_cast<int>(i));
            break;
        }
    }

    _fractalType->SetSelection(0);
    for (size_t i = 0; i < _fractalTypes.size(); ++i)
    {
        if (_fractalTypes[i] == config.type)
        {
            _fractalType->SetSelection(static_cast<int>(i));
            break;
        }
    }

    _gradient = wxGradient();
    _gradient.SetMin(0);
    _gradient.SetMax(config.paletteSize);
    _gradient.FromString(wxString::FromUTF8(config.colorStyleGrad.c_str()));
    UpdateGradientPreview();
}

AppConfig SettingsFrame::ReadControls()
{
    AppConfig config;
    const int selection = _fractalType->GetSelection();
    if (selection != wxNOT_FOUND)
        config.type = _fractalTypes[selection];
    config.maxIterations = _maxIterations->GetValue();
    config.automaticIterations = _automaticIterations->GetValue();
    config.paletteSize = _paletteSize->GetValue();
    config.colorCycleLength = _colorCycleLength->GetValue();
    const int colorStyleSelection = _colorStyle->GetSelection();
    if (colorStyleSelection != wxNOT_FOUND)
        config.colorStyle = _colorStyles[colorStyleSelection];
    config.colorStyleGrad = _gradient.ToString().ToStdString();
    config.constantWindow = _constantWindow->GetValue();
    config.commandConsole = _commandConsole->GetValue();
    config.juliaMode = _juliaMode->GetValue();
    config.colorPaletteWindow = _colorPaletteWindow->GetValue();
    config.colorFractal = _colorFractal->GetValue();
    config.colorSet = _colorSet->GetValue();
    config.firstUse = _firstUse->GetValue();
    config.zoomStepPercent = _zoomStepPercent->GetValue();
    config.zoomInertiaMilliseconds = _zoomInertiaMilliseconds->GetValue();
    switch (_theme->GetSelection())
    {
        case 1:
            config.appearance = AppAppearance::Light;
            break;
        case 2:
            config.appearance = AppAppearance::Dark;
            break;
        default:
            config.appearance = AppAppearance::System;
            break;
    }
    return config;
}

void SettingsFrame::UpdateGradientPreview() const
{
    const wxSize size = _gradientPreview->GetMinSize();
    wxBitmap bitmap(size.GetWidth(), size.GetHeight());
    wxMemoryDC dc(bitmap);
    wxGradient preview = _gradient;
    preview.SetMin(0);
    preview.SetMax(size.GetWidth());
    for (int x = 0; x < size.GetWidth(); ++x)
    {
        dc.SetPen(wxPen(preview.GetColorAt(x)));
        dc.DrawLine(x, 0, x, size.GetHeight());
    }
    dc.SelectObject(wxNullBitmap);
    _gradientPreview->SetBitmap(bitmap);
}

void SettingsFrame::ApplyColorStyle(const ColorPaletteTypes style)
{
    if (style == CustomGradient)
        return;

    ColorPalette palette;
    palette.SetStyle(AppConfig::ResolveColorStyle(style));
    _paletteSize->SetValue(palette.paletteSize);
    _colorCycleLength->SetValue(palette.colorCycleLength);
    _gradient = wxGradient();
    _gradient.SetMin(0);
    _gradient.SetMax(palette.paletteSize);
    _gradient.FromString(wxString::FromUTF8(palette.grad.c_str()));
    UpdateGradientPreview();
}

void SettingsFrame::SaveSettings(const bool closeAfterSave)
{
    AppConfig config = ReadControls();
    AppConfigStore(AppPaths::ToStdPath(AppPaths::ConfigFile())).Save(config);
    if (_configChanged)
        _configChanged(config);
    if (closeAfterSave)
        Close();
}

void SettingsFrame::OnColorStyleChanged(wxCommandEvent&)
{
    const int selection = _colorStyle->GetSelection();
    if (selection != wxNOT_FOUND)
        ApplyColorStyle(_colorStyles[selection]);
}

void SettingsFrame::OnEditGradient(wxCommandEvent&)
{
    _colorStyle->SetSelection(static_cast<int>(_colorStyles.size()) - 1);
    wxGradientDialog dialog(this, _gradient);
    if (dialog.ShowModal() == wxID_OK)
    {
        _gradient = dialog.GetGradient();
        UpdateGradientPreview();
    }
}

void SettingsFrame::OnRestoreDefaults(wxCommandEvent&)
{
    LoadControls(AppConfig{});
}

void SettingsFrame::OnApply(wxCommandEvent&)
{
    SaveSettings(false);
}

void SettingsFrame::OnOk(wxCommandEvent&)
{
    SaveSettings(true);
}

void SettingsFrame::OnCancel(wxCommandEvent&)
{
    Close();
}

void SettingsFrame::OnClose(wxCloseEvent&)
{
    if (!_closing)
    {
        _closing = true;
        wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_SETTINGS_FRAME_CLOSED));
    }
    Destroy();
}
