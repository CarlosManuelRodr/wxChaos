#include <wx/wx.h>
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
    case ColorPalettes::Retro:
        {
            grad = wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);");
            paletteSize = 300;
        }
        break;
    case ColorPalettes::Hakim:
        {
            grad = wxT("rgb(255,255,255);rgb(91,91,91);rgb(0,0,0);rgb(125,199,44);rgb(228,213,12);rgb(192,5,5);rgb(61,43,94);rgb(255,255,255);");
            paletteSize = 300;
        }
        break;
    case ColorPalettes::Aguamarina:
        {
            grad = wxT("rgb(30,102,162);rgb(255,255,255);rgb(91,18,122);rgb(0,0,0);rgb(16,33,112);rgb(30,145,87);rgb(30,102,162);");
            paletteSize = 300;
        }
        break;
    case ColorPalettes::PastelDream:
        {
            grad = wxT("rgb(255,209,220);rgb(255,245,186);rgb(207,244,252);rgb(210,230,255);rgb(230,214,255);rgb(255,209,220);");
            paletteSize = 320;
        }
        break;
    case ColorPalettes::RoseGold:
        {
            grad = wxT("rgb(47,36,36);rgb(122,82,72);rgb(224,170,146);rgb(255,230,214);rgb(201,138,111);rgb(77,52,50);rgb(47,36,36);");
            paletteSize = 300;
        }
        break;
    case ColorPalettes::Gunmetal:
        {
            grad = wxT("rgb(8,12,18);rgb(32,45,58);rgb(82,95,110);rgb(201,208,214);rgb(94,106,119);rgb(24,31,40);rgb(8,12,18);");
            paletteSize = 280;
        }
        break;
    case ColorPalettes::SunsetDrive:
        {
            grad = wxT("rgb(34,10,66);rgb(92,29,115);rgb(180,52,108);rgb(255,126,95);rgb(255,210,120);rgb(89,166,255);rgb(34,10,66);");
            paletteSize = 360;
        }
        break;
    case ColorPalettes::AuroraBorealis:
        {
            grad = wxT("rgb(2,24,43);rgb(0,78,92);rgb(0,168,150);rgb(126,255,214);rgb(96,139,255);rgb(48,64,173);rgb(2,24,43);");
            paletteSize = 340;
        }
        break;
    case ColorPalettes::Vaporwave:
        {
            grad = wxT("rgb(22,15,70);rgb(59,32,145);rgb(255,71,181);rgb(255,183,77);rgb(91,240,255);rgb(255,255,255);rgb(22,15,70);");
            paletteSize = 360;
        }
        break;
    case ColorPalettes::DeepOcean:
        {
            grad = wxT("rgb(0,7,20);rgb(0,32,63);rgb(0,91,150);rgb(72,202,228);rgb(173,232,244);rgb(0,91,150);rgb(0,7,20);");
            paletteSize = 320;
        }
        break;
    case ColorPalettes::Ember:
        {
            grad = wxT("rgb(18,6,6);rgb(82,17,0);rgb(173,44,0);rgb(255,120,24);rgb(255,210,94);rgb(255,248,212);rgb(18,6,6);");
            paletteSize = 320;
        }
        break;
    case ColorPalettes::RainbowFire:
        {
            grad = wxT("rgb(255,0,102);rgb(255,94,0);rgb(255,217,0);rgb(0,214,143);rgb(0,153,255);rgb(123,63,228);rgb(255,0,102);");
            paletteSize = 420;
        }
        break;
    case ColorPalettes::CustomGradient:
        break;
    };
}

