#pragma once
#include "../../raster/RasterFractal.h"
#include "../../renderers/HenonMapRenderer.h"

/**
 * @class HenonMap
 * @brief Chaotic map plotting x_{n+1} = y_n + 1 - alpha*x_n^2, y_{n+1} = beta*x_n.
 */
class HenonMap : public RasterFractal
{
    double _alpha;
    double _beta;
    double _x0;
    double _y0;
    HenonMapRenderer* _myRender;

public:
    HenonMap(unsigned int width, unsigned int height);
    ~HenonMap() override;
    wxString GetName() const override { return "Henon Map"; }

    void Render() override;
    void CopyOptionFromPanel() override;
};
