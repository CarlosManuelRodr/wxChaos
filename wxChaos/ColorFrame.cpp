#include "ColorFrame.h"
#include "StringFuncs.h"
#include "global.h"


ESTFractalColor::ESTFractalColor()
{

}
void ESTFractalColor::SetStyle(EST_STYLES style)
{
    // Sets the parameters of each color style.
    switch(style)
    {
    case SUMMER_DAY:
        {
            redInt = 255;
            redMed = 34;
            redDes = 8;
            greenInt = 201;
            greenMed = 28;
            greenDes = 12;
            blueInt = 255;
            blueMed = 21;
            blueDes = 9;
            paletteSize = 60;
            break;
        }
    case COOL_BLUE:
        {
            redInt = 252;
            redMed = 104;
            redDes = 37;
            greenInt = 255;
            greenMed = 96;
            greenDes = 34;
            blueInt = 255;
            blueMed = 73;
            blueDes = 40;
            paletteSize = 300;
            break;
        }
    case HARD_RED:
        {
            redInt = 210;
            redMed = 19;
            redDes = 14;
            greenInt = 102;
            greenMed = 26;
            greenDes = 10;
            blueInt = 97;
            blueMed = 29;
            blueDes = 10;
            paletteSize = 60;
            break;
        }
    case BLACK_AND_WHITE:
        {
            redInt = 200;
            redMed = 30;
            redDes = 9;
            greenInt = 200;
            greenMed = 30;
            greenDes = 9;
            blueInt = 200;
            blueMed = 30;
            blueDes = 9;
            paletteSize = 60;
            break;
        }
    case PASTEL:
        {
            redInt = 205;
            redMed = 23;
            redDes = 14;
            greenInt = 196;
            greenMed = 26;
            greenDes = 11;
            blueInt = 198;
            blueMed = 38;
            blueDes = 9;
            paletteSize = 60;
            break;
        }
    case PSYCH_EXPERIENCE:
        {
            redInt = 126;
            redMed = 3;
            redDes = 11;
            greenInt = 200;
            greenMed = 36;
            greenDes = 9;
            blueInt = 200;
            blueMed = 24;
            blueDes = 12;
            paletteSize = 60;
            break;
        }
    case VIVID_COLORS:
        {
            redInt = 200;
            redMed = 0;
            redDes = 10;
            greenInt = 200;
            greenMed = 17;
            greenDes = 10;
            blueInt = 200;
            blueMed = 37;
            blueDes = 10;
            paletteSize = 60;
            break;
        }
    case CUSTOM:
            break;

    };
}

GradFractalColor::GradFractalColor()
{

}
void GradFractalColor::SetStyle(GRAD_STYLES style)
{
    // Sets the wxString parameter of each color style.
    switch(style)
    {
    case RETRO:
        {
            grad = wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);");
            paletteSize = 300;
        }
        break;
    case HAKIM:
        {
            grad = wxT("rgb(255,255,255);rgb(91,91,91);rgb(0,0,0);rgb(125,199,44);rgb(228,213,12);rgb(192,5,5);rgb(61,43,94);rgb(255,255,255);");
            paletteSize = 300;
        }
        break;
    case AGUAMARINA:
        {
            grad = wxT("rgb(30,102,162);rgb(255,255,255);rgb(91,18,122);rgb(0,0,0);rgb(16,33,112);rgb(30,145,87);rgb(30,102,162);");
            paletteSize = 300;
        }
        break;
    case GRAD_CUSTOM:
        break;
    };
}

