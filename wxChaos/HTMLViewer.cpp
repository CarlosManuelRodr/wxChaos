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

	wxBoxSizer* mainBoxxy;
	mainBoxxy = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* textBoxxy;
	textBoxxy = new wxBoxSizer( wxVERTICAL );

	htmlView = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	textBoxxy->Add( htmlView, 1, wxALL|wxEXPAND, 5 );

	htmlView->LoadPage(htmlFile);

	mainBoxxy->Add( textBoxxy, 9, wxEXPAND, 5 );

	wxBoxSizer* buttonBoxxy;
	buttonBoxxy = new wxBoxSizer( wxVERTICAL );

	CloseButton = new wxButton( this, wxID_ANY, wxT(closeTxt), wxDefaultPosition, wxDefaultSize, 0 );	// Txt: "Close"
	buttonBoxxy->Add( CloseButton, 0, wxALL, 5 );

	mainBoxxy->Add( buttonBoxxy, 0, wxEXPAND, 5 );

	this->SetSizer( mainBoxxy );
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
