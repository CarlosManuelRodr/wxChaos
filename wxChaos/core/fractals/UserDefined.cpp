#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
UserDefined::UserDefined(sf::RenderWindow* Window) : Fractal(Window)
{
    // Adjust the scale.
    minX = -2.5;
    maxX = 2.5;
    minY = -1.5;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    type = FractalType::UserDefined;
    hasOrbit = true;
    myRender = new RenderUserDefined[threadNumber];
    SetWatchdog<RenderUserDefined>(myRender, &watchdog, threadNumber);

    // Specify algorithms.
    alg = RenderingAlgorithm::EscapeTime;
    availableAlg.push_back(RenderingAlgorithm::EscapeTime);
}
UserDefined::UserDefined(int width, int height) : Fractal(width, height)
{
    // Adjust the scale.
    minX = -2.5;
    maxX = 2.5;
    minY = -1.5;
    maxY = minY + (maxX - minX) * screenHeight / screenWidth;
    this->SetOutermostZoom();

    type = FractalType::UserDefined;
    myRender = new RenderUserDefined[threadNumber];
    SetWatchdog<RenderUserDefined>(myRender, &watchdog, threadNumber);
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
    userFormula = formula;
    for(unsigned int i = 0; i < threadNumber; i++)
        myRender[i].SetFormula(formula);

    if(formula.julia)
        juliaVariety = true;
}
void UserDefined::DrawOrbit()
{
    bool julia = userFormula.julia;
    vector< complex<double> > zVector;
    mup::ParserX parser;
    parser.SetExpr(userFormula.userFormula.wc_str());

    int bailout = userFormula.bailout;
    mup::Value zVal;
    mup::Value cVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("c"),  mup::Variable(&cVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    parser.DefineVar(_T("C"),  mup::Variable(&cVal));
    if(julia) cVal = mup::cmplx_type(kReal, kImaginary);
    bool outOfSet = false;

    zVal = mup::cmplx_type(orbitX, orbitY);
    if(!julia) cVal = mup::cmplx_type(orbitX, orbitY);

    try
    {
        for(unsigned n=0; n<maxIter; n++)
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

        orbitDrawn = true;
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

