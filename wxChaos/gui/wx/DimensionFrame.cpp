#include <limits>
#include <fstream>
#include <mpParser.h>
#include "AppPaths.h"
#include "DimensionFrame.h"
#include "TextUtils.h"
#include "SizeDialogSave.h"
#include "BmpImageWriter.h"
#include "AngelscriptBindings.h"
#include "HTMLViewer.h"
#include "SystemUtilities.h"
using namespace std;

wxDEFINE_EVENT(wxEVT_DIMENSION_FRAME_CLOSED, wxCommandEvent);

double GetMaxElement(const vector<double> &in)
{
    double max = -std::numeric_limits<double>::infinity();
    for (const double i : in)
    {
        if (i > max)
            max = i;
    }
    return max;
}
double GetMinElement(const vector<double> &in)
{
    double min = std::numeric_limits<double>::infinity();
    for (const double i : in)
    {
        if (i < min)
            min = i;
    }
    return min;
}

// DimensionCalculator
DimensionCalculator::DimensionCalculator() : _ho(0), _hf(0), _map(nullptr), _size(0), _div(0)
{
    _running = false;
    _boxCountN = 0;
}

void DimensionCalculator::SetMap(bool** map, const int size, const int ho, const int hf)
{
    _map = map;
    _size = size;
    _ho = ho;
    _hf = hf;
}
void DimensionCalculator::SetDiv(const int div)
{
    _div = div;
}
void DimensionCalculator::Run()
{
    _running = true;

    _boxCountN = 0;
    const double epsilon = static_cast<double>(_size) / static_cast<double>(_div);
    const int ey_init = static_cast<int>(static_cast<double>(_ho) / epsilon);
    const int ey_end = static_cast<int>(static_cast<double>(_hf) / epsilon);

    // Iterate through the boxes.
    for (int ey = ey_init; ey < ey_end; ey++)
    {
        for (int ex = 0; ex < _div; ex++)
        {
            if (!_running) return; // Check for termination signal
            bool found = false;

            // And count the elements of the map that belong to the set.
            for (int w = static_cast<int>(ex * epsilon); w < (ex + 1) * epsilon && !found; w++)
            {
                for (int h = static_cast<int>(ey * epsilon); h < (ey + 1) * epsilon; h++)
                {
                    if (w < _size && h < _size)
                    {
                        if (_map[w][h] == true)
                        {
                            found = true;
                            _boxCountN++;
                            break;
                        }
                    }
                    else break;
                }
            }
        }
    }
    _running = false;
}
int DimensionCalculator::GetBoxCount() const
{
    return _boxCountN;
}
bool DimensionCalculator::IsRunning() const
{
    return _running;
}
void DimensionCalculator::Terminate()
{
    _running = false;
}

