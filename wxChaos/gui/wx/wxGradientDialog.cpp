#include <wx/dcbuffer.h>
#include <wx/colordlg.h>
#include "wxGradientDialog.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxGradientDialog, wxDialog)

wxGradientDialog::wxGradientDialog() = default;

wxGradientDialog::wxGradientDialog(wxWindow* parent, const wxGradient& gradient) : _gradient(gradient)
{
    Create(parent, gradient);
}
wxGradientDialog::~wxGradientDialog() = default;

bool wxGradientDialog::Create(wxWindow* parent, const wxGradient& grad)
{
    if (!wxDialog::Create(parent, wxID_ANY, "Gradient editor"))
        return false;
    CreateWidgets();
    Bind(wxEVT_CLOSE_WINDOW, &wxGradientDialog::OnWindowClose, this);
    Bind(wxEVT_BUTTON, &wxGradientDialog::OnEditColor, this, wxID_EDIT);
    Bind(wxEVT_BUTTON, &wxGradientDialog::OnDeleteColor, this, wxID_DELETE);
    _stopsStatBmp->Bind(wxEVT_LEFT_DOWN, &wxGradientDialog::OnStopsAreaClick, this, ID_STOPS_AREA);
    _selectedColorStop = -1;
    _gradient = grad;
    paintGradient();
    paintStops();
    return true;
}
wxGradient wxGradientDialog::GetGradient() const
{
    return _gradient;
}
int wxGradientDialog::ShowModal()
{
    return wxDialog::ShowModal();
}
void wxGradientDialog::CreateWidgets()
{
    _topSizer = new wxBoxSizer(wxVERTICAL);

    _gradientSizer = new wxStaticBoxSizer(wxVERTICAL, this, wxString("Gradient"));
    _topSizer->Add(_gradientSizer, 1, wxEXPAND | wxALL, 12);
    
    _gradientStatBmp = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(300, 50), wxBORDER_SIMPLE );
    _gradientSizer->Add(_gradientStatBmp, 1, wxALL | wxALIGN_CENTER_HORIZONTAL);

    _stopAreaSizer = new wxBoxSizer(wxHORIZONTAL);
    _gradientSizer->Add(_stopAreaSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 6);

    _stopsStatBmp = new wxStaticBitmap(this, ID_STOPS_AREA, wxNullBitmap, wxDefaultPosition, wxSize(311, 16));
    _stopAreaSizer->Add(_stopsStatBmp);

    _stopEditSizer = new wxStaticBoxSizer(wxVERTICAL, this, wxString("Stops"));
    _gradientSizer->Add(_stopEditSizer, 1, wxEXPAND | wxALL, 2);

    _colorSizer = new wxBoxSizer(wxHORIZONTAL);
    _stopEditSizer->Add(_colorSizer, 1, wxEXPAND | wxALL , 6);

    _colorTxt = new wxStaticText(this, wxID_ANY, wxString("Color: "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    _colorSizer->Add(_colorTxt, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 12);

    _colorStatBmp = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(30, 20), wxBORDER_DOUBLE);
    _colorSizer->Add(_colorStatBmp, 0, wxALIGN_CENTER_VERTICAL);
    
    _colorEditButton = new wxButton(this, wxID_EDIT, wxString("..."), wxDefaultPosition, wxSize(30,22));
    _colorSizer->Add(_colorEditButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 6);
    _colorEditButton->Enable(false);

    _colorDeleteButton = new wxButton(this, wxID_DELETE, wxString("Erase"), wxDefaultPosition, wxSize(60, 22));
    _colorSizer->Add(_colorDeleteButton, 0, wxLEFT, 100);
    _colorDeleteButton->Enable(false);

    _buttonSizer = CreateButtonSizer( wxOK | wxCANCEL );
    if (_buttonSizer)
        _topSizer->Add(_buttonSizer, wxSizerFlags().Expand().DoubleBorder());

    this->SetSizer(_topSizer);
    GetSizer()->Fit(this); 
    GetSizer()->SetSizeHints(this); 
}
void wxGradientDialog::paintGradient()
{
    wxBufferedDC dc;
    wxGradient temp = _gradient;
    temp.SetMax(300);
    _gradientBmp = wxBitmap(_gradientStatBmp->GetSize().GetWidth(), _gradientStatBmp->GetSize().GetHeight());
    dc.SelectObject(_gradientBmp);
    for (unsigned int i = temp.GetMin(); i<temp.GetMax(); i++)
    {
        dc.SetPen(wxPen(temp.GetColorAt(i), 1));
        int position = static_cast<int>(i);
        dc.DrawLine(position, 0, position, _gradientStatBmp->GetSize().GetHeight());
    }
    dc.SelectObject(wxNullBitmap);
    _gradientStatBmp->SetBitmap(_gradientBmp);
}
void wxGradientDialog::paintStops()
{
    _displayedStops = _gradient.GetStops();
    int ctr = 0;
    const unsigned int dist = (_gradientStatBmp->GetSize().GetWidth() / (_displayedStops.size() - 1));
    wxBufferedDC dc;
    _stopsBmp = wxBitmap(_stopsStatBmp->GetSize().GetWidth(), _stopsStatBmp->GetSize().GetHeight());
    dc.SelectObject(_stopsBmp);
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    for (auto & _displayedStop : _displayedStops)
    {
        // Draw triangle.
        dc.SetPen(wxPen(*wxBLACK));
        dc.SetBrush(wxBrush(wxNullColour, wxBRUSHSTYLE_TRANSPARENT));
        const wxPoint triangle[] = {wxPoint(ctr, 5), wxPoint(ctr+5, 0), wxPoint(ctr+10, 5)};
        dc.DrawPolygon(3, triangle);

        // Draw color.
        dc.SetBrush(wxBrush(_displayedStop));
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
    _stopsStatBmp->SetBitmap(_stopsBmp);
}
void wxGradientDialog::OnStopsAreaClick(wxMouseEvent& event)
{    
    _selectedColorStop = -1;
    int ctr = 0;
    unsigned int dist = (_gradientStatBmp->GetSize().GetWidth() / (_displayedStops.size() - 1));
    for (auto itr = _displayedStops.begin(); itr!=_displayedStops.end(); ++itr)
    {
        if (event.GetX() >= ctr*dist && event.GetX() <= ctr*dist + 11)
        {
            _selectedColorStop = ctr;
            break;
        }
        ctr++;
    }
    _displayedStops = _gradient.GetStops();
    if (_selectedColorStop!=-1)
    {
        _colorStatBmp->SetBackgroundColour(_displayedStops[_selectedColorStop]);
        _colorStatBmp->ClearBackground();
        paintStops();
    }
    else
    {
        const unsigned int insertAt = _displayedStops.size() - (_gradientStatBmp->GetSize().GetWidth() - event.GetX())/dist - 1;
        _gradient.InsertColorStop(insertAt, *wxBLACK);
        _selectedColorStop = static_cast<int>(insertAt);
        _displayedStops = _gradient.GetStops();
        dist = (_gradientStatBmp->GetSize().GetWidth() / (_displayedStops.size() - 1));
        _colorStatBmp->SetBackgroundColour(_displayedStops[_selectedColorStop]);
        _colorStatBmp->ClearBackground();
        paintGradient();
        paintStops();
    }

    // Triangle selected indicator.
    wxBufferedDC dc;
    dc.SelectObject(_stopsBmp);
    dc.SetPen(wxPen(*wxLIGHT_GREY));
    dc.SetBrush(wxBrush(*wxBLACK));
    const int xPosition = static_cast<int>(_selectedColorStop*dist);
    const wxPoint triangle[] = {wxPoint(xPosition, 5), wxPoint(xPosition + 5, 0), wxPoint(xPosition + 10, 5)};
    dc.DrawPolygon(3, triangle);
    dc.SelectObject(wxNullBitmap);
    _stopsStatBmp->SetBitmap(_stopsBmp);

    _colorEditButton->Enable(true);
    if (_displayedStops.size() > 2)
        _colorDeleteButton->Enable(true);

    event.Skip();
}
void wxGradientDialog::OnEditColor(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetCustomColour(0, _colorStatBmp->GetBackgroundColour());
    data.SetColour(_colorStatBmp->GetBackgroundColour());
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal()==wxID_OK)
    {
        _colorStatBmp->SetBackgroundColour(dialog.GetColourData().GetColour());
        _colorStatBmp->ClearBackground();
        _gradient.EditColorStop(_selectedColorStop, dialog.GetColourData().GetColour());
        paintGradient();
        paintStops();

        const unsigned int dist = (_gradientStatBmp->GetSize().GetWidth() / (_displayedStops.size() - 1));
        wxBufferedDC dc;
        dc.SelectObject(_stopsBmp);
        dc.SetPen(wxPen(*wxLIGHT_GREY));
        dc.SetBrush(wxBrush(*wxBLACK));
        const int xPosition = static_cast<int>(_selectedColorStop * dist);
        const wxPoint triangle[] = {wxPoint(xPosition, 5), wxPoint(xPosition + 5, 0), wxPoint(xPosition + 10, 5)};
        dc.DrawPolygon(3, triangle);
        dc.SelectObject(wxNullBitmap);
        _stopsStatBmp->SetBitmap(_stopsBmp);
    }
}
void wxGradientDialog::OnDeleteColor(wxCommandEvent& WXUNUSED(event))
{
    if (_selectedColorStop!=-1 && _displayedStops.size()>2)
    {
        _gradient.RemoveColorStop(_selectedColorStop);
        paintGradient();
        paintStops();
        _selectedColorStop=-1;
        _colorStatBmp->SetBackgroundColour(wxNullColour);
        _colorStatBmp->ClearBackground();
        _colorEditButton->Enable(false);
        _colorDeleteButton->Enable(false);
    }
}
void wxGradientDialog::OnWindowClose(wxCloseEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}
