#include "DPExplorer.h"
#include "StringFuncs.h"
#include "ConfigParser.h"
#include "HTMLViewer.h"
#include <wx/clipbrd.h>

std::vector<FormData> formulas;
bool dpExplorerState = false;

//DPSim
DPSim::DPSim(DPSimOpt myOpt)
{
    th1 = myOpt.th1;
    th2 = myOpt.th2;
    m1 = myOpt.m1;
    m2 = myOpt.m2;
    l = myOpt.l;
    g = myOpt.g;
    dt = myOpt.dt;
    vth1 = 0;
    vth2 = 0;
    th1Bailout = myOpt.th1Bailout;
    th1NumBailout = myOpt.th1NumBailout;
    th2Bailout = myOpt.th2Bailout;
    th2NumBailout = myOpt.th2NumBailout;

    isRunning = true;
    clock.Reset();
}
DPSim::~DPSim()
{

}
#ifdef _WIN32
void DPSim::Run()
{
    sf::Image icon;
    icon.LoadFromFile(GetAbsPath("Resources/iconPNG.png"));
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(450, 450, 32), dpSimTxt,
                                                        sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);    // Txt: "Double pendulum simulation"
    window->SetIcon(icon.GetWidth(), icon.GetHeight(), icon.GetPixelsPtr());
    window->SetFramerateLimit(300);

    int xRef = window->GetWidth()/2;
    float refPlano = 40;
    float scale = 5;

    sf::Image image;
    image.Create(window->GetWidth(), window->GetHeight(), sf::Color(255, 255, 255));
    sf::Sprite outImage;
    outImage.SetImage(image);

    // Info text.
    sf::String text, infoText;
    sf::Font font;
    font.LoadFromFile(GetAbsPath("Resources/DiavloFont.otf"));
    text.SetFont(font);
    infoText.SetFont(font);
    text.SetSize(18);
    infoText.SetSize(16);
    text.SetPosition(0, window->GetHeight()-20);
    infoText.SetPosition(window->GetWidth()-120, window->GetHeight()-40);
    text.SetColor(sf::Color(0,0,0));
    infoText.SetColor(sf::Color(0,0,0));
    text.SetText(eventReachTxt);    // Txt: "Event reached"
    infoText.SetText(fastSlowTxt);  // Txt: "UP: Faster\nDOWN: Slower"
    eventReached = false;

    sf::Event Event;
    int skip = 0;
    int skipNumber = 2;

    while (window->IsOpened())
    {
        // Perform calculations.
        ecMov1 = -g*(m1+m2)*sin(th1)+g*m2*sin(th2)*cos(th1-th2)-l*m2*(vth2*vth2)*sin(th1-th2)-l*m2*(vth1*vth1)*sin(th1-th2)*cos(th1-th2);
        ecMov1 /= l*(m1+m2)-l*m2*(pow(cos(th1-th2), 2));
        ecMov2 = (m1+m2)*l*(vth1*vth1)*sin(th1-th2)-(m1+m2)*g*sin(th2)+m2*l*(vth2*vth2)*sin(th1-th2)*cos(th1-th2)+(m1+m2)*g*sin(th1)*cos(th1-th2);
        ecMov2 /= l*(m1+m2)-l*m2*(pow(cos(th1-th2), 2));
        vth1 += dt*ecMov1;
        vth2 += dt*ecMov2;
        th1 += dt*vth1;
        th2 += dt*vth2;

        // Check for bailout conditions.
        if(th1Bailout)
        {
            if(abs(th1) > th1NumBailout) eventReached = true;
        }
        if(th2Bailout)
        {
            if(abs(th2) > th2NumBailout) eventReached = true;
        }

        if(skip > skipNumber)
        {
            skip = 0;

            // Process events.
            while (window->GetEvent(Event))
            {
                if (Event.Type == sf::Event::Closed)
                    window->Close();

                if(Event.Type == sf::Event::KeyPressed)
                {
                    if(Event.Key.Code == sf::Key::Up)
                    {
                        skipNumber++;
                    }
                    if(Event.Key.Code == sf::Key::Down)
                    {
                        if((skipNumber - 1) > 0) skipNumber--;
                    }
                }
            }

            window->Clear(sf::Color(255, 255, 255));

            // Draw predefined shapes.
            image.SetPixel(scale*l*sin(th1)+xRef, scale*(l*cos(th1)+refPlano), sf::Color::Red);
            image.SetPixel(scale*(l*sin(th1) + l*sin(th2)) + xRef, scale*(l*cos(th1) + refPlano + l*cos(th2)), sf::Color::Blue);
            window->Draw(outImage);
            window->Draw(sf::Shape::Line(xRef-20, refPlano*scale, xRef+20, refPlano*scale, 15, sf::Color::Black));    // Support.
            window->Draw(sf::Shape::Line(xRef, refPlano*scale, scale*l*sin(th1)+xRef, scale*(l*cos(th1)+refPlano), 5, sf::Color::Black));    // Line 1.
            window->Draw(sf::Shape::Line(scale*l*sin(th1)+xRef, scale*l*cos(th1)+refPlano*scale,
                                        scale*(l*sin(th1) + l*sin(th2)) + xRef, scale*(l*cos(th1) + refPlano + l*cos(th2)), 5, sf::Color::Black));        // Line 2.
            window->Draw(sf::Shape::Circle(scale*l*sin(th1)+xRef, scale*(l*cos(th1)+refPlano), 15, sf::Color::Red, 3));    // Ball 1.
            window->Draw(sf::Shape::Circle(scale*(l*sin(th1) + l*sin(th2)) + xRef, scale*(l*cos(th1) + refPlano + l*cos(th2)), 15, sf::Color::Blue, 3));    // Ball 2.

            if(eventReached)
                window->Draw(text);

            window->Draw(infoText);

            window->Display();
        }
        else skip++;
    }
    delete window;
}
#elif __linux__
void DPSim::Run()
{
    eventReached = false;

    while(isRunning)
    {
        if(clock.GetElapsedTime() >= 0.0005)
        {
            // Perform calculations.
            ecMov1 = -g*(m1+m2)*sin(th1)+g*m2*sin(th2)*cos(th1-th2)-l*m2*(vth2*vth2)*sin(th1-th2)-l*m2*(vth1*vth1)*sin(th1-th2)*cos(th1-th2);
            ecMov1 /= l*(m1+m2)-l*m2*(pow(cos(th1-th2), 2));
            ecMov2 = (m1+m2)*l*(vth1*vth1)*sin(th1-th2)-(m1+m2)*g*sin(th2)+m2*l*(vth2*vth2)*sin(th1-th2)*cos(th1-th2)+(m1+m2)*g*sin(th1)*cos(th1-th2);
            ecMov2 /= l*(m1+m2)-l*m2*(pow(cos(th1-th2), 2));
            vth1 += dt*ecMov1;
            vth2 += dt*ecMov2;
            th1 += dt*vth1;
            th2 += dt*vth2;

            // Check for bailout conditions.
            if(th1Bailout)
            {
                if(abs(th1) > th1NumBailout) eventReached = true;
            }
            if(th2Bailout)
            {
                if(abs(th2) > th2NumBailout) eventReached = true;
            }

            clock.Reset();
        }
    }
}
#endif
void DPSim::Stop()
{
    isRunning = false;
}


