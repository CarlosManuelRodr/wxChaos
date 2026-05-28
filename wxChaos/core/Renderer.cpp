#include "Renderer.h"

// RenderFractal
Renderer::Renderer() : _myOpt()
{
    _setMap = nullptr;
    _colorMap = nullptr;
    _auxMap = nullptr;
    _x = _y = 0;
    _threadProgress = 0;
    _widthOrigin = _heightOrigin = _widthFinal = _heightFinal = _oldHeightOrigin = 0;

    _specialRenderMode = false;
    _stopped = false;
    _threadRunning = false;

    _type = FractalType::Undefined;
    _xFactor = 0.0;
    _yFactor = 0.0;
    _minX = _maxX = _minY = _maxY = _maxIter = 0.0;
    _kReal = _kImaginary = 0.0;
}

void Renderer::SetOptions(const Options& opt)
{
    _myOpt = opt;
    _xFactor = opt.xFactor;
    _yFactor = opt.yFactor;
    _minX = opt.minX;
    _maxX = opt.maxX;
    _minY = opt.minY;
    _maxY = opt.maxY;
    _maxIter = opt.maxIter;
    _type = opt.type;
}
void Renderer::SetLimits(const int widthOrigin, const int heightOrigin, const int widthFinal, const int heightFinal)
{
    _widthOrigin = widthOrigin;
    _oldHeightOrigin = _heightOrigin = heightOrigin;
    _heightFinal = heightFinal;
    _widthFinal = widthFinal;
}
void Renderer::UpdateLimits(const int heightOrigin)
{
    _heightOrigin = heightOrigin;
}
void Renderer::SetOldHeightOrigin(const int oldHeightOrigin)
{
    _oldHeightOrigin = oldHeightOrigin;
}
void Renderer::run()
{
    _y = _heightOrigin;
    _threadRunning = true;
    _stopped = false;

    this->Render();

    _threadRunning = false;
}
void Renderer::Stop()
{
    if (_type != FractalType::ScriptFractal)
    {
        _stopped = true;

        if (_y != 0)
            _heightOrigin = _y;

        _x = _widthFinal - 1;
        _y = _heightFinal - 1;
    }
}
void Renderer::SetSpecialRenderMode(const bool mode)
{
    _specialRenderMode = mode;
}
void Renderer::SetRenderOut(bool** outSetMap, int** outColorMap, unsigned int** outAux)
{
    _setMap = outSetMap;
    _colorMap = outColorMap;
    _auxMap = outAux;
}
void Renderer::SetK(const double re, const double im)
{
    _kReal = re;
    _kImaginary = im;
}
void Renderer::Reset()
{
    _x = 0;
    _y = 0;
}
void Renderer::PreTerminate()
{
    // Do nothing.
}
unsigned int Renderer::GetProgress()
{
    if (!_stopped)
        _threadProgress = static_cast<int>(floor(100.0 * (static_cast<double>(_y + 1 - _oldHeightOrigin) / static_cast<double>(_heightFinal - _oldHeightOrigin))));

    return _threadProgress;
}
Vector2Int Renderer::GetCoords() const
{
    const Vector2Int pos{0, _heightOrigin};
    return pos;
}
Vector2Int Renderer::GetStartPoints() const
{
    const Vector2Int pos{_widthOrigin, _heightOrigin};
    return pos;
}
Vector2Int Renderer::GetEndPoints() const
{
    const Vector2Int pos{_widthFinal, _heightFinal};
    return pos;
}
bool Renderer::IsRunning() const
{
    return _threadRunning;
}
Options Renderer::GetOptions()
{
    return _myOpt;
}
