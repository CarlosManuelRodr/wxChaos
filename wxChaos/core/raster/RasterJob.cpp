#include "raster/RasterJob.h"

RasterJob::RasterJob()
{
    _region = RasterRegion();
    _progressOriginY = 0;
}

RasterJob::RasterJob(const RasterRegion& region)
{
    _region = region;
    _progressOriginY = region.GetTop();
}

RasterJob::RasterJob(const RasterRegion& region, const int progressOriginY)
{
    _region = region;
    _progressOriginY = progressOriginY;
}

const RasterRegion& RasterJob::GetRegion() const
{
    return _region;
}

int RasterJob::GetProgressOriginY() const
{
    return _progressOriginY;
}

bool RasterJob::IsEmpty() const
{
    return _region.IsEmpty();
}
