#pragma once
#include "../RenderWorker.h"

/**
* @class HenonMapRenderer
* @brief Plots the orbit of the Henon map into the current viewport.
*/
class HenonMapRenderer : public RenderWorker
{
    double _alpha;
    double _beta;
    double _x0;
    double _y0;
    unsigned int _currentIteration;

public:
    HenonMapRenderer();
    void Render() override;
    void SetParams(double alpha, double beta, double x0, double y0);
    unsigned int GetProgress() override;
};
