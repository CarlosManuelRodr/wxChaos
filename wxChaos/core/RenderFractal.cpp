#include "RenderFractal.h"

// RenderFractal
RenderFractal::RenderFractal() 
{
    setMap = nullptr;
    colorMap = nullptr;
    auxMap = nullptr;
    x = y = 0;
    threadProgress = 0;
    wo = ho = wf = hf = oldHo = 0;

    specialRenderMode = false;
    stopped = false;
    threadRunning = false;

    type = FractalType::Undefined;
    xFactor = 0.0;
    yFactor = 0.0;
    minX = maxX = minY = maxY = maxIter = 0.0;
    kReal = kImaginary = 0.0;
}
void RenderFractal::SetOpt(Options opt)
{
    myOpt = opt;
    xFactor = opt.xFactor;
    yFactor = opt.yFactor;
    minX = opt.minX;
    maxX = opt.maxX;
    minY = opt.minY;
    maxY = opt.maxY;
    maxIter = opt.maxIter;
    type = opt.type;
}
void RenderFractal::SetLimits(int widthO, int heightO, int widthF, int heightF)
{
    wo = widthO;
    oldHo = ho = heightO;
    hf = heightF;
    wf = widthF;
}
void RenderFractal::UpdateLimits(int heightO)
{
    ho = heightO;
}
void RenderFractal::SetOldHo(int _oldHo)
{
    oldHo = _oldHo;
}
void RenderFractal::run()
{
    y = ho;
    threadRunning = true;
    stopped = false;

    if (specialRenderMode)
        this->SpecialRender();
    else
        this->Render();

    threadRunning = false;
}
void RenderFractal::Stop()
{
    if (type != FractalType::ScriptFractal)
    {
        stopped = true;

        if (y != 0)
            ho = y;

        x = wf - 1;
        y = hf - 1;
    }
}
void RenderFractal::SetSpecialRenderMode(bool mode)
{
    specialRenderMode = mode;
}
void RenderFractal::SetRenderOut(bool** outSetMap, int** outColorMap, unsigned int** outAux)
{
    setMap = outSetMap;
    colorMap = outColorMap;
    auxMap = outAux;
}
void RenderFractal::SetK(double re, double im)
{
    kReal = re;
    kImaginary = im;
}
void RenderFractal::Reset()
{
    x = 0;
    y = 0;
}
void RenderFractal::PreTerminate()
{
    // Do nothing.
}
int RenderFractal::AskProgress()
{
    if (!stopped)
        threadProgress = static_cast<int>(floor(100.0 * ((double)(y + 1 - oldHo) / (double)(hf - oldHo))));

    return threadProgress;
}
Vector2Int RenderFractal::GetCoords()
{
    Vector2Int pos;
    pos.x = 0;
    pos.y = ho;
    return pos;
}
Vector2Int RenderFractal::GetStartPoints()
{
    Vector2Int pos;
    pos.x = wo;
    pos.y = ho;
    return pos;
}
Vector2Int RenderFractal::GetEndPoints()
{
    Vector2Int pos;
    pos.x = wf;
    pos.y = hf;
    return pos;
}
bool RenderFractal::IsRunning()
{
    return threadRunning;
}
Options RenderFractal::GetOpt()
{
    return myOpt;
}
