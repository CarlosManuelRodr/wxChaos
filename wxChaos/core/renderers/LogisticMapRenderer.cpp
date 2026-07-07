#include "LogisticMapRenderer.h"
#include <algorithm>
#include <cmath>

LogisticMapRenderer::LogisticMapRenderer()
{
    _seed = 0.0;
    _stabilizePoint = false;
    _currentStep = 0;
    _totalSteps = 1;
}

void LogisticMapRenderer::Render()
{
    _currentStep = 0;

    const auto maxIterations = static_cast<unsigned int>(_maxIter);
    _totalSteps = std::max(1U, maxIterations * static_cast<unsigned int>(_myOpt.screenWidth));
    if (_myOpt.alg == RenderingAlgorithmType::ChaoticMap && _stabilizePoint)
        _totalSteps *= 2;

    if (_myOpt.alg == RenderingAlgorithmType::ChaoticMap)
    {
        for (int i = 0; i < _myOpt.screenWidth && !_stopped; i++)
        {
            const double a = _minX + i * _xFactor;
            double x = _seed;

            if (_stabilizePoint)
            {
                for (unsigned int n = 0; n < maxIterations && !_stopped; n++)
                {
                    x = a * x * (1.0 - x);
                    _currentStep++;
                }
            }

            for (unsigned int n = 0; n < maxIterations && !_stopped; n++)
            {
                x = a * x * (1.0 - x);
                const auto coordX = static_cast<int>((a - _minX) / _xFactor);
                const auto coordY = static_cast<int>((_maxY - x) / _yFactor);
                if (coordX >= 0 && coordX < _myOpt.screenWidth && coordY >= 0 && coordY < _myOpt.screenHeight)
                    _setMap[coordX][coordY] = true;
                _currentStep++;
            }
        }
        return;
    }

    if (_myOpt.alg == RenderingAlgorithmType::Lyapunov)
    {
        for (int i = 0; i < _myOpt.screenWidth && !_stopped; i++)
        {
            const double a = _minX + i * _xFactor;
            double x = _seed;
            double derivativeLogSum = 0.0;

            for (unsigned int n = 0; n < maxIterations && !_stopped; n++)
            {
                x = a * x * (1.0 - x);
                derivativeLogSum += std::log(std::abs(a * (1.0 - 2.0 * x)));
                _currentStep++;
            }

            const auto coordX = static_cast<int>((a - _minX) / _xFactor);
            const auto coordY = static_cast<int>((_maxY - derivativeLogSum / maxIterations) / _yFactor);
            if (coordX >= 0 && coordX < _myOpt.screenWidth && coordY >= 0 && coordY < _myOpt.screenHeight)
                _setMap[coordX][coordY] = true;
        }
    }
}

void LogisticMapRenderer::SetParams(const double seed, const bool stabilizePoint)
{
    _seed = seed;
    _stabilizePoint = stabilizePoint;
}

unsigned int LogisticMapRenderer::GetProgress()
{
    if (!_stopped)
        _threadProgress = std::min(100U, static_cast<unsigned int>(100.0 * _currentStep / _totalSteps));

    return _threadProgress;
}
