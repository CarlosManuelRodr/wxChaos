#include <mpParser.h>
#include "UserDefined.h"
using namespace std;

UserDefined::UserDefined(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    _minX = -2.5;
    _maxX = 2.5;
    _minY = -1.5;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::UserDefined;
    _hasOrbit = true;
    myRender = new RenderUserDefined[_threadNumber];
    SetWatchdog<RenderUserDefined>(myRender, &_watchdog, _threadNumber);

    // Specify algorithms.
    _algorithm = RenderingAlgorithm::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithm::EscapeTime);
}
UserDefined::UserDefined(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -2.5;
    _maxX = 2.5;
    _minY = -1.5;
    _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
    this->SetOutermostZoom();

    _type = FractalType::UserDefined;
    myRender = new RenderUserDefined[_threadNumber];
    SetWatchdog<RenderUserDefined>(myRender, &_watchdog, _threadNumber);
}
UserDefined::~UserDefined()
{
    this->StopRender();
    delete[] myRender;
}
void UserDefined::Render()
{
    this->TRender<RenderUserDefined>(myRender);
}
void UserDefined::SetFormula(FormulaOpt formula)
{
    _userFormula = formula;
    for(unsigned int i = 0; i < _threadNumber; i++)
        myRender[i].SetFormula(formula);

    if(formula.julia)
        _juliaVariety = true;
}
void UserDefined::DrawOrbit()
{
    bool julia = _userFormula.julia;
    vector< complex<double> > zVector;
    mup::ParserX parser;
    parser.SetExpr(_userFormula.userFormula.wc_str());

    int bailout = _userFormula.bailout;
    mup::Value zVal;
    mup::Value cVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("c"),  mup::Variable(&cVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    parser.DefineVar(_T("C"),  mup::Variable(&cVal));
    if(julia) cVal = mup::cmplx_type(_kReal, _kImaginary);
    bool outOfSet = false;

    zVal = mup::cmplx_type(_orbitX, _orbitY);
    if(!julia) cVal = mup::cmplx_type(_orbitX, _orbitY);

    try
    {
        for(unsigned n=0; n<_maxIter; n++)
        {
            zVector.push_back(complex<double>(zVal.GetFloat(),zVal.GetImag()));
            if(zVal.GetFloat()*zVal.GetFloat() + zVal.GetImag()*zVal.GetImag() > bailout*bailout)
            {
                outOfSet = true;
                break;
            }
            zVal = parser.Eval();
        }
        sf::Color color;
        if(outOfSet) color = sf::Color(255, 0, 0);
        else color = sf::Color(0, 255, 0);

        for(unsigned int i=0; i<zVector.size()-1; i++)
            this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

        _orbitDrawn = true;
    }
    catch (mup::ParserError&)
    {
        return;
    }
}
void UserDefined::PostRender()
{
    if(myRender[0].IsThereError())
    {
        wxString out = wxString(wxT("Fatal error in formula.\n")) + myRender[0].GetErrorInfo() + wxT("\n");
        myRender[0].ClearErrorInfo();
        wxMessageDialog errorDialog(NULL, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}

