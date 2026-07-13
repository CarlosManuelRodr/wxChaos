#include <mpParser.h>
#include "UserDefinedEscapeTime.h"
using namespace std;

UserDefinedEscapeTime::UserDefinedEscapeTime(const unsigned int width, const unsigned int height) : RasterFractal(width, height)
{
    // Adjust the scale.
    _minX = -2.5;
    _maxX = 2.5;
    _minY = -1.5;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::UserDefinedEscapeTime;
    _hasOrbit = true;
    _myRender = new UserDefinedEscapeTimeRenderer[_threadNumber];

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::EscapeTime;
    _availableAlg.push_back(RenderingAlgorithmType::EscapeTime);
}
UserDefinedEscapeTime::~UserDefinedEscapeTime()
{
    this->StopRender();
    delete[] _myRender;
}
void UserDefinedEscapeTime::Render()
{
    this->SetRendererBounds<UserDefinedEscapeTimeRenderer>(_myRender);
}
void UserDefinedEscapeTime::SetFormula(FormulaOptions formula)
{
    _userFormula = formula;
    for (unsigned int i = 0; i < _threadNumber; i++)
        _myRender[i].SetFormula(formula);

    if (formula.julia)
        _juliaVariety = true;
}
void UserDefinedEscapeTime::DrawOrbit()
{
    bool julia = _userFormula.julia;
    mup::ParserX parser;
    parser.SetExpr(_userFormula.userFormula.utf8_string());

    int bailout = _userFormula.bailout;
    mup::Value zVal;
    mup::Value cVal;
    parser.DefineVar("z", mup::Variable(&zVal));
    parser.DefineVar("c",  mup::Variable(&cVal));
    parser.DefineVar("Z", mup::Variable(&zVal));
    parser.DefineVar("C",  mup::Variable(&cVal));
    if (julia) cVal = mup::cmplx_type(_kReal, _kImaginary);

    zVal = mup::cmplx_type(_orbitX, _orbitY);
    if (!julia) cVal = mup::cmplx_type(_orbitX, _orbitY);

    try
    {
        bool outOfSet = false;
        vector< complex<double> > zVector;
        for (unsigned n=0; n<_maxIterations; n++)
        {
            zVector.emplace_back(zVal.GetFloat(),zVal.GetImag());
            if (zVal.GetFloat()*zVal.GetFloat() + zVal.GetImag()*zVal.GetImag() > bailout*bailout)
            {
                outOfSet = true;
                break;
            }
            zVal = parser.Eval();
        }

        const auto color = outOfSet ? sf::Color(255, 0, 0) : sf::Color(0, 255, 0);
        for (unsigned int i=0; i<zVector.size()-1; i++)
            this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

        _orbitDrawn = true;
    }
    catch (mup::ParserError&) {}
}
void UserDefinedEscapeTime::PostRender()
{
    if (_myRender[0].IsThereError())
    {
        const wxString out = wxString("Fatal error in formula.\n") + _myRender[0].GetErrorInfo() + "\n";
        _myRender[0].ClearErrorInfo();
        wxMessageDialog errorDialog(nullptr, out, "Error", wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}
