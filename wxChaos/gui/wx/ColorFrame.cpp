#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "ColorFrame.h"
#include "StringFuncs.h"
#include "Filesystem.h"

ColorPalette::ColorPalette()
{
    paletteSize = -1;
}
void ColorPalette::SetStyle(ColorPalettes palette)
{
    // Sets the wxString parameter of each color style.
    switch(palette)
    {
    case Retro:
        {
            grad = wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);");
            paletteSize = 300;
        }
        break;
    case Hakim:
        {
            grad = wxT("rgb(255,255,255);rgb(91,91,91);rgb(0,0,0);rgb(125,199,44);rgb(228,213,12);rgb(192,5,5);rgb(61,43,94);rgb(255,255,255);");
            paletteSize = 300;
        }
        break;
    case Aguamarina:
        {
            grad = wxT("rgb(30,102,162);rgb(255,255,255);rgb(91,18,122);rgb(0,0,0);rgb(16,33,112);rgb(30,145,87);rgb(30,102,162);");
            paletteSize = 300;
        }
        break;
    case PastelDream:
        {
            grad = wxT("rgb(255,209,220);rgb(255,245,186);rgb(207,244,252);rgb(210,230,255);rgb(230,214,255);rgb(255,209,220);");
            paletteSize = 320;
        }
        break;
    case RoseGold:
        {
            grad = wxT("rgb(47,36,36);rgb(122,82,72);rgb(224,170,146);rgb(255,230,214);rgb(201,138,111);rgb(77,52,50);rgb(47,36,36);");
            paletteSize = 300;
        }
        break;
    case Gunmetal:
        {
            grad = wxT("rgb(8,12,18);rgb(32,45,58);rgb(82,95,110);rgb(201,208,214);rgb(94,106,119);rgb(24,31,40);rgb(8,12,18);");
            paletteSize = 280;
        }
        break;
    case SunsetDrive:
        {
            grad = wxT("rgb(34,10,66);rgb(92,29,115);rgb(180,52,108);rgb(255,126,95);rgb(255,210,120);rgb(89,166,255);rgb(34,10,66);");
            paletteSize = 360;
        }
        break;
    case AuroraBorealis:
        {
            grad = wxT("rgb(2,24,43);rgb(0,78,92);rgb(0,168,150);rgb(126,255,214);rgb(96,139,255);rgb(48,64,173);rgb(2,24,43);");
            paletteSize = 340;
        }
        break;
    case Vaporwave:
        {
            grad = wxT("rgb(22,15,70);rgb(59,32,145);rgb(255,71,181);rgb(255,183,77);rgb(91,240,255);rgb(255,255,255);rgb(22,15,70);");
            paletteSize = 360;
        }
        break;
    case DeepOcean:
        {
            grad = wxT("rgb(0,7,20);rgb(0,32,63);rgb(0,91,150);rgb(72,202,228);rgb(173,232,244);rgb(0,91,150);rgb(0,7,20);");
            paletteSize = 320;
        }
        break;
    case Ember:
        {
            grad = wxT("rgb(18,6,6);rgb(82,17,0);rgb(173,44,0);rgb(255,120,24);rgb(255,210,94);rgb(255,248,212);rgb(18,6,6);");
            paletteSize = 320;
        }
        break;
    case RainbowFire:
        {
            grad = wxT("rgb(255,0,102);rgb(255,94,0);rgb(255,217,0);rgb(0,214,143);rgb(0,153,255);rgb(123,63,228);rgb(255,0,102);");
            paletteSize = 420;
        }
        break;
    case CustomGradient:
        break;
    };
}

