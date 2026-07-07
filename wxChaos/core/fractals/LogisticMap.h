#pragma once
#include "../Fractal.h"
#include "../renderers/LogisticMapRenderer.h"

/**
* @class LogisticMap
* @brief Renders the logistic map bifurcation diagram and Lyapunov curve.
*/
class LogisticMap : public Fractal
{
    LogisticMapRenderer* _myRender;
    double _logisticSeed;
    bool _stabilizePoint;

public:
    LogisticMap(unsigned int width, unsigned int height);
    ~LogisticMap() override;

    wxString GetName() const override { return "Logistic Map"; }
    void Render() override;
    void CopyOptionFromPanel() override;
};
