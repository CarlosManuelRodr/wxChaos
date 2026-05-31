#include <limits>
#include <fstream>
#include <mpParser.h>
#include "DimensionFrame.h"
#include "StringFuncs.h"
#include "SizeDialogSave.h"
#include "BmpWriter.h"
#include "AngelscriptBindings.h"
#include "HTMLViewer.h"
#include "Filesystem.h"
#include "SystemUtils.h"
using namespace std;

bool dimensionFrameState = false;

double GetMaxElement(const vector<double> &in)
{
    double max = -std::numeric_limits<double>::infinity();
    for (unsigned int i = 0; i < in.size(); i++)
    {
        if (in[i] > max) max = in[i];
    }
    return max;
}
double GetMinElement(const vector<double> &in)
{
    double min = std::numeric_limits<double>::infinity();
    for (unsigned int i = 0; i < in.size(); i++)
    {
        if (in[i] < min) min = in[i];
    }
    return min;
}

// DimensionCalculator
DimensionCalculator::DimensionCalculator() : ho(0), hf(0), map(nullptr), size(0), div(0)
{
    running = false;
    N = 0;
}

void DimensionCalculator::SetMap(bool** _map, int _size, int _ho, int _hf)
{
    map = _map;
    size = _size;
    ho = _ho;
    hf = _hf;
}
void DimensionCalculator::SetDiv(int _div)
{
    div = _div;
}
void DimensionCalculator::Run()
{
    running = true;

    N = 0;
    double epsilon = static_cast<double>(size) / static_cast<double>(div);
    int ey_init = static_cast<double>(ho) / epsilon;
    int ey_end = static_cast<double>(hf) / epsilon;

    // Iterate through the boxes.
    for (int ey = ey_init; ey < ey_end; ey++)
    {
        for (int ex = 0; ex < div; ex++)
        {
            if (!running) return; // Check for termination signal
            bool found = false;

            // And count the elements of the map that belong to the set.
            for (int w = ex * epsilon; w < (ex + 1) * epsilon && !found; w++)
            {
                for (int h = ey * epsilon; h < (ey + 1) * epsilon; h++)
                {
                    if (w < size && h < size)
                    {
                        if (map[w][h] == true)
                        {
                            found = true;
                            N++;
                            break;
                        }
                    }
                    else break;
                }
            }
        }
    }
    running = false;
}
int DimensionCalculator::GetBoxCount() const
{
    return N;
}
bool DimensionCalculator::IsRunning() const
{
    return running;
}
void DimensionCalculator::Terminate()
{
    running = false;
}

// ImagePanel
ImagePanel::ImagePanel(wxWindow* parent, int id, int _size) : wxPanel(parent, id, wxDefaultPosition, wxSize(_size, _size))
{
    map = nullptr;
    size = _size;
    div = 20; // Default.

    // Allocates memory for the maps.
    map = new bool* [size];
    for (int i = 0; i < size; i++)
        map[i] = new bool[size];

    // Initializes maps.
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            map[i][j] = false;
    }

    this->SetSize(size, size);
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(ImagePanel::OnPaint));
}
ImagePanel::~ImagePanel()
{
    // Cleanup.
    for (int i = 0; i < size; i++)
    {
        delete[] map[i];
    }
    delete[] map;
}
void ImagePanel::OnPaint(wxPaintEvent&)
{
    // Draw white background.
    wxPaintDC dc(this);
    dc.SetBrush(wxBrush(wxColour(255, 255, 255)));
    dc.SetPen(wxPen(wxColour(255, 255, 255)));
    dc.DrawRectangle(0, 0, size, size);

    if (map != nullptr)
    {
        double epsilon = static_cast<double>(size) / static_cast<double>(div);
        int boxes = 0;

        // Fill color squares.
        dc.SetPen(wxColour(50, 50, 255));
        dc.SetBrush(wxBrush(wxColour(50, 50, 255)));

        // Iterate through the boxes.
        for (int ey = 0; ey < div; ey++)
        {
            for (int ex = 0; ex < div; ex++)
            {
                bool found = false;

                // And count the elements of the map that belong to the set.
                for (int w = ex * epsilon; w < (ex + 1) * epsilon && !found; w++)
                {
                    for (int h = ey * epsilon; h < (ey + 1) * epsilon; h++)
                    {
                        if (w < size && h < size)
                        {
                            if (map[w][h] == true)
                            {
                                found = true;
                                boxes++;
                                dc.DrawRectangle(ex * epsilon, ey * epsilon, ceil(epsilon), ceil(epsilon));
                                break;
                            }
                        }
                        else break;
                    }
                }
            }
        }

        // Now draw the horizontal division lines.
        dc.SetPen(wxColour(0, 0, 0));
        for (int ey = 0; ey < div; ey++)
        {
            int y = ey * epsilon;
            dc.DrawLine(0, y, size, y);
        }
        dc.DrawLine(0, size - 1, size, size - 1);

        // Vertical lines.
        for (int ex = 0; ex < div; ex++)
        {
            int x = ex * epsilon;
            dc.DrawLine(x, 0, x, size);
        }
        dc.DrawLine(size - 1, 0, size - 1, size);

        // Draw fractal.
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (map[i][j] == true)
                {
                    dc.DrawPoint(i, j);
                }
            }
        }

        // Draw text.
        // Calculate the numbers of digits in the number.
        int number = boxes;
        int digits = 1;
        while (number >= 10)
        {
            number /= 10;
            digits++;
        }
        int extra = 0;
        digits -= 3;
        if (digits > 0)
            extra += 10 * digits;

        dc.SetBrush(wxBrush(wxColour(0, 0, 0, 100)));
        dc.SetTextForeground(wxColour(255, 255, 255));
        dc.DrawRectangle(0, 383, 60 + extra, 17);
        wxString outText = wxT("N = ");
        outText += num_to_string(boxes);
        dc.DrawText(outText, 5, 383);
    }
}
void ImagePanel::SetMap(bool** _map, int _div)
{
    // Copy map.
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            map[i][j] = _map[i][j];
        }
    }
    div = _div;
}

