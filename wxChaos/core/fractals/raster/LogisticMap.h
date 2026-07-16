#pragma once
#include "../../raster/RasterFractal.h"
#include "../../renderers/raster/LogisticMapRenderer.h"

/**
* @class LogisticMap
* @brief Renders the logistic map bifurcation diagram.
*/
class LogisticMap : public RasterFractal
{
    LogisticMapRenderer* _myRender;
    double _logisticSeed;
    bool _stabilizePoint;

public:
    LogisticMap(unsigned int width, unsigned int height);
    ~LogisticMap() override;

    wxString GetName() const override { return "Logistic Map"; }
    wxString InspectPoint(double x, double y, std::optional<unsigned int> iterations = std::nullopt) const override;
    void Render() override;
    void CopyOptionFromPanel() override;
};