ColorFrame::ColorFrame(bool* active, Fractal* target, wxWindow* parent,
    wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long windowStyle)
    : wxFrame(parent, id, title, pos, size, windowStyle)
{
    // Constructs the ColorFrame. Gets color values from the target fractal so the frame parameters match the fractal parameters.
    wxIcon icon(GetWxAbsPath({ "Resources", "icon.ico" }), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxString text;
    _active = active;
    _target = target;

    this->SetSizeHints(wxSize(760, 700), wxDefaultSize);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    _mPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    _mPanel->SetScrollRate(5, 5);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* setSizer = new wxBoxSizer(wxVERTICAL);

    _colorOptBitmap = new wxStaticBitmap(_mPanel, wxID_ANY, wxBitmap(GetWxAbsPath({ "Resources","color_opt.png" }), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
    setSizer->Add(_colorOptBitmap, 0, wxALL, 0);

    _algorithmText = new wxStaticText(_mPanel, wxID_ANY, wxT("Color algorithm"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color algorithm"
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
    _algorithmChoice = new wxChoice(_mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    setSizer->Add(_algorithmChoice, 0, wxALL|wxEXPAND, 5);

    _optionsText = new wxStaticText(_mPanel, wxID_ANY, wxT("Options"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Options"
    _optionsText->Wrap(-1);
    setSizer->Add(_optionsText, 0, wxALL, 5);

    //
    _relativeCheck = new wxCheckBox(_mPanel, wxID_ANY, wxT(" Relative colors"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Relative colors"
    setSizer->Add(_relativeCheck, 0, wxALL, 5);
    if(_target->GetRelativeColorMode())
        _relativeCheck->SetValue(true);
    else
        _relativeCheck->SetValue(false);
    //

    _colorFractal = new wxCheckBox(_mPanel, wxID_ANY, wxT(" Fractal color (external color)"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Color fractal (external color)"
    //
    if(_target->GetExteriorColorMode())
        _colorFractal->SetValue(true);
    else
        _colorFractal->SetValue(false);
    //
    setSizer->Add(_colorFractal, 0, wxALL, 5);

    _colorSet = new wxCheckBox(_mPanel, wxID_ANY, wxT(" Set color (internal color)"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Color set (internal color)"
    //
    if(_target->GetInteriorColorMode())
        _colorSet->SetValue(true);
    else
        _colorSet->SetValue(false);
    //
    setSizer->Add(_colorSet, 0, wxALL, 5);

    _orbitTrap = new wxCheckBox(_mPanel, wxID_ANY, wxT(" Orbit traps"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Orbit traps"
    //
    if(_target->HasOrbitTrapMode())
        _orbitTrap->Enable(true);
    else
        _orbitTrap->Enable(false);

    if(_target->OrbitTrapActivated())
        _orbitTrap->SetValue(true);
    else
        _orbitTrap->SetValue(false);
    //

    setSizer->Add(_orbitTrap, 0, wxALL, 5);

    _smoothRender = new wxCheckBox(_mPanel, wxID_ANY, wxT(" Smooth render"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Smooth render"
    //
    if(_target->HasSmoothRenderMode())
        _smoothRender->Enable(true);
    else
        _smoothRender->Enable(false);

    if(_target->SmoothRenderActivated())
        _smoothRender->SetValue(true);
    else
        _smoothRender->SetValue(false);
    //
    setSizer->Add(_smoothRender, 0, wxALL, 5);

    _colorVarText = new wxStaticText(_mPanel, wxID_ANY, wxT("Color variation"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color variation"
    _colorVarText->Wrap(-1);
    setSizer->Add(_colorVarText, 0, wxALL, 5);

    _colorVarSlider = new wxSlider(_mPanel, wxID_ANY, 0, 0, 300, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    setSizer->Add(_colorVarSlider, 0, wxALL|wxEXPAND, 5);

    wxStaticBoxSizer* colorSetSizer = new wxStaticBoxSizer(new wxStaticBox(_mPanel, wxID_ANY, wxT("Set color")), wxVERTICAL);    // Txt: "Set color"

    text = wxT("Red: ");
    _setColor = _target->GetSetColor();
    text += num_to_string(_setColor.r);
    _redSetText = new wxStaticText(_mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    _redSetText->Wrap(-1);
    colorSetSizer->Add(_redSetText, 0, wxALL, 5);

    _redSetSld = new wxSlider(_mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(_redSetSld, 0, wxALL|wxEXPAND, 5);

    text = wxT("Green: ");
    text += num_to_string(_setColor.g);
    _greenSetText = new wxStaticText(_mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    _greenSetText->Wrap(-1);
    colorSetSizer->Add(_greenSetText, 0, wxALL, 5);

    _greenSetSld = new wxSlider(_mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(_greenSetSld, 0, wxALL|wxEXPAND, 5);

    text = wxT("Blue: ");
    text += num_to_string(_setColor.b);
    _blueSetText = new wxStaticText(_mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    _blueSetText->Wrap(-1);
    colorSetSizer->Add(_blueSetText, 0, wxALL, 5);

    _blueSetSld = new wxSlider(_mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(_blueSetSld, 0, wxALL|wxEXPAND, 5);

    setSizer->Add(colorSetSizer, 1, wxEXPAND, 5);

    _okButton = new wxButton(_mPanel, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Ok"
    setSizer->Add(_okButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    mainSizer->Add(setSizer, 1, wxEXPAND, 5);

    _typeNotebook = new wxNotebook(_mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    _gradientLabel = new wxPanel(_typeNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* gradSizer = new wxBoxSizer(wxVERTICAL);

    _gradStylesLabel = new wxStaticText(_gradientLabel, wxID_ANY, wxT("Color styles:"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color styles:"
    _gradStylesLabel->Wrap(-1);
    gradSizer->Add(_gradStylesLabel, 0, wxALL, 5);

    wxString gradStyleChoiceChoices[] = {
        wxT("Retro"),
        wxT("Hakim"),
        wxT("Aguamarina"),
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
    int gradStyleChoiceNChoices = sizeof(gradStyleChoiceChoices) / sizeof(wxString);
    _gradStylesChoice = new wxChoice(_gradientLabel, wxID_ANY, wxDefaultPosition, wxDefaultSize, gradStyleChoiceNChoices, gradStyleChoiceChoices, 0);
    _gradStylesChoice->SetSelection(static_cast<int>(_target->GetColorPalette()));
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
    _gradPalSize->SetValue(_target->GetGradient()->getMax() - _target->GetGradient()->getMin());

    _gradientLabel->SetSizer(gradSizer);
    _gradientLabel->Layout();
    gradSizer->Fit(_gradientLabel);
    _typeNotebook->AddPage(_gradientLabel, wxT("Gradient color"), true);

    mainSizer->Add(_typeNotebook, 2, wxEXPAND | wxALL, 5);

    _mPanel->SetSizer(mainSizer);
    _mPanel->Layout();
    mainSizer->Fit(_mPanel);
    sizer->Add(_mPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    _typeNotebook->ChangeSelection(0);

    this->SetAlgorithmChoices();
    this->ConnectEvents();
}
ColorFrame::~ColorFrame()
{
    *_active = false;    // Warns the mainframe that this frame has been closed.
}

void ColorFrame::ConnectEvents()
{
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ColorFrame::OnClose));
    _gradStylesChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(ColorFrame::GradientColorChangeSelection), nullptr, this);
    _algorithmChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(ColorFrame::OnChangeAlgorithm), nullptr, this);
    _relativeCheck->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnRelativeColor), nullptr, this);
    _gradPalSize->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ColorFrame::OnGradPaletteSize), nullptr, this);
    _colorFractal->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnColorFractal), nullptr, this);
    _colorSet->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnColorSet), nullptr, this);
    _orbitTrap->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnOrbitTrap), nullptr, this);
    _smoothRender->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnSmoothRender), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _redSetSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _greenSetSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _blueSetSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    _okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColorFrame::OnOk), nullptr, this);
    _gradButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColorFrame::OnGrad), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    _colorVarSlider->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
}
void ColorFrame::SetAlgorithmChoices()
{
    // Construct the algorithm choice according to the algorithms available in the fractal.
    for(unsigned int i=0; i<_target->GetAvailableAlg().size(); i++)
    {
        switch(_target->GetAvailableAlg()[i])
        {
        case RenderingAlgorithm::EscapeTime:
            {
                _algorithmChoice->Append(wxT("Escape time"));
                _escapeTimeIndex = i;
            }
            break;
        case RenderingAlgorithm::GaussianInt:
            {
                _algorithmChoice->Append(wxT("Gaussian integer"));
                _gaussIntIndex = i;
            }
            break;
        case RenderingAlgorithm::EscapeAngle:
            {
                _algorithmChoice->Append(wxT("Escape angle"));
                _escapeAngleIndex = i;
            }
            break;
        case RenderingAlgorithm::TriangleInequality:
            {
                _algorithmChoice->Append(wxT("Triangle inequality"));
                _triangleIneqIndex = i;
            }
            break;
        case RenderingAlgorithm::ChaoticMap:
            {
                _algorithmChoice->Append(wxT("Chaotic map"));
                _chaoticMapIndex = i;
            }
            break;
        case RenderingAlgorithm::Lyapunov:
            {
                _algorithmChoice->Append(wxT("Lyapunov"));
                _lyapunovIndex = i;
            }
            break;
        case RenderingAlgorithm::ConvergenceTest:
            {
                _algorithmChoice->Append(wxT("Convergence test"));
                _convergenceTestIndex = i;
            }
        case RenderingAlgorithm::Other:
            break;
        };

        if (_target->GetCurrentAlg() == _target->GetAvailableAlg()[i])
            _algorithmChoice->SetSelection( i );
    }
    if (_algorithmChoice->GetCount() == 0)
    {
        _algorithmChoice->Append(wxT("Special algorithm"));
        _algorithmChoice->SetSelection(0);
    }

    // Adjust frame parameters when an algorithm is chosen.
    int selection = _algorithmChoice->GetSelection();
    if (selection == _escapeTimeIndex)
    {
        if (_target->HasOrbitTrapMode())
            _orbitTrap->Enable(true);

        if (_target->HasSmoothRenderMode())
            _smoothRender->Enable(true);
        else
            _smoothRender->Enable(false);
    }
    else if (selection == _convergenceTestIndex)
    {
        if (_target->HasOrbitTrapMode())
            _orbitTrap->Enable(true);
    }
    else
    {
        _orbitTrap->Enable(false);
        _smoothRender->Enable(false);
        _orbitTrap->SetValue(false);
        _smoothRender->SetValue(false);
        _target->SetOrbitTrapMode(false);
        _target->SetSmoothRender(false);
    }
}
void ColorFrame::SetTarget(Fractal* target)
{
    // Sets the new target fractal.
    _target = target;
    _gradPalSize->SetValue(_target->GetPaletteSize());

    if (_target->HasOrbitTrapMode())
        _orbitTrap->Enable(true);
    else
        _orbitTrap->Enable(false);

    if (_target->OrbitTrapActivated())
        _orbitTrap->SetValue(true);
    else
        _orbitTrap->SetValue(false);

    if (_target->SmoothRenderActivated())
        _smoothRender->SetValue(true);
    else
        _smoothRender->SetValue(false);

    if (_target->HasSmoothRenderMode())
        _smoothRender->Enable(true);
    else
        _smoothRender->Enable(false);

    _algorithmChoice->Clear();
    _escapeTimeIndex = -1;
    _gaussIntIndex = -1;
    _buddhabrotIndex = -1;
    _escapeAngleIndex = -1;
    this->SetAlgorithmChoices();

    if (_target->GetCurrentAlg() == RenderingAlgorithm::EscapeTime)
        _algorithmChoice->SetSelection(_escapeTimeIndex);
    else if (_target->GetCurrentAlg() == RenderingAlgorithm::GaussianInt)
        _algorithmChoice->SetSelection(_gaussIntIndex);
    else if (_target->GetCurrentAlg() == RenderingAlgorithm::EscapeAngle)
        _algorithmChoice->SetSelection(_escapeAngleIndex);
    else if (_target->GetCurrentAlg() == RenderingAlgorithm::ConvergenceTest)
        _algorithmChoice->SetSelection(_convergenceTestIndex);
    else
        _algorithmChoice->SetSelection(0);

    _typeNotebook->ChangeSelection(0);

    if (_target->GetRelativeColorMode())
        _relativeCheck->SetValue(true);
    else
        _relativeCheck->SetValue(false);

    if (_target->GetInteriorColorMode())
        _colorSet->SetValue(true);
    else
        _colorSet->SetValue(false);

    if (_target->GetExteriorColorMode())
        _colorFractal->SetValue(true);
    else
        _colorFractal->SetValue(false);

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
void ColorFrame::OnOk(wxCommandEvent& event)
{
    this->Destroy();
}
void ColorFrame::GradientColorChangeSelection(wxCommandEvent& event)
{
    // Changes the gradStyle.
    _gradFractalColor.SetStyle(static_cast<ColorPalettes>(_gradStylesChoice->GetCurrentSelection()));
    wxGradient myGrad;
    myGrad.setMin(0);
    myGrad.setMax(_gradFractalColor.paletteSize);
    myGrad.fromString(_gradFractalColor.grad);
    _target->SetColorPalette(static_cast<ColorPalettes>(_gradStylesChoice->GetCurrentSelection()));
    _target->SetGradient(myGrad);
    _gradPalSize->SetValue(_gradFractalColor.paletteSize);
    _colorVarSlider->SetRange(0, _gradFractalColor.paletteSize);
    _gradientMap->SetBitmap(PaintGradient());
    _gradientMap->SetWindowStyle(wxSIMPLE_BORDER);
    _gradientMap->Refresh();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ColorFrame::OnChangeAlgorithm( wxCommandEvent& event )
{
    // Adjust frame parameters when an algorithm is chosen.
    int selection = _algorithmChoice->GetSelection();
    if(selection == _escapeTimeIndex)
    {
        if (_target->HasOrbitTrapMode())
            _orbitTrap->Enable(true);

        if (_target->HasSmoothRenderMode())
            _smoothRender->Enable(true);
        else
            _smoothRender->Enable(false);

        _target->SetAlgorithm(RenderingAlgorithm::EscapeTime);
    }
    else if(selection == _convergenceTestIndex)
    {
        if (_target->HasOrbitTrapMode())
            _orbitTrap->Enable(true);
    }
    else
    {
        _orbitTrap->Enable(false);
        _smoothRender->Enable(false);
        _orbitTrap->SetValue(false);
        _smoothRender->SetValue(false);
        _target->SetOrbitTrapMode(false);
        _target->SetSmoothRender(false);
    }

    if (selection == _gaussIntIndex)
        _target->SetAlgorithm(RenderingAlgorithm::GaussianInt);
    else if (selection == _escapeAngleIndex)
        _target->SetAlgorithm(RenderingAlgorithm::EscapeAngle);
    else if (selection == _triangleIneqIndex)
        _target->SetAlgorithm(RenderingAlgorithm::TriangleInequality);
    else if (selection == _chaoticMapIndex)
        _target->SetAlgorithm(RenderingAlgorithm::ChaoticMap);
    else if (selection == _lyapunovIndex)
        _target->SetAlgorithm(RenderingAlgorithm::Lyapunov);
    else if (selection == _convergenceTestIndex)
        _target->SetAlgorithm(RenderingAlgorithm::ConvergenceTest);
}

// Option to change methods.
// ReSharper disable once CppMemberFunctionMayBeConst
void ColorFrame::OnRelativeColor(wxCommandEvent&)
{
    bool modo = _relativeCheck->IsChecked();
    _target->SetRelativeColor(modo);
    _relativeCheck->SetValue(modo);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ColorFrame::OnColorFractal(wxCommandEvent&)
{
    bool modo = _colorFractal->IsChecked();
    _target->SetExtColorMode(modo);
    _colorFractal->SetValue(modo);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ColorFrame::OnColorSet(wxCommandEvent&)
{
    bool modo = _colorSet->IsChecked();
    _target->SetFractalSetColorMode(modo);
    _colorSet->SetValue(modo);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ColorFrame::OnOrbitTrap(wxCommandEvent&)
{
    bool modo = _orbitTrap->IsChecked();
    _target->SetOrbitTrapMode(modo);
    _orbitTrap->SetValue(modo);
    _target->Redraw();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ColorFrame::OnSmoothRender(wxCommandEvent&)
{
    bool modo = _smoothRender->IsChecked();
    _target->SetSmoothRender(modo);
    _smoothRender->SetValue(modo);
    _target->Redraw();
}
void ColorFrame::OnSetRed(wxScrollEvent&)
{
    int value = _redSetSld->GetValue();
    _setColor.r = value;
    _target->SetFractalSetColor(_setColor);
    wxString text = L"Red: ";
    text += num_to_string(value);
    _redSetText->SetLabel(wxString(text));
}
void ColorFrame::OnSetGreen(wxScrollEvent&)
{
    int value = _greenSetSld->GetValue();
    _setColor.g = value;
    _target->SetFractalSetColor(_setColor);
    wxString text = L"Green: ";
    text += num_to_string(value);
    _greenSetText->SetLabel(wxString(text));
}
void ColorFrame::OnSetBlue(wxScrollEvent&)
{
    int value = _blueSetSld->GetValue();
    _setColor.b = value;
    _target->SetFractalSetColor(_setColor);
    wxString text;
    text = wxT("Blue: ");
    text += num_to_string(value);
    _blueSetText->SetLabel(wxString(text));
}
void ColorFrame::OnClose(wxCloseEvent&)
{
    *_active = false;
    this->Show(false);
    this->Destroy();
}
void ColorFrame::OnGrad(wxCommandEvent&)
{
    wxGradientDialog diag(this, *_target->GetGradient());
    diag.ShowModal();
    _target->SetGradient(diag.GetGradient());
    _gradientMap->SetBitmap(PaintGradient());
    _gradientMap->SetWindowStyle(wxSIMPLE_BORDER);
    _gradientMap->Refresh();
    _gradStylesChoice->SetSelection(CustomGradient);
}
wxBitmap ColorFrame::PaintGradient() const
{
    wxBufferedDC dc;
    wxGradient m_gradient = *_target->GetGradient();
    m_gradient.setMax(300);
    auto gradientBmp = new wxBitmap(m_gradient.getMax()-m_gradient.getMin(), 75);
    dc.SelectObject(*gradientBmp);
    for(int i = m_gradient.getMin(); i<m_gradient.getMax(); i++)
    {
        dc.SetPen(wxPen(m_gradient.getColorAt(i), 1));
        dc.DrawLine(i, 0, i, 75);
    }
    dc.SelectObject(wxNullBitmap);
    return *gradientBmp;
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ColorFrame::OnGradPaletteSize(wxSpinEvent&)
{
    const int size = _gradPalSize->GetValue();
    if(size > 0)
        _target->SetGradientSize(size);

    _gradientMap->SetBitmap(this->PaintGradient());
    _colorVarSlider->SetRange(0,size);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ColorFrame::OnColorVar(wxScrollEvent&)
{
    _target->SetVarGradient(_colorVarSlider->GetValue());
}