// ConfFractOptDialog
ConfFractOptDialog::ConfFractOptDialog(Fractal* _target, wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    target = _target;
    this->SetSizeHints(DimensionFrameSize, wxDefaultSize);

    auto mainBoxxy = new wxBoxSizer(wxVERTICAL);

    mainScroll = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    mainScroll->SetScrollRate(5, 5);
    optionsBoxxy = new wxBoxSizer(wxVERTICAL);

    kRealLabel = new wxStaticText(mainScroll, wxID_ANY, wxT("K real"), wxDefaultPosition, wxDefaultSize, 0);
    kRealLabel->Wrap(-1);
    optionsBoxxy->Add(kRealLabel, 0, wxALL, 5);

    kRealCtrl = new wxTextCtrl(mainScroll, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
    optionsBoxxy->Add(kRealCtrl, 0, wxALL | wxEXPAND, 5);

    kImaginaryLabel = new wxStaticText(mainScroll, wxID_ANY, wxT("K imaginary"), wxDefaultPosition, wxDefaultSize, 0);
    kImaginaryLabel->Wrap(-1);
    optionsBoxxy->Add(kImaginaryLabel, 0, wxALL, 5);

    kImaginaryCtrl = new wxTextCtrl(mainScroll, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
    optionsBoxxy->Add(kImaginaryCtrl, 0, wxALL | wxEXPAND, 5);

    mainScroll->SetSizer(optionsBoxxy);
    mainScroll->Layout();
    optionsBoxxy->Fit(mainScroll);
    mainBoxxy->Add(mainScroll, 7, wxEXPAND | wxALL, 1);

    staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainBoxxy->Add(staticLine, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* buttonBoxxy = new wxBoxSizer(wxHORIZONTAL);

    okButton = new wxButton(this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxxy->Add(okButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    applyButton = new wxButton(this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxxy->Add(applyButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    mainBoxxy->Add(buttonBoxxy, 0, wxEXPAND, 5);

    this->SetSizer(mainBoxxy);
    this->wxTopLevelWindowBase::Layout();

    this->Centre(wxBOTH);

    if (!target->IsJuliaVariety())
    {
        kRealCtrl->Enable(false);
        kImaginaryCtrl->Enable(false);
    }
    this->AdjustOptPanel();

    // Connect Events
    okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfFractOptDialog::OnOk), NULL, this);
    applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfFractOptDialog::OnApply), NULL, this);
}
ConfFractOptDialog::~ConfFractOptDialog()
{
    // Disconnect Events
    okButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfFractOptDialog::OnOk), NULL, this);
    applyButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfFractOptDialog::OnApply), NULL, this);
}

