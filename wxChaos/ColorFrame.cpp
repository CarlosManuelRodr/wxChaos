#include <wx/wx.h>
#include "global.h"
#include "ColorFrame.h"
#include "StringFuncs.h"
#include "Filesystem.h"


GaussianColorPalette::GaussianColorPalette()
{
    redInt = -1;
    redMean = -1;
    redStdDev = -1;
    greenInt = -1;
    greenMean = -1;
    greenStdDev = -1;
    blueInt = -1;
    blueMean = -1;
    blueStdDev = -1;
    paletteSize = -1;
}
void GaussianColorPalette::SetStyle(GaussianColorStyles style)
{
    // Sets the parameters of each color style.
    switch(style)
    {
    case GaussianColorStyles::SummerDay:
        {
            redInt = 255;
            redMean = 34;
            redStdDev = 8;
            greenInt = 201;
            greenMean = 28;
            greenStdDev = 12;
            blueInt = 255;
            blueMean = 21;
            blueStdDev = 9;
            paletteSize = 60;
            break;
        }
    case GaussianColorStyles::CoolBlue:
        {
            redInt = 252;
            redMean = 104;
            redStdDev = 37;
            greenInt = 255;
            greenMean = 96;
            greenStdDev = 34;
            blueInt = 255;
            blueMean = 73;
            blueStdDev = 40;
            paletteSize = 300;
            break;
        }
    case GaussianColorStyles::HardRed:
        {
            redInt = 210;
            redMean = 19;
            redStdDev = 14;
            greenInt = 102;
            greenMean = 26;
            greenStdDev = 10;
            blueInt = 97;
            blueMean = 29;
            blueStdDev = 10;
            paletteSize = 60;
            break;
        }
    case GaussianColorStyles::BlackAndWhite:
        {
            redInt = 200;
            redMean = 30;
            redStdDev = 9;
            greenInt = 200;
            greenMean = 30;
            greenStdDev = 9;
            blueInt = 200;
            blueMean = 30;
            blueStdDev = 9;
            paletteSize = 60;
            break;
        }
    case GaussianColorStyles::Pastel:
        {
            redInt = 205;
            redMean = 23;
            redStdDev = 14;
            greenInt = 196;
            greenMean = 26;
            greenStdDev = 11;
            blueInt = 198;
            blueMean = 38;
            blueStdDev = 9;
            paletteSize = 60;
            break;
        }
    case GaussianColorStyles::PsychExperience:
        {
            redInt = 126;
            redMean = 3;
            redStdDev = 11;
            greenInt = 200;
            greenMean = 36;
            greenStdDev = 9;
            blueInt = 200;
            blueMean = 24;
            blueStdDev = 12;
            paletteSize = 60;
            break;
        }
    case GaussianColorStyles::VividColors:
        {
            redInt = 200;
            redMean = 0;
            redStdDev = 10;
            greenInt = 200;
            greenMean = 17;
            greenStdDev = 10;
            blueInt = 200;
            blueMean = 37;
            blueStdDev = 10;
            paletteSize = 60;
            break;
        }
    case GaussianColorStyles::Custom:
            break;
    };
}

