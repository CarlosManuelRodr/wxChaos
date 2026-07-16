#include "renderers/vector/VectorSierpinskiTriangleRenderer.h"
#include <algorithm>

void VectorSierpinskiTriangleRenderer::Configure(const unsigned int iterations, const Options& options,
                                                 const sf::Color& color)
{
    Stop();
    _iterations = iterations;
    _color = color;
    ConfigureViewport(options);
}

template<class Real>
void VectorSierpinskiTriangleRenderer::RenderTyped(Context& context)
{
    const Viewport<Real> view = GetViewport<Real>();
    const Real squareRootThree = sqrt(Real(3));
    const Vertex<Real> left{Real(-1), -Real(1) / squareRootThree};
    const Vertex<Real> top{Real(0), Real(2) / squareRootThree};
    const Vertex<Real> right{Real(1), -Real(1) / squareRootThree};
    double completedWork = 0.0;

    AppendLine(context, view, left, top);
    AppendLine(context, view, top, right);
    AppendLine(context, view, right, left);

    if (_iterations == 0)
    {
        context.SetProgress(100);
        return;
    }
    AppendHoles(context, view, left, top, right, _iterations, 1.0, completedWork);
}

template<class Real>
bool VectorSierpinskiTriangleRenderer::AppendHoles(Context& context, const Viewport<Real>& view,
                                                   const Vertex<Real>& first, const Vertex<Real>& second,
                                                   const Vertex<Real>& third, const unsigned int iterations,
                                                   const double workWeight, double& completedWork)
{
    if (!context.Continue())
        return false;
    if (iterations == 0 || !IsTriangleVisible(view, first, second, third)
        || IsSubpixelTriangle(view, first, second, third))
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    const Vertex<Real> firstSecond = Midpoint(first, second);
    const Vertex<Real> secondThird = Midpoint(second, third);
    const Vertex<Real> thirdFirst = Midpoint(third, first);
    AppendLine(context, view, firstSecond, secondThird);
    AppendLine(context, view, secondThird, thirdFirst);
    AppendLine(context, view, thirdFirst, firstSecond);

    const unsigned int remainingIterations = iterations - 1;
    const double childWork = workWeight / 3.0;
    return AppendHoles(context, view, first, firstSecond, thirdFirst, remainingIterations, childWork, completedWork)
        && AppendHoles(context, view, firstSecond, second, secondThird, remainingIterations, childWork, completedWork)
        && AppendHoles(context, view, thirdFirst, secondThird, third, remainingIterations, childWork, completedWork);
}

template<class Real>
void VectorSierpinskiTriangleRenderer::AppendLine(Context& context, const Viewport<Real>& view,
                                                  const Vertex<Real>& first, const Vertex<Real>& second) const
{
    if (!IsLineVisible(view, first, second))
        return;
    if constexpr (std::is_same_v<Real, double>)
    {
        context.AddLine(first.x, first.y, second.x, second.y, _color);
    }
    else
    {
        Real entry(0);
        Real exit(1);
        const Real deltaX = second.x - first.x;
        const Real deltaY = second.y - first.y;
        if (!ClipDirection(-deltaX, first.x - view.left, entry, exit)
            || !ClipDirection(deltaX, view.right - first.x, entry, exit)
            || !ClipDirection(-deltaY, first.y - view.bottom, entry, exit)
            || !ClipDirection(deltaY, view.top - first.y, entry, exit))
            return;
        const Vertex<Real> clippedFirst{first.x + entry * deltaX, first.y + entry * deltaY};
        const Vertex<Real> clippedSecond{first.x + exit * deltaX, first.y + exit * deltaY};
        context.AddScreenLine(ToScreenX(clippedFirst.x, view), ToScreenY(clippedFirst.y, view),
                              ToScreenX(clippedSecond.x, view), ToScreenY(clippedSecond.y, view), _color);
    }
}

template<class Real>
bool VectorSierpinskiTriangleRenderer::IsTriangleVisible(const Viewport<Real>& view, const Vertex<Real>& first,
                                                         const Vertex<Real>& second, const Vertex<Real>& third) const
{
    const Real marginX = (view.right - view.left) / Real(std::max(1u, _screenWidth));
    const Real marginY = (view.top - view.bottom) / Real(std::max(1u, _screenHeight));
    const Real minimumX = std::min({first.x, second.x, third.x});
    const Real maximumX = std::max({first.x, second.x, third.x});
    const Real minimumY = std::min({first.y, second.y, third.y});
    const Real maximumY = std::max({first.y, second.y, third.y});
    return maximumX >= view.left - marginX && minimumX <= view.right + marginX
        && maximumY >= view.bottom - marginY && minimumY <= view.top + marginY;
}

template<class Real>
bool VectorSierpinskiTriangleRenderer::IsLineVisible(const Viewport<Real>& view, const Vertex<Real>& first,
                                                     const Vertex<Real>& second) const
{
    return IsTriangleVisible(view, first, second, Midpoint(first, second));
}

template<class Real>
bool VectorSierpinskiTriangleRenderer::IsSubpixelTriangle(const Viewport<Real>& view, const Vertex<Real>& first,
                                                          const Vertex<Real>& second,
                                                          const Vertex<Real>& third) const
{
    const Real width = std::max({first.x, second.x, third.x}) - std::min({first.x, second.x, third.x});
    const Real height = std::max({first.y, second.y, third.y}) - std::min({first.y, second.y, third.y});
    const double widthPixels = ToDouble(width * Real(std::max(1u, _screenWidth)) / (view.right - view.left));
    const double heightPixels = ToDouble(height * Real(std::max(1u, _screenHeight)) / (view.top - view.bottom));
    return std::max(widthPixels, heightPixels) <= 1.0;
}

template<class Real>
VectorSierpinskiTriangleRenderer::Vertex<Real> VectorSierpinskiTriangleRenderer::Midpoint(
    const Vertex<Real>& first, const Vertex<Real>& second)
{
    return {(first.x + second.x) / Real(2), (first.y + second.y) / Real(2)};
}

template<class Real>
bool VectorSierpinskiTriangleRenderer::ClipDirection(const Real& direction, const Real& distance,
                                                     Real& entry, Real& exit)
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

void VectorSierpinskiTriangleRenderer::RenderGeometry(Context& context)
{
    if (_useHighPrecision)
    {
        HighPrecisionReal::PrecisionScope precision(std::max(_highPrecisionBits, 64U));
        RenderTyped<HighPrecisionReal>(context);
    }
    else
        RenderTyped<double>(context);
}

void VectorSierpinskiTriangleRenderer::CompleteWork(Context& context, const double workWeight,
                                                    double& completedWork)
{
    completedWork = std::min(1.0, completedWork + workWeight);
    context.SetProgress(static_cast<int>(completedWork * 100.0));
}