// DPSimDialog
DPSimFrame::DPSimFrame(DPSimOpt myOpt, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    this->SetSizeHints( wxSize( 450,450 ), wxSize( 450,450 ) );

    dpSim = new DPSim(myOpt);
    dpSim->Launch();
    l = myOpt.l;

    this->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DPSimFrame::OnUpdateUI ) );
}
DPSimFrame::~DPSimFrame()
{
    dpSim->Stop();
    dpSim->Wait();
    delete dpSim;
}
void DPSimFrame::OnUpdateUI( wxUpdateUIEvent& event )
{
    this->Refresh();
    wxPaintDC dc(this);

    int xRef = this->GetSize().GetWidth()/2;
    float refPlano = 35;
    float scale = 5;

    wxBrush brush;
    wxPen pen(wxColour(0,0,0), 7);
    dc.SetPen(pen);

    dc.DrawLine(xRef-20, refPlano*scale, xRef+20, refPlano*scale);  // Support bar.

    pen.SetWidth(2);
    dc.SetPen(pen);
    dc.DrawLine(xRef, refPlano*scale, scale*l*sin(dpSim->th1)+xRef+15, scale*(l*cos(dpSim->th1)+refPlano)+15);  // Line 1.
    dc.DrawLine(scale*l*sin(dpSim->th1)+xRef+15, scale*l*cos(dpSim->th1)+refPlano*scale+15,
                scale*(l*sin(dpSim->th1) + l*sin(dpSim->th2)) + xRef+15, scale*(l*cos(dpSim->th1) + refPlano + l*cos(dpSim->th2))+15);  // Line 2.

    brush.SetColour(wxColour(255,0,0));
    dc.SetBrush(brush);
    dc.DrawEllipse(scale*l*sin(dpSim->th1)+xRef, scale*(l*cos(dpSim->th1)+refPlano), 30, 30);   // Ball 1.
    brush.SetColour(wxColour(0,0,255));
    dc.SetBrush(brush);
    dc.DrawEllipse(scale*(l*sin(dpSim->th1) + l*sin(dpSim->th2)) + xRef, scale*(l*cos(dpSim->th1) + refPlano + l*cos(dpSim->th2)), 30, 30); // Ball 2.

    if(dpSim->eventReached) dc.DrawText(wxT(eventReachTxt), 330, 430);
}

