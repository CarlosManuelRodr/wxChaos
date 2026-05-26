#include "rendering/RenderJob.h"

RenderJob::RenderJob()
{
    _region = RenderRegion();
    _progressOriginY = 0;
}

RenderJob::RenderJob(const RenderRegion& region)
{
    _region = region;
    _progressOriginY = region.GetTop();
}

RenderJob::RenderJob(const RenderRegion& region, const int progressOriginY)
{
    _region = region;
    _progressOriginY = progressOriginY;
}

const RenderRegion& RenderJob::GetRegion() const
{
    return _region;
}

int RenderJob::GetProgressOriginY() const
{
    return _progressOriginY;
}

bool RenderJob::IsEmpty() const
{
    return _region.IsEmpty();
}
