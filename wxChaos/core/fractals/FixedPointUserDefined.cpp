#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
FPUserDefined::FPUserDefined(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -1.8713;
    maxX = 1.82101;
    minY = -1.22781;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::FixedPointUserDefined;
    hasOrbit = true;
    myRender = new RenderFPUserDefined[threadNumber];
    SetWatchdog<RenderFPUserDefined>(myRender, &watchdog, threadNumber);

    // Creates panel.
    panelOpt.SetForceShow(true);
    panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Min step: "), &minStep, wxT("0.001"));
    minStep = 0.001;

    // Specify algorithms.
    alg = RenderingAlgorithm::ConvergenceTest;
    availableAlg.push_back(RenderingAlgorithm::ConvergenceTest);
}
FPUserDefined::FPUserDefined(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -1.8713;
    maxX = 1.82101;
    minY = -1.22781;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    minStep = 0.001;
    type = FractalType::FixedPointUserDefined;
    myRender = new RenderFPUserDefined[threadNumber];
    SetWatchdog<RenderFPUserDefined>(myRender, &watchdog, threadNumber);
}
FPUserDefined::~FPUserDefined()
{
    this->StopRender();
    delete[] myRender;
}
void FPUserDefined::Render()
{
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetParams(minStep);

    this->TRender<RenderFPUserDefined>(myRender);
}
void FPUserDefined::SetFormula(FormulaOpt formula)
{
    userFormula = formula;
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetFormula(formula);
}
void FPUserDefined::DrawOrbit()
{
    vector<complex<double>> zVector;
    mup::ParserX parser;
    parser.SetExpr(userFormula.userFormula.wc_str());

    mup::Value zVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    zVal = mup::cmplx_type(orbitX, orbitY);

    try
    {
        for(unsigned n=0; n<maxIter; n++)
        {
            zVector.push_back(complex<double>(zVal.GetFloat(),zVal.GetImag()));
            zVal = parser.Eval();
        }

        sf::Color color = sf::Color(0, 255, 0);
        for(unsigned int i=0; i<zVector.size()-1; i++)
            this->DrawLine(zVector[i].real(), zVector[i].imag(), zVector[i+1].real(), zVector[i+1].imag(), color, true);

        orbitDrawn = true;
    }
    catch(mup::ParserError&)
    {
        return;
    }
}
void FPUserDefined::CopyOptFromPanel()
{
    minStep = *panelOpt.GetDoubleElement(0);
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

