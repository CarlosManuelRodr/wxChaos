#include <complex>
#include <mpParser.h>
#include "UserDefinedFixedPoint.h"
using namespace std;

UserDefinedFixedPoint::UserDefinedFixedPoint(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    // Adjust the scale.
    _minX = -1.8713;
    _maxX = 1.82101;
    _minY = -1.22781;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _type = FractalType::UserDefinedFixedPoint;
    _hasOrbit = true;
    _myRender = new UserDefinedFixedPointRenderer[_threadNumber];

    // Creates panel.
    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, "Min step: ", &_minStep, "0.001");
    _minStep = 0.001;

    // Specify algorithms.
    _algorithm = RenderingAlgorithmType::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithmType::ConvergenceTest);
}
UserDefinedFixedPoint::~UserDefinedFixedPoint()
{
    this->StopRender();
    delete[] _myRender;
}
void UserDefinedFixedPoint::Render()
{
    for (unsigned int i=0; i<_threadNumber; i++)
        _myRender[i].SetParams(_minStep);

    this->SetRendererBounds<UserDefinedFixedPointRenderer>(_myRender);
}
void UserDefinedFixedPoint::SetFormula(const FormulaOptions formula)
{
    _userFormula = formula;
    for (unsigned int i=0; i<_threadNumber; i++)
        _myRender[i].SetFormula(formula);
}
void UserDefinedFixedPoint::DrawOrbit()
{
    mup::ParserX parser;
    parser.SetExpr(_userFormula.userFormula.utf8_string());

    mup::Value zVal;
    parser.DefineVar("z", mup::Variable(&zVal));
    parser.DefineVar("Z", mup::Variable(&zVal));
    zVal = mup::cmplx_type(_orbitX, _orbitY);

    try
    {
        vector<complex<double>> zVector;
        for (unsigned n=0; n<_maxIter; n++)
        {
            zVector.emplace_back(zVal.GetFloat(),zVal.GetImag());
            zVal = parser.Eval();
        }

        const auto color = sf::Color(0, 255, 0);
        for (unsigned int i=0; i<zVector.size()-1; i++)
            this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

        _orbitDrawn = true;
    }
    catch (mup::ParserError&) {}
}
void UserDefinedFixedPoint::CopyOptionFromPanel()
{
    _minStep = *_panelOpt.GetDoubleElement(0);
}
void UserDefinedFixedPoint::PostRender()
{
    if (_myRender[0].IsThereError())
    {
        const wxString out = wxString("Fatal error in formula.\n") + _myRender[0].GetErrorInfo() + "\n";
        _myRender[0].ClearErrorInfo();
        wxMessageDialog errorDialog(nullptr, out, "Error", wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}