// ImagePanel.
DPImagePanel::DPImagePanel(wxPanel *parent, int id, wxBitmap image) : wxPanel(parent, id)
{
    bmp = image;
    coordIndex = 0;
    plotIndex = 0;
    th1Fact = 0;
    th2Fact = 0;
    plotError = false;
    this->SetSize(bmp.GetWidth(), bmp.GetHeight());
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(DPImagePanel::OnPaint));
}
void DPImagePanel::OnPaint(wxPaintEvent& event)
{
    // Draw the bitmap and the points (in case they exist).
    wxPaintDC dc(this);
    dc.DrawBitmap(bmp, 0, 0);

    for(int n=0; n<coordIndex; n++)
    {
        wxPen pen(wxColour(200, 200, 200), 1, wxSOLID);
        dc.SetPen(pen);
        int size = 25;
        int i = coordVector[n].coordX - size;
        int j = coordVector[n].coordY - size;
        if(i < 0) i = 0;
        if(j < 0) j = 0;
        while(i < (coordVector[n].coordX + size) && i < this->GetSize().GetWidth())
        {
            dc.DrawPoint(i, coordVector[n].coordY);
            i++;
        }
        while(j < (coordVector[n].coordY + size) && j < this->GetSize().GetHeight())
        {
            dc.DrawPoint(coordVector[n].coordX, j);
            j++;
        }
    }

    // Draw the plots.
    if(!plotError)
    {
        int i, j;
        wxPen pen(wxColour(200, 200, 200), 1, wxSOLID);
        dc.SetPen(pen);
        for(int n=0; n<plotIndex; n++)
        {

            mup::Value th1, th2;
            parser[n].DefineVar(_T("th1"), mup::Variable(&th1));
            parser[n].DefineVar(_T("th2"), mup::Variable(&th2));

            // Evaluate formula with the parser.
            try
            {
                if(plotType[n] == TH2ONTH1)
                {
                    for(i=0; i<this->GetSize().GetWidth(); i++)
                    {
                        th1 = th1Fact*(i - myRefTh1);
                        th2 = parser[n].Eval().GetFloat();
                        dc.DrawPoint(i, -th2.GetFloat()/th2Fact+myRefTh2);
                    }
                }
                else
                {
                    for(j=0; j<this->GetSize().GetHeight(); j++)
                    {
                        th2 = th2Fact*(j - myRefTh2);
                        th1 = parser[n].Eval().GetFloat();
                        dc.DrawPoint(th1.GetFloat()/th1Fact+myRefTh1, j);
                    }
                }
            }
            catch(mup::ParserError &error)
            {
                wxString err = wxT(errorFormulaTxt);    // Txt: "Error in parser: On formula "
                err += wxString(num_to_string(plotIndex));
                err += wxT("\n");
                err += wxString(error.GetMsg());
                wxMessageDialog *dial = new wxMessageDialog(NULL, err, wxT("Error"), wxOK | wxICON_ERROR);
                dial->ShowModal();
                plotError = true;
                break;
            }
        }
    }
}
void DPImagePanel::SetBitmap(wxBitmap image)
{
    // Change bitmap and erase previous points.
    bmp = image;
    coordIndex = 0;
    coordVector.clear();
    colorVector.clear();
    plots.clear();
    parser.clear();
    plotType.clear();
    formulas.clear();
    plotIndex = 0;
    this->SetSize(bmp.GetWidth(), bmp.GetHeight());
    this->SetSizeHints(bmp.GetWidth(), bmp.GetHeight());
    this->Refresh();
}
int DPImagePanel::SetPoint(CoordDP coord, Color color)
{
    coordVector.push_back(coord);
    colorVector.push_back(color);
    coordIndex++;
    this->Refresh();
    return coordIndex-1;
}
void DPImagePanel::RemovePoint(int index)
{
    // If index wasn't specified erases the last on the list.
    if(index == -1)
    {
        coordVector.pop_back();
        colorVector.pop_back();
    }
    else
    {
        if(index >= 0 && index < coordIndex)
        {
            coordVector.erase(coordVector.begin()+coordIndex);
            colorVector.erase(colorVector.begin()+coordIndex);
        }
    }
    coordIndex--;
    this->Refresh();
}
void DPImagePanel::AddPlot(wxString plot, double th1Factor, double th2Factor, PLOT_TYPE type, int refTh1, int refTh2)
{
    th1Fact = th1Factor;
    th2Fact = th2Factor;
    myRefTh1 = refTh1;
    myRefTh2 = refTh2;

    mup::ParserX p;
    //p.SetExpr(plot.c_str());
#if wxCHECK_VERSION(2, 9, 0)
    p.SetExpr(plot.wc_str());
#else
    p.SetExpr(plot.c_str());
#endif
    parser.push_back(p);
    plots.push_back(plot);
    plotType.push_back(type);
    plotIndex++;

    this->Refresh();
}
void DPImagePanel::RemovePlots()
{
    plots.clear();
    parser.clear();
    plotType.clear();
    plotIndex = 0;
    plotError = false;
    this->Refresh();
}