ColorFrame::ColorFrame(bool* _active, Fractal* _target, wxWindow* parent,
    wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long windowStyle)
    : wxFrame(parent, id, title, pos, size, windowStyle)
{
    // Constructs the ColorFrame. Gets color values from the target fractal so the frame parameters match the fractal parameters.
    wxIcon icon(GetWxAbsPath({ "Resources", "icon.ico" }), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxString text;
    active = _active;
    target = _target;

    this->SetSizeHints(wxSize(760, 700), wxDefaultSize);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    mPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    mPanel->SetScrollRate(5, 5);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* setSizer = new wxBoxSizer(wxVERTICAL);

    colorOptBitmap = new wxStaticBitmap(mPanel, wxID_ANY, wxBitmap(GetWxAbsPath({ "Resources","color_opt.png" }), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
    setSizer->Add(colorOptBitmap, 0, wxALL, 0);

    algorithmText = new wxStaticText(mPanel, wxID_ANY, wxT("Color algorithm"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color algorithm"
    algorithmText->Wrap(-1);
    setSizer->Add(algorithmText, 0, wxALL, 5);

    // AlgorithmChoice.
    escapeTimeIndex = -1;
    gaussIntIndex = -1;
    buddhabrotIndex = -1;
    escapeAngleIndex = -1;
    triangleIneqIndex = -1;
    chaoticMapIndex = -1;
    lyapunovIndex = -1;
    convergenceTestIndex = -1;
    algorithmChoice = new wxChoice(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    setSizer->Add(algorithmChoice, 0, wxALL|wxEXPAND, 5);

    optionsText = new wxStaticText(mPanel, wxID_ANY, wxT("Options"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Options"
    optionsText->Wrap(-1);
    setSizer->Add(optionsText, 0, wxALL, 5);

    //
    relativeCheck = new wxCheckBox(mPanel, wxID_ANY, wxT(" Relative colors"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Relative colors"
    setSizer->Add(relativeCheck, 0, wxALL, 5);
    if(target->GetRelativeColorMode())
        relativeCheck->SetValue(true);
    else
        relativeCheck->SetValue(false);
    //

    colorFractal = new wxCheckBox(mPanel, wxID_ANY, wxT(" Fractal color (external color)"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Color fractal (external color)"
    //
    if(target->GetExteriorColorMode())
        colorFractal->SetValue(true);
    else
        colorFractal->SetValue(false);
    //
    setSizer->Add(colorFractal, 0, wxALL, 5);

    colorSet = new wxCheckBox(mPanel, wxID_ANY, wxT(" Set color (internal color)"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Color set (internal color)"
    //
    if(target->GetInteriorColorMode())
        colorSet->SetValue(true);
    else
        colorSet->SetValue(false);
    //
    setSizer->Add(colorSet, 0, wxALL, 5);

    orbitTrap = new wxCheckBox(mPanel, wxID_ANY, wxT(" Orbit traps"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Orbit traps"
    //
    if(target->HasOrbitTrapMode())
        orbitTrap->Enable(true);
    else 
        orbitTrap->Enable(false);

    if(target->OrbitTrapActivated())
        orbitTrap->SetValue(true);
    else
        orbitTrap->SetValue(false);
    //

    setSizer->Add(orbitTrap, 0, wxALL, 5);

    smoothRender = new wxCheckBox(mPanel, wxID_ANY, wxT(" Smooth render"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Smooth render"
    //
    if(target->HasSmoothRenderMode())
        smoothRender->Enable(true);
    else
        smoothRender->Enable(false);

    if(target->SmoothRenderActivated())
        smoothRender->SetValue(true);
    else
        smoothRender->SetValue(false);
    //
    setSizer->Add(smoothRender, 0, wxALL, 5);

    colorVarText = new wxStaticText(mPanel, wxID_ANY, wxT("Color variation"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color variation"
    colorVarText->Wrap(-1);
    setSizer->Add(colorVarText, 0, wxALL, 5);

    colorVarSlider = new wxSlider(mPanel, wxID_ANY, 0, 0, 300, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    setSizer->Add(colorVarSlider, 0, wxALL|wxEXPAND, 5);

    wxStaticBoxSizer* colorSetSizer = new wxStaticBoxSizer(new wxStaticBox(mPanel, wxID_ANY, wxT("Set color")), wxVERTICAL);    // Txt: "Set color"

    text = wxT("Red: ");
    setColor = target->GetSetColor();
    text += num_to_string(setColor.r);
    redSetText = new wxStaticText(mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    redSetText->Wrap(-1);
    colorSetSizer->Add(redSetText, 0, wxALL, 5);

    redSetSld = new wxSlider(mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(redSetSld, 0, wxALL|wxEXPAND, 5);

    text = wxT("Green: ");
    text += num_to_string(setColor.g);
    greenSetText = new wxStaticText(mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    greenSetText->Wrap(-1);
    colorSetSizer->Add(greenSetText, 0, wxALL, 5);

    greenSetSld = new wxSlider(mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(greenSetSld, 0, wxALL|wxEXPAND, 5);

    text = wxT("Blue: ");
    text += num_to_string(setColor.b);
    blueSetText = new wxStaticText(mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    blueSetText->Wrap(-1);
    colorSetSizer->Add(blueSetText, 0, wxALL, 5);

    blueSetSld = new wxSlider(mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(blueSetSld, 0, wxALL|wxEXPAND, 5);

    setSizer->Add(colorSetSizer, 1, wxEXPAND, 5);

    okButton = new wxButton(mPanel, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Ok"
    setSizer->Add(okButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    mainSizer->Add(setSizer, 1, wxEXPAND, 5);

    typeNotebook = new wxNotebook(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    gradientLabel = new wxPanel(typeNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* gradSizer = new wxBoxSizer(wxVERTICAL);

    gradStylesLabel = new wxStaticText(gradientLabel, wxID_ANY, wxT("Color styles:"), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color styles:"
    gradStylesLabel->Wrap(-1);
    gradSizer->Add(gradStylesLabel, 0, wxALL, 5);

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
    gradStylesChoice = new wxChoice(gradientLabel, wxID_ANY, wxDefaultPosition, wxDefaultSize, gradStyleChoiceNChoices, gradStyleChoiceChoices, 0);
    gradStylesChoice->SetSelection(static_cast<int>(target->GetColorPalette()));
    gradSizer->Add(gradStylesChoice, 0, wxALL, 5);

    gradientMap = new wxStaticBitmap(gradientLabel, wxID_ANY, PaintGradient(), wxDefaultPosition, wxDefaultSize, 0);
    gradSizer->Add(gradientMap, 0, wxALL, 5);

    gradButton = new wxButton(gradientLabel, wxID_ANY, wxT("Change gradient"), wxDefaultPosition, wxDefaultSize, 0);
    gradSizer->Add(gradButton, 0, wxALL, 5);

    gradPalText = new wxStaticText(gradientLabel, wxID_ANY, wxT("Palette size:"), wxDefaultPosition, wxDefaultSize, 0);
    gradPalText->Wrap(-1);
    gradSizer->Add(gradPalText, 0, wxALL, 5);

    gradPalSize = new wxSpinCtrl(gradientLabel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20000, 300);
    gradSizer->Add(gradPalSize, 0, wxALL, 5);
    gradPalSize->SetValue(target->GetGradient()->getMax() - target->GetGradient()->getMin());

    gradientLabel->SetSizer(gradSizer);
    gradientLabel->Layout();
    gradSizer->Fit(gradientLabel);
    typeNotebook->AddPage(gradientLabel, wxT("Gradient color"), true);

    mainSizer->Add(typeNotebook, 2, wxEXPAND | wxALL, 5);

    mPanel->SetSizer(mainSizer);
    mPanel->Layout();
    mainSizer->Fit(mPanel);
    sizer->Add(mPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->Layout();
    this->Centre(wxBOTH);

    typeNotebook->ChangeSelection(0);

    this->SetAlgorithmChoices();
    this->ConnectEvents();
}
ColorFrame::~ColorFrame()
{
    *active = false;    // Warns the mainframe that this frame has been closed.
}

void ColorFrame::ConnectEvents()
{
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ColorFrame::OnClose));
    gradStylesChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(ColorFrame::GradientColorChangeSelection), nullptr, this);
    algorithmChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(ColorFrame::OnChangeAlgorithm), nullptr, this);
    relativeCheck->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnRelativeColor), nullptr, this);
    gradPalSize->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ColorFrame::OnGradPaletteSize), nullptr, this);
    colorFractal->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnColorFractal), nullptr, this);
    colorSet->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnColorSet), nullptr, this);
    orbitTrap->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnOrbitTrap), nullptr, this);
    smoothRender->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColorFrame::OnSmoothRender), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    redSetSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::OnSetRed), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    greenSetSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::OnSetGreen), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    blueSetSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::OnSetBlue), nullptr, this);
    okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColorFrame::OnOk), nullptr, this);
    gradButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColorFrame::OnGrad), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
    colorVarSlider->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::OnColorVar), nullptr, this);
}
void ColorFrame::SetAlgorithmChoices()
{
    // Construct the alogrithmChoice according to the algorithms available in the fractal.
    for(unsigned int i=0; i<target->GetAvailableAlg().size(); i++)
    {
        switch(target->GetAvailableAlg()[i])
        {
        case RenderingAlgorithm::EscapeTime:
            {
                algorithmChoice->Append(wxT("Escape time"));    // Txt: "Escape time"
                escapeTimeIndex = i;
            }
            break;
        case RenderingAlgorithm::GaussianInt:
            {
                algorithmChoice->Append(wxT("Gaussian integer"));    // Txt: "Gaussian integer"
                gaussIntIndex = i;
            }
            break;
        case RenderingAlgorithm::EscapeAngle:
            {
                algorithmChoice->Append(wxT("Escape angle"));    // Txt: "Escape angle"
                escapeAngleIndex = i;
            }
            break;
        case RenderingAlgorithm::TriangleInequality:
            {
                algorithmChoice->Append(wxT("Triangle inequality"));    // Txt: "Triangle inequality"
                triangleIneqIndex = i;
            }
            break;
        case RenderingAlgorithm::ChaoticMap:
            {
                algorithmChoice->Append(wxT("Chaotic map"));    // Txt: "Chaotic map"
                chaoticMapIndex = i;
            }
            break;
        case RenderingAlgorithm::Lyapunov:
            {
                algorithmChoice->Append(wxT("Lyapunov"));    // Txt: "Lyapunov"
                lyapunovIndex = i;
            }
            break;
        case RenderingAlgorithm::ConvergenceTest:
            {
                algorithmChoice->Append(wxT("Convergence test"));    // Txt: "Convergence test"
                convergenceTestIndex = i;
            }
        case RenderingAlgorithm::Other:
            break;
        };

        if(target->GetCurrentAlg() == target->GetAvailableAlg()[i])
            algorithmChoice->SetSelection( i );
    }
    if(algorithmChoice->GetCount() == 0)
    {
        algorithmChoice->Append(wxT("Special algorithm"));    // Txt: "Special algorithm"
        algorithmChoice->SetSelection(0);
    }

    // Adjust frame parameters when an algorithm is chosen.
    int selection = algorithmChoice->GetSelection();
    if(selection == escapeTimeIndex)
    {
        if(target->HasOrbitTrapMode())
            orbitTrap->Enable(true);

        if(target->HasSmoothRenderMode())
            smoothRender->Enable(true);
        else
            smoothRender->Enable(false);
    }
    else if(selection == convergenceTestIndex)
    {
        if(target->HasOrbitTrapMode())
            orbitTrap->Enable(true);
    }
    else
    {
        orbitTrap->Enable(false);
        smoothRender->Enable(false);
        orbitTrap->SetValue(false);
        smoothRender->SetValue(false);
        target->SetOrbitTrapMode(false);
        target->SetSmoothRender(false);
    }
}
void ColorFrame::SetTarget(Fractal* _target)
{
    // Sets the new target fractal.
    target = _target;
    gradPalSize->SetValue(target->GetPaletteSize());

    if(target->HasOrbitTrapMode())
        orbitTrap->Enable(true);
    else
        orbitTrap->Enable(false);
    if(target->OrbitTrapActivated())
        orbitTrap->SetValue(true);
    else
        orbitTrap->SetValue(false);

    if(target->SmoothRenderActivated())
        smoothRender->SetValue(true);
    else
        smoothRender->SetValue(false);
    if(target->HasSmoothRenderMode())
        smoothRender->Enable(true);
    else
        smoothRender->Enable(false);

    algorithmChoice->Clear();
    escapeTimeIndex = -1;
    gaussIntIndex = -1;
    buddhabrotIndex = -1;
    escapeAngleIndex = -1;
    this->SetAlgorithmChoices();

    if(target->GetCurrentAlg() == RenderingAlgorithm::EscapeTime)
        algorithmChoice->SetSelection(escapeTimeIndex);
    else if(target->GetCurrentAlg() == RenderingAlgorithm::GaussianInt)
        algorithmChoice->SetSelection(gaussIntIndex);
    else if(target->GetCurrentAlg() == RenderingAlgorithm::EscapeAngle)
        algorithmChoice->SetSelection(escapeAngleIndex);
    else if(target->GetCurrentAlg() == RenderingAlgorithm::ConvergenceTest)
        algorithmChoice->SetSelection(convergenceTestIndex);
    else
        algorithmChoice->SetSelection(0);

    typeNotebook->ChangeSelection(0);

    if(target->GetRelativeColorMode())
        relativeCheck->SetValue(true);
    else
        relativeCheck->SetValue(false);

    if(target->GetInteriorColorMode())
        colorSet->SetValue(true);
    else
        colorSet->SetValue(false);

    if(target->GetExteriorColorMode())
        colorFractal->SetValue(true);
    else
        colorFractal->SetValue(false);

    // Color of the set.
    redSetText->SetLabel(wxString(wxT("Red: ")) + wxT("0"));
    greenSetText->SetLabel(wxString(wxT("Green: ")) + wxT("0"));
    blueSetText->SetLabel(wxString(wxT("Blue: ")) + wxT("0"));
    redSetSld->SetValue(0);
    greenSetSld->SetValue(0);
    blueSetSld->SetValue(0);
    colorVarSlider->SetValue(0);

    gradStylesChoice->SetSelection( static_cast<int>(target->GetColorPalette()) );
}
void ColorFrame::OnOk(wxCommandEvent& event)
{
    this->Destroy();
}
void ColorFrame::GradientColorChangeSelection( wxCommandEvent& event )
{
    // Changes the gradStyle.
    gradFractalColor.SetStyle(static_cast<ColorPalettes>(gradStylesChoice->GetCurrentSelection()));
    wxGradient myGrad;
    myGrad.setMin(0);
    myGrad.setMax(gradFractalColor.paletteSize);
    myGrad.fromString(gradFractalColor.grad);
    target->SetColorPalette(static_cast<ColorPalettes>(gradStylesChoice->GetCurrentSelection()));
    target->SetGradient(myGrad);
    gradPalSize->SetValue(gradFractalColor.paletteSize);
    colorVarSlider->SetRange(0, gradFractalColor.paletteSize);
    gradientMap->SetBitmap(PaintGradient());
    gradientMap->SetWindowStyle(wxSIMPLE_BORDER);
    gradientMap->Refresh();
}
void ColorFrame::OnChangeAlgorithm( wxCommandEvent& event )
{
    // Adjust frame parameters when an algorithm is chosen.
    int selection = algorithmChoice->GetSelection();
    if(selection == escapeTimeIndex)
    {
        if(target->HasOrbitTrapMode())
            orbitTrap->Enable(true);

        if(target->HasSmoothRenderMode())
            smoothRender->Enable(true);
        else
            smoothRender->Enable(false);
        target->SetAlgorithm(RenderingAlgorithm::EscapeTime);
    }
    else if(selection == convergenceTestIndex)
    {
        if(target->HasOrbitTrapMode())
            orbitTrap->Enable(true);
    }
    else
    {
        orbitTrap->Enable(false);
        smoothRender->Enable(false);
        orbitTrap->SetValue(false);
        smoothRender->SetValue(false);
        target->SetOrbitTrapMode(false);
        target->SetSmoothRender(false);
    }

    if(selection == gaussIntIndex)
        target->SetAlgorithm(RenderingAlgorithm::GaussianInt);
    else if(selection == escapeAngleIndex)
        target->SetAlgorithm(RenderingAlgorithm::EscapeAngle);
    else if(selection == triangleIneqIndex)
        target->SetAlgorithm(RenderingAlgorithm::TriangleInequality);
    else if(selection == chaoticMapIndex)
        target->SetAlgorithm(RenderingAlgorithm::ChaoticMap);
    else if(selection == lyapunovIndex)
        target->SetAlgorithm(RenderingAlgorithm::Lyapunov);
    else if(selection == convergenceTestIndex)
        target->SetAlgorithm(RenderingAlgorithm::ConvergenceTest);
}

// Option change methods.
void ColorFrame::OnRelativeColor( wxCommandEvent& event )
{
    bool modo = relativeCheck->IsChecked();
    target->SetRelativeColor(modo);
    relativeCheck->SetValue(modo);
}
void ColorFrame::OnColorFractal(wxCommandEvent& event)
{
    bool modo = colorFractal->IsChecked();
    target->SetExtColorMode(modo);
    colorFractal->SetValue(modo);
}
void ColorFrame::OnColorSet(wxCommandEvent& event)
{
    bool modo = colorSet->IsChecked();
    target->SetFractalSetColorMode(modo);
    colorSet->SetValue(modo);
}
void ColorFrame::OnOrbitTrap(wxCommandEvent& event)
{
    bool modo = orbitTrap->IsChecked();
    target->SetOrbitTrapMode(modo);
    orbitTrap->SetValue(modo);
    target->Redraw();
}
void ColorFrame::OnSmoothRender(wxCommandEvent& event)
{
    bool modo = smoothRender->IsChecked();
    target->SetSmoothRender(modo);
    smoothRender->SetValue(modo);
    target->Redraw();
}
void ColorFrame::OnSetRed(wxScrollEvent& event)
{
    int value = redSetSld->GetValue();
    setColor.r = value;
    target->SetFractalSetColor(setColor);
    wxString text;
    text = wxT("Red: ");
    text += num_to_string(value);
    redSetText->SetLabel(wxString(text));
}
void ColorFrame::OnSetGreen(wxScrollEvent& event)
{
    int value = greenSetSld->GetValue();
    setColor.g = value;
    target->SetFractalSetColor(setColor);
    wxString text;
    text = wxT("Green: ");
    text += num_to_string(value);
    greenSetText->SetLabel(wxString(text));
}
void ColorFrame::OnSetBlue(wxScrollEvent& event)
{
    int value = blueSetSld->GetValue();
    setColor.b = value;
    target->SetFractalSetColor(setColor);
    wxString text;
    text = wxT("Blue: ");
    text += num_to_string(value);
    blueSetText->SetLabel(wxString(text));
}
void ColorFrame::OnClose(wxCloseEvent& event)
{
    *active = false;
    this->Show(false);
    this->Destroy();
}
void ColorFrame::OnGrad(wxCommandEvent& event)
{
    wxGradientDialog diag(this, *target->GetGradient());
    diag.ShowModal();
    target->SetGradient(diag.GetGradient());
    gradientMap->SetBitmap(PaintGradient());
    gradientMap->SetWindowStyle(wxSIMPLE_BORDER);
    gradientMap->Refresh();
    gradStylesChoice->SetSelection(static_cast<int>(CustomGradient));
}
wxBitmap ColorFrame::PaintGradient()
{
    wxBufferedDC dc;
    wxGradient m_gradient = *target->GetGradient();
    m_gradient.setMax(300);
    wxBitmap* gradientBmp = new wxBitmap(m_gradient.getMax()-m_gradient.getMin(), 75);
    dc.SelectObject(*gradientBmp);
    for(int i = m_gradient.getMin(); i<m_gradient.getMax(); i++)
    {
        dc.SetPen(wxPen(m_gradient.getColorAt(i), 1));
        dc.DrawLine(i, 0, i, 75);
    }
    dc.SelectObject(wxNullBitmap);
    return *gradientBmp;
}
void ColorFrame::OnGradPaletteSize(wxSpinEvent& event)
{
    int size = gradPalSize->GetValue();
    if(size > 0)
        target->SetGradientSize(size);

    gradientMap->SetBitmap(this->PaintGradient());
    colorVarSlider->SetRange(0,size);
}
void ColorFrame::OnColorVar(wxScrollEvent& event)
{
    target->SetVarGradient(colorVarSlider->GetValue());
}
