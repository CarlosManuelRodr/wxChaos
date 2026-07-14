#pragma once
#include "../RasterRenderWorker.h"

/**
* @class LogisticMapRenderer
* @brief Plots the logistic map bifurcation diagram into the current viewport.
*/
class LogisticMapRenderer : public RasterRenderWorker
{
    double _seed;
    bool _stabilizePoint;
    unsigned int _currentStep;
    unsigned int _totalSteps;

public:
    LogisticMapRenderer();
    void Render() override;
    void SetParams(double seed, bool stabilizePoint);
    [[nodiscard]] unsigned int GetProgress() const override;
};