// EditFormDialog
EditFormDialog::EditFormDialog( wxWindow* parent, FormData* data, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : wxDialog( parent, id, title, pos, size, style )
{
    mData = data;
    this->SetSizeHints( EditFormDialogSize, EditFormDialogSize );

    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer( wxVERTICAL );

    mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* panelBoxxy;
    panelBoxxy = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* ctrlBoxxy;
    ctrlBoxxy = new wxBoxSizer( wxHORIZONTAL );

    funcLabel = new wxStaticText( mainPanel, wxID_ANY, wxT("Th1 = "), wxDefaultPosition, wxDefaultSize, 0 );
    funcLabel->Wrap( -1 );
    ctrlBoxxy->Add( funcLabel, 0, wxALL, 5 );

    ctrl = new wxTextCtrl( mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );

    ctrlBoxxy->Add( ctrl, 1, wxALL|wxEXPAND, 5 );

    panelBoxxy->Add( ctrlBoxxy, 2, wxEXPAND, 5 );

    staticLine = new wxStaticLine( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    panelBoxxy->Add( staticLine, 0, wxEXPAND | wxALL, 5 );

    wxBoxSizer* buttonBoxxy;
    buttonBoxxy = new wxBoxSizer( wxHORIZONTAL );

    acceptButton = new wxButton( mainPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Ok"
    buttonBoxxy->Add( acceptButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    cancelButton = new wxButton( mainPanel, wxID_ANY, wxT(cancelTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Cancel"
    buttonBoxxy->Add( cancelButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    th1on2btn = new wxRadioButton( mainPanel, wxID_ANY, wxT("Th1 = F(th2)"), wxDefaultPosition, wxDefaultSize, 0 );
    th1on2btn->SetValue( true );
    buttonBoxxy->Add( th1on2btn, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    th2on1btn = new wxRadioButton( mainPanel, wxID_ANY, wxT("Th2 = F(th1)"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonBoxxy->Add( th2on1btn, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    panelBoxxy->Add( buttonBoxxy, 1, wxEXPAND, 5 );

    mainPanel->SetSizer( panelBoxxy );
    mainPanel->Layout();
    panelBoxxy->Fit( mainPanel );
    mainBoxxy->Add( mainPanel, 1, wxEXPAND | wxALL, 1 );

    this->SetSizer( mainBoxxy );
    this->Layout();
    this->Centre( wxBOTH );

    // Previous formula.
    if(mData->formText.size() > 0)
    {
        // Sets function by type.
        ctrl->SetValue(wxString(mData->formText));
        if(mData->type == TH1ONTH2)
        {
            funcLabel->SetLabel(wxString(wxT("Th1 = ")));
            th1on2btn->SetValue(true);
        }
        else
        {
            funcLabel->SetLabel(wxString(wxT("Th2 = ")));
            th2on1btn->SetValue(true);
        }
    }

    acceptButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditFormDialog::OnAccept ), NULL, this );
    cancelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditFormDialog::OnCancel ), NULL, this );
    th1on2btn->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditFormDialog::OnTh1 ), NULL, this );
    th2on1btn->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditFormDialog::OnTh2 ), NULL, this );
}
EditFormDialog::~EditFormDialog()
{
    acceptButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditFormDialog::OnAccept ), NULL, this );
    cancelButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditFormDialog::OnCancel ), NULL, this );
}
void EditFormDialog::OnAccept(wxCommandEvent& event)
{
    mData->formText = ctrl->GetValue();
    if(th1on2btn->GetValue()) mData->type = TH1ONTH2;
    else mData->type = TH2ONTH1;
    this->Show(false);
    this->Destroy();
}
void EditFormDialog::OnCancel(wxCommandEvent& event)
{
    this->Show(false);
    this->Destroy();
}
void EditFormDialog::OnTh1(wxCommandEvent& event)
{
    funcLabel->SetLabel(wxT("Th1 = "));
}
void EditFormDialog::OnTh2(wxCommandEvent& event)
{
    funcLabel->SetLabel(wxT("Th2 = "));
}

