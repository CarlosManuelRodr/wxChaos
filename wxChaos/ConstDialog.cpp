#include "ConstDialog.h"
#include "StringFuncs.h"
#include "global.h"

ConstDialog::ConstDialog(bool *Active, Fractal *mTarget, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style)
{
	target = mTarget;
	wxString text;
	active = Active;

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* mainBoxxy;
	mainBoxxy = new wxBoxSizer(wxVERTICAL);
	
	dumbPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* boxxy;
	boxxy = new wxBoxSizer(wxVERTICAL);
	
	wxStaticBoxSizer* realBoxxy;
	realBoxxy = new wxStaticBoxSizer(new wxStaticBox(dumbPanel, wxID_ANY, wxT(realValTxt)), wxVERTICAL);	// Txt: "Real value"
	
	lastReal = target->GetKReal();
	text = num_to_string(target->GetKReal());
	realText = new wxTextCtrl(dumbPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
	realBoxxy->Add(realText, 0, wxALL|wxEXPAND, 5);
	
	boxxy->Add(realBoxxy, 2, wxEXPAND, 5);
	
	wxStaticBoxSizer* imBoxxy;
	imBoxxy = new wxStaticBoxSizer(new wxStaticBox(dumbPanel, wxID_ANY, wxT(imagValTxt)), wxVERTICAL);	// Txt: "Imaginary value"
	
	lastIm = target->GetKImaginary();
	text = num_to_string(target->GetKImaginary());
	imText = new wxTextCtrl(dumbPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
	imBoxxy->Add(imText, 0, wxALL|wxEXPAND, 5);
	
	boxxy->Add(imBoxxy, 2, wxEXPAND, 5);
	
	wxBoxSizer* buttonBoxxy;
	buttonBoxxy = new wxBoxSizer(wxHORIZONTAL);
	
	wxBoxSizer* okBoxxy;
	okBoxxy = new wxBoxSizer(wxVERTICAL);
	
	okButton = new wxButton(dumbPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0);	// Txt: "Ok"
	okBoxxy->Add(okButton, 0, wxALL, 5);
	
	buttonBoxxy->Add(okBoxxy, 1, wxEXPAND, 5);
	
	wxBoxSizer* applyBoxxy;
	applyBoxxy = new wxBoxSizer(wxVERTICAL);
	
	applyButton = new wxButton(dumbPanel, wxID_ANY, wxT(applyTxt), wxDefaultPosition, wxDefaultSize, 0);	// Txt: "Apply"
	applyBoxxy->Add(applyButton, 0, wxALL, 5);
	
	buttonBoxxy->Add(applyBoxxy, 1, wxEXPAND, 5);
	
	boxxy->Add(buttonBoxxy, 1, wxEXPAND, 5);
	
	dumbPanel->SetSizer(boxxy);
	dumbPanel->Layout();
	boxxy->Fit(dumbPanel);
	mainBoxxy->Add(dumbPanel, 1, wxEXPAND | wxALL, 0);
	
	this->SetSizer(mainBoxxy);
	this->Layout();
	
	this->Centre(wxBOTH);

	okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnOk), NULL, this);
	applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnApply), NULL, this);
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ConstDialog::OnClose));
}

ConstDialog::~ConstDialog()
{
	okButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnOk), NULL, this);
	*active = false;
}

void ConstDialog::OnOk(wxCommandEvent& event)
{
	double real = string_to_double(realText->GetLineText(0));
	double imag = string_to_double(imText->GetLineText(0));

	if(real != lastReal || imag != lastIm)
		target->SetK(real, imag);

	this->Close(true);
	this->Destroy();
}
void ConstDialog::OnApply(wxCommandEvent& event)
{
	double real = string_to_double(realText->GetLineText(0));
	double imag = string_to_double(imText->GetLineText(0));

	lastReal = real;
	lastIm = imag;

	target->SetK(real, imag);
}
void ConstDialog::OnClose(wxCloseEvent& event)
{
	this->Destroy();
}