#include "RasterFractal.h"

RasterFractal::RasterFractal(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    AllocateRenderMaps();
    ClearRenderMaps(InvalidColor);
}

RasterFractal::~RasterFractal()
{
    ReleaseRenderMaps();
}
