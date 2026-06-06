#include <wx/dcbuffer.h>
#include <wx/colordlg.h>
#include "wxGradientDialog.h"

IMPLEMENT_DYNAMIC_CLASS(wxGradientDialog, wxDialog)

wxGradientDialog::wxGradientDialog()
{
    topSizer = nullptr;
    topPanel = nullptr;
    buttonSizer = nullptr;
    gradientSizer = nullptr;
    gradientStatBmp = nullptr;
    stopsStatBmp = nullptr;
    stopAreaSizer = nullptr;
    colorSizer = nullptr;
    colorStatBmp = nullptr;
    colorEditBtn = nullptr;
    colorDeleteBtn = nullptr;
    gradientBmp = nullptr;
    stopsBmp = nullptr;
    stopEditSizer = nullptr;
    colorTxt = nullptr;
    m_gradient = nullptr;
    selectedColorStop = 0;
    gradientSize = 0;
}

wxGradientDialog::wxGradientDialog(wxWindow* parent, const wxGradient& grad)
{
    topSizer = nullptr;
    topPanel = nullptr;
    buttonSizer = nullptr;
    gradientSizer = nullptr;
    gradientStatBmp = nullptr;
    stopsStatBmp = nullptr;
    stopAreaSizer = nullptr;
    colorSizer = nullptr;
    colorStatBmp = nullptr;
    colorEditBtn = nullptr;
    colorDeleteBtn = nullptr;
    gradientBmp = nullptr;
    stopsBmp = nullptr;
    stopEditSizer = nullptr;
    colorTxt = nullptr;
    m_gradient = nullptr;
    selectedColorStop = 0;
    gradientSize = 0;
    Create(parent, grad);
}
wxGradientDialog::~wxGradientDialog() = default;