// PlotDialog
PlotDialog::PlotDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer( wxVERTICAL );

    mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* panelBoxxy;
    panelBoxxy = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* listBoxxy;
    listBoxxy = new wxBoxSizer( wxVERTICAL );

    list = new wxListBox( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
    listBoxxy->Add( list, 1, wxALL|wxEXPAND, 5 );

    this->RedrawList();

    panelBoxxy->Add( listBoxxy, 3, wxEXPAND, 5 );

    staticLine = new wxStaticLine( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    panelBoxxy->Add( staticLine, 0, wxEXPAND | wxALL, 5 );

    wxBoxSizer* buttonBoxxy;
    buttonBoxxy = new wxBoxSizer( wxVERTICAL );

    okButton = new wxButton( mainPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Ok"
    buttonBoxxy->Add( okButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

    addButton = new wxButton( mainPanel, wxID_ANY, wxT(addTxt), wxDefaultPosition, wxDefaultSize, 0 );
    buttonBoxxy->Add( addButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

    editButton = new wxButton( mainPanel, wxID_ANY, wxT(editTxt), wxDefaultPosition, wxDefaultSize, 0 );
    buttonBoxxy->Add( editButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

    eraseButton = new wxButton( mainPanel, wxID_ANY, wxT(eraseTxt), wxDefaultPosition, wxDefaultSize, 0 );
    buttonBoxxy->Add( eraseButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

    panelBoxxy->Add( buttonBoxxy, 1, wxEXPAND, 5 );

    mainPanel->SetSizer( panelBoxxy );
    mainPanel->Layout();
    panelBoxxy->Fit( mainPanel );
    mainBoxxy->Add( mainPanel, 1, wxEXPAND | wxALL, 1 );

    this->SetSizer( mainBoxxy );
    this->Layout();
    this->Centre( wxBOTH );

    okButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlotDialog::OnOk ), NULL, this );
    addButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlotDialog::OnAdd ), NULL, this );
    editButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlotDialog::OnEdit ), NULL, this );
    eraseButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlotDialog::OnErase ), NULL, this );
}
PlotDialog::~PlotDialog()
{
    okButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlotDialog::OnOk ), NULL, this );
    addButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlotDialog::OnAdd ), NULL, this );
    editButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlotDialog::OnEdit ), NULL, this );
    eraseButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlotDialog::OnErase ), NULL, this );
}

void PlotDialog::OnOk(wxCommandEvent& event)
{
    this->Show(false);
    this->Destroy();
}
void PlotDialog::OnAdd(wxCommandEvent& event)
{
    FormData data;
    EditFormDialog* diag = new EditFormDialog(this, &data);
    diag->ShowModal();
    diag->Destroy();

    if(data.formText.size() > 0)
    {
        formulas.push_back(data);
        this->RedrawList();
    }
}
void PlotDialog::OnEdit(wxCommandEvent& event)
{
    if(formulas.size() > 0)
    {
        wxString text;
        int pos = list->GetSelection();

        FormData data = formulas[pos];
        EditFormDialog* diag = new EditFormDialog(this, &data);
        diag->ShowModal();
        diag->Destroy();

        if(data.formText.size() > 0)
        {
            formulas.erase(formulas.begin()+pos);
            formulas.push_back(data);
            this->RedrawList();
        }
    }
    else
    {
        wxMessageDialog *dial = new wxMessageDialog(NULL, wxT(addFormFirstTxt), wxT("Warn"), wxOK | wxICON_WARNING);    // Txt: "You need to add a formula first."
        dial->ShowModal();
        dial->Destroy();
    }
}
void PlotDialog::OnErase(wxCommandEvent& event)
{
    if(formulas.size() > 0)
    {
        int pos = list->GetSelection();
        if(pos != -1)
        {
            formulas.erase(formulas.begin()+pos);
        }
        this->RedrawList();
    }
    else
    {
        wxMessageDialog *dial = new wxMessageDialog(NULL, wxT(addFormFirstTxt), wxT("Warn"), wxOK | wxICON_WARNING);    // Txt: "You need to add a formula first."
        dial->ShowModal();
        dial->Destroy();
    }
}
void PlotDialog::RedrawList()
{
    list->Clear();
    if(formulas.size() > 0)
    {
        wxString text;
        for(int i=formulas.size()-1; i>=0; i--)
        {
            if(formulas[i].type == TH1ONTH2) text = wxT("Th1 = ");
            else text = wxT("Th2 = ");
            text += formulas[i].formText;
            list->InsertItems(1, &text, 0);
        }
    }
}

