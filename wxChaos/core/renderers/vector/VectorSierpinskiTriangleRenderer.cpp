#include "renderers/vector/VectorSierpinskiTriangleRenderer.h"
#include <algorithm>
#include <limits>

void VectorSierpinskiTriangleRenderer::Configure(const unsigned int iterations, const Rect& view,
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

void VectorSierpinskiTriangleRenderer::RenderGeometry(Context& context)
{
    constexpr Vertex left{-1.0, -0.57735026918962576451};
    constexpr Vertex top{0.0, 1.1547005383792515290};
    constexpr Vertex right{1.0, -0.57735026918962576451};
    double completedWork = 0.0;

    AppendLine(context, left, top);
    AppendLine(context, top, right);
    AppendLine(context, right, left);

    if (_iterations == 0)
    {
        context.SetProgress(100);
        return;
    }

    AppendHoles(context, left, top, right, _iterations, 1.0, completedWork);
}

bool VectorSierpinskiTriangleRenderer::AppendHoles(Context& context, const Vertex& first, const Vertex& second,
                                                   const Vertex& third, const unsigned int iterations,
                                                   const double workWeight, double& completedWork)
{
    if (!context.Continue())
        return false;

    if (iterations == 0 || !IsTriangleVisible(first, second, third)
        || IsSubpixelTriangle(first, second, third))
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    const Vertex firstSecond = Midpoint(first, second);
    const Vertex secondThird = Midpoint(second, third);
    const Vertex thirdFirst = Midpoint(third, first);
    AppendLine(context, firstSecond, secondThird);
    AppendLine(context, secondThird, thirdFirst);
    AppendLine(context, thirdFirst, firstSecond);

    const unsigned int remainingIterations = iterations - 1;
    const double childWork = workWeight / 3.0;
    return AppendHoles(context, first, firstSecond, thirdFirst, remainingIterations, childWork, completedWork)
        && AppendHoles(context, firstSecond, second, secondThird, remainingIterations, childWork, completedWork)
        && AppendHoles(context, thirdFirst, secondThird, third, remainingIterations, childWork, completedWork);
}

void VectorSierpinskiTriangleRenderer::AppendLine(Context& context, const Vertex& first, const Vertex& second) const
{
    if (IsLineVisible(first, second))
        context.AddLine(first.x, first.y, second.x, second.y, _color);
}

bool VectorSierpinskiTriangleRenderer::IsTriangleVisible(const Vertex& first, const Vertex& second,
                                                         const Vertex& third) const
{
    const double viewWidth = std::max(_view._right - _view._left, std::numeric_limits<double>::epsilon());
    const double viewHeight = std::max(_view._top - _view._bottom, std::numeric_limits<double>::epsilon());
    const double marginX = viewWidth / std::max(1u, _screenWidth);
    const double marginY = viewHeight / std::max(1u, _screenHeight);
    const double minimumX = std::min({first.x, second.x, third.x});
    const double maximumX = std::max({first.x, second.x, third.x});
    const double minimumY = std::min({first.y, second.y, third.y});
    const double maximumY = std::max({first.y, second.y, third.y});
    return maximumX >= _view._left - marginX && minimumX <= _view._right + marginX
        && maximumY >= _view._bottom - marginY && minimumY <= _view._top + marginY;
}

bool VectorSierpinskiTriangleRenderer::IsLineVisible(const Vertex& first, const Vertex& second) const
{
    const Vertex midpoint = Midpoint(first, second);
    return IsTriangleVisible(first, second, midpoint);
}

bool VectorSierpinskiTriangleRenderer::IsSubpixelTriangle(const Vertex& first, const Vertex& second,
                                                          const Vertex& third) const
{
    const double viewWidth = std::max(_view._right - _view._left, std::numeric_limits<double>::epsilon());
    const double viewHeight = std::max(_view._top - _view._bottom, std::numeric_limits<double>::epsilon());
    const double widthPixels = (std::max({first.x, second.x, third.x}) - std::min({first.x, second.x, third.x}))
        * std::max(1u, _screenWidth) / viewWidth;
    const double heightPixels = (std::max({first.y, second.y, third.y}) - std::min({first.y, second.y, third.y}))
        * std::max(1u, _screenHeight) / viewHeight;
    return std::max(widthPixels, heightPixels) <= 1.0;
}

VectorSierpinskiTriangleRenderer::Vertex VectorSierpinskiTriangleRenderer::Midpoint(const Vertex& first,
                                                                                    const Vertex& second)
{
    return {(first.x + second.x) / 2.0, (first.y + second.y) / 2.0};
}

void VectorSierpinskiTriangleRenderer::CompleteWork(Context& context, const double workWeight,
                                                    double& completedWork)
{
    completedWork = std::min(1.0, completedWork + workWeight);
    context.SetProgress(static_cast<int>(completedWork * 100.0));
}
