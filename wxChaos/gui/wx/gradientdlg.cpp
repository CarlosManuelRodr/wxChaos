#include "gradientdlg.h"
#include "global.h"

IMPLEMENT_DYNAMIC_CLASS(wxGradientDialog, wxDialog)

BEGIN_EVENT_TABLE(wxGradientDialog, wxDialog)
    EVT_CLOSE(wxGradientDialog::OnCloseWindow)
    EVT_BUTTON(wxID_EDIT, wxGradientDialog::OnEditColor)
    EVT_BUTTON(wxID_DELETE, wxGradientDialog::OnDeleteColor)
END_EVENT_TABLE()

wxGradientDialog::wxGradientDialog()
{
}
wxGradientDialog::wxGradientDialog(wxWindow* parent, wxGradient grad)
{
    Create(parent, grad);
}
wxGradientDialog::~wxGradientDialog()
{
}
bool wxGradientDialog::Create(wxWindow* parent, wxGradient grad)
{
    if (!wxDialog::Create(parent, wxID_ANY, wxT(gradEditorTxt)))    // Txt: "Gradient editor"
        return false;
    CreateWidgets();
    stopsStatBmp->Connect(ID_STOPSAREA, wxEVT_LEFT_DOWN, wxMouseEventHandler(wxGradientDialog::OnStopsAreaClick), NULL, this);
    selectedColorStop = -1;
    m_gradient = new wxGradient(grad);
    paintGradient();
    paintStops();
    return true;
}
wxGradient wxGradientDialog::GetGradient()
{
    return *m_gradient;
}
int wxGradientDialog::ShowModal()
{
    return wxDialog::ShowModal();
}
void wxGradientDialog::CreateWidgets()
{
    topSizer = new wxBoxSizer(wxVERTICAL);

    gradientSizer = new wxStaticBoxSizer(wxVERTICAL, this, wxString(wxT(gradientTxt)));    // Txt: "Gradient"
    topSizer->Add(gradientSizer, 1, wxEXPAND | wxALL, 12);
    
    gradientStatBmp = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(300, 50), wxBORDER_SIMPLE );
    gradientSizer->Add(gradientStatBmp, 1, wxALL | wxALIGN_CENTER_HORIZONTAL);

    stopAreaSizer = new wxBoxSizer(wxHORIZONTAL);
    gradientSizer->Add(stopAreaSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 6);

    stopsStatBmp = new wxStaticBitmap(this, ID_STOPSAREA, wxNullBitmap, wxDefaultPosition, wxSize(311, 16));
    stopAreaSizer->Add(stopsStatBmp);

    stopEditSizer = new wxStaticBoxSizer(wxVERTICAL, this, wxString(wxT(stopsTxt)));    // Txt: "Stops"
    gradientSizer->Add(stopEditSizer, 1, wxEXPAND | wxALL, 2);

    colorSizer = new wxBoxSizer(wxHORIZONTAL);
    stopEditSizer->Add(colorSizer, 1, wxEXPAND | wxALL , 6);

    colorTxt = new wxStaticText(this, wxID_ANY, wxString(wxT(colortxt)), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);    // Txt: "Color: "
    colorSizer->Add(colorTxt, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 12);

    colorStatBmp = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(30, 20), wxBORDER_DOUBLE);
    colorSizer->Add(colorStatBmp, 0, wxALIGN_CENTER_VERTICAL);
    
    colorEditBtn = new wxButton(this, wxID_EDIT, wxString(wxT("...")), wxDefaultPosition, wxSize(30,22));
    colorSizer->Add(colorEditBtn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 6);
    colorEditBtn->Enable(false);

    colorDeleteBtn = new wxButton(this, wxID_DELETE, wxString(wxT(eraseTxt)), wxDefaultPosition, wxSize(60, 22));    // Txt: "Erase"
    colorSizer->Add(colorDeleteBtn, 0, wxLEFT, 100);
    colorDeleteBtn->Enable(false);

    buttonSizer = CreateButtonSizer( wxOK | wxCANCEL );
    if (buttonSizer)
        topSizer->Add(buttonSizer, wxSizerFlags().Expand().DoubleBorder());
    

    this->SetSizer(topSizer);
    GetSizer()->Fit(this); 
    GetSizer()->SetSizeHints(this); 
}
void wxGradientDialog::paintGradient()
{
    wxBufferedDC dc;
    wxGradient temp = *m_gradient;
    temp.setMax(300);
    gradientBmp = new wxBitmap(gradientStatBmp->GetSize().GetWidth(), gradientStatBmp->GetSize().GetHeight());
    dc.SelectObject(*gradientBmp);
    for(int i = temp.getMin(); i<temp.getMax(); i++)
    {
        dc.SetPen(wxPen(temp.getColorAt(i), 1));
        dc.DrawLine(i, 0, i, gradientStatBmp->GetSize().GetHeight());
    }
    dc.SelectObject(wxNullBitmap);
    gradientStatBmp->SetBitmap(*gradientBmp);
}
void wxGradientDialog::paintStops()
{
    m_displayedStops = m_gradient->getStops();
    std::vector<wxColor>::iterator itr;
    int ctr = 0;
    int dist = (gradientStatBmp->GetSize().GetWidth() / (m_displayedStops.size() - 1));
    wxBufferedDC dc;
    stopsBmp = new wxBitmap(stopsStatBmp->GetSize().GetWidth(), stopsStatBmp->GetSize().GetHeight());
    dc.SelectObject(*stopsBmp);
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    for(itr = m_displayedStops.begin(); itr!=m_displayedStops.end(); ++itr)
    {
        // Draw triangle.
        dc.SetPen(wxPen(*wxBLACK));
        dc.SetBrush(wxBrush(wxNullColour, wxBRUSHSTYLE_TRANSPARENT));
        wxPoint triangle[] = {wxPoint(ctr, 5), wxPoint(ctr+5, 0), wxPoint(ctr+10, 5)};
        dc.DrawPolygon(3, triangle);

        // Draw color.
        dc.SetBrush(wxBrush(*itr));
        dc.DrawRectangle(ctr, 5, 11, 11);

        // Draw accents.
        dc.SetPen(wxPen(*wxWHITE));
        dc.DrawLine(ctr+1, 6, ctr+10, 6);
        dc.DrawLine(ctr+1, 7, ctr+1, 15);
        dc.SetPen(wxPen(wxColor(156, 155, 159)));
        dc.DrawLine(ctr+2, 14, ctr+10, 14);
        dc.DrawLine(ctr+9, 7, ctr+9, 14);
        dc.DrawLine(ctr+5, 1, ctr+9, 5);

        ctr += dist;
    }
    dc.SelectObject(wxNullBitmap);
    stopsStatBmp->SetBitmap(*stopsBmp);
}
void wxGradientDialog::OnStopsAreaClick(wxMouseEvent& event)
{    
    selectedColorStop = -1;
    int ctr = 0;
    int dist = (gradientStatBmp->GetSize().GetWidth() / (m_displayedStops.size() - 1));
    std::vector<wxColor>::iterator itr;
    for(itr = m_displayedStops.begin(); itr!=m_displayedStops.end(); ++itr)
    {
        if(event.GetX() >= ctr*dist && event.GetX() <= ctr*dist + 11)
        {
            selectedColorStop = ctr;
            break;
        }
        ctr++;
    }
    m_displayedStops = m_gradient->getStops();
    if(selectedColorStop!=-1)
    {
        colorStatBmp->SetBackgroundColour(m_displayedStops[selectedColorStop]);
        colorStatBmp->ClearBackground();
        paintStops();
    }
    else
    {
        int insertAt = m_displayedStops.size() - (gradientStatBmp->GetSize().GetWidth() - event.GetX())/dist - 1;
        m_gradient->insertColorStop(insertAt, *wxBLACK);
        selectedColorStop = insertAt;
        m_displayedStops = m_gradient->getStops();
        dist = (gradientStatBmp->GetSize().GetWidth() / (m_displayedStops.size() - 1));
        colorStatBmp->SetBackgroundColour(m_displayedStops[selectedColorStop]);
        colorStatBmp->ClearBackground();
        paintGradient();
        paintStops();
    }

    // Triangle selected indicator.
    wxBufferedDC dc;
    dc.SelectObject(*stopsBmp);
    dc.SetPen(wxPen(*wxLIGHT_GREY));
    dc.SetBrush(wxBrush(*wxBLACK));
    wxPoint triangle[] = {wxPoint(selectedColorStop*dist, 5), wxPoint(selectedColorStop*dist+5, 0), wxPoint(selectedColorStop*dist+10, 5)};
    dc.DrawPolygon(3, triangle);
    dc.SelectObject(wxNullBitmap);
    stopsStatBmp->SetBitmap(*stopsBmp);

    colorEditBtn->Enable(true);
    if(m_displayedStops.size() > 2)
        colorDeleteBtn->Enable(true);

    event.Skip();
}
void wxGradientDialog::OnEditColor(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetCustomColour(0, colorStatBmp->GetBackgroundColour());
    data.SetColour(colorStatBmp->GetBackgroundColour());
    wxColourDialog dialog(this, &data);
    if(dialog.ShowModal()==wxID_OK)
    {
        colorStatBmp->SetBackgroundColour(dialog.GetColourData().GetColour());
        colorStatBmp->ClearBackground();
        m_gradient->editColorStop(selectedColorStop, dialog.GetColourData().GetColour());
        paintGradient();
        paintStops();

        int dist = (gradientStatBmp->GetSize().GetWidth() / (m_displayedStops.size() - 1));
        wxBufferedDC dc;
        dc.SelectObject(*stopsBmp);
        dc.SetPen(wxPen(*wxLIGHT_GREY));
        dc.SetBrush(wxBrush(*wxBLACK));
        wxPoint triangle[] = {wxPoint(selectedColorStop*dist, 5), wxPoint(selectedColorStop*dist+5, 0), wxPoint(selectedColorStop*dist+10, 5)};
        dc.DrawPolygon(3, triangle);
        dc.SelectObject(wxNullBitmap);
        stopsStatBmp->SetBitmap(*stopsBmp);
    }
}
void wxGradientDialog::OnDeleteColor(wxCommandEvent& WXUNUSED(event))
{
    if(selectedColorStop!=-1 && m_displayedStops.size()>2)
    {
        m_gradient->removeColorStop(selectedColorStop);
        paintGradient();
        paintStops();
        selectedColorStop=-1;
        colorStatBmp->SetBackgroundColour(wxNullColour);
        colorStatBmp->ClearBackground();
        colorEditBtn->Enable(false);
        colorDeleteBtn->Enable(false);
    }
}
void wxGradientDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}