bool wxGradientDialog::Create(wxWindow* parent, const wxGradient& grad)
{
    if (!wxDialog::Create(parent, wxID_ANY, wxT("Gradient editor")))
        return false;
    CreateWidgets();
    Bind(wxEVT_CLOSE_WINDOW, &wxGradientDialog::OnWindowClose, this);
    Bind(wxEVT_BUTTON, &wxGradientDialog::OnEditColor, this, wxID_EDIT);
    Bind(wxEVT_BUTTON, &wxGradientDialog::OnDeleteColor, this, wxID_DELETE);
    stopsStatBmp->Bind(wxEVT_LEFT_DOWN, &wxGradientDialog::OnStopsAreaClick, this, ID_STOPS_AREA);
    selectedColorStop = -1;
    m_gradient = new wxGradient(grad);
    paintGradient();
    paintStops();
    return true;
}
wxGradient wxGradientDialog::GetGradient() const
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

    gradientSizer = new wxStaticBoxSizer(wxVERTICAL, this, wxString(wxT("Gradient")));
    topSizer->Add(gradientSizer, 1, wxEXPAND | wxALL, 12);
    
    gradientStatBmp = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(300, 50), wxBORDER_SIMPLE );
    gradientSizer->Add(gradientStatBmp, 1, wxALL | wxALIGN_CENTER_HORIZONTAL);

    stopAreaSizer = new wxBoxSizer(wxHORIZONTAL);
    gradientSizer->Add(stopAreaSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 6);

    stopsStatBmp = new wxStaticBitmap(this, ID_STOPS_AREA, wxNullBitmap, wxDefaultPosition, wxSize(311, 16));
    stopAreaSizer->Add(stopsStatBmp);

    stopEditSizer = new wxStaticBoxSizer(wxVERTICAL, this, wxString(wxT("Stops")));
    gradientSizer->Add(stopEditSizer, 1, wxEXPAND | wxALL, 2);

    colorSizer = new wxBoxSizer(wxHORIZONTAL);
    stopEditSizer->Add(colorSizer, 1, wxEXPAND | wxALL , 6);

    colorTxt = new wxStaticText(this, wxID_ANY, wxString(wxT("Color: ")), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    colorSizer->Add(colorTxt, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 12);

    colorStatBmp = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(30, 20), wxBORDER_DOUBLE);
    colorSizer->Add(colorStatBmp, 0, wxALIGN_CENTER_VERTICAL);
    
    colorEditBtn = new wxButton(this, wxID_EDIT, wxString(wxT("...")), wxDefaultPosition, wxSize(30,22));
    colorSizer->Add(colorEditBtn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 6);
    colorEditBtn->Enable(false);

    colorDeleteBtn = new wxButton(this, wxID_DELETE, wxString(wxT("Erase")), wxDefaultPosition, wxSize(60, 22));
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
    temp.SetMax(300);
    gradientBmp = new wxBitmap(gradientStatBmp->GetSize().GetWidth(), gradientStatBmp->GetSize().GetHeight());
    dc.SelectObject(*gradientBmp);
    for (unsigned int i = temp.GetMin(); i<temp.GetMax(); i++)
    {
        dc.SetPen(wxPen(temp.GetColorAt(i), 1));
        int position = static_cast<int>(i);
        dc.DrawLine(position, 0, position, gradientStatBmp->GetSize().GetHeight());
    }
    dc.SelectObject(wxNullBitmap);
    gradientStatBmp->SetBitmap(*gradientBmp);
}
void wxGradientDialog::paintStops()
{
    m_displayedStops = m_gradient->GetStops();
    int ctr = 0;
    const unsigned int dist = (gradientStatBmp->GetSize().GetWidth() / (m_displayedStops.size() - 1));
    wxBufferedDC dc;
    stopsBmp = new wxBitmap(stopsStatBmp->GetSize().GetWidth(), stopsStatBmp->GetSize().GetHeight());
    dc.SelectObject(*stopsBmp);
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    for (auto itr = m_displayedStops.begin(); itr!=m_displayedStops.end(); ++itr)
    {
        // Draw triangle.
        dc.SetPen(wxPen(*wxBLACK));
        dc.SetBrush(wxBrush(wxNullColour, wxBRUSHSTYLE_TRANSPARENT));
        const wxPoint triangle[] = {wxPoint(ctr, 5), wxPoint(ctr+5, 0), wxPoint(ctr+10, 5)};
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

        ctr += static_cast<int>(dist);
    }
    dc.SelectObject(wxNullBitmap);
    stopsStatBmp->SetBitmap(*stopsBmp);
}
void wxGradientDialog::OnStopsAreaClick(wxMouseEvent& event)
{    
    selectedColorStop = -1;
    int ctr = 0;
    unsigned int dist = (gradientStatBmp->GetSize().GetWidth() / (m_displayedStops.size() - 1));
    for (auto itr = m_displayedStops.begin(); itr!=m_displayedStops.end(); ++itr)
    {
        if (event.GetX() >= ctr*dist && event.GetX() <= ctr*dist + 11)
        {
            selectedColorStop = ctr;
            break;
        }
        ctr++;
    }
    m_displayedStops = m_gradient->GetStops();
    if (selectedColorStop!=-1)
    {
        colorStatBmp->SetBackgroundColour(m_displayedStops[selectedColorStop]);
        colorStatBmp->ClearBackground();
        paintStops();
    }
    else
    {
        const unsigned int insertAt = m_displayedStops.size() - (gradientStatBmp->GetSize().GetWidth() - event.GetX())/dist - 1;
        m_gradient->InsertColorStop(insertAt, *wxBLACK);
        selectedColorStop = static_cast<int>(insertAt);
        m_displayedStops = m_gradient->GetStops();
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
    const int xPosition = static_cast<int>(selectedColorStop*dist);
    const wxPoint triangle[] = {wxPoint(xPosition, 5), wxPoint(xPosition + 5, 0), wxPoint(xPosition + 10, 5)};
    dc.DrawPolygon(3, triangle);
    dc.SelectObject(wxNullBitmap);
    stopsStatBmp->SetBitmap(*stopsBmp);

    colorEditBtn->Enable(true);
    if (m_displayedStops.size() > 2)
        colorDeleteBtn->Enable(true);

    event.Skip();
}
void wxGradientDialog::OnEditColor(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetCustomColour(0, colorStatBmp->GetBackgroundColour());
    data.SetColour(colorStatBmp->GetBackgroundColour());
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal()==wxID_OK)
    {
        colorStatBmp->SetBackgroundColour(dialog.GetColourData().GetColour());
        colorStatBmp->ClearBackground();
        m_gradient->EditColorStop(selectedColorStop, dialog.GetColourData().GetColour());
        paintGradient();
        paintStops();

        const unsigned int dist = (gradientStatBmp->GetSize().GetWidth() / (m_displayedStops.size() - 1));
        wxBufferedDC dc;
        dc.SelectObject(*stopsBmp);
        dc.SetPen(wxPen(*wxLIGHT_GREY));
        dc.SetBrush(wxBrush(*wxBLACK));
        const int xPosition = static_cast<int>(selectedColorStop * dist);
        const wxPoint triangle[] = {wxPoint(xPosition, 5), wxPoint(xPosition + 5, 0), wxPoint(xPosition + 10, 5)};
        dc.DrawPolygon(3, triangle);
        dc.SelectObject(wxNullBitmap);
        stopsStatBmp->SetBitmap(*stopsBmp);
    }
}
void wxGradientDialog::OnDeleteColor(wxCommandEvent& WXUNUSED(event))
{
    if (selectedColorStop!=-1 && m_displayedStops.size()>2)
    {
        m_gradient->RemoveColorStop(selectedColorStop);
        paintGradient();
        paintStops();
        selectedColorStop=-1;
        colorStatBmp->SetBackgroundColour(wxNullColour);
        colorStatBmp->ClearBackground();
        colorEditBtn->Enable(false);
        colorDeleteBtn->Enable(false);
    }
}
void wxGradientDialog::OnWindowClose(wxCloseEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}