void ConfFractOptDialog::AdjustOptPanel()
{
    // If there are elements from a previous panel deletes them.
    this->DeleteOptPanel();

    PanelOptions* pOptions = target->GetOptPanel();
    int index, labelIndex;

    // If there are elements in pOptions creates panel.
    if (pOptions->GetElementsSize() > 0)
    {
        // Creates elements from each kind.
        for (int i = 0; i < pOptions->GetElementsSize(); i++)
        {
            switch (pOptions->GetPanelOptType(i))
            {
            case PanelOptionType::Label:
            {
                labels.push_back(new wxStaticText(mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                labelIndex = labels.size() - 1;
                labels[labelIndex]->Wrap(-1);
                optionsBoxxy->Add(labels[labelIndex], 0, wxALL, 5);
                foundLabels.push_back(i);
            }
            break;
            case PanelOptionType::TextCtrl:
            {
                labels.push_back(new wxStaticText(mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                labelIndex = labels.size() - 1;
                labels[labelIndex]->Wrap(-1);
                optionsBoxxy->Add(labels[labelIndex], 0, wxALL, 5);

                textControls.push_back(new wxTextCtrl(mainScroll, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0));
                index = textControls.size() - 1;
                optionsBoxxy->Add(textControls[index], 0, wxALL | wxEXPAND, 5);
                foundTextControls.push_back(i);
            }
            break;
            case PanelOptionType::Spin:
            {
                labels.push_back(new wxStaticText(mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                labelIndex = labels.size() - 1;
                labels[labelIndex]->Wrap(-1);
                optionsBoxxy->Add(labels[labelIndex], 0, wxALL, 5);

                spinControls.push_back(new wxSpinCtrl(mainScroll, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0));
                index = spinControls.size() - 1;
                optionsBoxxy->Add(spinControls[index], 0, wxALL | wxEXPAND, 5);
                foundSpinControls.push_back(i);
            }
            break;
            case PanelOptionType::CheckBox:
            {
                checkBoxes.push_back(new wxCheckBox(mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                index = checkBoxes.size() - 1;
                if (pOptions->GetDefault(i) == wxT("true"))
                    checkBoxes[index]->SetValue(true);
                else
                    checkBoxes[index]->SetValue(false);
                optionsBoxxy->Add(checkBoxes[index], 0, wxALL | wxEXPAND, 5);
                foundCheckBoxes.push_back(i);
            }
            break;
            };
        }

        // Creates button to apply options.
        optionsBoxxy->Layout();
        mainScroll->SetScrollbars(20, 20, 0, 50);
    }
}
void ConfFractOptDialog::DeleteOptPanel()
{
    // Deletes panel elements.
    for (unsigned int i = 0; i < labels.size(); i++)
    {
        labels[i]->Destroy();
    }
    labels.clear();
    foundLabels.clear();
    for (unsigned int i = 0; i < textControls.size(); i++)
    {
        textControls[i]->Destroy();
    }
    textControls.clear();
    foundTextControls.clear();
    for (unsigned int i = 0; i < spinControls.size(); i++)
    {
        spinControls[i]->Destroy();
    }
    spinControls.clear();
    foundSpinControls.clear();
    for (unsigned int i = 0; i < checkBoxes.size(); i++)
    {
        checkBoxes[i]->Destroy();
    }
    checkBoxes.clear();
    foundCheckBoxes.clear();
}
void ConfFractOptDialog::SetNewTarget(Fractal* _target)
{
    target = _target;
    if (target->IsJuliaVariety())
    {
        kRealCtrl->Enable(true);
        kRealCtrl->SetValue(num_to_string(_target->GetKReal()));
        kImaginaryCtrl->Enable(true);
        kImaginaryCtrl->SetValue(num_to_string(_target->GetKImaginary()));
    }
    else
    {
        kRealCtrl->Enable(false);
        kImaginaryCtrl->Enable(false);
    }
    this->AdjustOptPanel();
}
void ConfFractOptDialog::OnOk(wxCommandEvent&)
{
    this->Show(false);
}
void ConfFractOptDialog::OnApply(wxCommandEvent&)
{
    // Pass parameters to the fractal and redraws it.
    PanelOptions* pOptions = target->GetOptPanel();
    for (unsigned int i = 0; i < foundTextControls.size(); i++)
    {
        *pOptions->GetDoubleElement(i) = string_to_double(textControls[i]->GetValue());
    }
    for (unsigned int i = 0; i < foundSpinControls.size(); i++)
    {
        *pOptions->GetIntElement(i) = spinControls[i]->GetValue();
    }
    for (unsigned int i = 0; i < foundCheckBoxes.size(); i++)
    {
        if (checkBoxes[i]->GetValue())
            *pOptions->GetBoolElement(i) = true;
        else
            *pOptions->GetBoolElement(i) = false;
    }
    if (target->IsJuliaVariety())
        target->SetK(string_to_double(kRealCtrl->GetValue()), string_to_double(kImaginaryCtrl->GetValue()));
    target->SetFractalPropChanged();
}

// LinePlotter
LinePlotter::LinePlotter(LineParams _params) : mpFX(wxT("Fit"))
{
    params = _params;
}
double LinePlotter::GetY(double x)
{
    return params.m * x + params.b;
}

// PlotWindow
PlotWindow::PlotWindow(const vector<double> &xList, const vector<double> &yList, wxWindow* parent, const wxWindowID id,
                       const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                       : wxFrame(parent, id, title, pos, size, style)
{
    wxIcon icon(GetWxAbsPath({ "Resources", "icon.ico" }), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxFont graphFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_plot = new mpWindow(this, -1, wxPoint(0, 0), wxSize(500, 500), wxBORDER_NONE);
    mpScaleX* xAxis = new mpScaleX(wxT("Epsilon"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    mpScaleY* yAxis = new mpScaleY(wxT("N"), mpALIGN_LEFT, true);
    xAxis->SetDrawOutsideMargins(false);
    yAxis->SetDrawOutsideMargins(false);
    xAxis->SetFont(graphFont);
    yAxis->SetFont(graphFont);

    m_plot->SetMargins(30, 30, 50, 100);
    m_plot->AddLayer(xAxis);
    m_plot->AddLayer(yAxis);
    mpFXYVector* vectorLayer = new mpFXYVector(_("Data"));
    vectorLayer->SetData(xList, yList);
    vectorLayer->SetPen(wxPen(*wxBLUE, 3, wxPENSTYLE_SOLID));
    m_plot->AddLayer(vectorLayer);
    m_plot->Fit();

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(m_plot, 1, wxEXPAND);
    SetAutoLayout(TRUE);
    SetSizer(topSizer);
}
PlotWindow::PlotWindow(LineParams params, const vector<double> &xList, const vector<double> &yList, wxWindow* parent, wxWindowID id,
                       const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                       : wxFrame(parent, id, title, pos, size, style), id_(id)
{
    wxIcon icon(wxT("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxFont graphFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_plot = new mpWindow(this, -1, wxPoint(0, 0), wxSize(500, 500), wxBORDER_NONE);
    mpScaleX* xAxis = new mpScaleX(wxT("Log(1/Epsilon)"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    mpScaleY* yAxis = new mpScaleY(wxT("Log(N)"), mpALIGN_LEFT, true);
    xAxis->SetDrawOutsideMargins(false);
    yAxis->SetDrawOutsideMargins(false);
    xAxis->SetFont(graphFont);
    yAxis->SetFont(graphFont);

    m_plot->SetMargins(30, 30, 50, 100);
    m_plot->AddLayer(xAxis);
    m_plot->AddLayer(yAxis);
    mpFXYVector* vectorLayer = new mpFXYVector(_("Data"));
    vectorLayer->SetData(xList, yList);
    vectorLayer->SetPen(wxPen(*wxBLUE, 3, wxPENSTYLE_SOLID));
    m_plot->AddLayer(new LinePlotter(params));
    m_plot->AddLayer(vectorLayer);
    m_plot->Fit();

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(m_plot, 1, wxEXPAND);
    SetAutoLayout(TRUE);
    SetSizer(topSizer);
}
PlotWindow::~PlotWindow() {}

// DimensionFrame
DimensionFrame::DimensionFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                               const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    threadNumber = Get_Cores();
    dimCalculator = new DimensionCalculator[threadNumber];
    dimThreads = new sf::Thread * [threadNumber];
    for (int i = 0; i < threadNumber; ++i)
    {
        dimThreads[i] = nullptr;
    }

    previewSize = 400;
    target = nullptr;
    confFractOptDialog = nullptr;
    renderingPreview = false;
    calculatingDimension = false;
    scriptSelected = false;
    firstRender = true;
    clock.restart();

    this->SetSizeHints(wxSize(960, 700), wxDefaultSize);

    wxIcon icon(wxT("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxBoxSizer* mainBoxxy = new wxBoxSizer(wxVERTICAL);
    mainPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    mainPanel->SetScrollRate(5, 5);

    wxBoxSizer* subMainBoxxy = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* paramBoxxy = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* fractalBoxxy = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, wxT("Fractal")), wxHORIZONTAL);
    wxBoxSizer* borderBoxxy = new wxBoxSizer(wxVERTICAL);

    wxString fractalChoiceChoices[] = { wxT("Mandelbrot"), wxT("MandelbrotZN"), wxT("Mandelbrot (Julia)"), wxT("MandelbrotZN (Julia)"), wxT("Sine (Julia)"), wxT("Jellyfish"),
                                        wxT("Manowar"), wxT("Manowar (Julia)"), wxT("Tricorn"), wxT("Burning Ship"), wxT("Burning Ship (Julia)"),
                                        wxT("Fractory"), wxT("Cell"), wxT("Magnet"), wxT("Double pendulum") };
    int fractalChoiceNChoices = sizeof(fractalChoiceChoices) / sizeof(wxString);
    fractalChoice = new wxChoice(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, fractalChoiceNChoices, fractalChoiceChoices, 0);
    borderBoxxy->Add(fractalChoice, 0, wxALL | wxEXPAND, 5);

    minXTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("MinX"), wxDefaultPosition, wxDefaultSize, 0);
    minXTxt->Wrap(-1);
    borderBoxxy->Add(minXTxt, 0, wxALL, 5);

    minXCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxT("-1.5"), wxDefaultPosition, wxDefaultSize, 0);
    borderBoxxy->Add(minXCtrl, 0, wxALL | wxEXPAND, 5);

    maxXTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("MaxX"), wxDefaultPosition, wxDefaultSize, 0);
    maxXTxt->Wrap(-1);
    borderBoxxy->Add(maxXTxt, 0, wxALL, 5);

    maxXCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxT("1.5"), wxDefaultPosition, wxDefaultSize, 0);
    borderBoxxy->Add(maxXCtrl, 0, wxALL | wxEXPAND, 5);

    minYTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("MinY"), wxDefaultPosition, wxDefaultSize, 0);
    minYTxt->Wrap(-1);
    borderBoxxy->Add(minYTxt, 0, wxALL, 5);

    minYCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxT("-0.4"), wxDefaultPosition, wxDefaultSize, 0);
    borderBoxxy->Add(minYCtrl, 0, wxALL | wxEXPAND, 5);

    manualMaxYChk = new wxCheckBox(mainPanel, wxID_ANY, wxT("Manual MaxY"), wxDefaultPosition, wxDefaultSize, 0);
    borderBoxxy->Add(manualMaxYChk, 0, wxALL, 5);
    manualMaxYChk->SetValue(true);

    maxYTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("MaxY"), wxDefaultPosition, wxDefaultSize, 0);
    maxYTxt->Wrap(-1);
    borderBoxxy->Add(maxYTxt, 0, wxALL, 5);

    maxYCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxT("0.4"), wxDefaultPosition, wxDefaultSize, 0);

    borderBoxxy->Add(maxYCtrl, 0, wxALL | wxEXPAND, 5);
    fractalBoxxy->Add(borderBoxxy, 1, wxEXPAND, 5);

    wxBoxSizer* fOptBoxxy = new wxBoxSizer(wxVERTICAL);

    iterTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("Iterations"), wxDefaultPosition, wxDefaultSize, 0);
    iterTxt->Wrap(-1);
    fOptBoxxy->Add(iterTxt, 0, wxALL, 5);

    iterCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxT("20000"), wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxxy->Add(iterCtrl, 0, wxALL | wxEXPAND, 5);

    sizeTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("Image size (pixels)"), wxDefaultPosition, wxDefaultSize, 0);
    sizeTxt->Wrap(-1);
    fOptBoxxy->Add(sizeTxt, 0, wxALL, 5);

    sizeCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxT("3000"), wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxxy->Add(sizeCtrl, 0, wxALL | wxEXPAND, 5);

    fOptButton = new wxButton(mainPanel, wxID_ANY, wxT("Configure fractal options"), wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxxy->Add(fOptButton, 0, wxALL | wxEXPAND, 5);

    wxStaticBoxSizer* previewBoxxy = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, wxT("Quick preview")), wxVERTICAL);

    nDivTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("Number of divisions"), wxDefaultPosition, wxDefaultSize, 0);
    nDivTxt->Wrap(-1);
    previewBoxxy->Add(nDivTxt, 0, wxALL, 5);

    nDivSpin = new wxSpinCtrl(mainPanel, wxID_ANY, wxT("20"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 200, 0);
    previewBoxxy->Add(nDivSpin, 0, wxALL | wxEXPAND, 5);

    wxBoxSizer* renderPreBoxxy = new wxBoxSizer(wxVERTICAL);

    previewButton = new wxButton(mainPanel, wxID_ANY, wxT("Render preview"), wxDefaultPosition, wxDefaultSize, 0);
    renderPreBoxxy->Add(previewButton, 0, wxALL | wxEXPAND, 5);

    savePreviewButton = new wxButton(mainPanel, wxID_ANY, wxT("Save preview"), wxDefaultPosition, wxDefaultSize, 0);
    renderPreBoxxy->Add(savePreviewButton, 0, wxALL | wxEXPAND, 5);
    previewBoxxy->Add(renderPreBoxxy, 1, wxEXPAND, 5);
    fOptBoxxy->Add(previewBoxxy, 1, wxEXPAND, 5);
    fractalBoxxy->Add(fOptBoxxy, 1, wxEXPAND, 5);
    paramBoxxy->Add(fractalBoxxy, 1, wxEXPAND, 5);

    wxStaticBoxSizer* dimBoxxy = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, wxT("Box-counting parameters")), wxVERTICAL);

    divTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("Divisions"), wxDefaultPosition, wxDefaultSize, 0);
    divTxt->Wrap(-1);
    dimBoxxy->Add(divTxt, 0, wxALL, 5);

    wxBoxSizer* divBoxxy = new wxBoxSizer(wxVERTICAL);

    divNotebook = new wxNotebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    byFunctionPanel = new wxPanel(divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* byFunctionBoxxy = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* fSide1Boxxy = new wxBoxSizer(wxVERTICAL);

    funcTxt = new wxStaticText(byFunctionPanel, wxID_ANY, wxT("Function:"), wxDefaultPosition, wxDefaultSize, 0);
    funcTxt->Wrap(-1);
    fSide1Boxxy->Add(funcTxt, 0, wxALL, 5);

    wxBoxSizer* fCtrlBoxxy = new wxBoxSizer(wxHORIZONTAL);

    fDeclTxt = new wxStaticText(byFunctionPanel, wxID_ANY, wxT("f(x) = "), wxDefaultPosition, wxDefaultSize, 0);
    fDeclTxt->Wrap(-1);
    fCtrlBoxxy->Add(fDeclTxt, 0, wxALL, 5);

    funcCtrl = new wxTextCtrl(byFunctionPanel, wxID_ANY, wxT("2*x"), wxDefaultPosition, wxDefaultSize, 0);
    fCtrlBoxxy->Add(funcCtrl, 1, wxALL, 5);
    fSide1Boxxy->Add(fCtrlBoxxy, 1, wxEXPAND, 5);
    byFunctionBoxxy->Add(fSide1Boxxy, 1, wxEXPAND, 5);

    funcLine = new wxStaticLine(byFunctionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
    byFunctionBoxxy->Add(funcLine, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* fSide2Boxxy = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* goesFromBoxxy = new wxBoxSizer(wxHORIZONTAL);

    goesFromTxt = new wxStaticText(byFunctionPanel, wxID_ANY, wxT("x goes from"), wxDefaultPosition, wxDefaultSize, 0);
    goesFromTxt->Wrap(-1);
    goesFromBoxxy->Add(goesFromTxt, 0, wxALL, 5);

    xMinSpin = new wxSpinCtrl(byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000000, 1);
    goesFromBoxxy->Add(xMinSpin, 1, wxALL, 5);
    fSide2Boxxy->Add(goesFromBoxxy, 1, wxEXPAND, 5);

    wxBoxSizer* goesToBoxxy = new wxBoxSizer(wxHORIZONTAL);

    goesToTxt = new wxStaticText(byFunctionPanel, wxID_ANY, wxT("to"), wxDefaultPosition, wxDefaultSize, 0);
    goesToTxt->Wrap(-1);
    goesToBoxxy->Add(goesToTxt, 0, wxALL, 5);

    xMaxSpin = new wxSpinCtrl(byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS,
        1, 1000000, 50);
    goesToBoxxy->Add(xMaxSpin, 0, wxALL, 5);
    fSide2Boxxy->Add(goesToBoxxy, 1, wxEXPAND, 5);
    byFunctionBoxxy->Add(fSide2Boxxy, 1, wxEXPAND, 5);

    byFunctionPanel->SetSizer(byFunctionBoxxy);
    byFunctionPanel->Layout();
    byFunctionBoxxy->Fit(byFunctionPanel);
    divNotebook->AddPage(byFunctionPanel, wxT("By function"), true);
    byListPanel = new wxPanel(divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* byListBoxxy;
    byListBoxxy = new wxBoxSizer(wxVERTICAL);

    listCtrl = new wxTextCtrl(byListPanel, wxID_ANY, wxT("2,4,5,6,9,100,200"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    byListBoxxy->Add(listCtrl, 1, wxALL | wxEXPAND, 5);

    byListPanel->SetSizer(byListBoxxy);
    byListPanel->Layout();
    byListBoxxy->Fit(byListPanel);
    divNotebook->AddPage(byListPanel, wxT("By list"), false);

    divBoxxy->Add(divNotebook, 0, wxEXPAND | wxALL, 5);
    dimBoxxy->Add(divBoxxy, 0, wxEXPAND, 5);

    wxBoxSizer* dumpBoxxy = new wxBoxSizer(wxVERTICAL);

    dumpCheck = new wxCheckBox(mainPanel, wxID_ANY, wxT("Dump results to file"), wxDefaultPosition, wxDefaultSize, 0);
    dumpBoxxy->Add(dumpCheck, 0, wxALL, 5);

    // Gets the current directory.
    string outFilePath = GetWorkingDirectory();
    outFilePath += "\\dump.csv";

    filePathCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxString(outFilePath.c_str(), wxConvUTF8), wxDefaultPosition, wxDefaultSize, 0);
    filePathCtrl->Enable(false);

    dumpBoxxy->Add(filePathCtrl, 0, wxALL | wxEXPAND, 5);
    dimBoxxy->Add(dumpBoxxy, 0, wxEXPAND, 5);

    wxBoxSizer* plotBoxxy = new wxBoxSizer(wxHORIZONTAL);

    dataCheck = new wxCheckBox(mainPanel, wxID_ANY, wxT("Plot data"), wxDefaultPosition, wxDefaultSize, 0);
    plotBoxxy->Add(dataCheck, 0, wxALL, 5);

    dataFitCheck = new wxCheckBox(mainPanel, wxID_ANY, wxT("Plot fitted data"), wxDefaultPosition, wxDefaultSize, 0);
    plotBoxxy->Add(dataFitCheck, 0, wxALL, 5);
    dimBoxxy->Add(plotBoxxy, 0, wxEXPAND, 5);

    wxBoxSizer* buttonBoxxy = new wxBoxSizer(wxHORIZONTAL);

    calcButton = new wxButton(mainPanel, wxID_ANY, wxT("Calculate"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxxy->Add(calcButton, 1, wxALL | wxALIGN_BOTTOM, 5);

    closeButton = new wxButton(mainPanel, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxxy->Add(closeButton, 1, wxALL | wxALIGN_BOTTOM, 5);

    helpButton = new wxBitmapButton(mainPanel, wxID_ANY, wxBitmap(GetWxAbsPath({ "Resources", "help.png" }), wxBITMAP_TYPE_ANY),
        wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    buttonBoxxy->Add(helpButton, 0, wxALIGN_BOTTOM | wxALL, 5);

    dimBoxxy->Add(buttonBoxxy, 1, wxEXPAND, 5);
    paramBoxxy->Add(dimBoxxy, 1, wxEXPAND, 5);
    subMainBoxxy->Add(paramBoxxy, 1, wxEXPAND, 5);

    wxBoxSizer* outputBoxxy = new wxBoxSizer(wxVERTICAL);
    //
    previewImage = new ImagePanel(mainPanel, wxID_ANY, previewSize);
    outputBoxxy->Add(previewImage, 0, wxEXPAND | wxALL, 5);

    outLine = new wxStaticLine(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    outputBoxxy->Add(outLine, 0, wxEXPAND | wxALL, 5);

    logCtrl = new wxRichTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxTE_READONLY | wxVSCROLL | wxHSCROLL | wxNO_BORDER | wxWANTS_CHARS);
    outputBoxxy->Add(logCtrl, 1, wxALL | wxEXPAND, 5);

    progressBar = new wxGauge(mainPanel, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
    outputBoxxy->Add(progressBar, 0, wxALL | wxEXPAND, 5);

    progressTxt = new wxStaticText(mainPanel, wxID_ANY, wxT("Progress: Stopped"), wxDefaultPosition, wxDefaultSize, 0);
    progressTxt->Wrap(-1);
    outputBoxxy->Add(progressTxt, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    subMainBoxxy->Add(outputBoxxy, 1, wxEXPAND, 5);

    mainPanel->SetSizer(subMainBoxxy);
    mainPanel->Layout();
    subMainBoxxy->Fit(mainPanel);
    mainBoxxy->Add(mainPanel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainBoxxy);
    this->wxTopLevelWindowBase::Layout();

    this->Centre(wxBOTH);

    // Set welcome log text.
    logCtrl->WriteText(wxT("Dimension calculator log.\n\n"));

    this->GetScriptFractals();

    // Set the default fractal.
    fractalChoice->SetSelection(0);
    this->CreateFractal(previewSize);
    myOpt = target->GetOptions();
    minXCtrl->SetValue(num_to_string(myOpt.minX));
    maxXCtrl->SetValue(num_to_string(myOpt.maxX));
    minYCtrl->SetValue(num_to_string(myOpt.minY));
    maxYCtrl->SetValue(num_to_string(myOpt.maxY));
    iterCtrl->SetValue(num_to_string((int)myOpt.maxIter));

    // Connect Events.
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(DimensionFrame::OnDestroy));
    this->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DimensionFrame::OnUpdateUI));
    this->Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DimensionFrame::OnClose));
    fractalChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(DimensionFrame::OnChangeFractal), NULL, this);
    dumpCheck->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(DimensionFrame::OnChangeDump), NULL, this);
    fOptButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnFractalOpt), NULL, this);
    previewButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnRenderPreview), NULL, this);
    calcButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnCalculate), NULL, this);
    manualMaxYChk->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(DimensionFrame::OnManualMaxY), NULL, this);
    closeButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnClose), NULL, this);
    savePreviewButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnSavePreview), NULL, this);
    helpButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnHelp), NULL, this);
}
DimensionFrame::~DimensionFrame()
{
    // Disconnect Events.
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(DimensionFrame::OnDestroy));
    this->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DimensionFrame::OnUpdateUI));
    fractalChoice->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(DimensionFrame::OnChangeFractal), NULL, this);
    dumpCheck->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(DimensionFrame::OnChangeDump), NULL, this);
    fOptButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnFractalOpt), NULL, this);
    previewButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnRenderPreview), NULL, this);
    calcButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnCalculate), NULL, this);
    manualMaxYChk->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(DimensionFrame::OnManualMaxY), NULL, this);
    closeButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnClose), NULL, this);
    savePreviewButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnSavePreview), NULL, this);
    helpButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DimensionFrame::OnHelp), NULL, this);

    if (calculatingDimension)
    {
        // Stop render.
        target->StopRender();

        // Stop all.
        for (int i = 0; i < threadNumber; i++)
        {
            if (dimCalculator[i].IsRunning())
                dimCalculator[i].Terminate();
            if (dimThreads[i])
            {
                dimThreads[i]->wait();
                delete dimThreads[i];
                dimThreads[i] = nullptr;
            }
        }
        calcButton->SetLabel(wxT("Calculate"));
        calculatingDimension = false;
    }
    if (renderingPreview)
        target->StopRender();

    delete[] dimCalculator;
    delete[] dimThreads;
    fractalHandler.DeleteFractal();
    delete confFractOptDialog;
}

