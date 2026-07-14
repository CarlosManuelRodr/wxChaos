#include "renderers/ApollonianGasketRenderer.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

void ApollonianGasketRenderer::Configure(const unsigned int iterations, const Rect& view,
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

void ApollonianGasketRenderer::RenderGeometry(Context& context)
{
    constexpr double squareRootThree = 1.7320508075688772935;
    constexpr double innerRadius = 2.0 * squareRootThree - 3.0;
    constexpr double centerDistance = 1.0 - innerRadius;
    constexpr double innerCurvature = 1.0 / innerRadius;
    const std::array<OrientedCircle, 4> initial = {{
        {-1.0, {0.0, 0.0}},
        {innerCurvature, {0.0, centerDistance}},
        {innerCurvature, {-squareRootThree * centerDistance / 2.0, -centerDistance / 2.0}},
        {innerCurvature, {squareRootThree * centerDistance / 2.0, -centerDistance / 2.0}}
    }};

    for (const OrientedCircle& circle : initial)
        AppendCircle(context, circle);

    if (_iterations == 0)
    {
        context.SetProgress(100);
        return;
    }

    double completedWork = 0.0;
    constexpr double rootWork = 0.25;
    if (!AppendGap(context, initial[1], initial[2], initial[3], initial[0], _iterations, rootWork, completedWork))
        return;
    if (!AppendGap(context, initial[0], initial[2], initial[3], initial[1], _iterations, rootWork, completedWork))
        return;
    if (!AppendGap(context, initial[0], initial[1], initial[3], initial[2], _iterations, rootWork, completedWork))
        return;
    AppendGap(context, initial[0], initial[1], initial[2], initial[3], _iterations, rootWork, completedWork);
}

bool ApollonianGasketRenderer::AppendGap(Context& context, const OrientedCircle& first,
                                         const OrientedCircle& second, const OrientedCircle& third,
                                         const OrientedCircle& excluded, const unsigned int iterations,
                                         const double workWeight, double& completedWork)
{
    if (!context.Continue())
        return false;

    if (iterations == 0 || !IsGapVisible(first, second, third))
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    const OrientedCircle circle = ReflectCircle(first, second, third, excluded);
    if (!std::isfinite(circle.curvature) || circle.curvature <= 0.0
        || !std::isfinite(circle.center.real()) || !std::isfinite(circle.center.imag()))
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    const bool subpixelCircle = IsSubpixelCircle(circle);
    if (!subpixelCircle)
        AppendCircle(context, circle);

    if (subpixelCircle)
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    const unsigned int remainingIterations = iterations - 1;
    const double childWork = workWeight / 3.0;
    return AppendGap(context, circle, second, third, first, remainingIterations, childWork, completedWork)
        && AppendGap(context, first, circle, third, second, remainingIterations, childWork, completedWork)
        && AppendGap(context, first, second, circle, third, remainingIterations, childWork, completedWork);
}

void ApollonianGasketRenderer::AppendCircle(Context& context, const OrientedCircle& circle) const
{
    if (IsCircleVisible(circle))
        context.AddCircle(circle.center.real(), circle.center.imag(), Radius(circle), _color, false);
}

ApollonianGasketRenderer::OrientedCircle ApollonianGasketRenderer::ReflectCircle(
    const OrientedCircle& first, const OrientedCircle& second, const OrientedCircle& third,
    const OrientedCircle& excluded) const
{
    const double curvature = 2.0 * (first.curvature + second.curvature + third.curvature) - excluded.curvature;
    const std::complex<double> weightedCenter = 2.0 * (first.curvature * first.center
        + second.curvature * second.center + third.curvature * third.center)
        - excluded.curvature * excluded.center;
    return {curvature, weightedCenter / curvature};
}

bool ApollonianGasketRenderer::IsGapVisible(const OrientedCircle& first, const OrientedCircle& second,
                                            const OrientedCircle& third) const
{
    double minimumX = std::numeric_limits<double>::max();
    double maximumX = std::numeric_limits<double>::lowest();
    double minimumY = std::numeric_limits<double>::max();
    double maximumY = std::numeric_limits<double>::lowest();
    double padding = 0.0;

    for (const OrientedCircle* circle : {&first, &second, &third})
    {
        if (circle->curvature <= 0.0)
            continue;

        const double radius = Radius(*circle);
        minimumX = std::min(minimumX, circle->center.real() - radius);
        maximumX = std::max(maximumX, circle->center.real() + radius);
        minimumY = std::min(minimumY, circle->center.imag() - radius);
        maximumY = std::max(maximumY, circle->center.imag() + radius);
        padding = std::max(padding, radius);
    }

    if (minimumX == std::numeric_limits<double>::max())
        return true;

    return maximumX + padding >= _view._left && minimumX - padding <= _view._right
        && maximumY + padding >= _view._bottom && minimumY - padding <= _view._top;
}

bool ApollonianGasketRenderer::IsCircleVisible(const OrientedCircle& circle) const
{
    const double radius = Radius(circle);
    return circle.center.real() + radius >= _view._left && circle.center.real() - radius <= _view._right
        && circle.center.imag() + radius >= _view._bottom && circle.center.imag() - radius <= _view._top;
}

bool ApollonianGasketRenderer::IsSubpixelCircle(const OrientedCircle& circle) const
{
    return PixelRadius(circle) <= 0.75;
}

double ApollonianGasketRenderer::PixelRadius(const OrientedCircle& circle) const
{
    const double viewWidth = std::max(_view._right - _view._left, std::numeric_limits<double>::epsilon());
    const double viewHeight = std::max(_view._top - _view._bottom, std::numeric_limits<double>::epsilon());
    const double horizontalRadius = Radius(circle) * std::max(1u, _screenWidth) / viewWidth;
    const double verticalRadius = Radius(circle) * std::max(1u, _screenHeight) / viewHeight;
    return std::min(horizontalRadius, verticalRadius);
}

double ApollonianGasketRenderer::Radius(const OrientedCircle& circle)
{
    return 1.0 / std::abs(circle.curvature);
}

void ApollonianGasketRenderer::CompleteWork(Context& context, const double workWeight, double& completedWork)
{
    completedWork = std::min(1.0, completedWork + workWeight);
    context.SetProgress(static_cast<int>(completedWork * 100.0));
}
