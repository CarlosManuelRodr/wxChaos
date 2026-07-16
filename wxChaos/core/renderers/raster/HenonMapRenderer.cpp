#include "HenonMapRenderer.h"
#include <algorithm>

HenonMapRenderer::HenonMapRenderer()
{
    _alpha = 0.0;
    _beta = 0.0;
    _x0 = 0.0;
    _y0 = 0.0;
    _currentIteration = 0;
}

void HenonMapRenderer::Render()
{
    double x = _x0;
    double y = _y0;
    _currentIteration = 0;

    if (_myOpt.alg != RenderingAlgorithmType::ChaoticMap)
        return;

    const auto maxIterations = static_cast<unsigned int>(_maxIterations);
    for (_currentIteration = 0; _currentIteration < maxIterations && !_stopped; _currentIteration++)
    {
        const auto coordX = static_cast<int>((x - _minX) / _xFactor);
        const auto coordY = static_cast<int>((_maxY - y) / _yFactor);
        if (coordX >= 0 && coordX < _myOpt.screenWidth && coordY >= 0 && coordY < _myOpt.screenHeight)
            _setMap[coordX][coordY] = true;

        const double previousX = x;
        x = y + 1.0 - _alpha * x * x;
        y = _beta * previousX;
    }
}

void HenonMapRenderer::SetParams(const double alpha, const double beta, const double x0, const double y0)
{
    _alpha = alpha;
    _beta = beta;
    _x0 = x0;
    _y0 = y0;
}

unsigned int HenonMapRenderer::GetProgress() const
{
    if (!_stopped)
        _threadProgress = std::min(100U, static_cast<unsigned int>(100.0 * _currentIteration / std::max(1.0, _maxIterations)));

    return _threadProgress;
}