void DimensionFrame::CreateFractal(int pSize)
{
    firstRender = true;
    int choice = fractalChoice->GetCurrentSelection();

    // Compiled fractals.
    switch (choice)
    {
        case FL_MANDELBROT:
        {
            fractalHandler.CreateFractal(FractalType::Mandelbrot, pSize, pSize);
            break;
        }
        case FL_MANDELBROT_ZN:
        {
            fractalHandler.CreateFractal(FractalType::MandelbrotZN, pSize, pSize);
            break;
        }
        case FL_JULIA:
        {
            fractalHandler.CreateFractal(FractalType::Julia, pSize, pSize);
            break;
        }
        case FL_JULIA_ZN:
        {
            fractalHandler.CreateFractal(FractalType::JuliaZN, pSize, pSize);
            break;
        }
        case FL_SINOIDAL:
        {
            fractalHandler.CreateFractal(FractalType::Sinoidal, pSize, pSize);
            break;
        }
        case FL_MEDUSA:
        {
            fractalHandler.CreateFractal(FractalType::Medusa, pSize, pSize);
            break;
        }
        case FL_MANOWAR:
        {
            fractalHandler.CreateFractal(FractalType::Manowar, pSize, pSize);
            break;
        }
        case FL_MANOWAR_JULIA:
        {
            fractalHandler.CreateFractal(FractalType::ManowarJulia, pSize, pSize);
            break;
        }
        case FL_TRICORN:
        {
            fractalHandler.CreateFractal(FractalType::Tricorn, pSize, pSize);
            break;
        }
        case FL_BURNING_SHIP:
        {
            fractalHandler.CreateFractal(FractalType::BurningShip, pSize, pSize);
            break;
        }
        case FL_BURNING_SHIP_JULIA:
        {
            fractalHandler.CreateFractal(FractalType::BurningShipJulia, pSize, pSize);
            break;
        }
        case FL_FRACTORY:
        {
            fractalHandler.CreateFractal(FractalType::Fractory, pSize, pSize);
            break;
        }
        case FL_CELL:
        {
            fractalHandler.CreateFractal(FractalType::Cell, pSize, pSize);
            break;
        }
        case FL_MAGNET:
        {
            fractalHandler.CreateFractal(FractalType::Magnetic, pSize, pSize);
            break;
        }
        case FL_DOUBLE_PENDULUM:
        {
            fractalHandler.CreateFractal(FractalType::DoublePendulum, pSize, pSize);
            break;
        }
        default: ;
    }

    // Script fractals.
    if (choice >= COUNT)
    {
        int idx = choice - COUNT;
        fractalHandler.CreateScriptFractal(pSize, pSize, loadedScripts[scriptList[idx]]);
        scriptSelected = true;
    }
    else
        scriptSelected = false;

    target = fractalHandler.GetFractalPtr();
    if (confFractOptDialog != nullptr)
        confFractOptDialog->SetNewTarget(target);
}
void DimensionFrame::OnRenderPreview(wxCommandEvent&)
{
    if (!renderingPreview)
    {
        target->Resize(previewSize, previewSize);
        myOpt = target->GetOptions();

        myOpt.minX = string_to_double(minXCtrl->GetValue());
        myOpt.maxX = string_to_double(maxXCtrl->GetValue());
        myOpt.minY = string_to_double(minYCtrl->GetValue());

        if (manualMaxYChk->GetValue())
            myOpt.maxY = string_to_double(maxYCtrl->GetValue());
        else
            myOpt.maxY = myOpt.minY + (myOpt.maxX - myOpt.minX);

        myOpt.maxIter = string_to_int(iterCtrl->GetValue());

        target->SetOptions(myOpt);
        target->PrepareRender();
        target->Render();

        this->WriteText(wxT("Starting to render preview\n"));
        previewButton->SetLabel(wxT("Stop"));
        calcButton->Enable(false);
        savePreviewButton->Enable(false);

        if (scriptSelected)
            progressBar->Enable(false);

        renderingPreview = true;
    }
    else
    {
        target->StopRender();
        logCtrl->WriteText(wxT("Preview render stopped\n"));
        previewButton->SetLabel(wxT("Render preview"));
        progressBar->SetValue(0);
        progressTxt->SetLabel(wxString(wxT("Progress: Stopped")));
        calcButton->Enable(true);
        savePreviewButton->Enable(true);

        if (scriptSelected)
            progressBar->Enable(true);

        renderingPreview = false;
    }
}
void DimensionFrame::OnChangeFractal(wxCommandEvent&)
{
    // Set default parameters.
    this->CreateFractal(previewSize);
    myOpt = target->GetOptions();
    minXCtrl->SetValue(num_to_string(myOpt.minX));
    maxXCtrl->SetValue(num_to_string(myOpt.maxX));
    minYCtrl->SetValue(num_to_string(myOpt.minY));
    maxYCtrl->SetValue(num_to_string(myOpt.maxY));
    iterCtrl->SetValue(num_to_string((int)myOpt.maxIter));
}
void DimensionFrame::OnManualMaxY(wxCommandEvent&)
{
    if (manualMaxYChk->GetValue())
        maxYCtrl->Enable(true);
    else
        maxYCtrl->Enable(false);
}
void DimensionFrame::OnChangeDump(wxCommandEvent&)
{
    if (dumpCheck->GetValue())
        filePathCtrl->Enable(true);
    else
        filePathCtrl->Enable(false);
}
void DimensionFrame::OnClose(wxCommandEvent&)
{
    this->Close(true);
    this->Destroy();
}
void DimensionFrame::OnCalculate(wxCommandEvent&)
{
    if (!calculatingDimension)
    {
        // Create divisions vector.
        div.clear();
        mup::ParserX parser;
        parser.SetExpr(funcCtrl->GetValue().wc_str());

        mup::Value xVal;
        parser.DefineVar(_T("x"), mup::Variable(&xVal));
        int xMin = xMinSpin->GetValue();
        int xMax = xMaxSpin->GetValue();

        bool errorStatus = false;
        if (divNotebook->GetSelection() == 0)
        {
            // Get div values from parser.
            try
            {
                for (int x = xMin; x < xMax; x++)
                {
                    xVal = x;
                    div.push_back(parser.Eval().GetFloat());
                }
            }
            catch (mup::ParserError& error)
            {
                wxString err = error.GetMsg();
                logCtrl->WriteText(wxT("Parser error: "));
                logCtrl->WriteText(err);
                logCtrl->WriteText(wxT("\n"));
                errorStatus = true;
            }
        }
        else
        {
            wxString listNumbers = listCtrl->GetValue();
            div = get_int_list(listNumbers);
            if (div.size() == 0)
                errorStatus = true;
        }

        if (!errorStatus)
        {
            // Create fractal.
            size = string_to_int(wxString(sizeCtrl->GetValue()));
            myOpt = target->GetOptions();
            myOpt.minX = string_to_double(minXCtrl->GetValue());
            myOpt.maxX = string_to_double(maxXCtrl->GetValue());
            myOpt.minY = string_to_double(minYCtrl->GetValue());

            if (manualMaxYChk->GetValue())
                myOpt.maxY = string_to_double(maxYCtrl->GetValue());
            else
                myOpt.maxY = myOpt.minY + (myOpt.maxX - myOpt.minX);

            myOpt.maxIter = string_to_int(iterCtrl->GetValue());

            // Compare with previous options.
            Options tempOpt = target->GetOptions();

            if (tempOpt.minX != myOpt.minX || tempOpt.maxX != myOpt.maxX || tempOpt.minY != myOpt.minY ||
                tempOpt.maxY != myOpt.maxY || tempOpt.maxIter != myOpt.maxIter || tempOpt.screenWidth != size ||
                target->GetChangeFractalProp() || firstRender)
            {
                // If a change was made or the render fractal was just created.
                target->Resize(size, size);
                target->SetOptions(myOpt);
                target->PrepareRender();
                target->Render();
                firstRender = false;
            }

            // Divide thread assignment.
            int sizeDiv = size / threadNumber;
            for (int i = 0; i < threadNumber; i++)
            {
                if (i < threadNumber - 1)
                    dimCalculator[i].SetMap(target->GetSetMap(), size, i * sizeDiv, (i + 1) * sizeDiv);
                else
                    dimCalculator[i].SetMap(target->GetSetMap(), size, i * sizeDiv, size);
            }

            if (div.size() > 0)
            {
                divIndex = -1;
                calcButton->SetLabel(wxT("Stop"));
                previewButton->Enable(false);
                savePreviewButton->Enable(false);
                if (scriptSelected) progressBar->Enable(false);
                calculatingDimension = true;
            }
        }
    }
    else
    {
        // Stop render.
        target->StopRender();

        // Stop all.
        for (int i = 0; i < threadNumber; i++)
        {
            if (dimCalculator[i].IsRunning())
                dimCalculator[i].Terminate();
            if (dimThreads[i]) {
                dimThreads[i]->wait();
                delete dimThreads[i];
                dimThreads[i] = nullptr;
            }
        }
        calcButton->SetLabel(wxT("Calculate"));
        previewButton->Enable(true);
        savePreviewButton->Enable(true);
        if (scriptSelected) progressBar->Enable(true);
        logCtrl->WriteText(wxT("Calculation stopped\n"));
        progressBar->SetValue(0);
        progressTxt->SetLabel(wxString(wxT("Calculation stopped\n")));
        calculatingDimension = false;
        firstRender = true;
    }
}
void DimensionFrame::OnUpdateUI(wxUpdateUIEvent&)
{
    if (clock.getElapsedTime().asSeconds() >= 0.05)
    {
        if (renderingPreview)
        {
            if (target->IsRendering())
            {
                // Update progress while rendering preview.
                progress = target->GetRenderProgress();
                if (scriptSelected)
                    progressTxt->SetLabel(wxString(wxT("Rendering")));
                else
                {
                    progressBar->SetValue(progress);
                    progressTxt->SetLabel(wxString(wxT("Progress: ")) + num_to_string(progress) + wxT("%"));
                }
            }
            else
            {
                // Set output image.
                previewImage->SetMap(target->GetSetMap(), nDivSpin->GetValue());
                previewImage->Refresh();
                progressBar->SetValue(0);
                progressTxt->SetLabel(wxString(wxT("Progress: Done")));
                this->WriteText(wxT("Done\n"));
                previewButton->SetLabel(wxT("Render preview"));
                calcButton->Enable(true);
                savePreviewButton->Enable(true);

                if (scriptSelected)
                    progressBar->Enable(true);

                renderingPreview = false;
            }
        }
        if (calculatingDimension)
        {
            if (!target->IsRendering())
            {
                // Check if there are threads running.
                bool threadRunning = false;
                for (int i = 0; i < threadNumber; i++)
                {
                    if (dimCalculator[i].IsRunning())
                        threadRunning = true;
                }

                if (!threadRunning)
                {
                    // Clean up finished threads
                    for (int i = 0; i < threadNumber; ++i) {
                        if (dimThreads[i]) {
                            delete dimThreads[i];
                            dimThreads[i] = nullptr;
                        }
                    }

                    if (divIndex == -1)
                    {
                        // Launch the first pack of threads.
                        this->WriteText(wxT("Starting box count.\n"));
                        this->WriteText(wxT("Epsilon   |   BoxCount.\n"));
                        this->WriteText(wxT("-------------------\n"));
                        divIndex++;
                        for (int i = 0; i < threadNumber; i++)
                        {
                            dimCalculator[i].SetDiv(div[divIndex]);
                            dimThreads[i] = new sf::Thread(&DimensionCalculator::Run, &dimCalculator[i]);
                            dimThreads[i]->launch();
                        }

                        epsilon.clear();
                        boxCount.clear();
                    }
                    else if (divIndex < (int)div.size() - 1)
                    {
                        // Update progress bar.
                        progress = 50 * (1 + (double)divIndex / (double)div.size());
                        progressBar->SetValue(progress);
                        progressTxt->SetLabel(wxString(wxT("Progress: ")) + num_to_string(progress) + wxT("%"));

                        // Get box count.
                        int boxNumber = 0;
                        for (int i = 0; i < threadNumber; i++)
                            boxNumber += dimCalculator[i].GetBoxCount();

                        epsilon.push_back((double)size / (double)div[divIndex]);
                        boxCount.push_back(boxNumber);

                        // Update log text.
                        wxString logOut = num_to_string(epsilon[divIndex]);
                        logOut += wxT(", ");
                        logOut += num_to_string(boxCount[divIndex]);
                        logOut += wxT("\n");
                        this->WriteText(logOut);

                        // Prepare new size and launch.
                        divIndex++;
                        for (int i = 0; i < threadNumber; i++)
                        {
                            dimCalculator[i].SetDiv(div[divIndex]);
                            dimThreads[i] = new sf::Thread(&DimensionCalculator::Run, &dimCalculator[i]);
                            dimThreads[i]->launch();
                        }
                    }
                    else
                    {
                        if (dumpCheck->GetValue())
                        {
                            // Dump results to file.
                            ofstream file;
                            file.open(string(filePathCtrl->GetValue().mb_str()).c_str(), ios::out);
                            for (unsigned int i = 0; i < epsilon.size(); i++)
                                file << epsilon[i] << ", " << boxCount[i] << endl;
                        }

                        // Calculate dimension.
                        vector<double> logEpsilon, logCount;
                        for (unsigned int i = 0; i < epsilon.size(); i++)
                        {
                            logEpsilon.push_back(log(1.0 / epsilon[i]));
                            logCount.push_back(log((double)boxCount[i]));
                        }

                        // Do least squares fitting for m.
                        double n, sumXY, sumX, sumY, sumXSquared;
                        sumXY = sumX = sumY = sumXSquared = 0;
                        n = static_cast<double>(epsilon.size());
                        for (int i = 0; i < n; i++)
                        {
                            sumXY += logEpsilon[i] * logCount[i];
                            sumX += logEpsilon[i];
                            sumY += logCount[i];
                            sumXSquared += pow(logEpsilon[i], 2);
                        }
                        double dimensionFit = (n * sumXY - sumX * sumY) / (n * sumXSquared - pow(sumX, 2));
                        this->WriteText(wxT("Dimension = "));
                        this->WriteText(num_to_string(dimensionFit));
                        this->WriteText(wxT("\n"));

                        // Least squares for b.
                        double b = (sumY * sumXSquared - sumX * sumXY) / (n * sumXSquared - pow(sumX, 2));

                        // Draw Plot
                        if (dataCheck->GetValue())
                        {
                            vector<double> doubleCount;
                            for (unsigned i = 0; i < boxCount.size(); i++)
                            {
                                doubleCount.push_back(boxCount[i]);
                            }
                            PlotWindow* plot = new PlotWindow(epsilon, doubleCount, this, wxID_ANY, wxT("Data plot"));    // Txt: "Data plot"
                            plot->Show(true);
                        }

                        // Draw fitted plot.
                        if (dataFitCheck->GetValue())
                        {
                            LineParams myLine;
                            myLine.m = dimensionFit;
                            myLine.b = b;
                            PlotWindow* plot = new PlotWindow(myLine, logEpsilon, logCount, this, wxID_ANY, wxT("Fitted data plot"));    // Txt: "Fitted data plot"
                            plot->Show(true);
                        }

                        // Update progress bar.
                        progressBar->SetValue(0);
                        progressTxt->SetLabel(wxString(wxT("Progress: Done")));
                        this->WriteText(wxT("Done\n"));

                        calcButton->SetLabel(wxT("Calculate"));
                        previewButton->Enable(true);
                        savePreviewButton->Enable(true);
                        if (scriptSelected) progressBar->Enable(true);
                        calculatingDimension = false;
                    }
                }
            }
            else
            {
                // Updates progress bar while rendering.
                progress = target->GetRenderProgress() / 2;

                if (scriptSelected)
                    progressTxt->SetLabel(wxString(wxT("Calculating dimension")));
                else
                {
                    progressBar->SetValue(progress);
                    progressTxt->SetLabel(wxString(wxT("Progress: ")) + num_to_string(progress) + wxT("%"));
                }
            }
        }
        clock.restart();
    }
}
void DimensionFrame::OnDestroy(wxCloseEvent&)
{
    dimensionFrameState = false;
    this->Destroy();
}
void DimensionFrame::WriteText(const wxString &txt) const
{
    logCtrl->MoveEnd();
    logCtrl->WriteText(txt);
    logCtrl->ShowPosition(logCtrl->GetCaretPosition());
}
void DimensionFrame::OnFractalOpt(wxCommandEvent&)
{
    if (confFractOptDialog == nullptr)
        confFractOptDialog = new ConfFractOptDialog(target, this);

    if (confFractOptDialog->IsVisible())
        confFractOptDialog->SetFocus();
    else
        confFractOptDialog->Show(true);

    // Adjust position.
    int h, w;
    GetDesktopResolution(h, w);
    if (this->GetPosition().x + this->GetSize().GetWidth() + 5 < w && this->GetPosition().y < h)
        confFractOptDialog->Move(this->GetPosition().x + this->GetSize().GetWidth() + 5, this->GetPosition().y);
}
void DimensionFrame::OnSavePreview(wxCommandEvent&)
{
    auto openFileDialog = new wxFileDialog(this, wxT("Select file name"), wxT(""),
                               wxT("dimension_preview.bmp"), wxT("BMP file (*.bmp)|*.bmp"), wxFD_SAVE);
    if (openFileDialog->ShowModal() == wxID_OK)
    {
        wxString wxFileName = openFileDialog->GetPath();
        wxString fileName = wxFileName.c_str();

        // Render the fractal.
        size = string_to_int(wxString(sizeCtrl->GetValue().c_str()));
        target->Resize(size, size);
        myOpt = target->GetOptions();
        myOpt.minX = string_to_double(minXCtrl->GetValue());
        myOpt.maxX = string_to_double(maxXCtrl->GetValue());
        myOpt.minY = string_to_double(minYCtrl->GetValue());

        if (manualMaxYChk->GetValue())
            myOpt.maxY = string_to_double(maxYCtrl->GetValue());
        else
            myOpt.maxY = myOpt.minY + (myOpt.maxX - myOpt.minX);

        myOpt.maxIter = string_to_int(iterCtrl->GetValue());

        target->SetOptions(myOpt);
        target->PrepareRender();
        target->Render();

        SaveProgressDiag* saveProgress = new SaveProgressDiag(target, this, false);
        saveProgress->ShowModal();

        if (saveProgress->IsFinished())
        {
            // Allocate.
            bool** setMap, ** tempSetMap, ** colorMap;
            setMap = target->GetSetMap();
            int nDiv = nDivSpin->GetValue();

            tempSetMap = new bool* [size];
            colorMap = new bool* [size];
            for (int i = 0; i < size; i++)
            {
                tempSetMap[i] = new bool[size];
                colorMap[i] = new bool[size];
            }
            // Copy and init.
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    tempSetMap[i][j] = setMap[i][j];
                    colorMap[i][j] = false;
                }
            }

            //int N = 0;
            double local_epsilon = static_cast<double>(size) / static_cast<double>(nDiv);

            for (int ey = 0; ey < nDiv; ey++)
            {
                for (int ex = 0; ex < nDiv; ex++)
                {
                    bool found = false;
                    for (int w = ex * local_epsilon; w < (ex + 1) * local_epsilon && !found; w++)
                    {
                        for (int h = ey * local_epsilon; h < (ey + 1) * local_epsilon; h++)
                        {
                            if (w < size && h < size)
                            {
                                if (setMap[w][h] == true)
                                {
                                    found = true;
                                    // Color square
                                    for (int y = ey * local_epsilon; y < (ey + 1) * local_epsilon; y++)
                                    {
                                        for (int x = ex * local_epsilon; x < (ex + 1) * local_epsilon; x++)
                                        {
                                            if (x < size && y < size)
                                                colorMap[x][y] = true;
                                        }
                                    }
                                    break;
                                }
                            }
                            else
                                break;
                        }
                    }
                }
            }

            // Horizontal lines.
            for (int ey = 0; ey < nDiv; ey++)
            {
                int y = ey * local_epsilon;
                for (int x = 0; x < size; x++)
                    tempSetMap[x][y] = true;
            }
            // Last line.
            for (int x = 0; x < size; x++)
                tempSetMap[x][size - 1] = true;

            // Vertical lines
            for (int ex = 0; ex < nDiv; ex++)
            {
                int x = ex * local_epsilon;
                for (int y = 0; y < size; y++)
                    tempSetMap[x][y] = true;
            }
            // Last line.
            for (int y = 0; y < size; y++)
                tempSetMap[size - 1][y] = true;

            // Write BMP.
            BMPWriter writer(fileName.mb_str(), size, size);
            BMPPixel* data = new BMPPixel[size];

            // Copy maps values to BMPWriter.
            for (int j = size - 1; j >= 0; j--)
            {
                for (int i = 0; i < size; i++)
                {
                    if (tempSetMap[i][j] == true)
                    {
                        data[i].r = 0;
                        data[i].g = 0;
                        data[i].b = 0;
                    }
                    else if (colorMap[i][j] == true)
                    {
                        data[i].r = static_cast<unsigned>(0x32);
                        data[i].g = static_cast<unsigned>(0x32);
                        data[i].b = static_cast<unsigned>(0xFF);
                    }
                    else
                    {
                        data[i].r = static_cast<unsigned>(0xFF);
                        data[i].g = static_cast<unsigned>(0xFF);
                        data[i].b = static_cast<unsigned>(0xFF);
                    }
                }
                writer.WriteLine(data);
            }
            writer.CloseBMP();
            delete[] data;

            // Cleanup.
            for (int i = 0; i < size; i++)
            {
                delete[] tempSetMap[i];
                delete[] colorMap[i];
            }
            delete[] tempSetMap;
            delete[] colorMap;
        }
        saveProgress->Destroy();
    }
    openFileDialog->Destroy();
}
void DimensionFrame::GetScriptFractals()
{
    loadedScripts = GetValidUserScripts();

    // Gets script parameters.
    for (unsigned int i = 0; i < loadedScripts.size(); i++)
    {
        if (!loadedScripts[i].noSetMap)
        {
            scriptList.push_back(i);
            fractalChoice->Append(wxString(loadedScripts[i].name.c_str(), wxConvUTF8));
        }
    }
}
void DimensionFrame::OnHelp(wxCommandEvent&)
{
    auto diag = new HTMLViewer(GetWxAbsPath({ "Resources", "Tutorials", "dimTut.html" }),
                         this, wxID_ANY, wxString(wxT("Calculate dimension help")));
    diag->ShowModal();
    diag->Destroy();
}
