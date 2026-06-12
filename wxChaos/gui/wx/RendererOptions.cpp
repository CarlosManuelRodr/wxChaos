#include <utility>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "AppPaths.h"
#include "RendererOptions.h"
#include "TextUtils.h"

wxDEFINE_EVENT(wxEVT_RENDERER_OPTIONS_CLOSED, wxCommandEvent);

RendererOptions::RendererOptions(SFMLFractal* presenter, wxWindow* parent,
                                 std::function<void(const Options&)> optionsChanged, const wxWindowID id,
                                 const wxString& title, const wxPoint& pos, const wxSize& size, const long windowStyle)
                                 : wxFrame(parent, id, title, pos, size, windowStyle)
{
    // Constructs the ColorFrame. Gets color values from the target fractal so the frame parameters match the fractal parameters.
    const wxIcon icon(AppPaths::ResourceFile({wxT("icon.ico")}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    _presenter = presenter;
    _target = _presenter->GetFractal();
    _optionsChanged = std::move(optionsChanged);

    this->SetSizeHints(wxSize(760, 700), wxDefaultSize);

    auto* sizer = new wxBoxSizer(wxVERTICAL);

    _mainPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    _mainPanel->SetScrollRate(5, 5);
    const auto mainSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* setSizer = new wxBoxSizer(wxVERTICAL);

    _algorithmText = new wxStaticText(_mainPanel, wxID_ANY, wxT("Color algorithm"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color algorithm"
    _algorithmText->Wrap(-1);
    setSizer->Add(_algorithmText, 0, wxALL, 5);

    // AlgorithmChoice.
    _escapeTimeIndex = -1;
    _gaussIntIndex = -1;
    _buddhabrotIndex = -1;
    _escapeAngleIndex = -1;
    _triangleIneqIndex = -1;
    _chaoticMapIndex = -1;
    _lyapunovIndex = -1;
    _convergenceTestIndex = -1;
    _algorithmChoice = new wxChoice(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    setSizer->Add(_algorithmChoice, 0, wxALL|wxEXPAND, 5);

    _optionsText = new wxStaticText(_mainPanel, wxID_ANY, wxT("Options"), wxDefaultPosition, wxDefaultSize, 0);
    _optionsText->Wrap(-1);
    setSizer->Add(_optionsText, 0, wxALL, 5);

    _relativeCheck = new wxCheckBox(_mainPanel, wxID_ANY, wxT(" Relative colors"), wxDefaultPosition, wxDefaultSize, 0);
    _relativeCheck->SetValue(_target->GetRelativeColorMode());
    setSizer->Add(_relativeCheck, 0, wxALL, 5);

    _colorFractal = new wxCheckBox(_mainPanel, wxID_ANY, wxT(" Fractal color (external color)"), wxDefaultPosition, wxDefaultSize, 0);
    _colorFractal->SetValue(_target->GetExteriorColorMode());
    setSizer->Add(_colorFractal, 0, wxALL, 5);

    _colorSet = new wxCheckBox(_mainPanel, wxID_ANY, wxT(" Set color (internal color)"), wxDefaultPosition, wxDefaultSize, 0);
    _colorSet->SetValue(_target->GetInteriorColorMode());
    setSizer->Add(_colorSet, 0, wxALL, 5);

    _orbitTrap = new wxCheckBox(_mainPanel, wxID_ANY, wxT(" Orbit traps"), wxDefaultPosition, wxDefaultSize, 0);
    _orbitTrap->Enable(_target->HasOrbitTrapMode());
    _orbitTrap->SetValue(_target->OrbitTrapActivated());
    setSizer->Add(_orbitTrap, 0, wxALL, 5);

    _smoothRender = new wxCheckBox(_mainPanel, wxID_ANY, wxT(" Smooth render"), wxDefaultPosition, wxDefaultSize, 0);
    _smoothRender->Enable(_target->HasSmoothRenderMode());
    _smoothRender->SetValue(_target->SmoothRenderActivated());

    setSizer->Add(_smoothRender, 0, wxALL, 5);

    _colorVarText = new wxStaticText(_mainPanel, wxID_ANY, wxT("Color variation"), wxDefaultPosition, wxDefaultSize, 0);
    _colorVarText->Wrap(-1);
    setSizer->Add(_colorVarText, 0, wxALL, 5);

    _colorVarSlider = new wxSlider(_mainPanel, wxID_ANY, 0, 0, 300, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    setSizer->Add(_colorVarSlider, 0, wxALL|wxEXPAND, 5);

    auto* colorSetSizer = new wxStaticBoxSizer(new wxStaticBox(_mainPanel, wxID_ANY, wxT("Set color")), wxVERTICAL);

    wxString text = L"Red: ";
    _setColor = _target->GetSetColor();
    text += TextUtils::ToWxString(_setColor.r);
    _redSetText = new wxStaticText(_mainPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    _redSetText->Wrap(-1);
    colorSetSizer->Add(_redSetText, 0, wxALL, 5);

    _redSetSld = new wxSlider(_mainPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(_redSetSld, 0, wxALL|wxEXPAND, 5);

    text = wxT("Green: ");
    text += TextUtils::ToWxString(_setColor.g);
    _greenSetText = new wxStaticText(_mainPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    _greenSetText->Wrap(-1);
    colorSetSizer->Add(_greenSetText, 0, wxALL, 5);

    _greenSetSld = new wxSlider(_mainPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(_greenSetSld, 0, wxALL|wxEXPAND, 5);

    text = wxT("Blue: ");
    text += TextUtils::ToWxString(_setColor.b);
    _blueSetText = new wxStaticText(_mainPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    _blueSetText->Wrap(-1);
    colorSetSizer->Add(_blueSetText, 0, wxALL, 5);

    _blueSetSld = new wxSlider(_mainPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(_blueSetSld, 0, wxALL|wxEXPAND, 5);

    setSizer->Add(colorSetSizer, 1, wxEXPAND, 5);

    _okButton = new wxButton(_mainPanel, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);
    setSizer->Add(_okButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    mainSizer->Add(setSizer, 1, wxEXPAND, 5);

    _typeNotebook = new wxNotebook(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    _gradientLabel = new wxPanel(_typeNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    auto* gradSizer = new wxBoxSizer(wxVERTICAL);

    _gradStylesLabel = new wxStaticText(_gradientLabel, wxID_ANY, wxT("Color styles:"), wxDefaultPosition, wxDefaultSize, 0);
    _gradStylesLabel->Wrap(-1);
    gradSizer->Add(_gradStylesLabel, 0, wxALL, 5);

    wxString gradStyleChoiceChoices[] = {
        wxT("Retro"),
        wxT("Hakim"),
        wxT("Aquamarine"),
        wxT("Pastel Dream"),
        wxT("Rose Gold"),
        wxT("Gunmetal"),
        wxT("Sunset Drive"),
        wxT("Aurora Borealis"),
        wxT("Vaporwave"),
        wxT("Deep Ocean"),
        wxT("Ember"),
        wxT("Rainbow Fire"),
        wxT("Custom")
    };
    constexpr int gradStyleChoiceNChoices = sizeof(gradStyleChoiceChoices) / sizeof(wxString);
    _gradStylesChoice = new wxChoice(_gradientLabel, wxID_ANY, wxDefaultPosition, wxDefaultSize, gradStyleChoiceNChoices, gradStyleChoiceChoices, 0);
    _gradStylesChoice->SetSelection(_target->GetColorPalette());
    gradSizer->Add(_gradStylesChoice, 0, wxALL, 5);

    _gradientMap = new wxStaticBitmap(_gradientLabel, wxID_ANY, PaintGradient(), wxDefaultPosition, wxDefaultSize, 0);
    gradSizer->Add(_gradientMap, 0, wxALL, 5);

    _gradButton = new wxButton(_gradientLabel, wxID_ANY, wxT("Change gradient"), wxDefaultPosition, wxDefaultSize, 0);
    gradSizer->Add(_gradButton, 0, wxALL, 5);

    _gradPalText = new wxStaticText(_gradientLabel, wxID_ANY, wxT("Palette size:"), wxDefaultPosition, wxDefaultSize, 0);
    _gradPalText->Wrap(-1);
    gradSizer->Add(_gradPalText, 0, wxALL, 5);

    _gradPalSize = new wxSpinCtrl(_gradientLabel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20000, 300);
    gradSizer->Add(_gradPalSize, 0, wxALL, 5);
    const auto gradientSize = static_cast<int>(_target->GetGradient()->GetMax() - _target->GetGradient()->GetMin());
    _gradPalSize->SetValue(gradientSize);

    _gradientLabel->SetSizer(gradSizer);
    _gradientLabel->Layout();
    gradSizer->Fit(_gradientLabel);
    _typeNotebook->AddPage(_gradientLabel, wxT("Gradient color"), true);

    mainSizer->Add(_typeNotebook, 2, wxEXPAND | wxALL, 5);

    _mainPanel->SetSizer(mainSizer);
    _mainPanel->Layout();
    mainSizer->Fit(_mainPanel);
    sizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    _typeNotebook->ChangeSelection(0);

    this->SetAlgorithmChoices();
    this->ConnectEvents();
}
void RendererOptions::ConnectEvents()
{
    this->Bind(wxEVT_CLOSE_WINDOW, &RendererOptions::OnClose, this);
    _gradStylesChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &RendererOptions::GradientColorChangeSelection, this);
    _algorithmChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &RendererOptions::OnChangeAlgorithm, this);
    _relativeCheck->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &RendererOptions::OnRelativeColor, this);
    _gradPalSize->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &RendererOptions::OnGradPaletteSize, this);
    _colorFractal->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &RendererOptions::OnColorFractal, this);
    _colorSet->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &RendererOptions::OnColorSet, this);
    _orbitTrap->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &RendererOptions::OnOrbitTrap, this);
    _smoothRender->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &RendererOptions::OnSmoothRender, this);
    _redSetSld->Bind(wxEVT_SCROLL_TOP, &RendererOptions::OnSetRed, this);
    _redSetSld->Bind(wxEVT_SCROLL_BOTTOM, &RendererOptions::OnSetRed, this);
    _redSetSld->Bind(wxEVT_SCROLL_LINEUP, &RendererOptions::OnSetRed, this);
    _redSetSld->Bind(wxEVT_SCROLL_LINEDOWN, &RendererOptions::OnSetRed, this);
    _redSetSld->Bind(wxEVT_SCROLL_PAGEUP, &RendererOptions::OnSetRed, this);
    _redSetSld->Bind(wxEVT_SCROLL_PAGEDOWN, &RendererOptions::OnSetRed, this);
    _redSetSld->Bind(wxEVT_SCROLL_THUMBTRACK, &RendererOptions::OnSetRed, this);
    _redSetSld->Bind(wxEVT_SCROLL_THUMBRELEASE, &RendererOptions::OnSetRed, this);
    _redSetSld->Bind(wxEVT_SCROLL_CHANGED, &RendererOptions::OnSetRed, this);
    _greenSetSld->Bind(wxEVT_SCROLL_TOP, &RendererOptions::OnSetGreen, this);
    _greenSetSld->Bind(wxEVT_SCROLL_BOTTOM, &RendererOptions::OnSetGreen, this);
    _greenSetSld->Bind(wxEVT_SCROLL_LINEUP, &RendererOptions::OnSetGreen, this);
    _greenSetSld->Bind(wxEVT_SCROLL_LINEDOWN, &RendererOptions::OnSetGreen, this);
    _greenSetSld->Bind(wxEVT_SCROLL_PAGEUP, &RendererOptions::OnSetGreen, this);
    _greenSetSld->Bind(wxEVT_SCROLL_PAGEDOWN, &RendererOptions::OnSetGreen, this);
    _greenSetSld->Bind(wxEVT_SCROLL_THUMBTRACK, &RendererOptions::OnSetGreen, this);
    _greenSetSld->Bind(wxEVT_SCROLL_THUMBRELEASE, &RendererOptions::OnSetGreen, this);
    _greenSetSld->Bind(wxEVT_SCROLL_CHANGED, &RendererOptions::OnSetGreen, this);
    _blueSetSld->Bind(wxEVT_SCROLL_TOP, &RendererOptions::OnSetBlue, this);
    _blueSetSld->Bind(wxEVT_SCROLL_BOTTOM, &RendererOptions::OnSetBlue, this);
    _blueSetSld->Bind(wxEVT_SCROLL_LINEUP, &RendererOptions::OnSetBlue, this);
    _blueSetSld->Bind(wxEVT_SCROLL_LINEDOWN, &RendererOptions::OnSetBlue, this);
    _blueSetSld->Bind(wxEVT_SCROLL_PAGEUP, &RendererOptions::OnSetBlue, this);
    _blueSetSld->Bind(wxEVT_SCROLL_PAGEDOWN, &RendererOptions::OnSetBlue, this);
    _blueSetSld->Bind(wxEVT_SCROLL_THUMBTRACK, &RendererOptions::OnSetBlue, this);
    _blueSetSld->Bind(wxEVT_SCROLL_THUMBRELEASE, &RendererOptions::OnSetBlue, this);
    _blueSetSld->Bind(wxEVT_SCROLL_CHANGED, &RendererOptions::OnSetBlue, this);
    _okButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &RendererOptions::OnOk, this);
    _gradButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &RendererOptions::OnGrad, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_TOP, &RendererOptions::OnColorVar, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_BOTTOM, &RendererOptions::OnColorVar, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_LINEUP, &RendererOptions::OnColorVar, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_LINEDOWN, &RendererOptions::OnColorVar, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_PAGEUP, &RendererOptions::OnColorVar, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_PAGEDOWN, &RendererOptions::OnColorVar, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_THUMBTRACK, &RendererOptions::OnColorVar, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_THUMBRELEASE, &RendererOptions::OnColorVar, this);
    _colorVarSlider->Bind(wxEVT_SCROLL_CHANGED, &RendererOptions::OnColorVar, this);
}
void RendererOptions::NotifyOptionsChanged() const
{
    if (_optionsChanged)
        _optionsChanged(_target->GetOptions());
}
void RendererOptions::SetAlgorithmChoices()
{
    // Construct the algorithm choice according to the algorithms available in the fractal.
    for (unsigned int i=0; i<_target->GetAvailableAlg().size(); i++)
    {
        switch (_target->GetAvailableAlg()[i])
        {
            case RenderingAlgorithmType::EscapeTime:
                {
                    _algorithmChoice->Append(wxT("Escape time"));
                    _escapeTimeIndex = static_cast<int>(i);
                }
                break;
            case RenderingAlgorithmType::GaussianInt:
                {
                    _algorithmChoice->Append(wxT("Gaussian integer"));
                    _gaussIntIndex = static_cast<int>(i);
                }
                break;
            case RenderingAlgorithmType::EscapeAngle:
                {
                    _algorithmChoice->Append(wxT("Escape angle"));
                    _escapeAngleIndex = static_cast<int>(i);
                }
                break;
            case RenderingAlgorithmType::TriangleInequality:
                {
                    _algorithmChoice->Append(wxT("Triangle inequality"));
                    _triangleIneqIndex = static_cast<int>(i);
                }
                break;
            case RenderingAlgorithmType::ChaoticMap:
                {
                    _algorithmChoice->Append(wxT("Chaotic map"));
                    _chaoticMapIndex = static_cast<int>(i);
                }
                break;
            case RenderingAlgorithmType::Lyapunov:
                {
                    _algorithmChoice->Append(wxT("Lyapunov"));
                    _lyapunovIndex = static_cast<int>(i);
                }
                break;
            case RenderingAlgorithmType::ConvergenceTest:
                {
                    _algorithmChoice->Append(wxT("Convergence test"));
                    _convergenceTestIndex = static_cast<int>(i);
                }
            case RenderingAlgorithmType::Buddhabrot:
                {
                    _algorithmChoice->Append(wxT("Buddhabrot"));
                    _buddhabrotIndex = static_cast<int>(i);
                }
            case RenderingAlgorithmType::Other:
                break;
        };

        if (_target->GetCurrentAlg() == _target->GetAvailableAlg()[i])
            _algorithmChoice->SetSelection(static_cast<int>(i));
    }
    if (_algorithmChoice->GetCount() == 0)
    {
        _algorithmChoice->Append(wxT("Special algorithm"));
        _algorithmChoice->SetSelection(0);
    }

    // Adjust frame parameters when an algorithm is chosen.
    const int selection = _algorithmChoice->GetSelection();
    if (selection == _escapeTimeIndex)
    {
        _orbitTrap->Enable(_target->HasOrbitTrapMode());
        _smoothRender->Enable(_target->HasSmoothRenderMode());
    }
    else if (selection == _convergenceTestIndex)
    {
        _orbitTrap->Enable(_target->HasOrbitTrapMode());
    }
    else
    {
        _orbitTrap->Enable(false);
        _smoothRender->Enable(false);
        _orbitTrap->SetValue(false);
        _smoothRender->SetValue(false);
        _presenter->SetOrbitTrapMode(false);
        _presenter->SetSmoothRender(false);
    }
}
int RendererOptions::GetPaletteSizeForAlgorithm(const RenderingAlgorithmType algorithm, const int paletteSize) const
{
    if (algorithm == RenderingAlgorithmType::Buddhabrot)
        return BuddhabrotPaletteSize;

    return paletteSize;
}
void RendererOptions::ApplyPaletteSize(const int paletteSize)
{
    _presenter->SetGradientSize(paletteSize);
    _gradPalSize->SetValue(paletteSize);
    _colorVarSlider->SetRange(0, paletteSize);
    NotifyOptionsChanged();
}
void RendererOptions::SetTarget(SFMLFractal* presenter)
{
    // Sets the new target fractal.
    _presenter = presenter;
    _target = _presenter->GetFractal();
    _gradPalSize->SetValue(static_cast<int>(_target->GetPaletteSize()));
    _orbitTrap->Enable(_target->HasOrbitTrapMode());
    _orbitTrap->SetValue(_target->OrbitTrapActivated());
    _smoothRender->SetValue(_target->SmoothRenderActivated());
    _smoothRender->Enable(_target->HasSmoothRenderMode());

    _algorithmChoice->Clear();
    _escapeTimeIndex = -1;
    _gaussIntIndex = -1;
    _buddhabrotIndex = -1;
    _escapeAngleIndex = -1;
    this->SetAlgorithmChoices();

    switch (_target->GetCurrentAlg())
    {
        case RenderingAlgorithmType::EscapeTime:
            _algorithmChoice->SetSelection(_escapeTimeIndex);
            break;
        case RenderingAlgorithmType::GaussianInt:
            _algorithmChoice->SetSelection(_gaussIntIndex);
            break;
        case RenderingAlgorithmType::EscapeAngle:
            _algorithmChoice->SetSelection(_escapeAngleIndex);
            break;
        case RenderingAlgorithmType::ConvergenceTest:
            _algorithmChoice->SetSelection(_convergenceTestIndex);
            break;
        case RenderingAlgorithmType::Buddhabrot:
            _algorithmChoice->SetSelection(_buddhabrotIndex);
            break;
        default:
            _algorithmChoice->SetSelection(0);
            break;
    }

    _typeNotebook->ChangeSelection(0);
    _relativeCheck->SetValue(_target->GetRelativeColorMode());
    _colorSet->SetValue(_target->GetInteriorColorMode());
    _colorFractal->SetValue(_target->GetExteriorColorMode());

    // Color of the set.
    _redSetText->SetLabel(wxString(wxT("Red: ")) + wxT("0"));
    _greenSetText->SetLabel(wxString(wxT("Green: ")) + wxT("0"));
    _blueSetText->SetLabel(wxString(wxT("Blue: ")) + wxT("0"));
    _redSetSld->SetValue(0);
    _greenSetSld->SetValue(0);
    _blueSetSld->SetValue(0);
    _colorVarSlider->SetValue(0);

    _gradStylesChoice->SetSelection(_target->GetColorPalette());
}
void RendererOptions::OnOk(wxCommandEvent&)
{
    this->Close(true);
}
void RendererOptions::GradientColorChangeSelection(wxCommandEvent&)
{
    // Changes the gradStyle.
    _gradFractalColor.SetStyle(static_cast<ColorPaletteTypes>(_gradStylesChoice->GetCurrentSelection()));
    wxGradient myGrad;
    myGrad.SetMin(0);
    const int paletteSize = GetPaletteSizeForAlgorithm(_target->GetCurrentAlg(), _gradFractalColor.paletteSize);
    myGrad.SetMax(paletteSize);
    myGrad.FromString(_gradFractalColor.grad);
    _presenter->SetColorPalette(static_cast<ColorPaletteTypes>(_gradStylesChoice->GetCurrentSelection()));
    _presenter->SetGradient(myGrad);
    _gradPalSize->SetValue(paletteSize);
    _colorVarSlider->SetRange(0, paletteSize);
    _gradientMap->SetBitmap(PaintGradient());
    _gradientMap->SetWindowStyle(wxSIMPLE_BORDER);
    _gradientMap->Refresh();
    NotifyOptionsChanged();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void RendererOptions::OnChangeAlgorithm(wxCommandEvent&)
{
    // Adjust frame parameters when an algorithm is chosen.
    const int selection = _algorithmChoice->GetSelection();
    if (selection == _escapeTimeIndex)
    {
        _orbitTrap->Enable(_target->HasOrbitTrapMode());
        _smoothRender->Enable(_target->HasSmoothRenderMode());
        _presenter->SetAlgorithm(RenderingAlgorithmType::EscapeTime);
    }
    else if (selection == _convergenceTestIndex)
    {
        _orbitTrap->Enable(_target->HasOrbitTrapMode());
    }
    else
    {
        _orbitTrap->Enable(false);
        _smoothRender->Enable(false);
        _orbitTrap->SetValue(false);
        _smoothRender->SetValue(false);
        _presenter->SetOrbitTrapMode(false);
        _presenter->SetSmoothRender(false);
    }

    // Map the selection index directly to the algorithm type from the available list
    const auto& availableAlgorithms = _target->GetAvailableAlg();
    if (selection >= 0 && static_cast<size_t>(selection) < availableAlgorithms.size())
    {
        const auto algorithm = availableAlgorithms[selection];
        _presenter->SetAlgorithm(algorithm);
        if (algorithm == RenderingAlgorithmType::Buddhabrot)
            ApplyPaletteSize(BuddhabrotPaletteSize);
        else
            NotifyOptionsChanged();
    }
}

// Option to change methods.
// ReSharper disable once CppMemberFunctionMayBeConst
void RendererOptions::OnRelativeColor(wxCommandEvent&)
{
    const bool mode = _relativeCheck->IsChecked();
    _presenter->SetRelativeColor(mode);
    _relativeCheck->SetValue(mode);
    NotifyOptionsChanged();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void RendererOptions::OnColorFractal(wxCommandEvent&)
{
    const bool mode = _colorFractal->IsChecked();
    _presenter->SetExteriorColorMode(mode);
    _colorFractal->SetValue(mode);
    NotifyOptionsChanged();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void RendererOptions::OnColorSet(wxCommandEvent&)
{
    const bool mode = _colorSet->IsChecked();
    _presenter->SetFractalSetColorMode(mode);
    _colorSet->SetValue(mode);
    NotifyOptionsChanged();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void RendererOptions::OnOrbitTrap(wxCommandEvent&)
{
    const bool mode = _orbitTrap->IsChecked();
    _presenter->SetOrbitTrapMode(mode);
    _orbitTrap->SetValue(mode);
    _presenter->Redraw();
    NotifyOptionsChanged();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void RendererOptions::OnSmoothRender(wxCommandEvent&)
{
    const bool mode = _smoothRender->IsChecked();
    _presenter->SetSmoothRender(mode);
    _smoothRender->SetValue(mode);
    _presenter->Redraw();
    NotifyOptionsChanged();
}
void RendererOptions::OnSetRed(wxScrollEvent&)
{
    const int value = _redSetSld->GetValue();
    _setColor.r = value;
    _presenter->SetFractalSetColor(_setColor);
    wxString text = L"Red: ";
    text += TextUtils::ToWxString(value);
    _redSetText->SetLabel(wxString(text));
    NotifyOptionsChanged();
}
void RendererOptions::OnSetGreen(wxScrollEvent&)
{
    int value = _greenSetSld->GetValue();
    _setColor.g = value;
    _presenter->SetFractalSetColor(_setColor);
    wxString text = L"Green: ";
    text += TextUtils::ToWxString(value);
    _greenSetText->SetLabel(wxString(text));
    NotifyOptionsChanged();
}
void RendererOptions::OnSetBlue(wxScrollEvent&)
{
    int value = _blueSetSld->GetValue();
    _setColor.b = value;
    _presenter->SetFractalSetColor(_setColor);
    wxString text = L"Blue: ";
    text += TextUtils::ToWxString(value);
    _blueSetText->SetLabel(wxString(text));
    NotifyOptionsChanged();
}
void RendererOptions::OnClose(wxCloseEvent&)
{
    wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_RENDERER_OPTIONS_CLOSED));
    this->Destroy();
}
void RendererOptions::OnGrad(wxCommandEvent&)
{
    wxGradientDialog diag(this, *_target->GetGradient());
    diag.ShowModal();
    _presenter->SetGradient(diag.GetGradient());
    _gradientMap->SetBitmap(PaintGradient());
    _gradientMap->SetWindowStyle(wxSIMPLE_BORDER);
    _gradientMap->Refresh();
    _gradStylesChoice->SetSelection(CustomGradient);
    NotifyOptionsChanged();
}
wxBitmap RendererOptions::PaintGradient() const
{
    wxBufferedDC dc;
    wxGradient m_gradient = *_target->GetGradient();
    m_gradient.SetMax(300);

    const auto gradientSize = static_cast<int>(m_gradient.GetMax() - m_gradient.GetMin());
    const auto gradientBmp = new wxBitmap(gradientSize, 75);
    dc.SelectObject(*gradientBmp);
    for (unsigned int i = m_gradient.GetMin(); i<m_gradient.GetMax(); i++)
    {
        dc.SetPen(wxPen(m_gradient.GetColorAt(i), 1));
        const auto gradientPosition = static_cast<int>(i);
        dc.DrawLine(gradientPosition, 0, gradientPosition, 75);
    }
    dc.SelectObject(wxNullBitmap);
    return *gradientBmp;
}
// ReSharper disable once CppMemberFunctionMayBeConst
void RendererOptions::OnGradPaletteSize(wxSpinEvent&)
{
    const int size = _gradPalSize->GetValue();
    if (size > 0)
        _presenter->SetGradientSize(size);

    _gradientMap->SetBitmap(this->PaintGradient());
    _colorVarSlider->SetRange(0,size);
    NotifyOptionsChanged();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void RendererOptions::OnColorVar(wxScrollEvent&)
{
    _presenter->SetVarGradient(_colorVarSlider->GetValue());
    NotifyOptionsChanged();
}
