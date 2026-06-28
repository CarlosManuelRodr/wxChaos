#include "AppPaths.h"
#include "HTMLViewer.h"

// TextViewer
HTMLViewer::HTMLViewer(const wxString& htmlFile, wxWindow* parent, const wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size, const long style) : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxSize(900, 620), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    this->SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    const auto textSizer = new wxBoxSizer(wxVERTICAL);

    _htmlView = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO);
    textSizer->Add(_htmlView, 1, wxALL|wxEXPAND, 5);
    _htmlView->LoadPage(htmlFile);
    mainSizer->Add(textSizer, 9, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxVERTICAL);
    _closeButton = new wxButton(this, wxID_ANY, "Close", wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_closeButton, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxEXPAND, 5);

    this->SetSizer(mainSizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    // Connect Events.
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &HTMLViewer::OnClose, this);
}
HTMLViewer::~HTMLViewer()
{
    // Disconnect Events.
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &HTMLViewer::OnClose, this);
}
void HTMLViewer::OnClose(wxCommandEvent&)
{
    this->Close(true);
}