ColorFrame::ColorFrame(bool *_active, Fractal *_target, wxWindow* parent,
    wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long windowStyle)
    : wxFrame(parent, id, title, pos, size, windowStyle)
{
    // Constructs the ColorFrame. Gets color values from the target fractal so the frame parameters match the fractal parameters.
#ifdef __linux__
    wxIcon icon(GetWxAbsPath("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
#elif _WIN32
    wxIcon icon(GetWxAbsPath("Resources\\icon.ico"), wxBITMAP_TYPE_ICO);
#endif
    this->SetIcon(icon);

    wxString text;
    active = _active;
    target = _target;

    this->SetSizeHints( wxSize( -1,-1 ), wxSize( -1,-1 ) );

    wxBoxSizer* boxxy;
    boxxy = new wxBoxSizer( wxVERTICAL );

    mPanel = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    mPanel->SetScrollRate( 5, 5 );
    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* setBoxxy;
    setBoxxy = new wxBoxSizer( wxVERTICAL );

    colorOptBitmap = new wxStaticBitmap( mPanel, wxID_ANY, wxBitmap( GetWxAbsPath("Resources/color_opt.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
    setBoxxy->Add( colorOptBitmap, 0, wxALL, 0 );

    algorithmText = new wxStaticText( mPanel, wxID_ANY, wxT(colorAlgTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Color algorithm"
    algorithmText->Wrap( -1 );
    setBoxxy->Add( algorithmText, 0, wxALL, 5 );

    // AlgorithmChoice.
    escapeTimeIndex = -1;
    gaussIntIndex = -1;
    buddhabrotIndex = -1;
    escapeAngleIndex = -1;
    triangleIneqIndex = -1;
    chaoticMapIndex = -1;
    lyapunovIndex = -1;
    convergenceTestIndex = -1;
    algorithmChoice = new wxChoice( mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize );

    setBoxxy->Add( algorithmChoice, 0, wxALL|wxEXPAND, 5 );

    optionsText = new wxStaticText( mPanel, wxID_ANY, wxT(colorOptTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Options"
    optionsText->Wrap( -1 );
    setBoxxy->Add( optionsText, 0, wxALL, 5 );

    //
    relativeCheck = new wxCheckBox( mPanel, wxID_ANY, wxT(relColorTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: " Relative colors"
    setBoxxy->Add( relativeCheck, 0, wxALL, 5 );
    if(target->GetRelativeColorMode()) relativeCheck->SetValue(true);
    else relativeCheck->SetValue(false);
    //

    colorFractal = new wxCheckBox( mPanel, wxID_ANY, wxT(extColorTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: " Color fractal (external color)"
    //
    if(target->GetExtColorMode()) colorFractal->SetValue(true);
    else colorFractal->SetValue(false);
    //
    setBoxxy->Add( colorFractal, 0, wxALL, 5 );

    colorSet = new wxCheckBox( mPanel, wxID_ANY, wxT(intColorTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: " Color set (internal color)"
    //
    if(target->GetSetColorMode()) colorSet->SetValue(true);
    else colorSet->SetValue(false);
    //
    setBoxxy->Add( colorSet, 0, wxALL, 5 );

    orbitTrap = new wxCheckBox( mPanel, wxID_ANY, wxT(orbitTrapTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: " Orbit traps"
    //
    if(target->HasOrbitTrapMode()) orbitTrap->Enable(true);
    else orbitTrap->Enable(false);
    if(target->OrbitTrapActivated()) orbitTrap->SetValue(true);
    else orbitTrap->SetValue(false);
    //

    setBoxxy->Add( orbitTrap, 0, wxALL, 5 );

    smoothRender = new wxCheckBox( mPanel, wxID_ANY, wxT(smoothRenTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: " Smooth render"
    //
    if(target->HasSmoothRenderMode()) smoothRender->Enable(true);
    else smoothRender->Enable(false);
    if(target->SmoothRenderActivated()) smoothRender->SetValue(true);
    else smoothRender->SetValue(false);
    //
    setBoxxy->Add( smoothRender, 0, wxALL, 5 );

    colorVarText = new wxStaticText( mPanel, wxID_ANY, wxT(colorVarTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Color variation"
    colorVarText->Wrap( -1 );
    setBoxxy->Add( colorVarText, 0, wxALL, 5 );

    colorVarSlider = new wxSlider( mPanel, wxID_ANY, 0, 0, 300, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    setBoxxy->Add( colorVarSlider, 0, wxALL|wxEXPAND, 5 );

    wxStaticBoxSizer* colorSetBoxxy;
    colorSetBoxxy = new wxStaticBoxSizer( new wxStaticBox( mPanel, wxID_ANY, wxT(setColorTxt) ), wxVERTICAL );    // Txt: "Set color"

    text = wxT(redColTxt);
    setColor = target->GetSetColor();
    text += num_to_string(setColor.r);
    redSetText = new wxStaticText(mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    redSetText->Wrap( -1 );
    colorSetBoxxy->Add( redSetText, 0, wxALL, 5 );

    redSetSld = new wxSlider( mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    colorSetBoxxy->Add( redSetSld, 0, wxALL|wxEXPAND, 5 );

    text = wxT(greenColTxt);
    text += num_to_string(setColor.g);
    greenSetText = new wxStaticText(mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    greenSetText->Wrap( -1 );
    colorSetBoxxy->Add( greenSetText, 0, wxALL, 5 );

    greenSetSld = new wxSlider(mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    colorSetBoxxy->Add( greenSetSld, 0, wxALL|wxEXPAND, 5 );

    text = wxT(blueColTxt);
    text += num_to_string(setColor.b);
    blueSetText = new wxStaticText( mPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0 );
    blueSetText->Wrap( -1 );
    colorSetBoxxy->Add( blueSetText, 0, wxALL, 5 );

    blueSetSld = new wxSlider( mPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    colorSetBoxxy->Add( blueSetSld, 0, wxALL|wxEXPAND, 5 );

    setBoxxy->Add( colorSetBoxxy, 1, wxEXPAND, 5 );

    okButton = new wxButton( mPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Ok"
    setBoxxy->Add( okButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    mainBoxxy->Add( setBoxxy, 1, wxEXPAND, 5 );

    typeNotebook = new wxNotebook( mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
    gradientLabel = new wxPanel( typeNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* gradBoxxy;
    gradBoxxy = new wxBoxSizer( wxVERTICAL );

    gradStylesLabel = new wxStaticText( gradientLabel, wxID_ANY, wxT(colorStlTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Color styles:"
    gradStylesLabel->Wrap( -1 );
    gradBoxxy->Add( gradStylesLabel, 0, wxALL, 5 );

    wxString gradStyleChoiceChoices[] = { wxT("Retro"), wxT("Hakim"), wxT("Aguamarina"), wxT(customStyleTxt) };
    int gradStyleChoiceNChoices = sizeof( gradStyleChoiceChoices ) / sizeof( wxString );
    gradStylesChoice = new wxChoice( gradientLabel, wxID_ANY, wxDefaultPosition, wxDefaultSize, gradStyleChoiceNChoices, gradStyleChoiceChoices, 0 );
    gradStylesChoice->SetSelection( static_cast<int>(target->GetGradStyle()) );
    gradBoxxy->Add( gradStylesChoice, 0, wxALL, 5 );

    gradientMap = new wxStaticBitmap( gradientLabel, wxID_ANY, PaintGradient(), wxDefaultPosition, wxDefaultSize, 0 );
    gradBoxxy->Add( gradientMap, 0, wxALL, 5 );

    gradButton = new wxButton( gradientLabel, wxID_ANY, wxT(changeGradTxt), wxDefaultPosition, wxDefaultSize, 0 );
    gradBoxxy->Add( gradButton, 0, wxALL, 5 );

    gradPalText = new wxStaticText( gradientLabel, wxID_ANY, wxT(paletteSizeTxt), wxDefaultPosition, wxDefaultSize, 0 );
    gradPalText->Wrap( -1 );
    gradBoxxy->Add( gradPalText, 0, wxALL, 5 );

    gradPalSize = new wxSpinCtrl( gradientLabel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20000, 300 );
    gradBoxxy->Add( gradPalSize, 0, wxALL, 5 );
    gradPalSize->SetValue(target->GetGradient()->getMax() - target->GetGradient()->getMin());

    gradientLabel->SetSizer( gradBoxxy );
    gradientLabel->Layout();
    gradBoxxy->Fit( gradientLabel );
    typeNotebook->AddPage( gradientLabel, wxT(useGradTxt), false );
    stdPanel = new wxPanel( typeNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* colorBoxxy;
    colorBoxxy = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* sideABoxxy;
    sideABoxxy = new wxBoxSizer( wxVERTICAL );

    estilosLabel = new wxStaticText( stdPanel, wxID_ANY, wxT(colorStlTxt), wxDefaultPosition, wxDefaultSize, 0 );
    estilosLabel->Wrap( -1 );
    sideABoxxy->Add( estilosLabel, 0, wxALL, 5 );

    wxString estilosChoices[] = { wxT(summerDayTxt), wxT(coolBlueTxt), wxT(hardRed), wxT(blackWhiteTxt), wxT(pastelTxt), wxT(psychExpTxt), wxT(vividColTxt), wxT(customStyleTxt) };
    int estilosNChoices = sizeof( estilosChoices ) / sizeof( wxString );
    ESTStylesChoice = new wxChoice( stdPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, estilosNChoices, estilosChoices, 0 );
    ESTStylesChoice->SetSelection( static_cast<int>(target->GetESTSyle()) );
    sideABoxxy->Add( ESTStylesChoice, 0, wxALL|wxEXPAND, 5 );

    pSizeText = new wxStaticText( stdPanel, wxID_ANY, wxT(paletteSizeTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Palette size:"
    pSizeText->Wrap( -1 );
    sideABoxxy->Add( pSizeText, 0, wxALL, 5 );

    pSizeSpin = new wxSpinCtrl( stdPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 2000, 60 );
    sideABoxxy->Add( pSizeSpin, 0, wxALL, 5 );

    wxStaticBoxSizer* boxxyred;
    boxxyred = new wxStaticBoxSizer( new wxStaticBox( stdPanel, wxID_ANY, wxT(redTxt) ), wxVERTICAL );    // Txt: "Red"

    wxBoxSizer* subBoxxyred;
    subBoxxyred = new wxBoxSizer( wxVERTICAL );

    //
    redIntText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(intensityTxt);
    text += num_to_string(target->GetInt(red));
    redIntText->SetLabel(text);
    //
    redIntText->Wrap( -1 );
    subBoxxyred->Add( redIntText, 0, wxALL, 5 );

    redIntSld = new wxSlider( stdPanel, wxID_ANY, 200, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxyred->Add( redIntSld, 0, wxALL|wxEXPAND, 5 );

    //
    redPosText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(positionTxt);
    text += num_to_string(target->GetMed(red));
    redPosText->SetLabel(text);
    //
    redPosText->Wrap( -1 );
    subBoxxyred->Add( redPosText, 0, wxALL, 5 );

    redPosSld = new wxSlider( stdPanel, wxID_ANY, 0, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxyred->Add( redPosSld, 0, wxALL|wxEXPAND, 5 );

    //
    redDesText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(stdDevTxt);
    text += num_to_string(target->GetDes(red));
    redDesText->SetLabel(text);
    //
    redDesText->Wrap( -1 );
    subBoxxyred->Add( redDesText, 0, wxALL, 5 );

    redDesSld = new wxSlider( stdPanel, wxID_ANY, 10, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxyred->Add( redDesSld, 0, wxALL|wxEXPAND, 5 );

    boxxyred->Add( subBoxxyred, 1, wxEXPAND, 5 );

    sideABoxxy->Add( boxxyred, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* boxxygreen;
    boxxygreen = new wxStaticBoxSizer( new wxStaticBox( stdPanel, wxID_ANY, wxT(greenTxt) ), wxVERTICAL );    // Txt: "Green"

    wxBoxSizer* subBoxxygreen;
    subBoxxygreen = new wxBoxSizer( wxVERTICAL );

    //
    greenIntText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(intensityTxt);
    text += num_to_string(target->GetInt(green));
    greenIntText->SetLabel(text);
    //
    greenIntText->Wrap( -1 );
    subBoxxygreen->Add( greenIntText, 0, wxALL, 5 );

    greenIntSld = new wxSlider( stdPanel, wxID_ANY, 200, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxygreen->Add( greenIntSld, 0, wxALL|wxEXPAND, 5 );

    //
    greenPosText = new wxStaticText( stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(positionTxt);
    text += num_to_string(target->GetMed(green));
    greenPosText->SetLabel(text);
    //
    greenPosText->Wrap( -1 );
    subBoxxygreen->Add( greenPosText, 0, wxALL, 5 );

    greenPosSld = new wxSlider( stdPanel, wxID_ANY, 17, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxygreen->Add( greenPosSld, 0, wxALL|wxEXPAND, 5 );

    //
    greenDesText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(stdDevTxt);
    text += num_to_string(target->GetDes(green));
    greenDesText->SetLabel(text);
    //
    greenDesText->Wrap( -1 );
    subBoxxygreen->Add( greenDesText, 0, wxALL, 5 );

    greenDesSld = new wxSlider( stdPanel, wxID_ANY, 10, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxygreen->Add( greenDesSld, 0, wxALL|wxEXPAND, 5 );

    boxxygreen->Add( subBoxxygreen, 1, wxEXPAND, 5 );

    sideABoxxy->Add( boxxygreen, 1, wxEXPAND, 5 );

    colorBoxxy->Add( sideABoxxy, 1, wxEXPAND, 5 );

    wxBoxSizer* bSizer13;
    bSizer13 = new wxBoxSizer( wxVERTICAL );

    wxStaticBoxSizer* boxxyblue;
    boxxyblue = new wxStaticBoxSizer( new wxStaticBox( stdPanel, wxID_ANY, wxT(blueTxt) ), wxVERTICAL );    // Txt: "Blue"

    wxBoxSizer* subBoxxyblue;
    subBoxxyblue = new wxBoxSizer( wxVERTICAL );

    //
    blueIntText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(intensityTxt);
    text += num_to_string(target->GetInt(blue));
    blueIntText->SetLabel(text);
    //
    blueIntText->Wrap( -1 );
    subBoxxyblue->Add( blueIntText, 0, wxALL, 5 );

    blueIntSld = new wxSlider( stdPanel, wxID_ANY, 200, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxyblue->Add( blueIntSld, 0, wxALL|wxEXPAND, 5 );

    //
    bluePosText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(positionTxt);
    text += num_to_string(target->GetMed(blue));
    bluePosText->SetLabel(text);
    //
    bluePosText->Wrap( -1 );
    subBoxxyblue->Add( bluePosText, 0, wxALL, 5 );

    bluePosSld = new wxSlider( stdPanel, wxID_ANY, 37, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxyblue->Add( bluePosSld, 0, wxALL|wxEXPAND, 5 );

    //
    blueDesText = new wxStaticText(stdPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
    text = wxT(stdDevTxt);
    text += num_to_string(target->GetDes(blue));
    blueDesText->SetLabel(text);
    //
    blueDesText->Wrap( -1 );
    subBoxxyblue->Add( blueDesText, 0, wxALL, 5 );

    blueDesSld = new wxSlider( stdPanel, wxID_ANY, 10, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    subBoxxyblue->Add( blueDesSld, 0, wxALL|wxEXPAND, 5 );

    boxxyblue->Add( subBoxxyblue, 1, wxEXPAND, 5 );

    bSizer13->Add( boxxyblue, 1, wxEXPAND, 5 );

    colorBoxxy->Add( bSizer13, 1, wxEXPAND, 5 );

    stdPanel->SetSizer( colorBoxxy );
    stdPanel->Layout();
    colorBoxxy->Fit( stdPanel );
    typeNotebook->AddPage( stdPanel, wxT(stdColorTxt), true );    // Txt: "STD Color"

    mainBoxxy->Add( typeNotebook, 2, wxEXPAND | wxALL, 5 );

    mPanel->SetSizer( mainBoxxy );
    mPanel->Layout();
    mainBoxxy->Fit( mPanel );
    boxxy->Add( mPanel, 1, wxEXPAND | wxALL, 0 );

    this->SetSizer( boxxy );
    this->Layout();

    this->Centre( wxBOTH );

    // Shows tab according to the color mode in the fractal.
    if(target->GetColorMode() == GRADIENT) typeNotebook->ChangeSelection(0);
    else typeNotebook->ChangeSelection(1);

    this->SetAlgorithmChoices();

    this->ConnectEvents();
}
ColorFrame::~ColorFrame()
{
    *active = false;    // Warns the mainframe that this frame has been closed.
}

void ColorFrame::ConnectEvents()
{
    this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ColorFrame::OnClose ) );
    gradStylesChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ColorFrame::GradChangeSelection ), NULL, this );
    algorithmChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ColorFrame::OnChangeAlgorithm ), NULL, this );
    relativeCheck->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ColorFrame::OnRelativeColor ), NULL, this );
    gradPalSize->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( ColorFrame::OnGradPaletteSize ), NULL, this );
    typeNotebook->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( ColorFrame::OnPageChange ), NULL, this );
    colorFractal->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ColorFrame::OnColorFractal ), NULL, this );
    colorSet->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ColorFrame::OnColorSet ), NULL, this );
    orbitTrap->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ColorFrame::OnOrbitTrap ), NULL, this );
    smoothRender->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ColorFrame::OnSmoothRender ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    redSetSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::OnSetRed ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    greenSetSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::OnSetGreen ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    blueSetSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::OnSetBlue ), NULL, this );
    okButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ColorFrame::OnOk ), NULL, this );
    gradButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ColorFrame::OnGrad ), NULL, this );
    ESTStylesChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ColorFrame::ESTChangeSelection ), NULL, this );
    pSizeSpin->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( ColorFrame::OnPaletteSize ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redIntSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangeIntRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redPosSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangePosRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    redDesSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangeDesRed ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenIntSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangeIntGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenPosSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangePosGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    greenDesSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangeDesGreen ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    blueIntSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangeIntBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    bluePosSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangePosBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    blueDesSld->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::ChangeDesBlue ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
    colorVarSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorFrame::OnColorVar ), NULL, this );
}
void ColorFrame::SetAlgorithmChoices()
{
    // Construct the alogrithmChoice according to the algorithms available in the fractal.
    for(unsigned int i=0; i<target->GetAvailableAlg().size(); i++)
    {
        switch(target->GetAvailableAlg()[i])
        {
        case ESCAPE_TIME:
            {
                algorithmChoice->Append(wxT(escapeTimeTxt));    // Txt: "Escape time"
                escapeTimeIndex = i;
            }
            break;
        case GAUSSIAN_INT:
            {
                algorithmChoice->Append(wxT(gaussianIntTxt));    // Txt: "Gaussian integer"
                gaussIntIndex = i;
            }
            break;
        case BUDDHABROT:
            {
                algorithmChoice->Append(wxT(buddhabrotTxt));    // Txt: "Buddhabrot"
                buddhabrotIndex = i;
            }
            break;
        case ESCAPE_ANGLE:
            {
                algorithmChoice->Append(wxT(escapeAngleTxt));    // Txt: "Escape angle"
                escapeAngleIndex = i;
            }
            break;
        case TRIANGLE_INEQ:
            {
                algorithmChoice->Append(wxT(triangIneqTxt));    // Txt: "Triangle inequality"
                triangleIneqIndex = i;
            }
            break;
        case CHAOTIC_MAP:
            {
                algorithmChoice->Append(wxT(chaosMapTxt));    // Txt: "Chaotic map"
                chaoticMapIndex = i;
            }
            break;
        case LYAPUNOV:
            {
                algorithmChoice->Append(wxT(lyapTxt));    // Txt: "Lyapunov"
                lyapunovIndex = i;
            }
            break;
        case CONVERGENCE_TEST:
            {
                algorithmChoice->Append(wxT(convTestTxt));    // Txt: "Convergence test"
                convergenceTestIndex = i;
            }
        case OTHER:
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
void ColorFrame::SetTarget(Fractal *_target)
{
    // Sets the new target fractal.
    target = _target;
    pSizeSpin->SetValue(target->GetPaletteSize());

    if(target->HasOrbitTrapMode()) orbitTrap->Enable(true);
    else orbitTrap->Enable(false);
    if(target->OrbitTrapActivated()) orbitTrap->SetValue(true);
    else orbitTrap->SetValue(false);

    if(target->SmoothRenderActivated()) smoothRender->SetValue(true);
    else smoothRender->SetValue(false);
    if(target->HasSmoothRenderMode()) smoothRender->Enable(true);
    else smoothRender->Enable(false);

    algorithmChoice->Clear();
    escapeTimeIndex = -1;
    gaussIntIndex = -1;
    buddhabrotIndex = -1;
    escapeAngleIndex = -1;
    this->SetAlgorithmChoices();

    if(target->GetCurrentAlg() == ESCAPE_TIME)
        algorithmChoice->SetSelection(escapeTimeIndex);
    else if(target->GetCurrentAlg() == GAUSSIAN_INT)
        algorithmChoice->SetSelection(gaussIntIndex);
    else if(target->GetCurrentAlg() == BUDDHABROT)
        algorithmChoice->SetSelection(buddhabrotIndex);
    else if(target->GetCurrentAlg() == ESCAPE_ANGLE)
        algorithmChoice->SetSelection(escapeAngleIndex);
    else if(target->GetCurrentAlg() == CONVERGENCE_TEST)
        algorithmChoice->SetSelection(convergenceTestIndex);
    else
        algorithmChoice->SetSelection(0);

    if(target->GetColorMode() == GRADIENT) typeNotebook->ChangeSelection(0);
    else typeNotebook->ChangeSelection(1);

    if(target->GetRelativeColorMode()) relativeCheck->SetValue(true);
    else relativeCheck->SetValue(false);

    if(target->GetSetColorMode()) colorSet->SetValue(true);
    else colorSet->SetValue(false);

    if(target->GetExtColorMode()) colorFractal->SetValue(true);
    else colorFractal->SetValue(false);

    // Color of the set.
    redSetText->SetLabel(wxString(wxT(redColTxt)) + wxT("0"));
    greenSetText->SetLabel(wxString(wxT(greenColTxt)) + wxT("0"));
    blueSetText->SetLabel(wxString(wxT(blueColTxt)) + wxT("0"));
    redSetSld->SetValue(0);
    greenSetSld->SetValue(0);
    blueSetSld->SetValue(0);

    colorVarSlider->SetValue(0);

    gradStylesChoice->SetSelection( static_cast<int>(target->GetGradStyle()) );
    ESTStylesChoice->SetSelection( static_cast<int>(target->GetESTSyle()) );
}

// Change in parameters methods.
void ColorFrame::ChangeIntRed(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = redIntSld->GetValue();
    target->SetInt(value, red);
    wxString text;
    text = wxT(intensityTxt);
    text += num_to_string(value);
    redIntText->SetLabel(text);
}
void ColorFrame::ChangePosRed(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = redPosSld->GetValue();
    target->SetMed(value, red);
    wxString text;
    text = wxT(positionTxt);
    text += num_to_string(value);
    redPosText->SetLabel(text);
}
void ColorFrame::ChangeDesRed(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = redDesSld->GetValue();
    target->SetDes(value, red);
    wxString text;
    text = wxT(stdDevTxt);
    text += num_to_string(value);
    redDesText->SetLabel(text);
}
void ColorFrame::ChangeIntGreen(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = greenIntSld->GetValue();
    target->SetInt(value, green);
    wxString text;
    text = wxT(intensityTxt);
    text += num_to_string(value);
    greenIntText->SetLabel(text);
}
void ColorFrame::ChangePosGreen(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = greenPosSld->GetValue();
    target->SetMed(value, green);
    wxString text;
    text = wxT(positionTxt);
    text += num_to_string(value);
    greenPosText->SetLabel(text);
}
void ColorFrame::ChangeDesGreen(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = greenDesSld->GetValue();
    target->SetDes(value, green);
    wxString text;
    text = wxT(stdDevTxt);
    text += num_to_string(value);
    greenDesText->SetLabel(text);
}
void ColorFrame::ChangeIntBlue(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = blueIntSld->GetValue();
    target->SetInt(value, blue);
    wxString text;
    text = wxT(intensityTxt);
    text += num_to_string(value);
    blueIntText->SetLabel(text);
}
void ColorFrame::ChangePosBlue(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = bluePosSld->GetValue();
    target->SetMed(value, blue);
    wxString text;
    text = wxT(positionTxt);
    text += num_to_string(value);
    bluePosText->SetLabel(text);
}
void ColorFrame::ChangeDesBlue(wxScrollEvent& event)
{
    ESTStylesChoice->SetSelection(CUSTOM);
    int value = blueDesSld->GetValue();
    target->SetDes(value, blue);
    wxString text;
    text = wxT(stdDevTxt);
    text += num_to_string(value);
    blueDesText->SetLabel(text);
}
void ColorFrame::OnOk(wxCommandEvent& event)
{
    this->Destroy();
}
void ColorFrame::ESTChangeSelection(wxCommandEvent& event)
{
    // Change colors according to selection.
    estFractalColor.redInt = redIntSld->GetValue(); estFractalColor.greenInt = greenIntSld->GetValue(); estFractalColor.blueInt = blueIntSld->GetValue();
    estFractalColor.redMed = redPosSld->GetValue(); estFractalColor.greenMed = greenPosSld->GetValue(); estFractalColor.blueMed = bluePosSld->GetValue();
    estFractalColor.redDes = redDesSld->GetValue(); estFractalColor.greenDes = greenDesSld->GetValue(); estFractalColor.blueDes = blueDesSld->GetValue();

    estFractalColor.SetStyle(static_cast<EST_STYLES>(ESTStylesChoice->GetCurrentSelection()));
    int paletteSize = estFractalColor.paletteSize;
    redPosSld->SetRange(0, paletteSize);
    greenPosSld->SetRange(0, paletteSize);
    bluePosSld->SetRange(0, paletteSize);
    colorVarSlider->SetRange(0, paletteSize);
    pSizeSpin->SetValue(paletteSize);
    target->SetPaletteSize(paletteSize);
    target->SetESTStyle(static_cast<EST_STYLES>(ESTStylesChoice->GetCurrentSelection()));

    // Red
    // Intensity
    wxString text;
    target->SetInt(estFractalColor.redInt, red);
    redIntSld->SetValue(estFractalColor.redInt);
    text = wxT(intensityTxt);
    text += num_to_string(estFractalColor.redInt);
    redIntText->SetLabel(text);

    // Mean
    target->SetMed(estFractalColor.redMed, red);
    redPosSld->SetValue(estFractalColor.redMed);
    text = wxT(positionTxt);
    text += num_to_string(estFractalColor.redMed);
    redPosText->SetLabel(text);

    // Standard deviation
    target->SetDes(estFractalColor.redDes, red);
    redDesSld->SetValue(estFractalColor.redDes);
    text = wxT(stdDevTxt);
    text += num_to_string(estFractalColor.redDes);
    redDesText->SetLabel(text);


    // Green
    // Intensity
    target->SetInt(estFractalColor.greenInt, green);
    greenIntSld->SetValue(estFractalColor.greenInt);
    text = wxT(intensityTxt);
    text += num_to_string(estFractalColor.greenInt);
    greenIntText->SetLabel(text);

    // Mean
    target->SetMed(estFractalColor.greenMed, green);
    greenPosSld->SetValue(estFractalColor.greenMed);
    text = wxT(positionTxt);
    text += num_to_string(estFractalColor.greenMed);
    greenPosText->SetLabel(text);

    // Standard deviation
    target->SetDes(estFractalColor.greenDes, green);
    greenDesSld->SetValue(estFractalColor.greenDes);
    text = wxT(stdDevTxt);
    text += num_to_string(estFractalColor.greenDes);
    greenDesText->SetLabel(text);

    // Blue
    // Intensity
    target->SetInt(estFractalColor.blueInt, blue);
    blueIntSld->SetValue(estFractalColor.blueInt);
    text = wxT(intensityTxt);
    text += num_to_string(estFractalColor.blueInt);
    blueIntText->SetLabel(text);

    // Mean
    target->SetMed(estFractalColor.blueMed, blue);
    bluePosSld->SetValue(estFractalColor.blueMed);
    text = wxT(positionTxt);
    text += num_to_string(estFractalColor.blueMed);
    bluePosText->SetLabel(text);

    // Standard deviation
    target->SetDes(estFractalColor.blueDes, blue);
    blueDesSld->SetValue(estFractalColor.blueDes);
    text = wxT(stdDevTxt);
    text += num_to_string(estFractalColor.blueDes);
    blueDesText->SetLabel(text);
}
void ColorFrame::GradChangeSelection( wxCommandEvent& event )
{
    // Changes the gradStyle.
    gradFractalColor.SetStyle(static_cast<GRAD_STYLES>(gradStylesChoice->GetCurrentSelection()));
    wxGradient myGrad;
    myGrad.setMin(0);
    myGrad.setMax(gradFractalColor.paletteSize);
    myGrad.fromString(gradFractalColor.grad);
    target->SetGradStyle(static_cast<GRAD_STYLES>(gradStylesChoice->GetCurrentSelection()));
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
        target->SetAlgorithm(ESCAPE_TIME);
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
    {
        target->SetAlgorithm(GAUSSIAN_INT);
    }
    else if(selection == buddhabrotIndex)
    {
        target->SetAlgorithm(BUDDHABROT);
    }
    else if(selection == escapeAngleIndex)
    {
        target->SetAlgorithm(ESCAPE_ANGLE);
    }
    else if(selection == triangleIneqIndex)
    {
        target->SetAlgorithm(TRIANGLE_INEQ);
    }
    else if(selection == chaoticMapIndex)
    {
        target->SetAlgorithm(CHAOTIC_MAP);
    }
    else if(selection == lyapunovIndex)
    {
        target->SetAlgorithm(LYAPUNOV);
    }
    else if(selection == convergenceTestIndex)
    {
        target->SetAlgorithm(CONVERGENCE_TEST);
    }
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
    redPosSld->SetRange(0, size);
    greenPosSld->SetRange(0, size);
    bluePosSld->SetRange(0, size);
    colorVarSlider->SetRange(0,size);
    target->SetPaletteSize(size);
}
void ColorFrame::OnClose(wxCloseEvent& event)
{
    *active = false;
    this->Show(false);
    this->Destroy();
}
void ColorFrame::OnGrad( wxCommandEvent& event )
{
    wxGradientDialog diag(this, *target->GetGradient());
    diag.ShowModal();
    target->SetGradient(diag.GetGradient());
    gradientMap->SetBitmap(PaintGradient());
    gradientMap->SetWindowStyle(wxSIMPLE_BORDER);
    gradientMap->Refresh();
    gradStylesChoice->SetSelection(static_cast<int>(GRAD_CUSTOM));
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
void ColorFrame::OnPageChange( wxNotebookEvent& event )
{
    // On Linux when the wxNotebook is destroyed it sends a event.
    if(*active)
    {
        if(event.GetSelection() == 0)
            target->SetPaletteMode(GRADIENT);
        else
            target->SetPaletteMode(EST_MODE);
        typeNotebook->ChangeSelection(event.GetSelection());
        colorVarSlider->SetRange(0, target->GetPaletteSize());
    }
}
void ColorFrame::OnGradPaletteSize( wxSpinEvent& event )
{
    int size = gradPalSize->GetValue();
    if(size > 0) target->SetGradientSize(size);
    gradientMap->SetBitmap(this->PaintGradient());
    colorVarSlider->SetRange(0,size);
}
void ColorFrame::OnColorVar( wxScrollEvent& event )
{
    target->SetVarGradient(colorVarSlider->GetValue());
}
