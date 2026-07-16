#include "renderers/vector/KochSnowflakeRenderer.h"
#include <algorithm>

void KochSnowflakeRenderer::Configure(const unsigned int iterations, const Options& options, const sf::Color& color)
{
    Stop();
    _iterations = iterations;
    _color = color;
    ConfigureViewport(options);
}

template<class Real>
void KochSnowflakeRenderer::RenderTyped(Context& context)
{
    const Viewport<Real> view = GetViewport<Real>();
    const Real squareRootThree = sqrt(Real(3));
    const Real lowerY = -Real(1) / squareRootThree;
    const Real upperY = Real(2) / squareRootThree;
    constexpr double sideWork = 1.0 / 3.0;
    double completedWork = 0.0;

    if (!AppendKochSegment(context, view, Real(-1), lowerY, Real(0), upperY, _iterations,
                           sideWork, completedWork))
        return;
    if (!AppendKochSegment(context, view, Real(0), upperY, Real(1), lowerY, _iterations,
                           sideWork, completedWork))
        return;
    AppendKochSegment(context, view, Real(1), lowerY, Real(-1), lowerY, _iterations,
                      sideWork, completedWork);
}

template<class Real>
bool KochSnowflakeRenderer::AppendKochSegment(Context& context, const Viewport<Real>& view, const Real& x1,
                                              const Real& y1, const Real& x2, const Real& y2,
                                              const unsigned int iterations, const double workWeight,
                                              double& completedWork)
{
    if (!context.Continue())
        return false;
    if (!IsCurveVisible(view, x1, y1, x2, y2))
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }
    if (iterations == 0 || IsSubpixelSegment(view, x1, y1, x2, y2))
    {
        if (IsSegmentVisible(view, x1, y1, x2, y2))
            AddLine(context, view, x1, y1, x2, y2);
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    const Real deltaX = (x2 - x1) / Real(3);
    const Real deltaY = (y2 - y1) / Real(3);
    const Real firstX = x1 + deltaX;
    const Real firstY = y1 + deltaY;
    const Real secondX = x1 + Real(2) * deltaX;
    const Real secondY = y1 + Real(2) * deltaY;
    const Real sine60 = sqrt(Real(3)) / Real(2);
    const Real peakX = firstX + deltaX / Real(2) - deltaY * sine60;
    const Real peakY = firstY + deltaX * sine60 + deltaY / Real(2);
    const unsigned int remainingIterations = iterations - 1;
    const double childWork = workWeight / 4.0;

    return AppendKochSegment(context, view, x1, y1, firstX, firstY, remainingIterations, childWork, completedWork)
        && AppendKochSegment(context, view, firstX, firstY, peakX, peakY, remainingIterations, childWork,
                             completedWork)
        && AppendKochSegment(context, view, peakX, peakY, secondX, secondY, remainingIterations, childWork,
                             completedWork)
        && AppendKochSegment(context, view, secondX, secondY, x2, y2, remainingIterations, childWork,
                             completedWork);
}

template<class Real>
bool KochSnowflakeRenderer::IsCurveVisible(const Viewport<Real>& view, const Real& x1, const Real& y1,
                                           const Real& x2, const Real& y2) const
{
    const Real deltaX = x2 - x1;
    const Real deltaY = y2 - y1;
    const Real padding = sqrt(deltaX * deltaX + deltaY * deltaY);
    return std::max(x1, x2) + padding >= view.left && std::min(x1, x2) - padding <= view.right
        && std::max(y1, y2) + padding >= view.bottom && std::min(y1, y2) - padding <= view.top;
}

template<class Real>
bool KochSnowflakeRenderer::IsSegmentVisible(const Viewport<Real>& view, const Real& x1, const Real& y1,
                                             const Real& x2, const Real& y2) const
{
    const Real marginX = (view.right - view.left) / Real(std::max(1u, _screenWidth));
    const Real marginY = (view.top - view.bottom) / Real(std::max(1u, _screenHeight));
    const Real deltaX = x2 - x1;
    const Real deltaY = y2 - y1;
    Real entry(0);
    Real exit(1);
    return ClipLine(-deltaX, x1 - (view.left - marginX), entry, exit)
        && ClipLine(deltaX, view.right + marginX - x1, entry, exit)
        && ClipLine(-deltaY, y1 - (view.bottom - marginY), entry, exit)
        && ClipLine(deltaY, view.top + marginY - y1, entry, exit);
}

template<class Real>
bool KochSnowflakeRenderer::IsSubpixelSegment(const Viewport<Real>& view, const Real& x1, const Real& y1,
                                              const Real& x2, const Real& y2) const
{
    const Real pixelDeltaX = (x2 - x1) * Real(std::max(1u, _screenWidth)) / (view.right - view.left);
    const Real pixelDeltaY = (y2 - y1) * Real(std::max(1u, _screenHeight)) / (view.top - view.bottom);
    return ToDouble(pixelDeltaX * pixelDeltaX + pixelDeltaY * pixelDeltaY) <= 0.75 * 0.75;
}

template<class Real>
bool KochSnowflakeRenderer::ClipLine(const Real& direction, const Real& distance, Real& entry, Real& exit)
{
    if (direction == Real(0))
        return distance >= Real(0);
    const Real ratio = distance / direction;
    if (direction < Real(0))
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

template<class Real>
void KochSnowflakeRenderer::AddLine(Context& context, const Viewport<Real>& view, const Real& x1, const Real& y1,
                                    const Real& x2, const Real& y2) const
{
    if constexpr (std::is_same_v<Real, double>)
    {
        context.AddLine(x1, y1, x2, y2, _color);
    }
    else
    {
        const Real deltaX = x2 - x1;
        const Real deltaY = y2 - y1;
        Real entry(0);
        Real exit(1);
        if (!ClipLine(-deltaX, x1 - view.left, entry, exit)
            || !ClipLine(deltaX, view.right - x1, entry, exit)
            || !ClipLine(-deltaY, y1 - view.bottom, entry, exit)
            || !ClipLine(deltaY, view.top - y1, entry, exit))
            return;
        const Real clippedX1 = x1 + entry * deltaX;
        const Real clippedY1 = y1 + entry * deltaY;
        const Real clippedX2 = x1 + exit * deltaX;
        const Real clippedY2 = y1 + exit * deltaY;
        context.AddScreenLine(ToScreenX(clippedX1, view), ToScreenY(clippedY1, view),
                              ToScreenX(clippedX2, view), ToScreenY(clippedY2, view), _color);
    }
}

void KochSnowflakeRenderer::RenderGeometry(Context& context)
{
    if (_useHighPrecision)
    {
        HighPrecisionReal::PrecisionScope precision(std::max(_highPrecisionBits, 64U));
        RenderTyped<HighPrecisionReal>(context);
    }
    else
        RenderTyped<double>(context);
}

void KochSnowflakeRenderer::CompleteWork(Context& context, const double workWeight, double& completedWork)
{
    completedWork = std::min(1.0, completedWork + workWeight);
    context.SetProgress(static_cast<int>(completedWork * 100.0));
}
