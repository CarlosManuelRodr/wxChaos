#pragma once

#include "rendering/RenderRegion.h"

/**
* @class RenderJob
* @brief Describes one renderable unit of work.
*
* RenderJob currently wraps a RenderRegion. It also stores the progress origin
* used by the legacy renderer-array progress calculation.
*/
class RenderJob
{
    RenderRegion _region;
    int _progressOriginY;

public:
    RenderJob();
    explicit RenderJob(const RenderRegion& region);
    RenderJob(const RenderRegion& region, int progressOriginY);

    [[nodiscard]] const RenderRegion& GetRegion() const;
    [[nodiscard]] int GetProgressOriginY() const;
    [[nodiscard]] bool IsEmpty() const;
};
