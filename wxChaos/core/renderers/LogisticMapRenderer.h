#pragma once
#include "../RenderWorker.h"

/**
* @class LogisticMapRenderer
* @brief Plots the logistic map bifurcation diagram into the current viewport.
*/
class LogisticMapRenderer : public RenderWorker
{
    double _seed;
    bool _stabilizePoint;
    unsigned int _currentStep;
    unsigned int _totalSteps;

public:
    LogisticMapRenderer();
    void Render() override;
    void SetParams(double seed, bool stabilizePoint);
    unsigned int GetProgress() override;
};
