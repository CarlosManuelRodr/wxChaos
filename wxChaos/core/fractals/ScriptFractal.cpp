#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
ScriptFractal::ScriptFractal(sf::RenderWindow* Window, ScriptData scriptData) : Fractal(Window)
{
    // Adjust the scale.
    minX = scriptData.minX;
    maxX = scriptData.maxX;
    minY = scriptData.minY;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    maxIter = scriptData.defaultIter;
    juliaVariety = scriptData.juliaVariety;
    redrawAlways = scriptData.redrawAlways;
    this->SetExtColorMode(scriptData.extColor);

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    path = scriptData.file;
    myScriptData = scriptData;
    type = FractalType::ScriptFractal;
    myRender = new RenderScriptFractal[threadNumber];
    for(unsigned int i=0; i<threadNumber; i++)
    {
        myRender[i].SetParams(i);
        myRender[i].SetPath(scriptData.file);
    }
    SetWatchdog<RenderScriptFractal>(myRender, &watchdog, threadNumber);
}
ScriptFractal::ScriptFractal(int width, int height, ScriptData scriptData, int renderThreads) 
    : Fractal(width, height)
{
    if (renderThreads != -1)
        threadNumber = (unsigned) renderThreads;

    // Adjust the scale.
    minX = scriptData.minX;
    maxX = scriptData.maxX;
    minY = scriptData.minY;
    maxY = minY+(maxX-minX)*screenHeight/screenWidth;
    this->SetOutermostZoom();

    maxIter = scriptData.defaultIter;
    juliaVariety = scriptData.juliaVariety;
    redrawAlways = scriptData.redrawAlways;
    this->SetExtColorMode(scriptData.extColor);

    xFactor = (maxX-minX)/(screenWidth-1);
    yFactor = (maxY-minY)/(screenHeight-1);

    path = scriptData.file;
    myScriptData = scriptData;
    type = FractalType::ScriptFractal;
    myRender = new RenderScriptFractal[threadNumber];
    for(unsigned int i=0; i<threadNumber; i++)
    {
        myRender[i].SetParams(i);
        myRender[i].SetPath(scriptData.file);
    }
    SetWatchdog<RenderScriptFractal>(myRender, &watchdog, threadNumber);
}
ScriptFractal::ScriptFractal(int width, int height, string scriptPath) : Fractal(width, height)
{
    path = scriptPath;
    AngelscriptConfigurationEngine configEngine;
    if (configEngine.CompileFromPath(path) && configEngine.Execute())
    {
        ScriptData params = configEngine.GetScriptData();
        minX = params.minX;
        maxX = params.maxX;
        minY = params.minY;
        maxY = minY + (maxX - minX) * screenHeight / screenWidth;
        this->SetOutermostZoom();
    }

    type = FractalType::ScriptFractal;
    myRender = new RenderScriptFractal[threadNumber];
    for(unsigned int i=0; i<threadNumber; i++)
        myRender[i].SetPath(scriptPath);
    SetWatchdog<RenderScriptFractal>(myRender, &watchdog, threadNumber);
}
ScriptFractal::~ScriptFractal()
{
    this->StopRender();
    delete[] myRender;
}
void ScriptFractal::Render()
{
    asSetMap = setMap;
    asColorMap = colorMap;
    asPrepareMultithread();
    this->TRender<RenderScriptFractal>(myRender);
    asUnprepareMultithread();
}
void ScriptFractal::PostRender()
{
    wxString errorLog = wxT("");
    for(unsigned int i=0; i<threadNumber; i++)
    {
        if(myRender[i].IsThereError())
        {
            errorLog += wxT("Thread ");
            errorLog += num_to_string((int)i);
            errorLog += wxT(" says:\n");
            errorLog += myRender[i].GetErrorInfo();
            errorLog += wxT("\n");
            myRender[i].ClearErrorInfo();
        }
    }
    if(errorLog.size() != 0)
    {
        wxString out = wxString(wxT("Fatal error in script.\n")) + errorLog;
        wxMessageDialog errorDialog(NULL, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}
void ScriptFractal::PreRestartRender()
{
    // Clears all the maps.
    for (int i = 0; i < screenWidth; i++)
    {
        for (int j = 0; j < screenHeight; j++)
        {
            setMap[i][j] = false;
            colorMap[i][j] = 0;
            auxMap[i][j] = 0;
        }
    }
}
bool ScriptFractal::IsThereError()
{
    return myRender[0].IsThereError();
}
wxString ScriptFractal::GetErrorInfo()
{
    return myRender[0].GetErrorInfo();
}
void ScriptFractal::ClearErrorInfo()
{
    return myRender[0].ClearErrorInfo();
}
string ScriptFractal::GetPath()
{
    return path;
}

