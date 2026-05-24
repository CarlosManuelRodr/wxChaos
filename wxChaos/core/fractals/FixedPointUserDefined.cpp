#include <complex>
#include <mpParser.h>
#include "FPUserDefined.h"
using namespace std;

FPUserDefined::FPUserDefined(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    _minX = -1.8713;
    _maxX = 1.82101;
    _minY = -1.22781;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::FixedPointUserDefined;
    _hasOrbit = true;
    myRender = new RenderFPUserDefined[_threadNumber];
    SetWatchdog<RenderFPUserDefined>(myRender, &_watchdog, _threadNumber);

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Min step: "), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    _alg = RenderingAlgorithm::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FPUserDefined::FPUserDefined(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.8713;
    _maxX = 1.82101;
    _minY = -1.22781;
    _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
    this->SetOutermostZoom();

    minStep = 0.001;
    _type = FractalType::FixedPointUserDefined;
    myRender = new RenderFPUserDefined[_threadNumber];
    SetWatchdog<RenderFPUserDefined>(myRender, &_watchdog, _threadNumber);
}
FPUserDefined::~FPUserDefined()
{
    this->StopRender();
    delete[] myRender;
}
void FPUserDefined::Render()
{
    for(unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetParams(minStep);

    this->TRender<RenderFPUserDefined>(myRender);
}
void FPUserDefined::SetFormula(FormulaOpt formula)
{
    _userFormula = formula;
    for(unsigned int i=0; i<_threadNumber; i++)
        myRender[i].SetFormula(formula);
}
void FPUserDefined::DrawOrbit()
{
    vector<complex<double>> zVector;
    mup::ParserX parser;
    parser.SetExpr(_userFormula.userFormula.wc_str());

    mup::Value zVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    zVal = mup::cmplx_type(_orbitX, _orbitY);

    try
    {
        for(unsigned n=0; n<_maxIter; n++)
        {
            zVector.push_back(complex<double>(zVal.GetFloat(),zVal.GetImag()));
            zVal = parser.Eval();
        }

        sf::Color color = sf::Color(0, 255, 0);
        for(unsigned int i=0; i<zVector.size()-1; i++)
            this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

        _orbitDrawn = true;
    }
    catch(mup::ParserError&)
    {
        return;
    }
}
void FPUserDefined::CopyOptFromPanel()
{
    minStep = *_panelOpt.GetDoubleElement(0);
}
void FPUserDefined::PostRender()
{
    if(myRender[0].IsThereError())
    {
        wxString out = wxString(wxT("Fatal error in formula.\n")) + myRender[0].GetErrorInfo() + wxT("\n");
        myRender[0].ClearErrorInfo();
        wxMessageDialog errorDialog(NULL, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}