// ImagePanel
ImagePanel::ImagePanel(wxWindow* parent, const int id, const int size)
                       : wxPanel(parent, id, wxDefaultPosition, wxSize(size, size))
{
    _map = nullptr;
    _size = size;
    _div = 20; // Default.

    // Allocates memory for the maps.
    _map = new bool* [_size];
    for (int i = 0; i < _size; i++)
        _map[i] = new bool[_size];

    // Initializes maps.
    for (int i = 0; i < _size; i++)
    {
        for (int j = 0; j < _size; j++)
            _map[i][j] = false;
    }

    this->SetSize(_size, _size);
    this->Bind(wxEVT_PAINT, &ImagePanel::OnPaintEvent, this);
}
ImagePanel::~ImagePanel()
{
    // Cleanup.
    for (int i = 0; i < _size; i++)
    {
        delete[] _map[i];
    }
    delete[] _map;
}
void ImagePanel::OnPaintEvent(wxPaintEvent&)
{
    // Draw white background.
    wxPaintDC dc(this);
    dc.SetBrush(wxBrush(wxColour(255, 255, 255)));
    dc.SetPen(wxPen(wxColour(255, 255, 255)));
    dc.DrawRectangle(0, 0, _size, _size);

    if (_map != nullptr)
    {
        const double epsilon = static_cast<double>(_size) / static_cast<double>(_div);
        int boxes = 0;

        // Fill color squares.
        dc.SetPen(wxColour(50, 50, 255));
        dc.SetBrush(wxBrush(wxColour(50, 50, 255)));

        // Iterate through the boxes.
        for (int ey = 0; ey < _div; ey++)
        {
            for (int ex = 0; ex < _div; ex++)
            {
                bool found = false;

                // And count the elements of the map that belong to the set.
                for (int w = static_cast<int>(ex * epsilon); w < (ex + 1) * epsilon && !found; w++)
                {
                    for (int h = static_cast<int>(ey * epsilon); h < (ey + 1) * epsilon; h++)
                    {
                        if (w < _size && h < _size)
                        {
                            if (_map[w][h] == true)
                            {
                                found = true;
                                boxes++;
                                const auto xPosition = static_cast<wxCoord>(ex * epsilon);
                                const auto yPosition = static_cast<wxCoord>(ey * epsilon);
                                dc.DrawRectangle(xPosition, yPosition, ceil(epsilon), ceil(epsilon));
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
        for (int ey = 0; ey < _div; ey++)
        {
            const int y = static_cast<int>(ey * epsilon);
            dc.DrawLine(0, y, _size, y);
        }
        dc.DrawLine(0, _size - 1, _size, _size - 1);

        // Vertical lines.
        for (int ex = 0; ex < _div; ex++)
        {
            const int x = static_cast<int>(ex * epsilon);
            dc.DrawLine(x, 0, x, _size);
        }
        dc.DrawLine(_size - 1, 0, _size - 1, _size);

        // Draw fractal.
        for (int i = 0; i < _size; i++)
        {
            for (int j = 0; j < _size; j++)
            {
                if (_map[i][j] == true)
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
        outText += TextUtils::ToWxString(boxes);
        dc.DrawText(outText, 5, 383);
    }
}
void ImagePanel::SetMap(bool** map, const int div)
{
    // Copy map.
    for (int i = 0; i < _size; i++)
    {
        for (int j = 0; j < _size; j++)
        {
            _map[i][j] = map[i][j];
        }
    }
    _div = div;
}

// ConfFractOptDialog
ConfigFractalOptionsDialog::ConfigFractalOptionsDialog(Fractal* target, wxWindow* parent, const wxWindowID id,
                                                       const wxString& title, const wxPoint& pos, const wxSize& size,
                                                       const long style) : wxDialog(parent, id, title, pos, size, style)
{
    _target = target;
    this->SetSizeHints(DimensionFrameSize, wxDefaultSize);

    const auto mainBoxSizer = new wxBoxSizer(wxVERTICAL);

    _mainScroll = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    _mainScroll->SetScrollRate(5, 5);
    _optionsBox = new wxBoxSizer(wxVERTICAL);

    _kRealLabel = new wxStaticText(_mainScroll, wxID_ANY, wxT("K real"), wxDefaultPosition, wxDefaultSize, 0);
    _kRealLabel->Wrap(-1);
    _optionsBox->Add(_kRealLabel, 0, wxALL, 5);

    _kRealCtrl = new wxTextCtrl(_mainScroll, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
    _optionsBox->Add(_kRealCtrl, 0, wxALL | wxEXPAND, 5);

    _kImaginaryLabel = new wxStaticText(_mainScroll, wxID_ANY, wxT("K imaginary"), wxDefaultPosition, wxDefaultSize, 0);
    _kImaginaryLabel->Wrap(-1);
    _optionsBox->Add(_kImaginaryLabel, 0, wxALL, 5);

    _kImaginaryCtrl = new wxTextCtrl(_mainScroll, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
    _optionsBox->Add(_kImaginaryCtrl, 0, wxALL | wxEXPAND, 5);

    _mainScroll->SetSizer(_optionsBox);
    _mainScroll->Layout();
    _optionsBox->Fit(_mainScroll);
    mainBoxSizer->Add(_mainScroll, 7, wxEXPAND | wxALL, 1);

    _staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainBoxSizer->Add(_staticLine, 0, wxEXPAND | wxALL, 5);

    const auto buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _okButton = new wxButton(this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_okButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    _applyButton = new wxButton(this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_applyButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    mainBoxSizer->Add(buttonBoxSizer, 0, wxEXPAND, 5);

    this->SetSizer(mainBoxSizer);
    this->wxTopLevelWindowBase::Layout();

    this->Centre(wxBOTH);

    if (!_target->IsJuliaVariety())
    {
        _kRealCtrl->Enable(false);
        _kImaginaryCtrl->Enable(false);
    }
    this->AdjustOptPanel();

    // Connect Events
    _okButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ConfigFractalOptionsDialog::OnOk, this);
    _applyButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ConfigFractalOptionsDialog::OnApply, this);
}
ConfigFractalOptionsDialog::~ConfigFractalOptionsDialog()
{
    // Disconnect Events
    _okButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ConfigFractalOptionsDialog::OnOk, this);
    _applyButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ConfigFractalOptionsDialog::OnApply, this);
}

void ConfigFractalOptionsDialog::AdjustOptPanel()
{
    // If there are elements from a previous panel, deletes them.
    this->DeleteOptPanel();

    // If there are elements in pOptions creates panel.
    if (PanelOptions* pOptions = _target->GetOptPanel(); pOptions->GetElementsSize() > 0)
    {
        size_t labelIndex;
        size_t index;

        // Creates elements of each kind.
        for (int i = 0; i < pOptions->GetElementsSize(); i++)
        {
            switch (pOptions->GetPanelOptType(i))
            {
                case PanelOptionType::Label:
                {
                    _labels.push_back(new wxStaticText(_mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                    labelIndex = _labels.size() - 1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionsBox->Add(_labels[labelIndex], 0, wxALL, 5);
                    _foundLabels.push_back(i);
                }
                break;
                case PanelOptionType::TextCtrl:
                {
                    _labels.push_back(new wxStaticText(_mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                    labelIndex = _labels.size() - 1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionsBox->Add(_labels[labelIndex], 0, wxALL, 5);

                    _textControls.push_back(new wxTextCtrl(_mainScroll, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, 0));
                    index = _textControls.size() - 1;
                    _optionsBox->Add(_textControls[index], 0, wxALL | wxEXPAND, 5);
                    _foundTextControls.push_back(i);
                }
                break;
                case PanelOptionType::Spin:
                {
                    _labels.push_back(new wxStaticText(_mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                    labelIndex = _labels.size() - 1;
                    _labels[labelIndex]->Wrap(-1);
                    _optionsBox->Add(_labels[labelIndex], 0, wxALL, 5);

                    _spinControls.push_back(new wxSpinCtrl(_mainScroll, wxID_ANY, wxString(pOptions->GetDefault(i)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0));
                    index = _spinControls.size() - 1;
                    _optionsBox->Add(_spinControls[index], 0, wxALL | wxEXPAND, 5);
                    _foundSpinControls.push_back(i);
                }
                break;
                case PanelOptionType::CheckBox:
                {
                    _checkBoxes.push_back(new wxCheckBox(_mainScroll, wxID_ANY, wxString(pOptions->GetLabelElement(i)), wxDefaultPosition, wxDefaultSize, 0));
                    index = _checkBoxes.size() - 1;
                    if (pOptions->GetDefault(i) == wxT("true"))
                        _checkBoxes[index]->SetValue(true);
                    else
                        _checkBoxes[index]->SetValue(false);
                    _optionsBox->Add(_checkBoxes[index], 0, wxALL | wxEXPAND, 5);
                    _foundCheckBoxes.push_back(i);
                }
                break;
            };
        }

        // Creates button to apply options.
        _optionsBox->Layout();
        _mainScroll->SetScrollbars(20, 20, 0, 50);
    }
}
void ConfigFractalOptionsDialog::DeleteOptPanel()
{
    // Deletes panel elements.
    for (const auto & _label : _labels)
        _label->Destroy();

    _labels.clear();
    _foundLabels.clear();
    for (const auto & _textControl : _textControls)
        _textControl->Destroy();

    _textControls.clear();
    _foundTextControls.clear();
    for (const auto & _spinControl : _spinControls)
        _spinControl->Destroy();

    _spinControls.clear();
    _foundSpinControls.clear();
    for (const auto & _checkBoxe : _checkBoxes)
        _checkBoxe->Destroy();

    _checkBoxes.clear();
    _foundCheckBoxes.clear();
}
void ConfigFractalOptionsDialog::SetNewTarget(Fractal* target)
{
    _target = target;
    if (_target->IsJuliaVariety())
    {
        _kRealCtrl->Enable(true);
        _kRealCtrl->SetValue(TextUtils::ToWxString(target->GetKReal()));
        _kImaginaryCtrl->Enable(true);
        _kImaginaryCtrl->SetValue(TextUtils::ToWxString(target->GetKImaginary()));
    }
    else
    {
        _kRealCtrl->Enable(false);
        _kImaginaryCtrl->Enable(false);
    }
    this->AdjustOptPanel();
}
void ConfigFractalOptionsDialog::OnOk(wxCommandEvent&)
{
    this->Show(false);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ConfigFractalOptionsDialog::OnApply(wxCommandEvent&)
{
    // Pass parameters to the fractal and redraws it.
    const PanelOptions* pOptions = _target->GetOptPanel();
    for (unsigned int i = 0; i < _foundTextControls.size(); i++)
    {
        *pOptions->GetDoubleElement(i) = TextUtils::ToDouble(_textControls[i]->GetValue());
    }
    for (unsigned int i = 0; i < _foundSpinControls.size(); i++)
    {
        *pOptions->GetIntElement(i) = _spinControls[i]->GetValue();
    }
    for (unsigned int i = 0; i < _foundCheckBoxes.size(); i++)
    {
        if (_checkBoxes[i]->GetValue())
            *pOptions->GetBoolElement(i) = true;
        else
            *pOptions->GetBoolElement(i) = false;
    }
    if (_target->IsJuliaVariety())
        _target->SetK(TextUtils::ToDouble(_kRealCtrl->GetValue()), TextUtils::ToDouble(_kImaginaryCtrl->GetValue()));
    _target->SetFractalPropChanged();
}

// LinePlotter
LinePlotter::LinePlotter(const LineParams params) : mpFX(wxT("Fit")), _params(params) {}
double LinePlotter::GetY(const double x)
{
    return _params.m * x + _params.b;
}

// PlotWindow
PlotWindow::PlotWindow(const vector<double> &xList, const vector<double> &yList, wxWindow* parent, const wxWindowID id,
                       const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                       : wxFrame(parent, id, title, pos, size, style)
{
    const wxIcon icon(AppPaths::ResourceFile({wxT("icon.ico")}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxFont graphFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    _plot = new mpWindow(this, -1, wxPoint(0, 0), wxSize(500, 500), wxBORDER_NONE);
    auto* xAxis = new mpScaleX(wxT("Epsilon"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    const auto yAxis = new mpScaleY(wxT("N"), mpALIGN_LEFT, true);
    xAxis->SetDrawOutsideMargins(false);
    yAxis->SetDrawOutsideMargins(false);
    xAxis->SetFont(graphFont);
    yAxis->SetFont(graphFont);

    _plot->SetMargins(30, 30, 50, 100);
    _plot->AddLayer(xAxis);
    _plot->AddLayer(yAxis);
    const auto vectorLayer = new mpFXYVector(_("Data"));
    vectorLayer->SetData(xList, yList);
    vectorLayer->SetPen(wxPen(*wxBLUE, 3, wxPENSTYLE_SOLID));
    _plot->AddLayer(vectorLayer);
    _plot->Fit();

    const auto topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(_plot, 1, wxEXPAND);
    SetAutoLayout(TRUE);
    SetSizer(topSizer);
}
PlotWindow::PlotWindow(const LineParams params, const vector<double> &xList, const vector<double> &yList, wxWindow* parent,
                       const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                       : wxFrame(parent, id, title, pos, size, style), _id(id)
{
    const wxIcon icon(wxT("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    wxFont graphFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    _plot = new mpWindow(this, -1, wxPoint(0, 0), wxSize(500, 500), wxBORDER_NONE);
    const auto xAxis = new mpScaleX(wxT("Log(1/Epsilon)"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    const auto yAxis = new mpScaleY(wxT("Log(N)"), mpALIGN_LEFT, true);
    xAxis->SetDrawOutsideMargins(false);
    yAxis->SetDrawOutsideMargins(false);
    xAxis->SetFont(graphFont);
    yAxis->SetFont(graphFont);

    _plot->SetMargins(30, 30, 50, 100);
    _plot->AddLayer(xAxis);
    _plot->AddLayer(yAxis);
    const auto vectorLayer = new mpFXYVector(_("Data"));
    vectorLayer->SetData(xList, yList);
    vectorLayer->SetPen(wxPen(*wxBLUE, 3, wxPENSTYLE_SOLID));
    _plot->AddLayer(new LinePlotter(params));
    _plot->AddLayer(vectorLayer);
    _plot->Fit();

    auto* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(_plot, 1, wxEXPAND);
    SetAutoLayout(TRUE);
    SetSizer(topSizer);
}

PlotWindow::~PlotWindow() = default;

// DimensionFrame
DimensionFrame::DimensionFrame(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
                               const wxSize& size, const long style) : wxFrame(parent, id, title, pos, size, style)
{
    _threadNumber = Get_Cores();
    _dimensionCalculator = new DimensionCalculator[_threadNumber];
    _dimThreads = new sf::Thread * [_threadNumber];
    for (int i = 0; i < _threadNumber; ++i)
        _dimThreads[i] = nullptr;

    _previewSize = 400;
    _target = nullptr;
    _confFractOptDialog = nullptr;
    _renderingPreview = false;
    _calculatingDimension = false;
    _scriptSelected = false;
    _firstRender = true;
    _clock.restart();

    this->SetSizeHints(wxSize(960, 700), wxDefaultSize);

    const wxIcon icon(wxT("Resources/icon.ico"), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    const auto mainBoxSizer = new wxBoxSizer(wxVERTICAL);
    _mainPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    _mainPanel->SetScrollRate(5, 5);

    const auto subMainBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto paramBoxSizer = new wxBoxSizer(wxVERTICAL);
    const auto fractalBoxSizer = new wxStaticBoxSizer(new wxStaticBox(_mainPanel, wxID_ANY, wxT("Fractal")), wxHORIZONTAL);
    const auto borderBoxSizer = new wxBoxSizer(wxVERTICAL);

    const wxString fractalChoiceChoices[] = { wxT("Mandelbrot"), wxT("MandelbrotZN"), wxT("Mandelbrot (Julia)"), wxT("MandelbrotZN (Julia)"), wxT("Sine (Julia)"), wxT("Jellyfish"),
                                              wxT("Manowar"), wxT("Manowar (Julia)"), wxT("Tricorn"), wxT("Burning Ship"), wxT("Burning Ship (Julia)"),
                                              wxT("Fractory"), wxT("Cell"), wxT("Magnet"), wxT("Double pendulum") };
    constexpr int fractalChoiceNChoices = sizeof(fractalChoiceChoices) / sizeof(wxString);
    _fractalChoice = new wxChoice(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, fractalChoiceNChoices, fractalChoiceChoices, 0);
    borderBoxSizer->Add(_fractalChoice, 0, wxALL | wxEXPAND, 5);

    _minXTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("MinX"), wxDefaultPosition, wxDefaultSize, 0);
    _minXTxt->Wrap(-1);
    borderBoxSizer->Add(_minXTxt, 0, wxALL, 5);

    _minXCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, wxT("-1.5"), wxDefaultPosition, wxDefaultSize, 0);
    borderBoxSizer->Add(_minXCtrl, 0, wxALL | wxEXPAND, 5);

    _maxXTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("MaxX"), wxDefaultPosition, wxDefaultSize, 0);
    _maxXTxt->Wrap(-1);
    borderBoxSizer->Add(_maxXTxt, 0, wxALL, 5);

    _maxXCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, wxT("1.5"), wxDefaultPosition, wxDefaultSize, 0);
    borderBoxSizer->Add(_maxXCtrl, 0, wxALL | wxEXPAND, 5);

    _minYTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("MinY"), wxDefaultPosition, wxDefaultSize, 0);
    _minYTxt->Wrap(-1);
    borderBoxSizer->Add(_minYTxt, 0, wxALL, 5);

    _minYCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, wxT("-0.4"), wxDefaultPosition, wxDefaultSize, 0);
    borderBoxSizer->Add(_minYCtrl, 0, wxALL | wxEXPAND, 5);

    _manualMaxYChk = new wxCheckBox(_mainPanel, wxID_ANY, wxT("Manual MaxY"), wxDefaultPosition, wxDefaultSize, 0);
    borderBoxSizer->Add(_manualMaxYChk, 0, wxALL, 5);
    _manualMaxYChk->SetValue(true);

    _maxYTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("MaxY"), wxDefaultPosition, wxDefaultSize, 0);
    _maxYTxt->Wrap(-1);
    borderBoxSizer->Add(_maxYTxt, 0, wxALL, 5);

    _maxYCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, wxT("0.4"), wxDefaultPosition, wxDefaultSize, 0);

    borderBoxSizer->Add(_maxYCtrl, 0, wxALL | wxEXPAND, 5);
    fractalBoxSizer->Add(borderBoxSizer, 1, wxEXPAND, 5);

    const auto fOptBoxSizer = new wxBoxSizer(wxVERTICAL);

    _iterTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("Iterations"), wxDefaultPosition, wxDefaultSize, 0);
    _iterTxt->Wrap(-1);
    fOptBoxSizer->Add(_iterTxt, 0, wxALL, 5);

    _iterCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, wxT("20000"), wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxSizer->Add(_iterCtrl, 0, wxALL | wxEXPAND, 5);

    _sizeTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("Image size (pixels)"), wxDefaultPosition, wxDefaultSize, 0);
    _sizeTxt->Wrap(-1);
    fOptBoxSizer->Add(_sizeTxt, 0, wxALL, 5);

    _sizeCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, wxT("3000"), wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxSizer->Add(_sizeCtrl, 0, wxALL | wxEXPAND, 5);

    _fractalOptionsButton = new wxButton(_mainPanel, wxID_ANY, wxT("Configure fractal options"), wxDefaultPosition, wxDefaultSize, 0);
    fOptBoxSizer->Add(_fractalOptionsButton, 0, wxALL | wxEXPAND, 5);

    const auto previewBoxSizer = new wxStaticBoxSizer(new wxStaticBox(_mainPanel, wxID_ANY, wxT("Quick preview")), wxVERTICAL);

    _nDivTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("Number of divisions"), wxDefaultPosition, wxDefaultSize, 0);
    _nDivTxt->Wrap(-1);
    previewBoxSizer->Add(_nDivTxt, 0, wxALL, 5);

    _nDivSpin = new wxSpinCtrl(_mainPanel, wxID_ANY, wxT("20"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 200, 0);
    previewBoxSizer->Add(_nDivSpin, 0, wxALL | wxEXPAND, 5);

    const auto renderPreBoxSizer = new wxBoxSizer(wxVERTICAL);

    _previewButton = new wxButton(_mainPanel, wxID_ANY, wxT("Render preview"), wxDefaultPosition, wxDefaultSize, 0);
    renderPreBoxSizer->Add(_previewButton, 0, wxALL | wxEXPAND, 5);

    _savePreviewButton = new wxButton(_mainPanel, wxID_ANY, wxT("Save preview"), wxDefaultPosition, wxDefaultSize, 0);
    renderPreBoxSizer->Add(_savePreviewButton, 0, wxALL | wxEXPAND, 5);
    previewBoxSizer->Add(renderPreBoxSizer, 1, wxEXPAND, 5);
    fOptBoxSizer->Add(previewBoxSizer, 1, wxEXPAND, 5);
    fractalBoxSizer->Add(fOptBoxSizer, 1, wxEXPAND, 5);
    paramBoxSizer->Add(fractalBoxSizer, 1, wxEXPAND, 5);

    const auto dimBoxSizer = new wxStaticBoxSizer(new wxStaticBox(_mainPanel, wxID_ANY, wxT("Box-counting parameters")), wxVERTICAL);

    _divTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("Divisions"), wxDefaultPosition, wxDefaultSize, 0);
    _divTxt->Wrap(-1);
    dimBoxSizer->Add(_divTxt, 0, wxALL, 5);

    const auto divBoxSizer = new wxBoxSizer(wxVERTICAL);

    _divNotebook = new wxNotebook(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    _byFunctionPanel = new wxPanel(_divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto byFunctionBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto fSide1BoxSizer = new wxBoxSizer(wxVERTICAL);

    _funcTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, wxT("Function:"), wxDefaultPosition, wxDefaultSize, 0);
    _funcTxt->Wrap(-1);
    fSide1BoxSizer->Add(_funcTxt, 0, wxALL, 5);

    const auto fCtrlBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _fDeclTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, wxT("f(x) = "), wxDefaultPosition, wxDefaultSize, 0);
    _fDeclTxt->Wrap(-1);
    fCtrlBoxSizer->Add(_fDeclTxt, 0, wxALL, 5);

    _funcCtrl = new wxTextCtrl(_byFunctionPanel, wxID_ANY, wxT("2*x"), wxDefaultPosition, wxDefaultSize, 0);
    fCtrlBoxSizer->Add(_funcCtrl, 1, wxALL, 5);
    fSide1BoxSizer->Add(fCtrlBoxSizer, 1, wxEXPAND, 5);
    byFunctionBoxSizer->Add(fSide1BoxSizer, 1, wxEXPAND, 5);

    _funcLine = new wxStaticLine(_byFunctionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
    byFunctionBoxSizer->Add(_funcLine, 0, wxEXPAND | wxALL, 5);

    const auto fSide2BoxSizer = new wxBoxSizer(wxVERTICAL);
    const auto goesFromBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _goesFromTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, wxT("x goes from"), wxDefaultPosition, wxDefaultSize, 0);
    _goesFromTxt->Wrap(-1);
    goesFromBoxSizer->Add(_goesFromTxt, 0, wxALL, 5);

    _xMinSpin = new wxSpinCtrl(_byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000000, 1);
    goesFromBoxSizer->Add(_xMinSpin, 1, wxALL, 5);
    fSide2BoxSizer->Add(goesFromBoxSizer, 1, wxEXPAND, 5);

    const auto goesToBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _goesToTxt = new wxStaticText(_byFunctionPanel, wxID_ANY, wxT("to"), wxDefaultPosition, wxDefaultSize, 0);
    _goesToTxt->Wrap(-1);
    goesToBoxSizer->Add(_goesToTxt, 0, wxALL, 5);

    _xMaxSpin = new wxSpinCtrl(_byFunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000000, 50);
    goesToBoxSizer->Add(_xMaxSpin, 0, wxALL, 5);
    fSide2BoxSizer->Add(goesToBoxSizer, 1, wxEXPAND, 5);
    byFunctionBoxSizer->Add(fSide2BoxSizer, 1, wxEXPAND, 5);

    _byFunctionPanel->SetSizer(byFunctionBoxSizer);
    _byFunctionPanel->Layout();
    byFunctionBoxSizer->Fit(_byFunctionPanel);
    _divNotebook->AddPage(_byFunctionPanel, wxT("By function"), true);
    _byListPanel = new wxPanel(_divNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto byListBoxSizer = new wxBoxSizer(wxVERTICAL);

    _listCtrl = new wxTextCtrl(_byListPanel, wxID_ANY, wxT("2,4,5,6,9,100,200"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    byListBoxSizer->Add(_listCtrl, 1, wxALL | wxEXPAND, 5);

    _byListPanel->SetSizer(byListBoxSizer);
    _byListPanel->Layout();
    byListBoxSizer->Fit(_byListPanel);
    _divNotebook->AddPage(_byListPanel, wxT("By list"), false);

    divBoxSizer->Add(_divNotebook, 0, wxEXPAND | wxALL, 5);
    dimBoxSizer->Add(divBoxSizer, 0, wxEXPAND, 5);

    const auto dumpBoxSizer = new wxBoxSizer(wxVERTICAL);

    _dumpCheck = new wxCheckBox(_mainPanel, wxID_ANY, wxT("Dump results to file"), wxDefaultPosition, wxDefaultSize, 0);
    dumpBoxSizer->Add(_dumpCheck, 0, wxALL, 5);

    _filePathCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, AppPaths::DumpFile(), wxDefaultPosition, wxDefaultSize, 0);
    _filePathCtrl->Enable(false);

    dumpBoxSizer->Add(_filePathCtrl, 0, wxALL | wxEXPAND, 5);
    dimBoxSizer->Add(dumpBoxSizer, 0, wxEXPAND, 5);

    const auto plotBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _dataCheck = new wxCheckBox(_mainPanel, wxID_ANY, wxT("Plot data"), wxDefaultPosition, wxDefaultSize, 0);
    plotBoxSizer->Add(_dataCheck, 0, wxALL, 5);

    _dataFitCheck = new wxCheckBox(_mainPanel, wxID_ANY, wxT("Plot fitted data"), wxDefaultPosition, wxDefaultSize, 0);
    plotBoxSizer->Add(_dataFitCheck, 0, wxALL, 5);
    dimBoxSizer->Add(plotBoxSizer, 0, wxEXPAND, 5);

    const auto buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);

    _calcButton = new wxButton(_mainPanel, wxID_ANY, wxT("Calculate"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_calcButton, 1, wxALL | wxALIGN_BOTTOM, 5);

    _closeButton = new wxButton(_mainPanel, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxSizer->Add(_closeButton, 1, wxALL | wxALIGN_BOTTOM, 5);

    _helpButton = new wxBitmapButton(_mainPanel, wxID_ANY, wxBitmap(AppPaths::ResourceFile({wxT("help.png")}),
                                    wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    buttonBoxSizer->Add(_helpButton, 0, wxALIGN_BOTTOM | wxALL, 5);

    dimBoxSizer->Add(buttonBoxSizer, 1, wxEXPAND, 5);
    paramBoxSizer->Add(dimBoxSizer, 1, wxEXPAND, 5);
    subMainBoxSizer->Add(paramBoxSizer, 1, wxEXPAND, 5);

    const auto outputBoxSizer = new wxBoxSizer(wxVERTICAL);
    //
    _previewImage = new ImagePanel(_mainPanel, wxID_ANY, _previewSize);
    outputBoxSizer->Add(_previewImage, 0, wxEXPAND | wxALL, 5);

    _outLine = new wxStaticLine(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    outputBoxSizer->Add(_outLine, 0, wxEXPAND | wxALL, 5);

    _logCtrl = new wxRichTextCtrl(_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxTE_READONLY | wxVSCROLL | wxHSCROLL | wxNO_BORDER | wxWANTS_CHARS);
    outputBoxSizer->Add(_logCtrl, 1, wxALL | wxEXPAND, 5);

    _progressBar = new wxGauge(_mainPanel, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
    outputBoxSizer->Add(_progressBar, 0, wxALL | wxEXPAND, 5);

    _progressTxt = new wxStaticText(_mainPanel, wxID_ANY, wxT("Progress: Stopped"), wxDefaultPosition, wxDefaultSize, 0);
    _progressTxt->Wrap(-1);
    outputBoxSizer->Add(_progressTxt, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    subMainBoxSizer->Add(outputBoxSizer, 1, wxEXPAND, 5);

    _mainPanel->SetSizer(subMainBoxSizer);
    _mainPanel->Layout();
    subMainBoxSizer->Fit(_mainPanel);
    mainBoxSizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainBoxSizer);
    this->wxTopLevelWindowBase::Layout();

    this->Centre(wxBOTH);

    // Set welcome log text.
    _logCtrl->WriteText(wxT("Dimension calculator log.\n\n"));

    this->GetScriptFractals();

    // Set the default fractal.
    _fractalChoice->SetSelection(0);
    this->CreateFractal(_previewSize);
    _myOpt = _target->GetOptions();
    _minXCtrl->SetValue(TextUtils::ToWxString(_myOpt.minX));
    _maxXCtrl->SetValue(TextUtils::ToWxString(_myOpt.maxX));
    _minYCtrl->SetValue(TextUtils::ToWxString(_myOpt.minY));
    _maxYCtrl->SetValue(TextUtils::ToWxString(_myOpt.maxY));
    _iterCtrl->SetValue(TextUtils::ToWxString(static_cast<int>(_myOpt.maxIter)));

    // Connect Events.
    this->Bind(wxEVT_CLOSE_WINDOW, &DimensionFrame::OnDestroy, this);
    this->Bind(wxEVT_UPDATE_UI, &DimensionFrame::OnUpdateUI, this);
    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &DimensionFrame::OnClose, this, wxID_EXIT);
    _fractalChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &DimensionFrame::OnChangeFractal, this);
    _dumpCheck->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &DimensionFrame::OnChangeDump, this);
    _fractalOptionsButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnFractalOpt, this);
    _previewButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnRenderPreview, this);
    _calcButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnCalculate, this);
    _manualMaxYChk->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &DimensionFrame::OnManualMaxY, this);
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnClose, this);
    _savePreviewButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnSavePreview, this);
    _helpButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnHelp, this);
}
DimensionFrame::~DimensionFrame()
{
    // Disconnect Events.
    this->Unbind(wxEVT_CLOSE_WINDOW, &DimensionFrame::OnDestroy, this);
    this->Unbind(wxEVT_UPDATE_UI, &DimensionFrame::OnUpdateUI, this);
    _fractalChoice->Unbind(wxEVT_COMMAND_CHOICE_SELECTED, &DimensionFrame::OnChangeFractal, this);
    _dumpCheck->Unbind(wxEVT_COMMAND_CHECKBOX_CLICKED, &DimensionFrame::OnChangeDump, this);
    _fractalOptionsButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnFractalOpt, this);
    _previewButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnRenderPreview, this);
    _calcButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnCalculate, this);
    _manualMaxYChk->Unbind(wxEVT_COMMAND_CHECKBOX_CLICKED, &DimensionFrame::OnManualMaxY, this);
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnClose, this);
    _savePreviewButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnSavePreview, this);
    _helpButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DimensionFrame::OnHelp, this);

    if (_calculatingDimension)
    {
        // Stop render.
        _target->StopRender();

        // Stop all.
        for (int i = 0; i < _threadNumber; i++)
        {
            if (_dimensionCalculator[i].IsRunning())
                _dimensionCalculator[i].Terminate();
            if (_dimThreads[i])
            {
                _dimThreads[i]->wait();
                delete _dimThreads[i];
                _dimThreads[i] = nullptr;
            }
        }
        _calcButton->SetLabel(wxT("Calculate"));
        _calculatingDimension = false;
    }
    if (_renderingPreview)
        _target->StopRender();

    delete[] _dimensionCalculator;
    delete[] _dimThreads;
    _fractalHandler.DeleteFractal();
    delete _confFractOptDialog;
}

void DimensionFrame::CreateFractal(int size)
{
    _firstRender = true;
    const int choice = _fractalChoice->GetCurrentSelection();
    switch (choice)
    {
        case FL_MANDELBROT:
        {
            _fractalHandler.CreateFractal(FractalType::Mandelbrot, size, size);
            break;
        }
        case FL_MANDELBROT_ZN:
        {
            _fractalHandler.CreateFractal(FractalType::MandelbrotZN, size, size);
            break;
        }
        case FL_JULIA:
        {
            _fractalHandler.CreateFractal(FractalType::Julia, size, size);
            break;
        }
        case FL_JULIA_ZN:
        {
            _fractalHandler.CreateFractal(FractalType::JuliaZN, size, size);
            break;
        }
        case FL_SINOIDAL:
        {
            _fractalHandler.CreateFractal(FractalType::Sinoidal, size, size);
            break;
        }
        case FL_MEDUSA:
        {
            _fractalHandler.CreateFractal(FractalType::Medusa, size, size);
            break;
        }
        case FL_MANOWAR:
        {
            _fractalHandler.CreateFractal(FractalType::Manowar, size, size);
            break;
        }
        case FL_MANOWAR_JULIA:
        {
            _fractalHandler.CreateFractal(FractalType::ManowarJulia, size, size);
            break;
        }
        case FL_TRICORN:
        {
            _fractalHandler.CreateFractal(FractalType::Tricorn, size, size);
            break;
        }
        case FL_BURNING_SHIP:
        {
            _fractalHandler.CreateFractal(FractalType::BurningShip, size, size);
            break;
        }
        case FL_BURNING_SHIP_JULIA:
        {
            _fractalHandler.CreateFractal(FractalType::BurningShipJulia, size, size);
            break;
        }
        case FL_FRACTORY:
        {
            _fractalHandler.CreateFractal(FractalType::Fractory, size, size);
            break;
        }
        case FL_CELL:
        {
            _fractalHandler.CreateFractal(FractalType::Cell, size, size);
            break;
        }
        case FL_MAGNET:
        {
            _fractalHandler.CreateFractal(FractalType::Magnetic, size, size);
            break;
        }
        case FL_DOUBLE_PENDULUM:
        {
            _fractalHandler.CreateFractal(FractalType::DoublePendulum, size, size);
            break;
        }
        default: ;
    }

    // Script fractals.
    if (choice >= COUNT)
    {
        const int idx = choice - COUNT;
        _fractalHandler.CreateScriptFractal(size, size, _loadedScripts[_scriptList[idx]]);
        _scriptSelected = true;
    }
    else
        _scriptSelected = false;

    _target = _fractalHandler.GetFractalPtr();
    if (_confFractOptDialog != nullptr)
        _confFractOptDialog->SetNewTarget(_target);
}
void DimensionFrame::OnRenderPreview(wxCommandEvent&)
{
    if (!_renderingPreview)
    {
        _target->Resize(_previewSize, _previewSize);
        _myOpt = _target->GetOptions();

        _myOpt.minX = TextUtils::ToDouble(_minXCtrl->GetValue());
        _myOpt.maxX = TextUtils::ToDouble(_maxXCtrl->GetValue());
        _myOpt.minY = TextUtils::ToDouble(_minYCtrl->GetValue());

        if (_manualMaxYChk->GetValue())
            _myOpt.maxY = TextUtils::ToDouble(_maxYCtrl->GetValue());
        else
            _myOpt.maxY = _myOpt.minY + (_myOpt.maxX - _myOpt.minX);

        _myOpt.maxIter = TextUtils::ToInt(_iterCtrl->GetValue());

        _target->SetOptions(_myOpt);
        _target->PrepareRender();
        _target->Render();

        this->WriteText(wxT("Starting to render preview\n"));
        _previewButton->SetLabel(wxT("Stop"));
        _calcButton->Enable(false);
        _savePreviewButton->Enable(false);

        if (_scriptSelected)
            _progressBar->Enable(false);

        _renderingPreview = true;
    }
    else
    {
        _target->StopRender();
        _logCtrl->WriteText(wxT("Preview render stopped\n"));
        _previewButton->SetLabel(wxT("Render preview"));
        _progressBar->SetValue(0);
        _progressTxt->SetLabel(wxString(wxT("Progress: Stopped")));
        _calcButton->Enable(true);
        _savePreviewButton->Enable(true);

        if (_scriptSelected)
            _progressBar->Enable(true);

        _renderingPreview = false;
    }
}
void DimensionFrame::OnChangeFractal(wxCommandEvent&)
{
    // Set default parameters.
    this->CreateFractal(_previewSize);
    _myOpt = _target->GetOptions();
    _minXCtrl->SetValue(TextUtils::ToWxString(_myOpt.minX));
    _maxXCtrl->SetValue(TextUtils::ToWxString(_myOpt.maxX));
    _minYCtrl->SetValue(TextUtils::ToWxString(_myOpt.minY));
    _maxYCtrl->SetValue(TextUtils::ToWxString(_myOpt.maxY));
    _iterCtrl->SetValue(TextUtils::ToWxString(static_cast<int>(_myOpt.maxIter)));
}
// ReSharper disable once CppMemberFunctionMayBeConst
void DimensionFrame::OnManualMaxY(wxCommandEvent&)
{
    if (_manualMaxYChk->GetValue())
        _maxYCtrl->Enable(true);
    else
        _maxYCtrl->Enable(false);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void DimensionFrame::OnChangeDump(wxCommandEvent&)
{
    if (_dumpCheck->GetValue())
        _filePathCtrl->Enable(true);
    else
        _filePathCtrl->Enable(false);
}
void DimensionFrame::OnClose(wxCommandEvent&)
{
    this->Close(true);
}
void DimensionFrame::OnCalculate(wxCommandEvent&)
{
    if (!_calculatingDimension)
    {
        // Create divisions vector.
        _div.clear();
        mup::ParserX parser;
        parser.SetExpr(_funcCtrl->GetValue().utf8_string());

        mup::Value xVal;
        parser.DefineVar("x", mup::Variable(&xVal));
        const int xMin = _xMinSpin->GetValue();
        const int xMax = _xMaxSpin->GetValue();

        bool errorStatus = false;
        if (_divNotebook->GetSelection() == 0)
        {
            // Get div values from parser.
            try
            {
                for (int x = xMin; x < xMax; x++)
                {
                    xVal = static_cast<double>(x);
                    _div.push_back(static_cast<int>(parser.Eval().GetInteger()));
                }
            }
            catch (mup::ParserError& error)
            {
                const wxString err = error.GetMsg();
                _logCtrl->WriteText(wxT("Parser error: "));
                _logCtrl->WriteText(err);
                _logCtrl->WriteText(wxT("\n"));
                errorStatus = true;
            }
        }
        else
        {
            const wxString listNumbers = _listCtrl->GetValue();
            _div = TextUtils::ParseIntList(listNumbers);
            if (_div.empty())
                errorStatus = true;
        }

        if (!errorStatus)
        {
            // Create fractal.
            _size = TextUtils::ToInt(wxString(_sizeCtrl->GetValue()));
            _myOpt = _target->GetOptions();
            _myOpt.minX = TextUtils::ToDouble(_minXCtrl->GetValue());
            _myOpt.maxX = TextUtils::ToDouble(_maxXCtrl->GetValue());
            _myOpt.minY = TextUtils::ToDouble(_minYCtrl->GetValue());

            if (_manualMaxYChk->GetValue())
                _myOpt.maxY = TextUtils::ToDouble(_maxYCtrl->GetValue());
            else
                _myOpt.maxY = _myOpt.minY + (_myOpt.maxX - _myOpt.minX);

            _myOpt.maxIter = TextUtils::ToInt(_iterCtrl->GetValue());

            // Compare with previous options.
            const Options tempOpt = _target->GetOptions();

            if (tempOpt.minX != _myOpt.minX || tempOpt.maxX != _myOpt.maxX || tempOpt.minY != _myOpt.minY ||
                tempOpt.maxY != _myOpt.maxY || tempOpt.maxIter != _myOpt.maxIter || tempOpt.screenWidth != _size ||
                _target->GetChangeFractalProp() || _firstRender)
            {
                // If a change was made or the render fractal was just created.
                _target->Resize(_size, _size);
                _target->SetOptions(_myOpt);
                _target->PrepareRender();
                _target->Render();
                _firstRender = false;
            }

            // Divide thread assignment.
            const int sizeDiv = _size / _threadNumber;
            for (int i = 0; i < _threadNumber; i++)
            {
                if (i < _threadNumber - 1)
                    _dimensionCalculator[i].SetMap(_target->GetSetMap(), _size, i * sizeDiv, (i + 1) * sizeDiv);
                else
                    _dimensionCalculator[i].SetMap(_target->GetSetMap(), _size, i * sizeDiv, _size);
            }

            if (!_div.empty())
            {
                _divIndex = -1;
                _calcButton->SetLabel(wxT("Stop"));
                _previewButton->Enable(false);
                _savePreviewButton->Enable(false);
                if (_scriptSelected) _progressBar->Enable(false);
                _calculatingDimension = true;
            }
        }
    }
    else
    {
        // Stop render.
        _target->StopRender();

        // Stop all.
        for (int i = 0; i < _threadNumber; i++)
        {
            if (_dimensionCalculator[i].IsRunning())
                _dimensionCalculator[i].Terminate();
            if (_dimThreads[i]) {
                _dimThreads[i]->wait();
                delete _dimThreads[i];
                _dimThreads[i] = nullptr;
            }
        }
        _calcButton->SetLabel(wxT("Calculate"));
        _previewButton->Enable(true);
        _savePreviewButton->Enable(true);
        if (_scriptSelected) _progressBar->Enable(true);
        _logCtrl->WriteText(wxT("Calculation stopped\n"));
        _progressBar->SetValue(0);
        _progressTxt->SetLabel(wxString(wxT("Calculation stopped\n")));
        _calculatingDimension = false;
        _firstRender = true;
    }
}
void DimensionFrame::OnUpdateUI(wxUpdateUIEvent&)
{
    if (_clock.getElapsedTime().asSeconds() >= 0.05)
    {
        if (_renderingPreview)
        {
            if (_target->IsRendering())
            {
                // Update progress while rendering preview.
                _progress = _target->GetRenderProgress();
                if (_scriptSelected)
                    _progressTxt->SetLabel(wxString(wxT("Rendering")));
                else
                {
                    _progressBar->SetValue(_progress);
                    _progressTxt->SetLabel(wxString(wxT("Progress: ")) + TextUtils::ToWxString(_progress) + wxT("%"));
                }
            }
            else
            {
                // Set output image.
                _previewImage->SetMap(_target->GetSetMap(), _nDivSpin->GetValue());
                _previewImage->Refresh();
                _progressBar->SetValue(0);
                _progressTxt->SetLabel(wxString(wxT("Progress: Done")));
                this->WriteText(wxT("Done\n"));
                _previewButton->SetLabel(wxT("Render preview"));
                _calcButton->Enable(true);
                _savePreviewButton->Enable(true);

                if (_scriptSelected)
                    _progressBar->Enable(true);

                _renderingPreview = false;
            }
        }
        if (_calculatingDimension)
        {
            if (!_target->IsRendering())
            {
                // Check if there are threads running.
                bool threadRunning = false;
                for (int i = 0; i < _threadNumber; i++)
                {
                    if (_dimensionCalculator[i].IsRunning())
                        threadRunning = true;
                }

                if (!threadRunning)
                {
                    // Clean up finished threads
                    for (int i = 0; i < _threadNumber; ++i) {
                        if (_dimThreads[i]) {
                            delete _dimThreads[i];
                            _dimThreads[i] = nullptr;
                        }
                    }

                    if (_divIndex == -1)
                    {
                        // Launch the first pack of threads.
                        this->WriteText(wxT("Starting box count.\n"));
                        this->WriteText(wxT("Epsilon   |   BoxCount.\n"));
                        this->WriteText(wxT("-------------------\n"));
                        _divIndex++;
                        for (int i = 0; i < _threadNumber; i++)
                        {
                            _dimensionCalculator[i].SetDiv(_div[_divIndex]);
                            _dimThreads[i] = new sf::Thread(&DimensionCalculator::Run, &_dimensionCalculator[i]);
                            _dimThreads[i]->launch();
                        }

                        _epsilon.clear();
                        _boxCount.clear();
                    }
                    else if (_divIndex < static_cast<int>(_div.size()) - 1)
                    {
                        // Update progress bar.
                        _progress = static_cast<int>(50 * (1 + static_cast<double>(_divIndex) / static_cast<double>(_div.size())));
                        _progressBar->SetValue(_progress);
                        _progressTxt->SetLabel(wxString(wxT("Progress: ")) + TextUtils::ToWxString(_progress) + wxT("%"));

                        // Get box count.
                        int boxNumber = 0;
                        for (int i = 0; i < _threadNumber; i++)
                            boxNumber += _dimensionCalculator[i].GetBoxCount();

                        _epsilon.push_back(static_cast<double>(_size) / static_cast<double>(_div[_divIndex]));
                        _boxCount.push_back(boxNumber);

                        // Update log text.
                        wxString logOut = TextUtils::ToWxString(_epsilon[_divIndex]);
                        logOut += wxT(", ");
                        logOut += TextUtils::ToWxString(_boxCount[_divIndex]);
                        logOut += wxT("\n");
                        this->WriteText(logOut);

                        // Prepare new size and launch.
                        _divIndex++;
                        for (int i = 0; i < _threadNumber; i++)
                        {
                            _dimensionCalculator[i].SetDiv(_div[_divIndex]);
                            _dimThreads[i] = new sf::Thread(&DimensionCalculator::Run, &_dimensionCalculator[i]);
                            _dimThreads[i]->launch();
                        }
                    }
                    else
                    {
                        if (_dumpCheck->GetValue())
                        {
                            // Dump results to file.
                            ofstream file;
                            file.open(string(_filePathCtrl->GetValue().mb_str()).c_str(), ios::out);
                            for (unsigned int i = 0; i < _epsilon.size(); i++)
                                file << _epsilon[i] << ", " << _boxCount[i] << endl;
                        }

                        // Calculate dimension.
                        vector<double> logEpsilon, logCount;
                        for (unsigned int i = 0; i < _epsilon.size(); i++)
                        {
                            logEpsilon.push_back(log(1.0 / _epsilon[i]));
                            logCount.push_back(log(static_cast<double>(_boxCount[i])));
                        }

                        // Do least squares fitting for m.
                        double n, sumXY, sumX, sumY, sumXSquared;
                        sumXY = sumX = sumY = sumXSquared = 0;
                        n = static_cast<double>(_epsilon.size());
                        for (int i = 0; i < n; i++)
                        {
                            sumXY += logEpsilon[i] * logCount[i];
                            sumX += logEpsilon[i];
                            sumY += logCount[i];
                            sumXSquared += pow(logEpsilon[i], 2);
                        }
                        double dimensionFit = (n * sumXY - sumX * sumY) / (n * sumXSquared - pow(sumX, 2));
                        this->WriteText(wxT("Dimension = "));
                        this->WriteText(TextUtils::ToWxString(dimensionFit));
                        this->WriteText(wxT("\n"));

                        // Least squares for b.
                        double b = (sumY * sumXSquared - sumX * sumXY) / (n * sumXSquared - pow(sumX, 2));

                        // Draw Plot
                        if (_dataCheck->GetValue())
                        {
                            vector<double> doubleCount;
                            for (int i : _boxCount)
                                doubleCount.push_back(i);

                            auto plot = new PlotWindow(_epsilon, doubleCount, this, wxID_ANY, wxT("Data plot"));    // Txt: "Data plot"
                            plot->Show(true);
                        }

                        // Draw fitted plot.
                        if (_dataFitCheck->GetValue())
                        {
                            LineParams myLine{};
                            myLine.m = dimensionFit;
                            myLine.b = b;
                            auto plot = new PlotWindow(myLine, logEpsilon, logCount, this, wxID_ANY, wxT("Fitted data plot"));    // Txt: "Fitted data plot"
                            plot->Show(true);
                        }

                        // Update progress bar.
                        _progressBar->SetValue(0);
                        _progressTxt->SetLabel(wxString(wxT("Progress: Done")));
                        this->WriteText(wxT("Done\n"));

                        _calcButton->SetLabel(wxT("Calculate"));
                        _previewButton->Enable(true);
                        _savePreviewButton->Enable(true);
                        if (_scriptSelected) _progressBar->Enable(true);
                        _calculatingDimension = false;
                    }
                }
            }
            else
            {
                // Updates progress bar while rendering.
                _progress = _target->GetRenderProgress() / 2;

                if (_scriptSelected)
                    _progressTxt->SetLabel(wxString(wxT("Calculating dimension")));
                else
                {
                    _progressBar->SetValue(_progress);
                    _progressTxt->SetLabel(wxString(wxT("Progress: ")) + TextUtils::ToWxString(_progress) + wxT("%"));
                }
            }
        }
        _clock.restart();
    }
}
void DimensionFrame::OnDestroy(wxCloseEvent&)
{
    wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_DIMENSION_FRAME_CLOSED));
    this->Destroy();
}
void DimensionFrame::WriteText(const wxString &txt) const
{
    _logCtrl->MoveEnd();
    _logCtrl->WriteText(txt);
    _logCtrl->ShowPosition(_logCtrl->GetCaretPosition());
}
void DimensionFrame::OnFractalOpt(wxCommandEvent&)
{
    if (_confFractOptDialog == nullptr)
        _confFractOptDialog = new ConfigFractalOptionsDialog(_target, this);

    if (_confFractOptDialog->IsVisible())
        _confFractOptDialog->SetFocus();
    else
        _confFractOptDialog->Show(true);

    // Adjust position.
    int h, w;
    GetDesktopResolution(h, w);
    if (this->GetPosition().x + this->GetSize().GetWidth() + 5 < w && this->GetPosition().y < h)
        _confFractOptDialog->Move(this->GetPosition().x + this->GetSize().GetWidth() + 5, this->GetPosition().y);
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
        _size = TextUtils::ToInt(wxString(_sizeCtrl->GetValue().c_str()));
        _target->Resize(_size, _size);
        _myOpt = _target->GetOptions();
        _myOpt.minX = TextUtils::ToDouble(_minXCtrl->GetValue());
        _myOpt.maxX = TextUtils::ToDouble(_maxXCtrl->GetValue());
        _myOpt.minY = TextUtils::ToDouble(_minYCtrl->GetValue());

        if (_manualMaxYChk->GetValue())
            _myOpt.maxY = TextUtils::ToDouble(_maxYCtrl->GetValue());
        else
            _myOpt.maxY = _myOpt.minY + (_myOpt.maxX - _myOpt.minX);

        _myOpt.maxIter = TextUtils::ToInt(_iterCtrl->GetValue());

        _target->SetOptions(_myOpt);
        _target->PrepareRender();
        _target->Render();

        auto* saveProgress = new SaveProgressDiag(_target, this, false);
        saveProgress->ShowModal();

        if (saveProgress->IsFinished())
        {
            // Allocate.
            bool** setMap, ** tempSetMap, ** colorMap;
            setMap = _target->GetSetMap();
            int nDiv = _nDivSpin->GetValue();

            tempSetMap = new bool* [_size];
            colorMap = new bool* [_size];
            for (int i = 0; i < _size; i++)
            {
                tempSetMap[i] = new bool[_size];
                colorMap[i] = new bool[_size];
            }
            // Copy and init.
            for (int i = 0; i < _size; i++)
            {
                for (int j = 0; j < _size; j++)
                {
                    tempSetMap[i][j] = setMap[i][j];
                    colorMap[i][j] = false;
                }
            }

            //int N = 0;
            double local_epsilon = static_cast<double>(_size) / static_cast<double>(nDiv);

            for (int ey = 0; ey < nDiv; ey++)
            {
                for (int ex = 0; ex < nDiv; ex++)
                {
                    bool found = false;
                    for (int w = static_cast<int>(ex * local_epsilon); w < (ex + 1) * local_epsilon && !found; w++)
                    {
                        for (int h = static_cast<int>(ey * local_epsilon); h < (ey + 1) * local_epsilon; h++)
                        {
                            if (w < _size && h < _size)
                            {
                                if (setMap[w][h] == true)
                                {
                                    found = true;
                                    // Color square
                                    for (int y = static_cast<int>(ey * local_epsilon); y < (ey + 1) * local_epsilon; y++)
                                    {
                                        for (int x = static_cast<int>(ex * local_epsilon); x < (ex + 1) * local_epsilon; x++)
                                        {
                                            if (x < _size && y < _size)
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
                int y = static_cast<int>(ey * local_epsilon);
                for (int x = 0; x < _size; x++)
                    tempSetMap[x][y] = true;
            }
            // Last line.
            for (int x = 0; x < _size; x++)
                tempSetMap[x][_size - 1] = true;

            // Vertical lines
            for (int ex = 0; ex < nDiv; ex++)
            {
                int x = static_cast<int>(ex * local_epsilon);
                for (int y = 0; y < _size; y++)
                    tempSetMap[x][y] = true;
            }
            // Last line.
            for (int y = 0; y < _size; y++)
                tempSetMap[_size - 1][y] = true;

            // Write BMP.
            const std::string outputPath(fileName.mb_str());
            BmpImageWriter writer(outputPath, _size, _size);
            std::vector<BmpPixel> data(_size);
            bool writeSuccess = writer.IsOpen();

            // Copy maps values to BmpImageWriter.
            for (int j = 0; j < _size && writeSuccess; j++)
            {
                for (int i = 0; i < _size; i++)
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
                writeSuccess = writer.WriteRow(data);
            }
            writeSuccess = writer.Close() && writeSuccess;
            if (!writeSuccess)
                wxMessageBox("Failed to save image to file: " + outputPath, "Error", wxOK | wxICON_ERROR);

            // Cleanup.
            for (int i = 0; i < _size; i++)
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
    _loadedScripts = GetValidUserScripts();

    // Gets script parameters.
    for (unsigned int i = 0; i < _loadedScripts.size(); i++)
    {
        if (!_loadedScripts[i].noSetMap)
        {
            _scriptList.push_back(i);
            _fractalChoice->Append(wxString(_loadedScripts[i].name.c_str(), wxConvUTF8));
        }
    }
}
void DimensionFrame::OnHelp(wxCommandEvent&)
{
    const auto diag = new HTMLViewer(AppPaths::ResourceFile({wxT("Tutorials"), wxT("dimTut.html")}),
                                this, wxID_ANY, wxString(wxT("Calculate dimension help")));
    diag->ShowModal();
    diag->Destroy();
}
