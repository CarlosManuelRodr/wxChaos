#include "About.h"
#include <wx/mstream.h>

IMPLEMENT_DYNAMIC_CLASS(AboutDialog, wxDialog)

BEGIN_EVENT_TABLE(AboutDialog, wxDialog)

END_EVENT_TABLE()



AboutDialog::AboutDialog()
{
    Init();
}
AboutDialog::AboutDialog(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, caption, pos, size, style);
}
bool AboutDialog::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create(parent, id, caption, pos, size, style);

    CreateControls();
    if (GetSizer())
        GetSizer()->SetSizeHints(this);

    Centre();
    return true;
}
AboutDialog::~AboutDialog()
{

}
void AboutDialog::Init()
{
    m_ContentPanel = nullptr;
    m_HeaderStaticBitmap = nullptr;
    m_AppNameStaticText = nullptr;
    m_CopyrightStaticText = nullptr;
    m_VersionStaticText = nullptr;
    m_BuildInfoStaticText = nullptr;
}
void AboutDialog::CreateControls()
{    
    AboutDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_ContentPanel = new wxPanel( itemDialog1, ID_ContentPanel, wxDefaultPosition, wxSize(290, 350), wxNO_BORDER|wxTAB_TRAVERSAL );
    m_ContentPanel->SetBackgroundColour(wxColour(255, 255, 255));
    itemBoxSizer2->Add(m_ContentPanel, 0, wxGROW, 0);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_ContentPanel->SetSizer(itemBoxSizer4);

    m_HeaderStaticBitmap = new wxStaticBitmap( m_ContentPanel, wxID_STATIC, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_HeaderStaticBitmap, 0, wxGROW, 0);

    wxGridBagSizer* itemGridBagSizer6 = new wxGridBagSizer(0, 0);
    itemGridBagSizer6->AddGrowableRow(2);
    itemGridBagSizer6->AddGrowableRow(3);
    itemGridBagSizer6->SetEmptyCellSize(wxSize(10, 20));
    itemBoxSizer4->Add(itemGridBagSizer6, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 10);

    m_AppNameStaticText = new wxStaticText( m_ContentPanel, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_AppNameStaticText->SetForegroundColour(wxColour(128, 0, 0));
    m_AppNameStaticText->SetFont(wxFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial Narrow")));
    itemGridBagSizer6->Add(m_AppNameStaticText, wxGBPosition(0, 0), wxGBSpan(1, 2), wxALIGN_LEFT|wxALIGN_BOTTOM|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( m_ContentPanel, wxID_STATIC, _("Version"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText8->SetForegroundColour(wxColour(150, 150, 150));
    itemStaticText8->SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
    itemGridBagSizer6->Add(itemStaticText8, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_RIGHT|wxALIGN_TOP|wxLEFT|wxBOTTOM, 5);

    m_CopyrightStaticText = new wxStaticText( m_ContentPanel, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE );
    itemGridBagSizer6->Add(m_CopyrightStaticText, wxGBPosition(2, 0), wxGBSpan(1, 2), wxGROW|wxGROW|wxALL, 5);

    m_VersionStaticText = new wxStaticText( m_ContentPanel, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_VersionStaticText->SetForegroundColour(wxColour(150, 150, 150));
    m_VersionStaticText->SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial")));
    itemGridBagSizer6->Add(m_VersionStaticText, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALIGN_LEFT|wxALIGN_TOP|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_BuildInfoStaticText = new wxStaticText( m_ContentPanel, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE );
    itemGridBagSizer6->Add(m_BuildInfoStaticText, wxGBPosition(3, 0), wxGBSpan(1, 2), wxGROW|wxGROW|wxALL, 5);

    itemBoxSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticLine* itemStaticLine13 = new wxStaticLine( itemDialog1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(itemStaticLine13, 0, wxGROW, 0);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer14 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer14, 0, wxALIGN_RIGHT|wxALL, 5);
    wxButton* itemButton15 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer14->AddButton(itemButton15);

    itemStdDialogButtonSizer14->Realize();

    m_BuildInfoStaticText->SetLabel(AboutDialog::GetBuildInfo(wxBUILDINFO_LONG));
}
wxBitmap AboutDialog::GetBitmapResource( const wxString& name )
{
    wxUnusedVar(name);
    return wxNullBitmap;
}
wxIcon AboutDialog::GetIconResource( const wxString& name )
{
    wxUnusedVar(name);
    return wxNullIcon;
}
wxString AboutDialog::GetBuildInfo(wxBuildInfoFormat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == wxBUILDINFO_LONG)
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode");
#else
        wxbuild << _T("-ANSI");
#endif // wxUSE_UNICODE
    }
    wxbuild << _(" build");
    return wxbuild;
}
void AboutDialog::SetHeaderBitmap(const wxBitmap & value)
{
    m_HeaderStaticBitmap->SetBitmap(value);
}
void AboutDialog::ApplyInfo()
{
    wxASSERT_MSG(m_HeaderStaticBitmap->GetBitmap().IsOk(), _("Header bitmap for About box is empty"));
    SetTitle(wxString::Format(wxT("%s %s"), _("About"), m_AppName.GetData()));
    m_AppNameStaticText->SetLabel(m_AppName);
    m_VersionStaticText->SetLabel(m_Version);
    m_CopyrightStaticText->SetLabel(m_Copyright);
    wxString buildInfo;
    if(m_CustomBuildInfo.IsEmpty())
    {
        buildInfo = AboutDialog::GetBuildInfo(wxBUILDINFO_LONG);
    }
    else
    {
        buildInfo = m_CustomBuildInfo;
    }
    m_BuildInfoStaticText->SetLabel(buildInfo);
    int labelWidth = m_HeaderStaticBitmap->GetSize().GetWidth() - 20;
    m_VersionStaticText->Wrap(labelWidth);
    m_CopyrightStaticText->Wrap(labelWidth);
    m_BuildInfoStaticText->Wrap(labelWidth);
    m_ContentPanel->Layout();
    m_ContentPanel->GetSizer()->Fit(m_ContentPanel);
    GetSizer()->Fit(this);
    Centre();
}