// DPFrame
DPFrame::DPFrame( wxString filePath, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxSize( 800,600 ), wxDefaultSize );
    th1Factor = 0;
    th2Factor = 0;
    locked = false;
    text = wxT("");

#ifdef __linux__
    wxIcon icon(GetWxAbsPath("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
#elif _WIN32
    wxIcon icon(GetWxAbsPath("Resources\\icon.ico"), wxBITMAP_TYPE_ICO);
#endif
    this->SetIcon(icon);

    mStatus = this->CreateStatusBar(1, wxST_SIZEGRIP, wxID_ANY);
    mToolbar = this->CreateToolBar(wxTB_HORIZONTAL, wxID_ANY);
    mToolbar->AddTool(ID_DPOPEN, wxT(openTxt), wxBitmap( GetWxAbsPath("Resources/open.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT(openTxt), wxT(infoOpenTxt));    // Txt: "Open"
    mToolbar->AddTool(ID_RUN, wxT(runTxt), wxBitmap( GetWxAbsPath("Resources/play.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT(simulationTxt), wxT(infoSimTxt));    // Txt: "Simulation"
    mToolbar->AddTool(ID_PLOT, wxT(plotTxt), wxBitmap( GetWxAbsPath("Resources/plot.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT(addPlotTxt), wxT(addPlotTxt));    // Txt: "Add plot"
    mToolbar->AddTool(ID_HELP, wxT(helpTxt), wxBitmap( GetWxAbsPath("Resources/help.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT(helpTxt), wxT(helpTxt));    // Txt: "Help"
    mToolbar->Realize();

    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer(wxVERTICAL);

    scrollPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    scrollPanel->SetScrollRate( 5, 5 );
    wxBoxSizer* imageBoxxy;
    imageBoxxy = new wxBoxSizer(wxVERTICAL);

    // Load default file.
    wxBitmap bmp;
    if(!bmp.LoadFile(filePath, wxBITMAP_TYPE_BMP)) allOk = false;
    image = new DPImagePanel(scrollPanel, wxID_ANY, bmp);

    // Load data with parser.
    if( !this->LoadFile(replace_ext(string(filePath.mb_str()), ".dpe")) ) allOk = false;

    th1Factor = (maxTh1-minTh1)/static_cast<double>(bmp.GetWidth());
    th2Factor = (maxTh2-minTh2)/static_cast<double>(bmp.GetHeight());

    imageBoxxy->Add(image, 0, wxALL, 5);

    scrollPanel->SetSizer(imageBoxxy);
    scrollPanel->Layout();
    imageBoxxy->Fit(scrollPanel);
    mainBoxxy->Add(scrollPanel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainBoxxy);
    this->Layout();

    this->Centre(wxBOTH);
    this->Connect(ID_DPOPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DPFrame::OnOpen));
    this->Connect(ID_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DPFrame::OnRun));
    this->Connect(ID_PLOT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DPFrame::OnPlot));
    this->Connect(ID_HELP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DPFrame::OnHelp));
    this->Connect(ID_DATTOCLIP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DPFrame::OnDataToClipboard));
    this->Connect(ID_GET_INFO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DPFrame::OnGetInfo));
    mToolbar->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler( DPFrame::OnLeaveTool ), NULL, this);
    image->Connect(wxEVT_MOTION, wxMouseEventHandler(DPFrame::OnMoveMouse), NULL, this );
    image->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(DPFrame::OnClick), NULL, this );
    image->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( DPFrame::OnContextMenu ), NULL, this );
}
DPFrame::~DPFrame()
{
    dpExplorerState = false;
}

void DPFrame::OnOpen(wxCommandEvent &event)
{
    wxFileDialog * openFileDialog = new wxFileDialog(this, wxT(selectOpenFile), wxT(""),
                                    wxT(""), wxT("BMP file (*.bmp)|*.bmp"), wxFD_OPEN);    // Txt: "Select file to open"
    wxString filePath;
    string path = string(filePath.mb_str());
    if(openFileDialog->ShowModal() == wxID_OK)
    {
        filePath = openFileDialog->GetPath();
        wxBitmap bmp;
        bmp.LoadFile(filePath, wxBITMAP_TYPE_BMP);
        image->Show(false);
        image->SetBitmap(bmp);
        image->Show(true);

        // Load data with parser.
        this->LoadFile(replace_ext(string(filePath.mb_str()), ".dpe"));
    }
    scrollPanel->FitInside();
    openFileDialog->Destroy();
}
void DPFrame::OnMoveMouse(wxMouseEvent &event)
{
    this->WriteStatusBar(CoordDP(event.GetPosition().x, event.GetPosition().y));
}
void DPFrame::OnClick(wxMouseEvent& event)
{
    // Image click event.
    if(locked)
    {
        image->RemovePoint();
    }
    else
    {
        selected = CoordDP(event.GetPosition().x, event.GetPosition().y);
        image->SetPoint(selected, Color(255, 20, 0));
    }
    locked = !locked;
    this->WriteStatusBar();
}
void DPFrame::OnRun(wxCommandEvent &event)
{
    if(locked)
    {
        // Launch simulation.
        DPSimOpt opt;
        opt.th1 = th1;
        opt.th2 = th2;
        opt.m1 = m1;
        opt.m2 = m2;
        opt.l = l;
        opt.g = g;
        opt.dt = dt;
        opt.th1Bailout = th1Bailout;
        opt.th2Bailout = th2Bailout;
        opt.th1NumBailout = th1NumBailout;
        opt.th2NumBailout = th2NumBailout;
#ifdef _WIN32
        sim = new DPSim(opt);
        sim->Launch();
#elif __linux__
        DPSimFrame* dpSimFrame = new DPSimFrame(opt, this);
        dpSimFrame->Show(true);
#endif
    }
    else
    {
        wxMessageDialog *dial = new wxMessageDialog(NULL, wxT(toLaunchWarnTxt), wxT("Warn"), wxOK | wxICON_WARNING);    // Txt: "To launch a simulation you must..."
        dial->ShowModal();
        dial->Destroy();
    }
}
void DPFrame::OnLeaveTool( wxMouseEvent& event )
{
    // Evento de puntero regresando a la imagen.
    text = wxT("th1: ");
    text += num_to_string(th1);
    text += wxT("   th2: ");
    text += num_to_string(th2);
    mStatus->SetLabel(text);
}
void DPFrame::OnHelp(wxCommandEvent &event)
{
#ifdef __linux__
    HTMLViewer *diag = new HTMLViewer(GetWxAbsPath("Resources/Tutorials/dpTut.html"), this, wxID_ANY, wxString(wxT(dpHelpTxt)));
#elif _WIN32
    HTMLViewer *diag = new HTMLViewer(GetWxAbsPath("Resources\\Tutorials\\dpTut.html"), this, wxID_ANY, wxString(wxT(dpHelpTxt)));
#endif
    diag->ShowModal();
    diag->Destroy();

}
void DPFrame::WriteStatusBar(CoordDP pos)
{
    // Update status bar.
    if(!locked)
    {
        if(pos.coordX != -1 || pos.coordY != -1)
        {
            th2 = maxTh2-th2Factor*static_cast<double>(pos.coordY);
            th1 = minTh1+th1Factor*static_cast<double>(pos.coordX);
        }
#ifdef UNICODE
        text = wxT("θ1: ");
        text += num_to_string(th1);
        text += wxT("   θ2: ");
        text += num_to_string(th2);
#else
        text = wxT("th1: ");
        text += num_to_string(th1);
        text += wxT("   th2: ");
        text += num_to_string(th2);
#endif
        text += wxT(mcPointTxt);    // Txt: "\t Make click on the image to select a point"
    }
    else
    {
#ifdef UNICODE
        text = wxT("θ1: ");
        text += num_to_string(th1);
        text += wxT("   θ2: ");
        text += num_to_string(th2);
#else
        text = wxT("th1: ");
        text += num_to_string(th1);
        text += wxT("   th2: ");
        text += num_to_string(th2);
#endif
        text += wxT(pointSelTxt);    // Txt: "\t Point selected"
    }
    mStatus->SetStatusText(text);
}
void DPFrame::OnPlot(wxCommandEvent &event)
{
    // Open formula editor dialog.
    PlotDialog* diag = new PlotDialog(this);
    diag->ShowModal();

    image->RemovePlots();
    for(unsigned int i=0; i<formulas.size(); i++)
    {
        image->AddPlot(formulas[i].formText, th1Factor, th2Factor, formulas[i].type, -minTh1/th1Factor, maxTh2/th2Factor);
    }
    diag->Destroy();
}
void DPFrame::OnContextMenu(wxMouseEvent& event)
{
    wxMenu* menu = new wxMenu();
    wxPoint point = event.GetPosition();
    menu->Append(ID_DATTOCLIP, wxT(copyCoordTxt));    // Txt: "Copy coord"
    menu->Append(ID_GET_INFO, wxT(getInfoTxt));    // Txt: "Get info"

#ifdef _WIN32
    PopupMenu(menu, point.x+7, point.y  + image->GetPosition().y+49); // TODO: Fix this.
#elif __linux__
    PopupMenu(menu);
#endif
}
void DPFrame::OnDataToClipboard(wxCommandEvent &event)
{
    if(locked)
    {
        if (wxTheClipboard->Open())
        {
            wxString out;
            out += num_to_string(th1);
            out += wxT(", ");
            out += num_to_string(th2);
            wxTheClipboard->SetData( new wxTextDataObject(out) );
            wxTheClipboard->Close();
        }
    }
    else
    {
        wxMessageDialog *dial = new wxMessageDialog(NULL, wxT(selectFirstTxt), wxT("Warn"), wxOK | wxICON_WARNING);    // Txt: "Select a point first."
        dial->ShowModal();
        dial->Destroy();
    }
}
void DPFrame::OnGetInfo(wxCommandEvent &event)
{
    wxString text;
    text = wxT(dpParamsTxt);    // Txt: "DP Parameters\n\n"
#ifdef UNICODE
    text += wxT("θ1= ");
    text += num_to_string(th1);
    text += wxT("\nθ2= ");
    text += num_to_string(th2);
#else
    text += wxT("th1= ");
    text += num_to_string(th1);
    text += wxT("\nth2= ");
    text += num_to_string(th2);
#endif
    text += wxT("\nm1= ");
    text += num_to_string(m1);
    text += wxT("\nm2= ");
    text += num_to_string(m2);
    text += wxT("\nl= ");
    text += num_to_string(l);
    text += wxT("\ng= ");
    text += num_to_string(g);
    text += wxT("\nIter= ");
    text += num_to_string(maxIter);
    text += wxT("\nDt= ");
    text += num_to_string(dt);
    wxMessageDialog *dial = new wxMessageDialog(NULL, text, wxT("Info"), wxOK | wxICON_QUESTION);
    dial->ShowModal();
    dial->Destroy();
}
bool DPFrame::LoadFile(string filePath)
{
    // Load data with parser.
    ConfigParser p(filePath.c_str());
    if(p.FileOpened())
    {
        p.DblArgToVar(minTh1, "minTh1", 0);
        p.DblArgToVar(maxTh1, "maxTh1", 0);
        p.DblArgToVar(minTh2, "minTh2", 0);
        p.DblArgToVar(maxTh2, "maxTh2", 0);
        p.DblArgToVar(m1, "Mass1", 0);
        p.DblArgToVar(m2, "Mass2", 0);
        p.DblArgToVar(l, "Longitude", 0);
        p.DblArgToVar(g, "Gravity", 0);
        p.DblArgToVar(th1Factor, "th1Factor", 0);
        p.DblArgToVar(th2Factor, "th2Factor", 0);
        p.DblArgToVar(maxIter, "maxIter", 4000);
        p.DblArgToVar(dt, "deltaT", 0.001);
        p.BoolArgToVar(th1Bailout, "th1Bailout", true);
        p.DblArgToVar(th1NumBailout, "th1NumBailout", 3.14159);
        p.BoolArgToVar(th2Bailout, "th2Bailout", true);
        p.DblArgToVar(th2NumBailout, "th2NumBailout", 3.14159);
        Event = wxT("");
        return true;
    }
    else
    {
        wxMessageDialog *dial = new wxMessageDialog(NULL, wxT(fileNotFoundTxt), wxT("Error"), wxOK | wxICON_ERROR);    // Txt: "Error: Info file not found."
        dial->ShowModal();
        dial->Destroy();

        minTh2 = 0;
        maxTh2 = 0;
        minTh1 = 0;
        maxTh1 = 0;
        th2Factor = 0;
        th1Factor = 0;
        return false;
    }
}
