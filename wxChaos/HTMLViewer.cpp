#include "HTMLViewer.h"
#include "global.h"

// TextViewer
HTMLViewer::HTMLViewer( wxString htmlFile, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : wxDialog( parent, id, title, pos, size, style )
{
    //wxImage::AddHandler(new wxPNGHandler);

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

#ifdef __linux__
    wxIcon icon(GetWxAbsPath("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
#elif _WIN32
    wxIcon icon(GetWxAbsPath("Resources\\icon.ico"), wxBITMAP_TYPE_ICO);
#endif
    this->SetIcon(icon);

    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* textSizer;
    textSizer = new wxBoxSizer( wxVERTICAL );

    htmlView = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
    textSizer->Add( htmlView, 1, wxALL|wxEXPAND, 5 );

    htmlView->LoadPage(htmlFile);

    mainSizer->Add( textSizer, 9, wxEXPAND, 5 );

    wxBoxSizer* buttonSizer;
    buttonSizer = new wxBoxSizer( wxVERTICAL );

    CloseButton = new wxButton( this, wxID_ANY, wxT(closeTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Close"
    buttonSizer->Add( CloseButton, 0, wxALL, 5 );

    mainSizer->Add( buttonSizer, 0, wxEXPAND, 5 );

    this->SetSizer( mainSizer );
    this->Layout();

    this->Centre( wxBOTH );

    // Connect Events.
    CloseButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HTMLViewer::OnClose ), NULL, this );
}
HTMLViewer::~HTMLViewer()
{
    // Disconnect Events.
    CloseButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HTMLViewer::OnClose ), NULL, this );
}
void HTMLViewer::OnClose( wxCommandEvent& event )
{
    this->Close(true);
}
