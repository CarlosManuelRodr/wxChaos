#pragma once

#include "raster/RasterRegion.h"

/**
* @class RasterJob
* @brief Describes one renderable unit of work.
*
* RasterJob currently wraps a RenderRegion. It also stores the progress origin
* used by the legacy renderer-array progress calculation.
*/
class RasterJob
{
    RasterRegion _region;
    int _progressOriginY;

public:
    RasterJob();
    explicit RasterJob(const RasterRegion& region);
    RasterJob(const RasterRegion& region, int progressOriginY);

    [[nodiscard]] const RasterRegion& GetRegion() const;
    [[nodiscard]] int GetProgressOriginY() const;
    [[nodiscard]] bool IsEmpty() const;
};
