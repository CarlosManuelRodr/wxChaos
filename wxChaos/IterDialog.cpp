#include "IterDialog.h"
#include "StringFuncs.h"
#include "global.h"

IterDialog::IterDialog(bool *Active, Fractal *_target, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxFrame(parent, id, title, pos, size, style)
{
	// WX Frame.
#ifdef __linux__
	wxIcon icon(GetWxAbsPath("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
#elif _WIN32
    wxIcon icon(GetWxAbsPath("Resources\\icon.ico"), wxBITMAP_TYPE_ICO);
#endif
	this->SetIcon(icon);

	active = Active;
	target = _target;
	this->SetSizeHints(wxSize(314,124), wxSize(314,124));

	wxBoxSizer* boxxy;
	boxxy = new wxBoxSizer(wxVERTICAL);

	panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* subBoxxy;
	subBoxxy = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* textBoxxy;
	textBoxxy = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT(iterationsTxt)), wxHORIZONTAL);	// Txt: "Iterations"

	number = target->GetIterations();
	text = num_to_string((int)number);
	textCtrl = new wxTextCtrl(panel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
	textBoxxy->Add(textCtrl, 0, wxALL, 5);

	plusButton = new wxButton(panel, wxID_ANY, wxT("+"), wxDefaultPosition, wxDefaultSize, 0);
	textBoxxy->Add(plusButton, 0, wxALL, 5);

	minusButton = new wxButton(panel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0);
	textBoxxy->Add(minusButton, 0, wxALL, 5);

	subBoxxy->Add(textBoxxy, 1, wxEXPAND, 5);

	wxBoxSizer* buttonBoxxy;
	buttonBoxxy = new wxBoxSizer(wxHORIZONTAL);

	acceptButton = new wxButton(panel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0);	// Txt: "Ok"
	buttonBoxxy->Add(acceptButton, 0, wxALL, 5);

	applyButton = new wxButton(panel, wxID_ANY, wxT(applyTxt), wxDefaultPosition, wxDefaultSize, 0);	// Txt: "Apply"
	buttonBoxxy->Add(applyButton, 0, wxALL, 5);

	subBoxxy->Add(buttonBoxxy, 0, 0, 5);

	panel->SetSizer(subBoxxy);
	panel->Layout();
	subBoxxy->Fit(panel);
	boxxy->Add(panel, 1, wxEXPAND | wxALL, 0);

	this->SetSizer(boxxy);
	this->Layout();

	this->Centre(wxBOTH);

	plusButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnPlus), NULL, this);
	minusButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnMinus), NULL, this);
	acceptButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnOk), NULL, this);
	applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnApply), NULL, this);
}

IterDialog::~IterDialog()
{
	*active = false;
	plusButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnPlus), NULL, this);
	minusButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnMinus), NULL, this);
	acceptButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnOk), NULL, this);
	applyButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnApply), NULL, this);
}

void IterDialog::OnPlus(wxCommandEvent& event)
{
	// Increases iterations.
	number++;
	text = num_to_string((int)number);
	textCtrl->SetValue(text);
	target->ChangeIterations(number);
}
void IterDialog::OnMinus(wxCommandEvent& event)
{
	// Decreases iterations.
	if(number - 1 > 0) number--;
	text = num_to_string((int)number);
	textCtrl->SetValue(text);
	target->ChangeIterations(number);
}
void IterDialog::OnOk(wxCommandEvent& event)
{
	// Closes dialog.
	this->Close(true);
	this->Destroy();
}
void IterDialog::OnApply(wxCommandEvent& event)
{
	// Redraw fractal.
	text = textCtrl->GetValue();
	number = string_to_int(text);
	target->ChangeIterations(number);
}
void IterDialog::SetTarget(Fractal* _target)
{
	target = _target;
	textCtrl->SetValue(num_to_string((int)target->GetIterations()));
	number = target->GetIterations();
}
