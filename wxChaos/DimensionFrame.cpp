#include <limits>
#include <fstream>
#include "DimensionFrame.h"
#include "StringFuncs.h"
#include "SizeDialogSave.h"
#include "BmpWriter.h"
#include "AngelScriptFunc.h"
#include "HTMLViewer.h"

bool dimFrameState = false;

double GetMaxElement(const vector<double> in)
{
    double max = -std::numeric_limits<double>::infinity();
    for(unsigned int i=0; i<in.size(); i++)
    {
        if(in[i] > max) max = in[i];
    }
    return max;
}
double GetMinElement(const vector<double> in)
{
    double min = std::numeric_limits<double>::infinity();
    for(unsigned int i=0; i<in.size(); i++)
    {
        if(in[i] < min) min = in[i];
    }
    return min;
}

// DimCalculator
DimCalculator::DimCalculator()
{
    running = false;
    N = 0;
}
void DimCalculator::SetMap(bool **_map, int _size, int _ho, int _hf)
{
    map = _map;
    size = _size;
    ho = _ho;
    hf = _hf;
}
void DimCalculator::SetDiv(int _div)
{
    div = _div;
}
void DimCalculator::Run()
{
    running = true;

    N = 0;
    double epsilon = (double)size/(double)div;
    int ey_init = (double)ho/epsilon;
    int ey_end = (double)hf/epsilon;

    // Iterate through the boxes.
    for(int ey=ey_init; ey < ey_end; ey++)
    {
        for(int ex=0; ex<div; ex++)
        {
            bool found = false;

            // And count the elements of the map that belong to the set.
            for(int w=ex*epsilon; w<(ex+1)*epsilon && !found; w++)
            {
                for(int h=ey*epsilon; h<(ey+1)*epsilon; h++)
                {
                    if(w < size && h < size)
                    {
                        if(map[w][h] == true)
                        {
                            found = true;
                            N++;
                            break;
                        }
                    }
                    else break;
                }
            }
        }
    }
    running = false;
}
int DimCalculator::GetBoxCount()
{
    return N;
}
bool DimCalculator::IsRunning()
{
    return running;
}

// ImagePanel
ImagePanel::ImagePanel(wxWindow* parent, int id, int _size) : wxPanel(parent, id)
{
    map = NULL;
    size = _size;
    div = 20; // Default.

    // Allocates memory for the maps.
    map = new bool*[size];
    for(int i=0; i<size; i++)
        map[i] = new bool[size];

    // Initializes maps.
    for(int i=0; i<size; i++)
    {
        for(int j=0; j<size; j++)
        {
            map[i][j] = false;
        }
    }

    this->SetSize(size, size);
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(ImagePanel::OnPaint));
}
ImagePanel::~ImagePanel()
{
    // Cleanup.
    for(int i=0; i< size; i++)
    {
        delete[] map[i];
    }
    delete[] map;
}
void ImagePanel::OnPaint(wxPaintEvent& event)
{
    // Draw white background.
    wxPaintDC dc(this);
    dc.SetBrush(wxBrush(wxColour(255,255,255)));
    dc.SetPen(wxPen(wxColour(255,255,255)));
    dc.DrawRectangle(0,0, size, size);

    if(map != NULL)
    {
        double epsilon = (double)size/(double)div;
        int boxes = 0;

        // Fill color squares.
        dc.SetPen(wxColour(50, 50, 255));
        dc.SetBrush(wxColour(50, 50, 255));

        // Iterate through the boxes.
        for(int ey=0; ey<div; ey++)
        {
            for(int ex=0; ex<div; ex++)
            {
                bool found = false;

                // And count the elements of the map that belong to the set.
                for(int w=ex*epsilon; w<(ex+1)*epsilon && !found; w++)
                {
                    for(int h=ey*epsilon; h<(ey+1)*epsilon; h++)
                    {
                        if(w < size && h < size)
                        {
                            if(map[w][h] == true)
                            {
                                found = true;
                                boxes++;
                                dc.DrawRectangle(ex*epsilon, ey*epsilon, ceil(epsilon), ceil(epsilon));
                                break;
                            }
                        }
                        else break;
                    }
                }
            }
        }

        // Now draw the horizontal division lines.
        dc.SetPen(wxColour(0, 0, 0));
        for(int ey=0; ey < div; ey++)
        {
            int y = ey*epsilon;
            dc.DrawLine(0, y, size, y);
        }
        dc.DrawLine(0, 399, size, 399);

        // Vertical lines.
        for(int ex=0; ex < div; ex++)
        {
            int x = ex*epsilon;
            dc.DrawLine(x, 0, x, size);
        }
        dc.DrawLine(399, 0, 399, size);

        // Draw fractal.
        for(int i=0; i<size; i++)
        {
            for(int j=0; j<size; j++)
            {
                if(map[i][j] == true)
                {
                    dc.DrawPoint(i, j);
                }
            }
        }

        // Draw text.
        // Calculate the numbers of digits in the number.
        int number = boxes;
        int digits = 1;
        while(number >= 10)
        {
            number /= 10;
            digits++;
        }
        int extra = 0;
        digits -= 3;
        if(digits > 0)
            extra += 10*digits;

        dc.SetBrush(wxBrush(wxColour(0, 0, 0, 100)));
        dc.SetTextForeground(wxColour(255,255,255));
        dc.DrawRectangle(0,383, 60 + extra, 17);
        wxString outText = wxT("N = ");
        outText += num_to_string(boxes);
        dc.DrawText(outText, 5, 383);
    }
}
void ImagePanel::SetMap(bool **_map, int _div)
{
    // Copy map.
    for(int i=0; i<size; i++)
    {
        for(int j=0; j<size; j++)
        {
            map[i][j] = _map[i][j];
        }
    }
    div = _div;
}

// ConfFractOptDialog
ConfFractOptDialog::ConfFractOptDialog( Fractal* _target, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    target = _target;
    this->SetSizeHints( wxSize( 213,361 ), wxDefaultSize );

    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer( wxVERTICAL );

    mainScroll = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    mainScroll->SetScrollRate( 5, 5 );
    optionsBoxxy = new wxBoxSizer( wxVERTICAL );

    kRealLabel = new wxStaticText( mainScroll, wxID_ANY, wxT(optKRealTxt), wxDefaultPosition, wxDefaultSize, 0 );
    kRealLabel->Wrap( -1 );
    optionsBoxxy->Add( kRealLabel, 0, wxALL, 5 );

    kRealCtrl = new wxTextCtrl( mainScroll, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    optionsBoxxy->Add( kRealCtrl, 0, wxALL|wxEXPAND, 5 );

    kImaginaryLabel = new wxStaticText( mainScroll, wxID_ANY, wxT(optKImagTxt), wxDefaultPosition, wxDefaultSize, 0 );
    kImaginaryLabel->Wrap( -1 );
    optionsBoxxy->Add( kImaginaryLabel, 0, wxALL, 5 );

    kImaginaryCtrl = new wxTextCtrl( mainScroll, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    optionsBoxxy->Add( kImaginaryCtrl, 0, wxALL|wxEXPAND, 5 );

    mainScroll->SetSizer( optionsBoxxy );
    mainScroll->Layout();
    optionsBoxxy->Fit( mainScroll );
    mainBoxxy->Add( mainScroll, 7, wxEXPAND | wxALL, 1 );

    staticLine = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    mainBoxxy->Add( staticLine, 0, wxEXPAND | wxALL, 5 );

    wxBoxSizer* buttonBoxxy;
    buttonBoxxy = new wxBoxSizer( wxHORIZONTAL );

    okButton = new wxButton( this, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0 );
    buttonBoxxy->Add( okButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    applyButton = new wxButton( this, wxID_ANY, wxT(applyTxt), wxDefaultPosition, wxDefaultSize, 0 );
    buttonBoxxy->Add( applyButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    mainBoxxy->Add( buttonBoxxy, 0, wxEXPAND, 5 );

    this->SetSizer( mainBoxxy );
    this->Layout();

    this->Centre( wxBOTH );

    if(!target->IsJuliaVariety())
    {
        kRealCtrl->Enable(false);
        kImaginaryCtrl->Enable(false);
    }
    this->AdjustOptPanel();

    // Connect Events
    okButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfFractOptDialog::OnOk ), NULL, this );
    applyButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfFractOptDialog::OnApply ), NULL, this );
}
ConfFractOptDialog::~ConfFractOptDialog()
{
    // Disconnect Events
    okButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfFractOptDialog::OnOk ), NULL, this );
    applyButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfFractOptDialog::OnApply ), NULL, this );
}