GradientColorPalette::GradientColorPalette()
{
    paletteSize = -1;
}
void GradientColorPalette::SetStyle(GradientColorStyles style)
{
    // Sets the wxString parameter of each color style.
    switch(style)
    {
    case GradientColorStyles::Retro:
        {
            grad = wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);");
            paletteSize = 300;
        }
        break;
    case GradientColorStyles::Hakim:
        {
            grad = wxT("rgb(255,255,255);rgb(91,91,91);rgb(0,0,0);rgb(125,199,44);rgb(228,213,12);rgb(192,5,5);rgb(61,43,94);rgb(255,255,255);");
            paletteSize = 300;
        }
        break;
    case GradientColorStyles::Aguamarina:
        {
            grad = wxT("rgb(30,102,162);rgb(255,255,255);rgb(91,18,122);rgb(0,0,0);rgb(16,33,112);rgb(30,145,87);rgb(30,102,162);");
            paletteSize = 300;
        }
        break;
    case GradientColorStyles::CustomGradient:
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

    this->SetSizeHints(wxSize(-1,-1), wxSize(-1,-1));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    mPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    mPanel->SetScrollRate(5, 5);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* setSizer = new wxBoxSizer(wxVERTICAL);

    colorOptBitmap = new wxStaticBitmap(mPanel, wxID_ANY, wxBitmap(GetWxAbsPath({ "Resources","color_opt.png" }), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
    setSizer->Add(colorOptBitmap, 0, wxALL, 0);

    algorithmText = new wxStaticText(mPanel, wxID_ANY, wxT(colorAlgTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color algorithm"
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

    optionsText = new wxStaticText(mPanel, wxID_ANY, wxT(colorOptTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Options"
    optionsText->Wrap(-1);
    setSizer->Add(optionsText, 0, wxALL, 5);

    //
    relativeCheck = new wxCheckBox(mPanel, wxID_ANY, wxT(relColorTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Relative colors"
    setSizer->Add(relativeCheck, 0, wxALL, 5);
    if(target->GetRelativeColorMode())
        relativeCheck->SetValue(true);
    else
        relativeCheck->SetValue(false);
    //

    colorFractal = new wxCheckBox(mPanel, wxID_ANY, wxT(extColorTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Color fractal (external color)"
    //
    if(target->GetExtColorMode())
        colorFractal->SetValue(true);
    else
        colorFractal->SetValue(false);
    //
    setSizer->Add(colorFractal, 0, wxALL, 5);

    colorSet = new wxCheckBox(mPanel, wxID_ANY, wxT(intColorTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Color set (internal color)"
    //
    if(target->GetSetColorMode())
        colorSet->SetValue(true);
    else
        colorSet->SetValue(false);
    //
    setSizer->Add(colorSet, 0, wxALL, 5);

    orbitTrap = new wxCheckBox(mPanel, wxID_ANY, wxT(orbitTrapTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Orbit traps"
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

    smoothRender = new wxCheckBox(mPanel, wxID_ANY, wxT(smoothRenTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: " Smooth render"
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

    colorVarText = new wxStaticText(mPanel, wxID_ANY, wxT(colorVarTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color variation"
    colorVarText->Wrap(-1);
    setSizer->Add(colorVarText, 0, wxALL, 5);

    colorVarSlider = new wxSlider(mPanel, wxID_ANY, 0, 0, 300, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    setSizer->Add(colorVarSlider, 0, wxALL|wxEXPAND, 5);

    wxStaticBoxSizer* colorSetSizer = new wxStaticBoxSizer(new wxStaticBox(mPanel, wxID_ANY, wxT(setColorTxt)), wxVERTICAL);    // Txt: "Set color"

    text = wxT(redColTxt);
    setColor = target->GetSetColor();
    text += num_to_string(setColor.r);
    redSetText = new wxStaticText(mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    redSetText->Wrap(-1);
    colorSetSizer->Add(redSetText, 0, wxALL, 5);

    redSetSld = new wxSlider(mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(redSetSld, 0, wxALL|wxEXPAND, 5);

    text = wxT(greenColTxt);
    text += num_to_string(setColor.g);
    greenSetText = new wxStaticText(mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    greenSetText->Wrap(-1);
    colorSetSizer->Add(greenSetText, 0, wxALL, 5);

    greenSetSld = new wxSlider(mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(greenSetSld, 0, wxALL|wxEXPAND, 5);

    text = wxT(blueColTxt);
    text += num_to_string(setColor.b);
    blueSetText = new wxStaticText(mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    blueSetText->Wrap(-1);
    colorSetSizer->Add(blueSetText, 0, wxALL, 5);

    blueSetSld = new wxSlider(mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetSizer->Add(blueSetSld, 0, wxALL|wxEXPAND, 5);

    setSizer->Add(colorSetSizer, 1, wxEXPAND, 5);

    okButton = new wxButton(mPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Ok"
    setSizer->Add(okButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    mainSizer->Add(setSizer, 1, wxEXPAND, 5);

    typeNotebook = new wxNotebook(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    gradientLabel = new wxPanel(typeNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* gradSizer = new wxBoxSizer(wxVERTICAL);

    gradStylesLabel = new wxStaticText(gradientLabel, wxID_ANY, wxT(colorStlTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Color styles:"
    gradStylesLabel->Wrap(-1);
    gradSizer->Add(gradStylesLabel, 0, wxALL, 5);

    wxString gradStyleChoiceChoices[] = { wxT("Retro"), wxT("Hakim"), wxT("Aguamarina"), wxT(customStyleTxt) };
    int gradStyleChoiceNChoices = sizeof(gradStyleChoiceChoices) / sizeof(wxString);
    gradStylesChoice = new wxChoice(gradientLabel, wxID_ANY, wxDefaultPosition, wxDefaultSize, gradStyleChoiceNChoices, gradStyleChoiceChoices, 0);
    gradStylesChoice->SetSelection(static_cast<int>(target->GetGradStyle()));
    gradSizer->Add(gradStylesChoice, 0, wxALL, 5);

    gradientMap = new wxStaticBitmap(gradientLabel, wxID_ANY, PaintGradient(), wxDefaultPosition, wxDefaultSize, 0);
    gradSizer->Add(gradientMap, 0, wxALL, 5);

    gradButton = new wxButton(gradientLabel, wxID_ANY, wxT(changeGradTxt), wxDefaultPosition, wxDefaultSize, 0);
    gradSizer->Add(gradButton, 0, wxALL, 5);

    gradPalText = new wxStaticText(gradientLabel, wxID_ANY, wxT(paletteSizeTxt), wxDefaultPosition, wxDefaultSize, 0);
    gradPalText->Wrap(-1);
    gradSizer->Add(gradPalText, 0, wxALL, 5);

    gradPalSize = new wxSpinCtrl(gradientLabel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20000, 300);
    gradSizer->Add(gradPalSize, 0, wxALL, 5);
    gradPalSize->SetValue(target->GetGradient()->getMax() - target->GetGradient()->getMin());

    gradientLabel->SetSizer(gradSizer);
    gradientLabel->Layout();
    gradSizer->Fit(gradientLabel);
    typeNotebook->AddPage(gradientLabel, wxT(useGradTxt), false);
    stdPanel = new wxPanel(typeNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* colorSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* sideASizer = new wxBoxSizer(wxVERTICAL);

    stylesLabel = new wxStaticText(stdPanel, wxID_ANY, wxT(colorStlTxt), wxDefaultPosition, wxDefaultSize, 0);
    stylesLabel->Wrap(-1);
    sideASizer->Add(stylesLabel, 0, wxALL, 5);

    wxString stylesChoices[] = { wxT(summerDayTxt), wxT(coolBlueTxt), wxT(hardRed), wxT(blackWhiteTxt), wxT(pastelTxt), wxT(psychExpTxt), wxT(vividColTxt), wxT(customStyleTxt) };
    int stylesChoicesNum = sizeof(stylesChoices) / sizeof(wxString);
    gaussianStylesChoice = new wxChoice(stdPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, stylesChoicesNum, stylesChoices, 0);
    gaussianStylesChoice->SetSelection(static_cast<int>(target->GetGaussianColorStyle()));
    sideASizer->Add( gaussianStylesChoice, 0, wxALL|wxEXPAND, 5 );

    pSizeText = new wxStaticText(stdPanel, wxID_ANY, wxT(paletteSizeTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Palette size:"
    pSizeText->Wrap(-1);
    sideASizer->Add(pSizeText, 0, wxALL, 5);

    pSizeSpin = new wxSpinCtrl(stdPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000, 60);
    sideASizer->Add(pSizeSpin, 0, wxALL, 5);

    wxStaticBoxSizer* sizerred = new wxStaticBoxSizer(new wxStaticBox( stdPanel, wxID_ANY, wxT(redTxt) ), wxVERTICAL);    // Txt: "Red"

    wxBoxSizer* subSizerred = new wxBoxSizer(wxVERTICAL);

    //
    redIntText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(intensityTxt);
    text += num_to_string(target->GetGaussianColorIntensity(Color::Red));
    redIntText->SetLabel(text);
    //
    redIntText->Wrap(-1);
    subSizerred->Add(redIntText, 0, wxALL, 5);

    redIntSld = new wxSlider(stdPanel, wxID_ANY, 200, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizerred->Add(redIntSld, 0, wxALL|wxEXPAND, 5);

    //
    redMeanText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(positionTxt);
    text += num_to_string(target->GetGaussianColorMean(Color::Red));
    redMeanText->SetLabel(text);
    //
    redMeanText->Wrap(-1);
    subSizerred->Add(redMeanText, 0, wxALL, 5);

    redMeanSld = new wxSlider(stdPanel, wxID_ANY, 0, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizerred->Add(redMeanSld, 0, wxALL|wxEXPAND, 5);

    //
    redStdDevText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(stdDevTxt);
    text += num_to_string(target->GetGaussianColorStdDev(Color::Red));
    redStdDevText->SetLabel(text);
    //
    redStdDevText->Wrap(-1);
    subSizerred->Add(redStdDevText, 0, wxALL, 5);

    redStdDevSld = new wxSlider(stdPanel, wxID_ANY, 10, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizerred->Add(redStdDevSld, 0, wxALL|wxEXPAND, 5);

    sizerred->Add(subSizerred, 1, wxEXPAND, 5);

    sideASizer->Add(sizerred, 1, wxEXPAND, 5);

    wxStaticBoxSizer* sizergreen = new wxStaticBoxSizer(new wxStaticBox(stdPanel, wxID_ANY, wxT(greenTxt)), wxVERTICAL);    // Txt: "Green"
    wxBoxSizer* subSizergreen = new wxBoxSizer(wxVERTICAL);

    //
    greenIntText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(intensityTxt);
    text += num_to_string(target->GetGaussianColorIntensity(Color::Green));
    greenIntText->SetLabel(text);
    //
    greenIntText->Wrap(-1);
    subSizergreen->Add(greenIntText, 0, wxALL, 5);

    greenIntSld = new wxSlider(stdPanel, wxID_ANY, 200, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizergreen->Add(greenIntSld, 0, wxALL|wxEXPAND, 5);

    //
    greenMeanText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(positionTxt);
    text += num_to_string(target->GetGaussianColorMean(Color::Green));
    greenMeanText->SetLabel(text);
    //
    greenMeanText->Wrap(-1);
    subSizergreen->Add(greenMeanText, 0, wxALL, 5);

    greenMeanSld = new wxSlider(stdPanel, wxID_ANY, 17, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizergreen->Add(greenMeanSld, 0, wxALL|wxEXPAND, 5);

    //
    greenStdDevText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(stdDevTxt);
    text += num_to_string(target->GetGaussianColorStdDev(Color::Green));
    greenStdDevText->SetLabel(text);
    //
    greenStdDevText->Wrap(-1);
    subSizergreen->Add(greenStdDevText, 0, wxALL, 5);

    greenStdDevSld = new wxSlider(stdPanel, wxID_ANY, 10, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizergreen->Add(greenStdDevSld, 0, wxALL|wxEXPAND, 5);
    sizergreen->Add(subSizergreen, 1, wxEXPAND, 5);
    sideASizer->Add(sizergreen, 1, wxEXPAND, 5);
    colorSizer->Add(sideASizer, 1, wxEXPAND, 5);

    wxBoxSizer* bSizer13 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizerblue = new wxStaticBoxSizer(new wxStaticBox(stdPanel, wxID_ANY, wxT(blueTxt)), wxVERTICAL);    // Txt: "Blue"
    wxBoxSizer* subSizerblue = new wxBoxSizer(wxVERTICAL);

    //
    blueIntText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(intensityTxt);
    text += num_to_string(target->GetGaussianColorIntensity(Color::Blue));
    blueIntText->SetLabel(text);
    //
    blueIntText->Wrap(-1);
    subSizerblue->Add(blueIntText, 0, wxALL, 5);

    blueIntSld = new wxSlider(stdPanel, wxID_ANY, 200, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizerblue->Add(blueIntSld, 0, wxALL|wxEXPAND, 5);

    //
    blueMeanText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(positionTxt);
    text += num_to_string(target->GetGaussianColorMean(Color::Blue));
    blueMeanText->SetLabel(text);
    //
    blueMeanText->Wrap(-1);
    subSizerblue->Add(blueMeanText, 0, wxALL, 5);

    blueMeanSld = new wxSlider(stdPanel, wxID_ANY, 37, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizerblue->Add(blueMeanSld, 0, wxALL|wxEXPAND, 5);

    //
    blueStdDevText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(stdDevTxt);
    text += num_to_string(target->GetGaussianColorStdDev(Color::Blue));
    blueStdDevText->SetLabel(text);
    //
    blueStdDevText->Wrap(-1);
    subSizerblue->Add(blueStdDevText, 0, wxALL, 5);

    blueStdDevSld = new wxSlider(stdPanel, wxID_ANY, 10, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    subSizerblue->Add(blueStdDevSld, 0, wxALL|wxEXPAND, 5);

    sizerblue->Add(subSizerblue, 1, wxEXPAND, 5);
    bSizer13->Add(sizerblue, 1, wxEXPAND, 5);
    colorSizer->Add(bSizer13, 1, wxEXPAND, 5);

    stdPanel->SetSizer(colorSizer);
    stdPanel->Layout();
    colorSizer->Fit(stdPanel);
    typeNotebook->AddPage(stdPanel, wxT(stdColorTxt), true);    // Txt: "STD Color"

    mainSizer->Add(typeNotebook, 2, wxEXPAND | wxALL, 5);

    mPanel->SetSizer(mainSizer);
    mPanel->Layout();
    mainSizer->Fit(mPanel);
    sizer->Add(mPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->Layout();
    this->Centre(wxBOTH);

    // Shows tab according to the color mode in the fractal.
    if(target->GetColorMode() == ColorMode::Gradient)
        typeNotebook->ChangeSelection(0);
    else
        typeNotebook->ChangeSelection(1);

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
    typeNotebook->Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(ColorFrame::OnPageChange), nullptr, this);
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
    gaussianStylesChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(ColorFrame::GaussianColorChangeSelection), nullptr, this);
    pSizeSpin->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(ColorFrame::OnPaletteSize), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redIntSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeRedIntensity), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redMeanSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeRedMean), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    redStdDevSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeRedStdDev), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenIntSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeGreenIntensity), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenMeanSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeGreenMean), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    greenStdDevSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeGreenStdDev), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueIntSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeBlueIntensity), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueMeanSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeBlueMean), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
    blueStdDevSld->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(ColorFrame::ChangeBlueStdDev), nullptr, this);
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
                algorithmChoice->Append(wxT(escapeTimeTxt));    // Txt: "Escape time"
                escapeTimeIndex = i;
            }
            break;
        case RenderingAlgorithm::GaussianInt:
            {
                algorithmChoice->Append(wxT(gaussianIntTxt));    // Txt: "Gaussian integer"
                gaussIntIndex = i;
            }
            break;
        case RenderingAlgorithm::Buddhabrot:
            {
                algorithmChoice->Append(wxT(buddhabrotTxt));    // Txt: "Buddhabrot"
                buddhabrotIndex = i;
            }
            break;
        case RenderingAlgorithm::EscapeAngle:
            {
                algorithmChoice->Append(wxT(escapeAngleTxt));    // Txt: "Escape angle"
                escapeAngleIndex = i;
            }
            break;
        case RenderingAlgorithm::TriangleInequality:
            {
                algorithmChoice->Append(wxT(triangIneqTxt));    // Txt: "Triangle inequality"
                triangleIneqIndex = i;
            }
            break;
        case RenderingAlgorithm::ChaoticMap:
            {
                algorithmChoice->Append(wxT(chaosMapTxt));    // Txt: "Chaotic map"
                chaoticMapIndex = i;
            }
            break;
        case RenderingAlgorithm::Lyapunov:
            {
                algorithmChoice->Append(wxT(lyapTxt));    // Txt: "Lyapunov"
                lyapunovIndex = i;
            }
            break;
        case RenderingAlgorithm::ConvergenceTest:
            {
                algorithmChoice->Append(wxT(convTestTxt));    // Txt: "Convergence test"
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
        algorithmChoice->Append(wxT(specialAlgTxt));    // Txt: "Special algorithm"
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
    pSizeSpin->SetValue(target->GetPaletteSize());

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
    else if(target->GetCurrentAlg() == RenderingAlgorithm::Buddhabrot)
        algorithmChoice->SetSelection(buddhabrotIndex);
    else if(target->GetCurrentAlg() == RenderingAlgorithm::EscapeAngle)
        algorithmChoice->SetSelection(escapeAngleIndex);
    else if(target->GetCurrentAlg() == RenderingAlgorithm::ConvergenceTest)
        algorithmChoice->SetSelection(convergenceTestIndex);
    else
        algorithmChoice->SetSelection(0);

    if(target->GetColorMode() == ColorMode::Gradient)
        typeNotebook->ChangeSelection(0);
    else
        typeNotebook->ChangeSelection(1);

    if(target->GetRelativeColorMode())
        relativeCheck->SetValue(true);
    else
        relativeCheck->SetValue(false);

    if(target->GetSetColorMode())
        colorSet->SetValue(true);
    else
        colorSet->SetValue(false);

    if(target->GetExtColorMode())
        colorFractal->SetValue(true);
    else
        colorFractal->SetValue(false);

    // Color of the set.
    redSetText->SetLabel(wxString(wxT(redColTxt)) + wxT("0"));
    greenSetText->SetLabel(wxString(wxT(greenColTxt)) + wxT("0"));
    blueSetText->SetLabel(wxString(wxT(blueColTxt)) + wxT("0"));
    redSetSld->SetValue(0);
    greenSetSld->SetValue(0);
    blueSetSld->SetValue(0);
    colorVarSlider->SetValue(0);

    gradStylesChoice->SetSelection( static_cast<int>(target->GetGradStyle()) );
    gaussianStylesChoice->SetSelection( static_cast<int>(target->GetGaussianColorStyle()) );
}

// Change in parameters methods.
void ColorFrame::ChangeRedIntensity(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(GaussianColorStyles::Custom);
    int value = redIntSld->GetValue();
    target->SetGaussianColorIntensity(value, Color::Red);
    wxString text;
    text = wxT(intensityTxt);
    text += num_to_string(value);
    redIntText->SetLabel(text);
}
void ColorFrame::ChangeRedMean(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(GaussianColorStyles::Custom);
    int value = redMeanSld->GetValue();
    target->SetGaussianColorMean(value, Color::Red);
    wxString text;
    text = wxT(positionTxt);
    text += num_to_string(value);
    redMeanText->SetLabel(text);
}
void ColorFrame::ChangeRedStdDev(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(GaussianColorStyles::Custom);
    int value = redStdDevSld->GetValue();
    target->SetGaussianColorStdDev(value, Color::Red);
    wxString text;
    text = wxT(stdDevTxt);
    text += num_to_string(value);
    redStdDevText->SetLabel(text);
}
void ColorFrame::ChangeGreenIntensity(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(GaussianColorStyles::Custom);
    int value = greenIntSld->GetValue();
    target->SetGaussianColorIntensity(value, Color::Green);
    wxString text;
    text = wxT(intensityTxt);
    text += num_to_string(value);
    greenIntText->SetLabel(text);
}
void ColorFrame::ChangeGreenMean(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(GaussianColorStyles::Custom);
    int value = greenMeanSld->GetValue();
    target->SetGaussianColorMean(value, Color::Green);
    wxString text;
    text = wxT(positionTxt);
    text += num_to_string(value);
    greenMeanText->SetLabel(text);
}
void ColorFrame::ChangeGreenStdDev(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(GaussianColorStyles::Custom);
    int value = greenStdDevSld->GetValue();
    target->SetGaussianColorStdDev(value, Color::Green);
    wxString text;
    text = wxT(stdDevTxt);
    text += num_to_string(value);
    greenStdDevText->SetLabel(text);
}
void ColorFrame::ChangeBlueIntensity(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(GaussianColorStyles::Custom);
    int value = blueIntSld->GetValue();
    target->SetGaussianColorIntensity(value, Color::Blue);
    wxString text;
    text = wxT(intensityTxt);
    text += num_to_string(value);
    blueIntText->SetLabel(text);
}
void ColorFrame::ChangeBlueMean(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(GaussianColorStyles::Custom);
    int value = blueMeanSld->GetValue();
    target->SetGaussianColorMean(value, Color::Blue);
    wxString text;
    text = wxT(positionTxt);
    text += num_to_string(value);
    blueMeanText->SetLabel(text);
}
void ColorFrame::ChangeBlueStdDev(wxScrollEvent& event)
{
    gaussianStylesChoice->SetSelection(Custom);
    int value = blueStdDevSld->GetValue();
    target->SetGaussianColorStdDev(value, Color::Blue);
    wxString text;
    text = wxT(stdDevTxt);
    text += num_to_string(value);
    blueStdDevText->SetLabel(text);
}
void ColorFrame::OnOk(wxCommandEvent& event)
{
    this->Destroy();
}
void ColorFrame::GaussianColorChangeSelection(wxCommandEvent& event)
{
    // Change colors according to selection.
    gaussianFractalColor.redInt = redIntSld->GetValue(); gaussianFractalColor.greenInt = greenIntSld->GetValue(); gaussianFractalColor.blueInt = blueIntSld->GetValue();
    gaussianFractalColor.redMean = redMeanSld->GetValue(); gaussianFractalColor.greenMean = greenMeanSld->GetValue(); gaussianFractalColor.blueMean = blueMeanSld->GetValue();
    gaussianFractalColor.redStdDev = redStdDevSld->GetValue(); gaussianFractalColor.greenStdDev = greenStdDevSld->GetValue(); gaussianFractalColor.blueStdDev = blueStdDevSld->GetValue();

    gaussianFractalColor.SetStyle(static_cast<GaussianColorStyles>(gaussianStylesChoice->GetCurrentSelection()));
    int paletteSize = gaussianFractalColor.paletteSize;
    redMeanSld->SetRange(0, paletteSize);
    greenMeanSld->SetRange(0, paletteSize);
    blueMeanSld->SetRange(0, paletteSize);
    colorVarSlider->SetRange(0, paletteSize);
    pSizeSpin->SetValue(paletteSize);
    target->SetPaletteSize(paletteSize);
    target->SetGaussianColorStyle(static_cast<GaussianColorStyles>(gaussianStylesChoice->GetCurrentSelection()));

    // Red
    // Intensity
    wxString text;
    target->SetGaussianColorIntensity(gaussianFractalColor.redInt, Color::Red);
    redIntSld->SetValue(gaussianFractalColor.redInt);
    text = wxT(intensityTxt);
    text += num_to_string(gaussianFractalColor.redInt);
    redIntText->SetLabel(text);

    // Mean
    target->SetGaussianColorMean(gaussianFractalColor.redMean, Color::Red);
    redMeanSld->SetValue(gaussianFractalColor.redMean);
    text = wxT(positionTxt);
    text += num_to_string(gaussianFractalColor.redMean);
    redMeanText->SetLabel(text);

    // Standard deviation
    target->SetGaussianColorStdDev(gaussianFractalColor.redStdDev, Color::Red);
    redStdDevSld->SetValue(gaussianFractalColor.redStdDev);
    text = wxT(stdDevTxt);
    text += num_to_string(gaussianFractalColor.redStdDev);
    redStdDevText->SetLabel(text);


    // Green
    // Intensity
    target->SetGaussianColorIntensity(gaussianFractalColor.greenInt, Color::Green);
    greenIntSld->SetValue(gaussianFractalColor.greenInt);
    text = wxT(intensityTxt);
    text += num_to_string(gaussianFractalColor.greenInt);
    greenIntText->SetLabel(text);

    // Mean
    target->SetGaussianColorMean(gaussianFractalColor.greenMean, Color::Green);
    greenMeanSld->SetValue(gaussianFractalColor.greenMean);
    text = wxT(positionTxt);
    text += num_to_string(gaussianFractalColor.greenMean);
    greenMeanText->SetLabel(text);

    // Standard deviation
    target->SetGaussianColorStdDev(gaussianFractalColor.greenStdDev, Color::Green);
    greenStdDevSld->SetValue(gaussianFractalColor.greenStdDev);
    text = wxT(stdDevTxt);
    text += num_to_string(gaussianFractalColor.greenStdDev);
    greenStdDevText->SetLabel(text);

    // Blue
    // Intensity
    target->SetGaussianColorIntensity(gaussianFractalColor.blueInt, Color::Blue);
    blueIntSld->SetValue(gaussianFractalColor.blueInt);
    text = wxT(intensityTxt);
    text += num_to_string(gaussianFractalColor.blueInt);
    blueIntText->SetLabel(text);

    // Mean
    target->SetGaussianColorMean(gaussianFractalColor.blueMean, Color::Blue);
    blueMeanSld->SetValue(gaussianFractalColor.blueMean);
    text = wxT(positionTxt);
    text += num_to_string(gaussianFractalColor.blueMean);
    blueMeanText->SetLabel(text);

    // Standard deviation
    target->SetGaussianColorStdDev(gaussianFractalColor.blueStdDev, Color::Blue);
    blueStdDevSld->SetValue(gaussianFractalColor.blueStdDev);
    text = wxT(stdDevTxt);
    text += num_to_string(gaussianFractalColor.blueStdDev);
    blueStdDevText->SetLabel(text);
}
void ColorFrame::GradientColorChangeSelection( wxCommandEvent& event )
{
    // Changes the gradStyle.
    gradFractalColor.SetStyle(static_cast<GradientColorStyles>(gradStylesChoice->GetCurrentSelection()));
    wxGradient myGrad;
    myGrad.setMin(0);
    myGrad.setMax(gradFractalColor.paletteSize);
    myGrad.fromString(gradFractalColor.grad);
    target->SetGradStyle(static_cast<GradientColorStyles>(gradStylesChoice->GetCurrentSelection()));
    target->SetGradient(myGrad);
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
    else if(selection == buddhabrotIndex)
        target->SetAlgorithm(RenderingAlgorithm::Buddhabrot);
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
    text = wxT(redColTxt);
    text += num_to_string(value);
    redSetText->SetLabel(wxString(text));
}
void ColorFrame::OnSetGreen(wxScrollEvent& event)
{
    int value = greenSetSld->GetValue();
    setColor.g = value;
    target->SetFractalSetColor(setColor);
    wxString text;
    text = wxT(greenColTxt);
    text += num_to_string(value);
    greenSetText->SetLabel(wxString(text));
}
void ColorFrame::OnSetBlue(wxScrollEvent& event)
{
    int value = blueSetSld->GetValue();
    setColor.b = value;
    target->SetFractalSetColor(setColor);
    wxString text;
    text = wxT(blueColTxt);
    text += num_to_string(value);
    blueSetText->SetLabel(wxString(text));
}
void ColorFrame::OnPaletteSize(wxSpinEvent& event)
{
    int size = pSizeSpin->GetValue();
    redMeanSld->SetRange(0, size);
    greenMeanSld->SetRange(0, size);
    blueMeanSld->SetRange(0, size);
    colorVarSlider->SetRange(0,size);
    target->SetPaletteSize(size);
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
void ColorFrame::OnPageChange(wxNotebookEvent& event)
{
    // On Linux when the wxNotebook is destroyed it sends a event.
    if(*active)
    {
        if(event.GetSelection() == 0)
            target->SetPaletteMode(ColorMode::Gradient);
        else
            target->SetPaletteMode(ColorMode::Gaussian);

        typeNotebook->ChangeSelection(event.GetSelection());
        colorVarSlider->SetRange(0, target->GetPaletteSize());
    }
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