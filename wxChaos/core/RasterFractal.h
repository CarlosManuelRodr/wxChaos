#pragma once

#include "Fractal.h"

/**
 * @class RasterFractal
 * @brief Base for fractals rendered into rectangular set and color maps.
 */
class RasterFractal : public Fractal
{
protected:
    bool** _setMap{};                ///< Points that belong to the fractal set.
    double** _colorMap{};            ///< Continuous values used by coloring algorithms.
    RenderThreadPool _renderPool;    ///< Reusable pool for raster render jobs.
    std::vector<LineData> _orbitLines;

    bool**& SetMapStorage() override { return _setMap; }
    bool** SetMapStorage() const override { return _setMap; }
    double**& ColorMapStorage() override { return _colorMap; }
    double** ColorMapStorage() const override { return _colorMap; }
    RenderThreadPool& RenderPoolStorage() override { return _renderPool; }
    const RenderThreadPool& RenderPoolStorage() const override { return _renderPool; }
    std::vector<LineData>& OrbitLinesStorage() override { return _orbitLines; }
    const std::vector<LineData>& OrbitLinesStorage() const override { return _orbitLines; }

public:
    RasterFractal(unsigned int width, unsigned int height);
    ~RasterFractal() override;
};