void ConfFractOptDialog::AdjustOptPanel()
{
    // If there are elements from a previous panel deletes them.
    this->DeleteOptPanel();

    PanelOptions* pOptions = target->GetOptPanel();
    int index, labelIndex;

    // If there are elements in pOptions creates panel.
    if(pOptions->GetElementsSize() > 0)
    {
        // Creates elements from each kind.
        for(int i=0; i<pOptions->GetElementsSize(); i++)
        {
            switch(pOptions->GetPanelOptType(i))
            {
            case LABEL:
                {
                    labels.push_back(new wxStaticText( mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap( -1 );
                    optionsBoxxy->Add( labels[labelIndex], 0, wxALL, 5 );
                    foundLabels.push_back(i);
                }
                break;
            case TXTCTRL:
                {
                    labels.push_back(new wxStaticText( mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap( -1 );
                    optionsBoxxy->Add( labels[labelIndex], 0, wxALL, 5 );

                    textControls.push_back(new wxTextCtrl( mainScroll, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    index = textControls.size()-1;
                    optionsBoxxy->Add( textControls[index], 0, wxALL|wxEXPAND, 5 );
                    foundTextControls.push_back(i);
                }
                break;
            case SPIN:
                {
                    labels.push_back(new wxStaticText( mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    labelIndex = labels.size()-1;
                    labels[labelIndex]->Wrap( -1 );
                    optionsBoxxy->Add( labels[labelIndex], 0, wxALL, 5 );

                    spinControls.push_back(new wxSpinCtrl( mainScroll, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0 ));
                    index = spinControls.size()-1;
                    optionsBoxxy->Add( spinControls[index], 0, wxALL|wxEXPAND, 5 );
                    foundSpinControls.push_back(i);
                }
                break;
            case CHECKBOX:
                {
                    checkBoxes.push_back(new wxCheckBox( mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0 ));
                    index = checkBoxes.size()-1;
                    if(pOptions->GetDefault(i) == wxT("true"))
                        checkBoxes[index]->SetValue(true);
                    else
                        checkBoxes[index]->SetValue(false);
                    optionsBoxxy->Add( checkBoxes[index], 0, wxALL|wxEXPAND, 5 );
                    foundCheckBoxes.push_back(i);
                }
                break;
            };
        }

        // Creates button to apply options.
        optionsBoxxy->Layout();
        mainScroll->SetScrollbars(20, 20, 0, 50);
    }
}
void ConfFractOptDialog::DeleteOptPanel()
{
    // Deletes panel elements.
    for(unsigned int i=0; i<labels.size(); i++)
    {
        labels[i]->Destroy();
    }
    labels.clear();
    foundLabels.clear();
    for(unsigned int i=0; i<textControls.size(); i++)
    {
        textControls[i]->Destroy();
    }
    textControls.clear();
    foundTextControls.clear();
    for(unsigned int i=0; i<spinControls.size(); i++)
    {
        spinControls[i]->Destroy();
    }
    spinControls.clear();
    foundSpinControls.clear();
    for(unsigned int i=0; i<checkBoxes.size(); i++)
    {
        checkBoxes[i]->Destroy();
    }
    checkBoxes.clear();
    foundCheckBoxes.clear();
}
void ConfFractOptDialog::SetNewTarget(Fractal* _target)
{
    target = _target;
    if(target->IsJuliaVariety())
    {
        kRealCtrl->Enable(true);
        kRealCtrl->SetValue(num_to_string(_target->GetKReal()));
        kImaginaryCtrl->Enable(true);
        kImaginaryCtrl->SetValue(num_to_string(_target->GetKImaginary()));
    }
    else
    {
        kRealCtrl->Enable(false);
        kImaginaryCtrl->Enable(false);
    }
    this->AdjustOptPanel();
}
void ConfFractOptDialog::OnOk( wxCommandEvent& event )
{
    this->Show(false);
}
void ConfFractOptDialog::OnApply( wxCommandEvent& event )
{
    // Pass parameters to the fractal and redraws it.
    PanelOptions* pOptions = target->GetOptPanel();
    for(unsigned int i=0; i<foundTextControls.size(); i++)
    {
        *pOptions->GetDoubleElement(i) = string_to_double(textControls[i]->GetValue());
    }
    for(unsigned int i=0; i<foundSpinControls.size(); i++)
    {
        *pOptions->GetIntElement(i) = spinControls[i]->GetValue();
    }
    for(unsigned int i=0; i<foundCheckBoxes.size(); i++)
    {
        if(checkBoxes[i]->GetValue())
            *pOptions->GetBoolElement(i) = true;
        else
            *pOptions->GetBoolElement(i) = false;
    }
    if(target->IsJuliaVariety())
        target->SetK(string_to_double(kRealCtrl->GetValue()), string_to_double(kImaginaryCtrl->GetValue()));
    target->SetFractalPropChanged();
}

// LinePlotter
LinePlotter::LinePlotter(LineParams _params) : mpFX( wxT("Fit"))
{
    params = _params;
}
double LinePlotter::GetY(double x)
{
    return params.m*x + params.b;
}

// PlotWindow
PlotWindow::PlotWindow(vector<double> xList, vector<double> yList, wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style) : wxFrame( parent, id, title, pos, size, style )
{
#ifdef __linux__
    wxIcon icon(GetWxAbsPath("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
#elif _WIN32
    wxIcon icon(GetWxAbsPath("Resources\\icon.ico"), wxBITMAP_TYPE_ICO);
#endif
    this->SetIcon(icon);

    wxFont graphFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_plot = new mpWindow( this, -1, wxPoint(0,0), wxSize(500,500), wxBORDER_NONE );
    mpScaleX* xaxis = new mpScaleX(wxT("Epsilon"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    mpScaleY* yaxis = new mpScaleY(wxT("N"), mpALIGN_LEFT, true);
    xaxis->SetDrawOutsideMargins(false);
    yaxis->SetDrawOutsideMargins(false);
    xaxis->SetFont(graphFont);
    yaxis->SetFont(graphFont);

    m_plot->SetMargins(30, 30, 50, 100);
    m_plot->AddLayer(xaxis);
    m_plot->AddLayer(yaxis);
    mpFXYVector* vectorLayer = new mpFXYVector(_("Data"));
    vectorLayer->SetData(xList, yList);
    vectorLayer->SetPen(wxPen(*wxBLUE, 3, wxSOLID));
    m_plot->AddLayer(vectorLayer);

    m_plot->Fit();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    topsizer->Add( m_plot, 1, wxEXPAND );
    SetAutoLayout( TRUE );
    SetSizer( topsizer );
}
PlotWindow::PlotWindow(LineParams params, vector<double> xList, vector<double> yList, wxWindow* parent, wxWindowID id,
    const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame( parent, id, title, pos, size, style )
{
#ifdef _WIN32
    wxIcon icon(wxT("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);
#endif

    wxFont graphFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_plot = new mpWindow( this, -1, wxPoint(0,0), wxSize(500,500), wxBORDER_NONE );
    mpScaleX* xaxis = new mpScaleX(wxT("Log(1/Epsilon)"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    mpScaleY* yaxis = new mpScaleY(wxT("Log(N)"), mpALIGN_LEFT, true);
    xaxis->SetDrawOutsideMargins(false);
    yaxis->SetDrawOutsideMargins(false);
    xaxis->SetFont(graphFont);
    yaxis->SetFont(graphFont);

    m_plot->SetMargins(30, 30, 50, 100);
    m_plot->AddLayer(xaxis);
    m_plot->AddLayer(yaxis);
    mpFXYVector* vectorLayer = new mpFXYVector(_("Data"));
    vectorLayer->SetData(xList, yList);
    vectorLayer->SetPen(wxPen(*wxBLUE, 3, wxSOLID));
    m_plot->AddLayer(new LinePlotter(params));
    m_plot->AddLayer(vectorLayer);

    m_plot->Fit();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    topsizer->Add( m_plot, 1, wxEXPAND );
    SetAutoLayout( TRUE );
    SetSizer( topsizer );
}
PlotWindow::~PlotWindow()
{

}

// DimensionFrame
DimensionFrame::DimensionFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : wxFrame( parent, id, title, pos, size, style )
{
    threadNumber = Get_Cores();
    dimCalculator = new DimCalculator[threadNumber];
    previewSize = 400;
    target = NULL;
    confFractOptDialog = NULL;
    renderingPreview = false;
    calculatingDimension = false;
    scriptSelected = false;
    firstRender = true;
    clock.Reset();

    this->SetSizeHints( wxSize( 300,300 ), wxDefaultSize );

#ifdef _WIN32
    wxIcon icon(wxT("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);
#endif

    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer( wxVERTICAL );

    mainPanel = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    mainPanel->SetScrollRate( 5, 5 );
    wxBoxSizer* subMainBoxxy;
    subMainBoxxy = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* paramBoxxy;
    paramBoxxy = new wxBoxSizer( wxVERTICAL );

    wxStaticBoxSizer* fractalBoxxy;
    fractalBoxxy = new wxStaticBoxSizer( new wxStaticBox( mainPanel, wxID_ANY, wxT("Fractal") ), wxHORIZONTAL );

    wxBoxSizer* borderBoxxy;
    borderBoxxy = new wxBoxSizer( wxVERTICAL );

    wxString fractalChoiceChoices[] = { wxT(menuHenonMapTxt), wxT(menuMandelbrotTxt), wxT(menuMandelbrotZNTxt), wxT(menuJuliaTxt), wxT(menuJuliaZNTxt), wxT(menuSineTxt), wxT(menuJellyfishTxt),
                                        wxT(menuManowarTxt), wxT(menuManowarJuliaTxt), wxT(menuTricornTxt), wxT(menuBurningShipTxt), wxT(menuBurningShipJuliaTxt),
                                        wxT(menuFractoryTxt), wxT(menuCellTxt), wxT(menuMagnetTxt), wxT(menuDoublePendulumTxt), wxT(menuLogisticTxt) };
    int fractalChoiceNChoices = sizeof( fractalChoiceChoices ) / sizeof( wxString );
    fractalChoice = new wxChoice( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, fractalChoiceNChoices, fractalChoiceChoices, 0 );
    borderBoxxy->Add( fractalChoice, 0, wxALL|wxEXPAND, 5 );

    minXTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(minXLblTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "MinX"
    minXTxt->Wrap( -1 );
    borderBoxxy->Add( minXTxt, 0, wxALL, 5 );

    minXCtrl = new wxTextCtrl( mainPanel, wxID_ANY, wxT("-1.5"), wxDefaultPosition, wxDefaultSize, 0 );
    borderBoxxy->Add( minXCtrl, 0, wxALL|wxEXPAND, 5 );

    maxXTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(maxXLblTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "MaxX"
    maxXTxt->Wrap( -1 );
    borderBoxxy->Add( maxXTxt, 0, wxALL, 5 );

    maxXCtrl = new wxTextCtrl( mainPanel, wxID_ANY, wxT("1.5"), wxDefaultPosition, wxDefaultSize, 0 );
    borderBoxxy->Add( maxXCtrl, 0, wxALL|wxEXPAND, 5 );

    minYTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(minYLblTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "MinY"
    minYTxt->Wrap( -1 );
    borderBoxxy->Add( minYTxt, 0, wxALL, 5 );

    minYCtrl = new wxTextCtrl( mainPanel, wxID_ANY, wxT("-0.4"), wxDefaultPosition, wxDefaultSize, 0 );
    borderBoxxy->Add( minYCtrl, 0, wxALL|wxEXPAND, 5 );

    manualMaxYChk = new wxCheckBox( mainPanel, wxID_ANY, wxT(manualMaxYTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Manual MaxY"
    borderBoxxy->Add( manualMaxYChk, 0, wxALL, 5 );
    manualMaxYChk->SetValue(true);

    maxYTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(maxYLblTxy), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "MaxY"
    maxYTxt->Wrap( -1 );
    borderBoxxy->Add( maxYTxt, 0, wxALL, 5 );

    maxYCtrl = new wxTextCtrl( mainPanel, wxID_ANY, wxT("0.4"), wxDefaultPosition, wxDefaultSize, 0 );

    borderBoxxy->Add( maxYCtrl, 0, wxALL|wxEXPAND, 5 );

    fractalBoxxy->Add( borderBoxxy, 1, wxEXPAND, 5 );

    wxBoxSizer* fOptBoxxy;
    fOptBoxxy = new wxBoxSizer( wxVERTICAL );

    iterTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(dimIterTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Iterations"
    iterTxt->Wrap( -1 );
    fOptBoxxy->Add( iterTxt, 0, wxALL, 5 );

    iterCtrl = new wxTextCtrl( mainPanel, wxID_ANY, wxT("20000"), wxDefaultPosition, wxDefaultSize, 0 );
    fOptBoxxy->Add( iterCtrl, 0, wxALL|wxEXPAND, 5 );

    sizeTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(imagSizeTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Image size (pixels)"
    sizeTxt->Wrap( -1 );
    fOptBoxxy->Add( sizeTxt, 0, wxALL, 5 );

    sizeCtrl = new wxTextCtrl( mainPanel, wxID_ANY, wxT("3000"), wxDefaultPosition, wxDefaultSize, 0 );
    fOptBoxxy->Add( sizeCtrl, 0, wxALL|wxEXPAND, 5 );

    fOptButton = new wxButton( mainPanel, wxID_ANY, wxT(confFOpt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Configure fractal options"
    fOptBoxxy->Add( fOptButton, 0, wxALL|wxEXPAND, 5 );

    wxStaticBoxSizer* previewBoxxy;
    previewBoxxy = new wxStaticBoxSizer( new wxStaticBox( mainPanel, wxID_ANY, wxT(quickPrevTxt) ), wxVERTICAL );    // Txt: "Quick preview"

    nDivTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(nDivLblTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Number of divisions"
    nDivTxt->Wrap( -1 );
    previewBoxxy->Add( nDivTxt, 0, wxALL, 5 );

    nDivSpin = new wxSpinCtrl( mainPanel, wxID_ANY, wxT("20"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 200, 0 );
    previewBoxxy->Add( nDivSpin, 0, wxALL|wxEXPAND, 5 );

    wxBoxSizer* renderPreBoxxy;
    renderPreBoxxy = new wxBoxSizer( wxVERTICAL );

    previewButton = new wxButton( mainPanel, wxID_ANY, wxT(renderPrevTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Render preview"
    renderPreBoxxy->Add( previewButton, 0, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );

    savePreviewButton = new wxButton( mainPanel, wxID_ANY, wxT(savePrevTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Save preview"
    renderPreBoxxy->Add( savePreviewButton, 0, wxALL|wxEXPAND, 5 );

    previewBoxxy->Add( renderPreBoxxy, 1, wxEXPAND, 5 );

    fOptBoxxy->Add( previewBoxxy, 1, wxEXPAND, 5 );

    fractalBoxxy->Add( fOptBoxxy, 1, wxEXPAND, 5 );

    paramBoxxy->Add( fractalBoxxy, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* dimBoxxy;
    dimBoxxy = new wxStaticBoxSizer( new wxStaticBox( mainPanel, wxID_ANY, wxT(boxCountTxt) ), wxVERTICAL );    // Txt: "Box-counting parameters"

    divTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(divLblTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Divisions"
    divTxt->Wrap( -1 );
    dimBoxxy->Add( divTxt, 0, wxALL, 5 );

    wxBoxSizer* divBoxxy;
    divBoxxy = new wxBoxSizer( wxVERTICAL );

    divNotebook = new wxNotebook( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
    byFunctionPanel = new wxPanel( divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* byFunctionBoxxy;
    byFunctionBoxxy = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* fSide1Boxxy;
    fSide1Boxxy = new wxBoxSizer( wxVERTICAL );

    funcTxt = new wxStaticText( byFunctionPanel, wxID_ANY, wxT(funcLblTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Function"
    funcTxt->Wrap( -1 );
    fSide1Boxxy->Add( funcTxt, 0, wxALL, 5 );

    wxBoxSizer* fCtrlBoxxy;
    fCtrlBoxxy = new wxBoxSizer( wxHORIZONTAL );

    fDeclTxt = new wxStaticText( byFunctionPanel, wxID_ANY, wxT("f(x) = "), wxDefaultPosition, wxDefaultSize, 0 );
    fDeclTxt->Wrap( -1 );
    fCtrlBoxxy->Add( fDeclTxt, 0, wxALL, 5 );

    funcCtrl = new wxTextCtrl( byFunctionPanel, wxID_ANY, wxT("2*x"), wxDefaultPosition, wxDefaultSize, 0 );
    fCtrlBoxxy->Add( funcCtrl, 1, wxALL, 5 );

    fSide1Boxxy->Add( fCtrlBoxxy, 1, wxEXPAND, 5 );

    byFunctionBoxxy->Add( fSide1Boxxy, 1, wxEXPAND, 5 );

    funcLine = new wxStaticLine( byFunctionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    byFunctionBoxxy->Add( funcLine, 0, wxEXPAND | wxALL, 5 );

    wxBoxSizer* fSide2Boxxy;
    fSide2Boxxy = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* goesFromBoxxy;
    goesFromBoxxy = new wxBoxSizer( wxHORIZONTAL );

    goesFromTxt = new wxStaticText( byFunctionPanel, wxID_ANY, wxT(xGoesFromTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "x goes from"
    goesFromTxt->Wrap( -1 );
    goesFromBoxxy->Add( goesFromTxt, 0, wxALL, 5 );

    xMinSpin = new wxSpinCtrl( byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000000, 1 );
    goesFromBoxxy->Add( xMinSpin, 1, wxALL, 5 );

    fSide2Boxxy->Add( goesFromBoxxy, 1, wxEXPAND, 5 );

    wxBoxSizer* goesToBoxxy;
    goesToBoxxy = new wxBoxSizer( wxHORIZONTAL );

    goesToTxt = new wxStaticText( byFunctionPanel, wxID_ANY, wxT(toTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "to"
    goesToTxt->Wrap( -1 );
    goesToBoxxy->Add( goesToTxt, 0, wxALL, 5 );

    xMaxSpin = new wxSpinCtrl( byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000000, 50 );
    goesToBoxxy->Add( xMaxSpin, 0, wxALL, 5 );

    fSide2Boxxy->Add( goesToBoxxy, 1, wxEXPAND, 5 );

    byFunctionBoxxy->Add( fSide2Boxxy, 1, wxEXPAND, 5 );

    byFunctionPanel->SetSizer( byFunctionBoxxy );
    byFunctionPanel->Layout();
    byFunctionBoxxy->Fit( byFunctionPanel );
    divNotebook->AddPage( byFunctionPanel, wxT(byFuncTxt), true );    // Txt: "By function"
    byListPanel = new wxPanel( divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* byListBoxxy;
    byListBoxxy = new wxBoxSizer( wxVERTICAL );

    listCtrl = new wxTextCtrl( byListPanel, wxID_ANY, wxT("2,4,5,6,9,100,200"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    byListBoxxy->Add( listCtrl, 1, wxALL|wxEXPAND, 5 );

    byListPanel->SetSizer( byListBoxxy );
    byListPanel->Layout();
    byListBoxxy->Fit( byListPanel );
    divNotebook->AddPage( byListPanel, wxT(byListTxt), false );    // Txt: "By list"

    divBoxxy->Add( divNotebook, 0, wxEXPAND | wxALL, 5 );

    dimBoxxy->Add( divBoxxy, 0, wxEXPAND, 5 );

    wxBoxSizer* dumpBoxxy;
    dumpBoxxy = new wxBoxSizer( wxVERTICAL );

    dumpCheck = new wxCheckBox( mainPanel, wxID_ANY, wxT(dumpResTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Dump results to file"
    dumpBoxxy->Add( dumpCheck, 0, wxALL, 5 );

    // Gets the current directory.
    string outFilePath = GetWorkingDirectory();
#ifdef WIN32
    outFilePath += "\\dump.csv";
#elif __linux__
    outFilePath += "/dump.csv";
#endif

    filePathCtrl = new wxTextCtrl( mainPanel, wxID_ANY, wxString(outFilePath.c_str(), wxConvUTF8), wxDefaultPosition, wxDefaultSize, 0 );
    filePathCtrl->Enable( false );

    dumpBoxxy->Add( filePathCtrl, 0, wxALL|wxEXPAND, 5 );

    dimBoxxy->Add( dumpBoxxy, 0, wxEXPAND, 5 );

    wxBoxSizer* plotBoxxy;
    plotBoxxy = new wxBoxSizer( wxHORIZONTAL );

    dataCheck = new wxCheckBox( mainPanel, wxID_ANY, wxT(plotDataTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Plot data"
    plotBoxxy->Add( dataCheck, 0, wxALL, 5 );

    dataFitCheck = new wxCheckBox( mainPanel, wxID_ANY, wxT(plotFitted), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Plot fitted data"
    plotBoxxy->Add( dataFitCheck, 0, wxALL, 5 );

    dimBoxxy->Add( plotBoxxy, 0, wxEXPAND, 5 );

    wxBoxSizer* buttonBoxxy;
    buttonBoxxy = new wxBoxSizer( wxHORIZONTAL );

    calcButton = new wxButton( mainPanel, wxID_ANY, wxT(calculateTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Calculate"
    buttonBoxxy->Add( calcButton, 1, wxALL|wxALIGN_BOTTOM, 5 );

    closeButton = new wxButton( mainPanel, wxID_ANY, wxT(closeTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Close"
    buttonBoxxy->Add( closeButton, 1, wxALL|wxALIGN_BOTTOM, 5 );

    helpButton = new wxBitmapButton( mainPanel, wxID_ANY, wxBitmap( GetWxAbsPath("Resources/help.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    buttonBoxxy->Add( helpButton, 0, wxALIGN_BOTTOM|wxALL, 5 );

    dimBoxxy->Add( buttonBoxxy, 1, wxEXPAND, 5 );

    paramBoxxy->Add( dimBoxxy, 1, wxEXPAND, 5 );

    subMainBoxxy->Add( paramBoxxy, 1, wxEXPAND, 5 );

    wxBoxSizer* outputBoxxy;
    outputBoxxy = new wxBoxSizer( wxVERTICAL );

    //
    previewImage = new ImagePanel(mainPanel, wxID_ANY, previewSize);
    outputBoxxy->Add( previewImage, 0, wxALL, 5 );

    outLine = new wxStaticLine( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    outputBoxxy->Add( outLine, 0, wxEXPAND | wxALL, 5 );

    logCtrl = new wxRichTextCtrl( mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
    outputBoxxy->Add( logCtrl, 1, wxALL|wxEXPAND, 5 );

    progressBar = new wxGauge( mainPanel, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    outputBoxxy->Add( progressBar, 0, wxALL|wxEXPAND, 5 );

    progressTxt = new wxStaticText( mainPanel, wxID_ANY, wxT(progStoppedTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Progress: Stopped"
    progressTxt->Wrap( -1 );
    outputBoxxy->Add( progressTxt, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

    subMainBoxxy->Add( outputBoxxy, 1, wxEXPAND, 5 );

    mainPanel->SetSizer( subMainBoxxy );
    mainPanel->Layout();
    subMainBoxxy->Fit( mainPanel );
    mainBoxxy->Add( mainPanel, 1, wxEXPAND | wxALL, 1 );

    this->SetSizer( mainBoxxy );
    this->Layout();

    this->Centre( wxBOTH );

    // Set welcome log text.
    logCtrl->WriteText(wxT(dimCalcLogTxt));    // Txt: "Dimension calculator log.\n\n"

    this->GetScriptFractals();

    // Set the default fractal.
    fractalChoice->SetSelection( 0 );
    this->CreateFractal(previewSize);
    myOpt = target->GetOptions();
    minXCtrl->SetValue(num_to_string(myOpt.minX));
    maxXCtrl->SetValue(num_to_string(myOpt.maxX));
    minYCtrl->SetValue(num_to_string(myOpt.minY));
    maxYCtrl->SetValue(num_to_string(myOpt.maxY));
    iterCtrl->SetValue(num_to_string((int)myOpt.maxIter));

    // Connect Events.
    this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DimensionFrame::OnDestroy ) );
    this->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DimensionFrame::OnUpdateUI ) );
    this->Connect( wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DimensionFrame::OnClose) );
    fractalChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DimensionFrame::OnChangeFractal ), NULL, this );
    dumpCheck->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DimensionFrame::OnChangeDump ), NULL, this );
    fOptButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnFractalOpt ), NULL, this );
    previewButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnRenderPreview ), NULL, this );
    calcButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnCalculate ), NULL, this );
    manualMaxYChk->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DimensionFrame::OnManualMaxY ), NULL, this );
    closeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnClose ), NULL, this );
    savePreviewButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnSavePreview ), NULL, this );
    helpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnHelp ), NULL, this );
}
DimensionFrame::~DimensionFrame()
{
    // Disconnect Events.
    this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DimensionFrame::OnDestroy ) );
    this->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DimensionFrame::OnUpdateUI ) );
    fractalChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DimensionFrame::OnChangeFractal ), NULL, this );
    dumpCheck->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DimensionFrame::OnChangeDump ), NULL, this );
    fOptButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnFractalOpt ), NULL, this );
    previewButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnRenderPreview ), NULL, this );
    calcButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnCalculate ), NULL, this );
    manualMaxYChk->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DimensionFrame::OnManualMaxY ), NULL, this );
    closeButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnClose ), NULL, this );
    savePreviewButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnSavePreview ), NULL, this );
    helpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DimensionFrame::OnHelp ), NULL, this );

    if(calculatingDimension)
    {
        // Stop render.
        target->StopRender();

        // Stop all.
        for(int i=0; i<threadNumber; i++)
        {
            if(dimCalculator[i].IsRunning())
                dimCalculator[i].Terminate();
        }
        calcButton->SetLabel(wxT(calculateTxt));
        calculatingDimension = false;
    }
    if(renderingPreview)
    {
        target->StopRender();
    }

    delete[] dimCalculator;
    fractalHandler.DeleteFractal();
    delete confFractOptDialog;
}

void DimensionFrame::CreateFractal(int size)
{
    firstRender = true;
    int choice = fractalChoice->GetCurrentSelection();

    // Compiled fractals.
    switch(choice)
    {
    case FL_HENON_MAP:
        {
            fractalHandler.CreateFractal(HENON_MAP, size, size);
            break;
        }
    case FL_MANDELBROT:
        {
            fractalHandler.CreateFractal(MANDELBROT, size, size);
            break;
        }
    case FL_MANDELBROT_ZN:
        {
            fractalHandler.CreateFractal(MANDELBROT_ZN, size, size);
            break;
        }
    case FL_JULIA:
        {
            fractalHandler.CreateFractal(JULIA, size, size);
            break;
        }
    case FL_JULIA_ZN:
        {
            fractalHandler.CreateFractal(JULIA_ZN, size, size);
            break;
        }
    case FL_SINOIDAL:
        {
            fractalHandler.CreateFractal(SINOIDAL, size, size);
            break;
        }
    case FL_MEDUSA:
        {
            fractalHandler.CreateFractal(MEDUSA, size, size);
            break;
        }
    case FL_MANOWAR:
        {
            fractalHandler.CreateFractal(MANOWAR, size, size);
            break;
        }
    case FL_MANOWAR_JULIA:
        {
            fractalHandler.CreateFractal(MANOWAR_JULIA, size, size);
            break;
        }
    case FL_TRICORN:
        {
            fractalHandler.CreateFractal(TRICORN, size, size);
            break;
        }
    case FL_BURNING_SHIP:
        {
            fractalHandler.CreateFractal(BURNING_SHIP, size, size);
            break;
        }
    case FL_BURNING_SHIP_JULIA:
        {
            fractalHandler.CreateFractal(BURNING_SHIP_JULIA, size, size);
            break;
        }
    case FL_FRACTORY:
        {
            fractalHandler.CreateFractal(FRACTORY, size, size);
            break;
        }
    case FL_CELL:
        {
            fractalHandler.CreateFractal(CELL, size, size);
            break;
        }
    case FL_MAGNET:
        {
            fractalHandler.CreateFractal(MAGNET, size, size);
            break;
        }
    case FL_DOUBLE_PENDULUM:
        {
            fractalHandler.CreateFractal(DOUBLE_PENDULUM, size, size);
            break;
        }
    case FL_LOGISTIC_MAP:
        {
            fractalHandler.CreateFractal(LOGISTIC, size, size);
            break;
        }
    }

    // Script fractals.
    if(choice >= COUNT)
    {
        int idx = choice - COUNT;
        fractalHandler.CreateScriptFractal(size, size, scriptDataVect[scriptList[idx]]);
        scriptSelected = true;
    }
    else scriptSelected = false;

    target = fractalHandler.GetTarget();
    if(confFractOptDialog != NULL)
        confFractOptDialog->SetNewTarget(target);
}
void DimensionFrame::OnRenderPreview( wxCommandEvent& event )
{
    if(!renderingPreview)
    {
        target->Resize(previewSize, previewSize);
        myOpt = target->GetOptions();

        myOpt.minX = string_to_double(minXCtrl->GetValue());
        myOpt.maxX = string_to_double(maxXCtrl->GetValue());
        myOpt.minY = string_to_double(minYCtrl->GetValue());

        if(manualMaxYChk->GetValue())
            myOpt.maxY = string_to_double(maxYCtrl->GetValue());
        else
            myOpt.maxY = myOpt.minY+(myOpt.maxX-myOpt.minX);

        myOpt.maxIter = string_to_int(iterCtrl->GetValue());

        target->SetOptions(myOpt);
        target->PrepareRender();
        target->Render();

        this->WriteText(wxT(startRenderPrevTxt));    // Txt: "Starting to render preview\n"
        previewButton->SetLabel(wxT(stopTxt));    // Txt: "Stop"
        calcButton->Enable(false);
        savePreviewButton->Enable(false);
        if(scriptSelected) progressBar->Enable(false);
        renderingPreview = true;
    }
    else
    {
        target->StopRender();
        logCtrl->WriteText(wxT(prevRenderStopTxt));  // Txt: "Preview render stopped\n"
        previewButton->SetLabel(wxT(renderPrevTxt));    // Txt: "Render preview"
        progressBar->SetValue(0);
        progressTxt->SetLabel(wxString( wxT(progStoppedTxt)));    // Txt: "Progress: Stopped"
        calcButton->Enable(true);
        savePreviewButton->Enable(true);
        if(scriptSelected) progressBar->Enable(true);
        renderingPreview = false;
    }
}
void DimensionFrame::OnChangeFractal( wxCommandEvent& event )
{
    // Set default parameters.
    this->CreateFractal(previewSize);
    myOpt = target->GetOptions();
    minXCtrl->SetValue(num_to_string(myOpt.minX));
    maxXCtrl->SetValue(num_to_string(myOpt.maxX));
    minYCtrl->SetValue(num_to_string(myOpt.minY));
    maxYCtrl->SetValue(num_to_string(myOpt.maxY));
    iterCtrl->SetValue(num_to_string((int)myOpt.maxIter));
}
void DimensionFrame::OnManualMaxY( wxCommandEvent& event )
{
    if(manualMaxYChk->GetValue())
        maxYCtrl->Enable(true);
    else
        maxYCtrl->Enable(false);
}
void DimensionFrame::OnChangeDump( wxCommandEvent& event )
{
    if(dumpCheck->GetValue())
        filePathCtrl->Enable(true);
    else
        filePathCtrl->Enable(false);
}
void DimensionFrame::OnClose( wxCommandEvent& event )
{
    this->Close(true);
    this->Destroy();
}
void DimensionFrame::OnCalculate( wxCommandEvent& event )
{
    if(!calculatingDimension)
    {
        // Create divisions vector.
        div.clear();
        mup::ParserX parser;
    #ifdef UNICODE
        parser.SetExpr(funcCtrl->GetValue().wc_str());
    #else
        parser.SetExpr(string(funcCtrl->GetValue().mb_str()));
    #endif
        mup::Value xVal;
        parser.DefineVar(_T("x"), mup::Variable(&xVal));
        int xMin = xMinSpin->GetValue();
        int xMax = xMaxSpin->GetValue();

        bool errorStatus = false;
        if(divNotebook->GetSelection() == 0)
        {
            // Get div values from parser.
            try
            {
                for(int x=xMin; x<xMax; x++)
                {
                    xVal = x;
                    div.push_back(parser.Eval().GetFloat());
                }
            }
            catch(mup::ParserError &error)
            {
                wxString err;
                err = error.GetMsg();
                logCtrl->WriteText(wxT(errorParserTxt));    // Txt: "Parser error: "
                logCtrl->WriteText(err);
                logCtrl->WriteText(wxT("\n"));
                errorStatus = true;
            }
        }
        else
        {
            wxString listNumbers = listCtrl->GetValue();
            div = get_int_list(listNumbers);
            if(div.size() == 0) errorStatus = true;
        }

        if(!errorStatus)
        {
            // Create fractal.
            size = string_to_int(wxString(sizeCtrl->GetValue()));
            myOpt = target->GetOptions();
            myOpt.minX = string_to_double(minXCtrl->GetValue());
            myOpt.maxX = string_to_double(maxXCtrl->GetValue());
            myOpt.minY = string_to_double(minYCtrl->GetValue());

            if(manualMaxYChk->GetValue())
                myOpt.maxY = string_to_double(maxYCtrl->GetValue());
            else
                myOpt.maxY = myOpt.minY+(myOpt.maxX-myOpt.minX);

            myOpt.maxIter = string_to_int(iterCtrl->GetValue());

            // Compare with previous options.
            Options tempOpt = target->GetOptions();

            if( tempOpt.minX != myOpt.minX || tempOpt.maxX != myOpt.maxX || tempOpt.minY != myOpt.minY ||
                tempOpt.maxY != myOpt.maxY || tempOpt.maxIter != myOpt.maxIter || tempOpt.screenWidth != size  ||
                target->GetChangeFractalProp() || firstRender )
            {
                // If a change was made or the render fractal was just created.
                target->Resize(size, size);
                target->SetOptions(myOpt);
                target->PrepareRender();
                target->Render();
                firstRender = false;
            }

            // Divide thread assignment.
            int sizeDiv = size/threadNumber;
            for(int i=0; i<threadNumber; i++)
            {
                if(i < threadNumber-1)
                    dimCalculator[i].SetMap(target->GetSetMap(), size, i*sizeDiv, (i+1)*sizeDiv);
                else
                    dimCalculator[i].SetMap(target->GetSetMap(), size, i*sizeDiv, size);
            }

            if(div.size() > 0)
            {
                divIndex = -1;
                calcButton->SetLabel(wxT(stopTxt));    // Txt: "Stop"
                previewButton->Enable(false);
                savePreviewButton->Enable(false);
                if(scriptSelected) progressBar->Enable(false);
                calculatingDimension = true;
            }
        }
    }
    else
    {
        // Stop render.
        target->StopRender();

        // Stop all.
        for(int i=0; i<threadNumber; i++)
        {
            if(dimCalculator[i].IsRunning())
                dimCalculator[i].Terminate();
        }
        calcButton->SetLabel(wxT(calculateTxt));    // Txt: "Calculate"
        previewButton->Enable(true);
        savePreviewButton->Enable(true);
        if(scriptSelected) progressBar->Enable(true);
        logCtrl->WriteText(wxT(calcStoppedTxt));    // Txt: "Calculation stopped\n"
        progressBar->SetValue(0);
        progressTxt->SetLabel(wxString( wxT(calcStoppedTxt)));    // Txt: "Calculation stopped\n"
        calculatingDimension = false;
        firstRender = true;
    }
}
void DimensionFrame::OnUpdateUI( wxUpdateUIEvent& event )
{
    if(clock.GetElapsedTime() >= 0.05)
    {
        if(renderingPreview)
        {
            if(target->IsRendering())
            {
                // Update progress while rendering preview.
                progress = target->GetWatchdog()->GetThreadProgress();
                if(scriptSelected)
                    progressTxt->SetLabel(wxString( wxT(renderingTxt)));    // Txt: "Rendering"
                else
                {
                    progressBar->SetValue(progress);
                    progressTxt->SetLabel(wxString(wxT(progressLblTxt)) + num_to_string(progress) +wxT("%") );    // Txt: "Progress: "
                }
            }
            else
            {
                // Set output image.
                previewImage->SetMap(target->GetSetMap(), nDivSpin->GetValue());
                previewImage->Refresh();
                progressBar->SetValue(0);
                progressTxt->SetLabel(wxString( wxT(progDoneTxt)));    // Txt: "Progress: Done"
                this->WriteText(wxT(doneTxt));    // Txt: "Done\n"
                previewButton->SetLabel(wxT(renderPrevTxt));    // Txt: "Render preview"
                calcButton->Enable(true);
                savePreviewButton->Enable(true);
                if(scriptSelected) progressBar->Enable(true);
                renderingPreview = false;
            }
        }
        if(calculatingDimension)
        {
            if(!target->IsRendering())
            {
                // Check if there are threads running.
                bool threadRunning = false;
                for(int i=0; i<threadNumber; i++)
                {
                    if(dimCalculator[i].IsRunning())
                        threadRunning = true;
                }

                if(!threadRunning)
                {
                    if(divIndex == -1)
                    {
                        // Launch the first pack of threads.
                        this->WriteText(wxT(startBCountTxt));    // Txt: "Starting box count.\n"
                        this->WriteText(wxT(epsilonCountTxt));    // Txt: "Epsilon   |   BoxCount.\n"
                    #ifdef _WIN32
                        this->WriteText(wxT("-------------------\n"));
                    #elif __linux__
                        this->WriteText(wxT("--------------------------\n"));
                    #endif
                        divIndex++;
                        for(int i=0; i<threadNumber; i++)
                        {
                            dimCalculator[i].SetDiv(div[divIndex]);
                            dimCalculator[i].Launch();
                        }

                        epsilon.clear();
                        boxCount.clear();
                    }
                    else if(divIndex < div.size()-1)
                    {
                        // Update progress bar.
                        progress = 50*(1 + (double)divIndex/(double)div.size());
                        progressBar->SetValue(progress);
                        progressTxt->SetLabel(wxString(wxT(progressLblTxt)) + num_to_string(progress) + wxT("%") );    // Txt: "Progress: "

                        // Get box count.
                        int boxNumber = 0;
                        for(int i=0; i<threadNumber; i++)
                        {
                            boxNumber += dimCalculator[i].GetBoxCount();
                        }
                        epsilon.push_back((double)size/(double)div[divIndex]);
                        boxCount.push_back(boxNumber);

                        // Update log text.
                        wxString logOut = num_to_string(epsilon[divIndex]);
                        logOut += wxT(", ");
                        logOut += num_to_string(boxCount[divIndex]);
                        logOut += wxT("\n");
                        this->WriteText(logOut);

                        // Prepare new size and launch.
                        divIndex++;
                        for(int i=0; i<threadNumber; i++)
                        {
                            dimCalculator[i].SetDiv(div[divIndex]);
                            dimCalculator[i].Launch();
                        }
                    }
                    else
                    {
                        if(dumpCheck->GetValue())
                        {
                            // Dump results to file.
                            ofstream file;
                            file.open(string(filePathCtrl->GetValue().mb_str()).c_str(), ios::out);
                            for(unsigned int i=0; i< epsilon.size(); i++)
                            {
                                file << epsilon[i] << ", " << boxCount[i] << endl;
                            }
                        }

                        // Calculate dimension.
                        vector<double> logEpsilon, logCount;
                        for(unsigned int i=0; i<epsilon.size(); i++)
                        {
                            logEpsilon.push_back(log(1.0/epsilon[i]));
                            logCount.push_back(log((double)boxCount[i]));
                        }
                        // Do least squares fitting for m.
                        double n, sumXY, sumX, sumY, sumXSquared;
                        sumXY = sumX = sumY = sumXSquared = 0;
                        n = static_cast<double>(epsilon.size());
                        for(int i=0; i<n; i++)
                        {
                            sumXY += logEpsilon[i]*logCount[i];
                            sumX += logEpsilon[i];
                            sumY += logCount[i];
                            sumXSquared += pow(logEpsilon[i], 2);
                        }
                        double dimensionFit = (n*sumXY - sumX*sumY)/(n*sumXSquared - pow(sumX,2));
                        this->WriteText(wxT(dimensionTxt));    // Txt: "Dimension = "
                        this->WriteText(num_to_string(dimensionFit));
                        this->WriteText(wxT("\n"));

                        // Least squares for b.
                        double b = (sumY*sumXSquared - sumX*sumXY)/(n*sumXSquared - pow(sumX,2));

                        // Draw Plot
                        if(dataCheck->GetValue())
                        {
                            vector<double> doubleCount;
                            for(unsigned i=0; i<boxCount.size(); i++)
                            {
                                doubleCount.push_back(static_cast<double>(boxCount[i]));
                            }
                            PlotWindow *plot = new PlotWindow(epsilon, doubleCount, this, wxID_ANY, wxT(dataPlotTxt));    // Txt: "Data plot"
                            plot->Show(true);
                        }

                        // Draw fitted plot.
                        if(dataFitCheck->GetValue())
                        {
                            LineParams myLine;
                            myLine.m = dimensionFit;
                            myLine.b = b;
                            PlotWindow *plot = new PlotWindow(myLine, logEpsilon, logCount, this, wxID_ANY, wxT(fittedPlotTxt));    // Txt: "Fitted data plot"
                            plot->Show(true);
                        }

                        // Update progress bar.
                        progressBar->SetValue(0);
                        progressTxt->SetLabel(wxString( wxT(progDoneTxt)));    // Txt: "Progress: Done"
                        this->WriteText(wxT(doneTxt));    // Txt: "Done\n"

                        calcButton->SetLabel(wxT(calculateTxt));    // Txt: "Calculate"
                        previewButton->Enable(true);
                        savePreviewButton->Enable(true);
                        if(scriptSelected) progressBar->Enable(true);
                        calculatingDimension = false;
                    }
                }
            }
            else
            {
                // Updates progress bar while rendering.
                progress = target->GetWatchdog()->GetThreadProgress()/2;

                if(scriptSelected)
                    progressTxt->SetLabel(wxString( wxT(calculatingTxt)));    // Txt: "Calculating dimension"
                else
                {
                    progressBar->SetValue(progress);
                    progressTxt->SetLabel(wxString(wxT(progressLblTxt)) + num_to_string(progress) +wxT("%") );    // Txt: "Progress: "
                }
            }
        }
        clock.Reset();
    }
}
void DimensionFrame::OnDestroy( wxCloseEvent& event )
{
    dimFrameState = false;
    this->Destroy();
}
void DimensionFrame::WriteText(wxString txt)
{
    logCtrl->MoveEnd();
    logCtrl->WriteText(txt);
    logCtrl->ShowPosition(logCtrl->GetCaretPosition());
}
void DimensionFrame::OnFractalOpt( wxCommandEvent& event )
{
    if(confFractOptDialog == NULL)
        confFractOptDialog = new ConfFractOptDialog(target, this);

    if(confFractOptDialog->IsVisible())
        confFractOptDialog->SetFocus();
    else
        confFractOptDialog->Show(true);

    // Adjust position.
    int h, w;
    GetDesktopResolution(h, w);
    if(this->GetPosition().x+this->GetSize().GetWidth()+5 < w && this->GetPosition().y < h)
        confFractOptDialog->Move(this->GetPosition().x+this->GetSize().GetWidth()+5, this->GetPosition().y);
}
void DimensionFrame::OnSavePreview( wxCommandEvent& event )
{
    wxFileDialog * openFileDialog = new wxFileDialog(this, wxT(menuSelectFileTxt), wxT(""),
                                                    wxT("dimension_preview.bmp"), wxT("BMP file (*.bmp)|*.bmp"), wxFD_SAVE);    // Txt: "Select file name"
    if(openFileDialog->ShowModal() == wxID_OK)
    {
        wxString wxFileName = openFileDialog->GetPath();
        wxString fileName = wxFileName.c_str();

        // Render the fractal.
        size = string_to_int(wxString(sizeCtrl->GetValue().c_str()));
        target->Resize(size, size);
        myOpt = target->GetOptions();
        myOpt.minX = string_to_double(minXCtrl->GetValue());
        myOpt.maxX = string_to_double(maxXCtrl->GetValue());
        myOpt.minY = string_to_double(minYCtrl->GetValue());

        if(manualMaxYChk->GetValue())
            myOpt.maxY = string_to_double(maxYCtrl->GetValue());
        else
            myOpt.maxY = myOpt.minY+(myOpt.maxX-myOpt.minX);

        myOpt.maxIter = string_to_int(iterCtrl->GetValue());

        target->SetOptions(myOpt);

        target->PrepareRender();
        target->Render();

        SaveProgressDiag *saveProgress = new SaveProgressDiag(target, this, false);
        saveProgress->ShowModal();

        if(saveProgress->IsFinished())
        {
            // Allocate.
            bool **setMap, **tempSetMap, **colorMap;
            setMap = target->GetSetMap();
            int nDiv = nDivSpin->GetValue();

            tempSetMap = new bool*[size];
            colorMap = new bool*[size];
            for(int i=0; i<size; i++)
            {
                tempSetMap[i] = new bool[size];
                colorMap[i] = new bool[size];
            }
            // Copy and init.
            for(int i=0; i<size; i++)
            {
                for(int j=0; j<size; j++)
                {
                    tempSetMap[i][j] = setMap[i][j];
                    colorMap[i][j] = false;
                }
            }

            //int N = 0;
            double epsilon = (double)size/(double)nDiv;

            for(int ey=0; ey<nDiv; ey++)
            {
                for(int ex=0; ex<nDiv; ex++)
                {
                    bool found = false;
                    for(int w=ex*epsilon; w<(ex+1)*epsilon && !found; w++)
                    {
                        for(int h=ey*epsilon; h<(ey+1)*epsilon; h++)
                        {
                            if(w < size && h < size)
                            {
                                if(setMap[w][h] == true)
                                {
                                    found = true;
                                    // Color square
                                    for(int y=ey*epsilon; y<(ey+1)*epsilon; y++)
                                    {
                                        for(int x=ex*epsilon; x<(ex+1)*epsilon; x++)
                                        {
                                            if(x < size && y < size)
                                            {
                                                colorMap[x][y] = true;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            else break;
                        }
                    }
                }
            }

            // Horizontal lines.
            for(int ey=0; ey < nDiv; ey++)
            {
                int y = ey*epsilon;
                for(int x=0; x<size; x++)
                {
                    tempSetMap[x][y] = true;
                }
            }
            // Last line.
            for(int x=0; x<size; x++)
            {
                tempSetMap[x][size-1] = true;
            }

            // Vertical lines
            for(int ex=0; ex < nDiv; ex++)
            {
                int x = ex*epsilon;
                for(int y=0; y<size; y++)
                {
                    tempSetMap[x][y] = true;
                }
            }
            // Last line.
            for(int y=0; y<size; y++)
            {
                tempSetMap[size-1][y] = true;
            }

            // Write BMP.
            BMPWriter writer(fileName.mb_str(), size, size);
            BMPPixel* data = new BMPPixel[size];

            // Copy maps values to BMPWriter.
            for(int j=size-1; j>=0; j--)
            {
                for(int i=0; i<size; i++)
                {
                    if(tempSetMap[i][j] == true)
                    {
                        data[i].r = 0;
                        data[i].g = 0;
                        data[i].b = 0;
                    }
                    else if(colorMap[i][j] == true)
                    {
                        data[i].r = static_cast<unsigned>(0x32);
                        data[i].g = static_cast<unsigned>(0x32);
                        data[i].b = static_cast<unsigned>(0xFF);
                    }
                    else
                    {
                        data[i].r = static_cast<unsigned>(0xFF);
                        data[i].g = static_cast<unsigned>(0xFF);
                        data[i].b = static_cast<unsigned>(0xFF);
                    }
                }
                writer.WriteLine(data);
            }
            writer.CloseBMP();
            delete[] data;

            // Cleanup.
            for(int i=0; i< size; i++)
            {
                delete[] tempSetMap[i];
                delete[] colorMap[i];
            }
            delete[] tempSetMap;
            delete[] colorMap;
        }
        saveProgress->Destroy();
    }
    openFileDialog->Destroy();
}
void DimensionFrame::GetScriptFractals()
{
    // Gets script parameters.
    for(unsigned int i=0; i<scriptDataVect.size(); i++)
    {
        if(!scriptDataVect[i].noSetMap)
        {
            scriptList.push_back(i);
            fractalChoice->Append(wxString(scriptDataVect[i].name.c_str(), wxConvUTF8));
        }
    }
}
void DimensionFrame::OnHelp( wxCommandEvent& event )
{
#ifdef __linux__
    HTMLViewer *diag = new HTMLViewer(GetWxAbsPath("Resources/Tutorials/dimTut.html"),
                                   this, wxID_ANY, wxString(wxT(calcDimHelpTxt)));    // Txt: "Calculate dimension help"
#elif _WIN32
    HTMLViewer *diag = new HTMLViewer(GetWxAbsPath("Resources\\Tutorials\\dimTut.html"),
                                   this, wxID_ANY, wxString(wxT(calcDimHelpTxt)));    // Txt: "Calculate dimension help"
#endif
    diag->ShowModal();
    diag->Destroy();
}
