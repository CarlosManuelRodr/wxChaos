#pragma once
#include "../raster/RasterFractal.h"
#include "../renderers/JuliaZMRenderer.h"

/**
 * @class JuliaZM
 * @brief Power Julia set with configurable exponent and bailout.
 *
 * Each pixel supplies z_0 and the UI Julia constant supplies k. The orbit
 * iterates z_{n+1} = z_n^m + k, where m is the panel exponent. Escape is
 * detected when |z|^2 exceeds bailout^2.
 */
class JuliaZM: public RasterFractal
{
    JuliaZMRenderer* _myRender;
    double _m;
    double _bailout;
public:
    JuliaZM(unsigned int width, unsigned int height);
    ~JuliaZM() override;
    wxString GetName() const override { return "Julia Z^m"; }

    void CopyOptionFromPanel() override;
    void Render() override;
    void DrawOrbit() override;
};
