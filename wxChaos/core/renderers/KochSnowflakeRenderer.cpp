#include "renderers/KochSnowflakeRenderer.h"
#include <algorithm>
#include <cmath>
#include <limits>

void KochSnowflakeRenderer::Configure(const unsigned int iterations, const Rect& view,
                                      const unsigned int screenWidth, const unsigned int screenHeight,
                                      const sf::Color& color)
{
    Stop();
    _iterations = iterations;
    _view = view;
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    _color = color;
}

void KochSnowflakeRenderer::RenderGeometry(Context& context)
{
    constexpr double lowerY = -0.57735026918962576451;
    constexpr double upperY = 1.1547005383792515290;
    constexpr double sideWork = 1.0 / 3.0;
    double completedWork = 0.0;

    if (!AppendKochSegment(context, -1.0, lowerY, 0.0, upperY, _iterations, sideWork, completedWork))
        return;
    if (!AppendKochSegment(context, 0.0, upperY, 1.0, lowerY, _iterations, sideWork, completedWork))
        return;
    AppendKochSegment(context, 1.0, lowerY, -1.0, lowerY, _iterations, sideWork, completedWork);
}

bool KochSnowflakeRenderer::AppendKochSegment(Context& context, const double x1, const double y1, const double x2,
                                              const double y2, const unsigned int iterations,
                                              const double workWeight, double& completedWork)
{
    if (!context.Continue())
        return false;

    if (!IsCurveVisible(x1, y1, x2, y2))
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    if (iterations == 0 || IsSubpixelSegment(x1, y1, x2, y2))
    {
        if (IsSegmentVisible(x1, y1, x2, y2))
            context.AddLine(x1, y1, x2, y2, _color);
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    constexpr double cosine60 = 0.5;
    constexpr double sine60 = 0.86602540378443864676;
    const double deltaX = (x2 - x1) / 3.0;
    const double deltaY = (y2 - y1) / 3.0;
    const double firstX = x1 + deltaX;
    const double firstY = y1 + deltaY;
    const double secondX = x1 + 2.0 * deltaX;
    const double secondY = y1 + 2.0 * deltaY;
    const double peakX = firstX + deltaX * cosine60 - deltaY * sine60;
    const double peakY = firstY + deltaX * sine60 + deltaY * cosine60;
    const unsigned int remainingIterations = iterations - 1;
    const double childWork = workWeight / 4.0;

    return AppendKochSegment(context, x1, y1, firstX, firstY, remainingIterations, childWork, completedWork)
        && AppendKochSegment(context, firstX, firstY, peakX, peakY, remainingIterations, childWork, completedWork)
        && AppendKochSegment(context, peakX, peakY, secondX, secondY, remainingIterations, childWork, completedWork)
        && AppendKochSegment(context, secondX, secondY, x2, y2, remainingIterations, childWork, completedWork);
}

bool KochSnowflakeRenderer::IsCurveVisible(const double x1, const double y1, const double x2, const double y2) const
{
    // Every descendant stays within one parent-segment length of these bounds.
    // This conservative envelope avoids clipping geometry while pruning distant branches.
    const double padding = std::hypot(x2 - x1, y2 - y1);
    return std::max(x1, x2) + padding >= _view._left && std::min(x1, x2) - padding <= _view._right
        && std::max(y1, y2) + padding >= _view._bottom && std::min(y1, y2) - padding <= _view._top;
}

bool KochSnowflakeRenderer::IsSegmentVisible(const double x1, const double y1, const double x2, const double y2) const
{
    const double viewWidth = std::max(_view._right - _view._left, std::numeric_limits<double>::epsilon());
    const double viewHeight = std::max(_view._top - _view._bottom, std::numeric_limits<double>::epsilon());
    const double marginX = viewWidth / std::max(1u, _screenWidth);
    const double marginY = viewHeight / std::max(1u, _screenHeight);
    const double deltaX = x2 - x1;
    const double deltaY = y2 - y1;
    double entry = 0.0;
    double exit = 1.0;
    return ClipLine(-deltaX, x1 - (_view._left - marginX), entry, exit)
        && ClipLine(deltaX, _view._right + marginX - x1, entry, exit)
        && ClipLine(-deltaY, y1 - (_view._bottom - marginY), entry, exit)
        && ClipLine(deltaY, _view._top + marginY - y1, entry, exit);
}

bool KochSnowflakeRenderer::IsSubpixelSegment(const double x1, const double y1, const double x2, const double y2) const
{
    const double viewWidth = std::max(_view._right - _view._left, std::numeric_limits<double>::epsilon());
    const double viewHeight = std::max(_view._top - _view._bottom, std::numeric_limits<double>::epsilon());
    const double pixelDeltaX = (x2 - x1) * std::max(1u, _screenWidth) / viewWidth;
    const double pixelDeltaY = (y2 - y1) * std::max(1u, _screenHeight) / viewHeight;
    return std::hypot(pixelDeltaX, pixelDeltaY) <= 0.75;
}

bool KochSnowflakeRenderer::ClipLine(const double direction, const double distance, double& entry, double& exit)
{
    if (direction == 0.0)
        return distance >= 0.0;

    const double ratio = distance / direction;
    if (direction < 0.0)
    {
        if (ratio > exit)
            return false;
        entry = std::max(entry, ratio);
    }
    else
    {
        if (ratio < entry)
            return false;
        exit = std::min(exit, ratio);
    }
    return true;
}

void KochSnowflakeRenderer::CompleteWork(Context& context, const double workWeight, double& completedWork)
{
    completedWork = std::min(1.0, completedWork + workWeight);
    context.SetProgress(static_cast<int>(completedWork * 100.0));
}
