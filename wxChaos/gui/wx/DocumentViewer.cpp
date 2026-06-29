#include "DocumentViewer.h"
#include <wx/filename.h>
#include <wx/filesys.h>
#include "AppPaths.h"

DocumentViewer::DocumentViewer(const wxString& htmlFile, wxWindow* parent, const wxWindowID id,
                               const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                               : wxDialog(parent, id, title, pos, size, style)
{
    SetSizeHints(wxSize(900, 620), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    _webView = wxWebView::New(this, wxID_ANY);
    mainSizer->Add(_webView, 1, wxALL | wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();
    _closeButton = new wxButton(this, wxID_ANY, "Close", wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_closeButton, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxEXPAND, 5);

    SetSizer(mainSizer);
    Layout();
    Centre(wxBOTH);

    _webView->LoadURL(wxFileSystem::FileNameToURL(wxFileName(htmlFile).GetFullPath()));
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnClose, this);
}

DocumentViewer::~DocumentViewer()
{
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnClose, this);
}

void DocumentViewer::OnClose(wxCommandEvent&)
{
    Close(true);
}
