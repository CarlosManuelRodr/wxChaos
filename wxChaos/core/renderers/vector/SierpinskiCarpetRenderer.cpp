#include "renderers/vector/SierpinskiCarpetRenderer.h"
#include <algorithm>
#include <limits>

void SierpinskiCarpetRenderer::Configure(const unsigned int iterations, const Rect& view,
                                         const unsigned int screenWidth, const unsigned int screenHeight,
                                         const sf::Color& setColor)
{
    Stop();
    _iterations = iterations;
    _view = view;
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    _setColor = setColor;
}

void SierpinskiCarpetRenderer::RenderGeometry(Context& context)
{
    constexpr double left = -1.0;
    constexpr double bottom = -1.0;
    constexpr double size = 2.0;
    if (!IsSquareVisible(left, bottom, size))
    {
        context.SetProgress(100);
        return;
    }

    context.AddRectangle(left, left + size, bottom, bottom + size, _setColor, true);
    if (_iterations == 0)
    {
        context.SetProgress(100);
        return;
    }

    double completedWork = 0.0;
    AppendHoles(context, left, bottom, size, _iterations, 1.0, completedWork);
}

bool SierpinskiCarpetRenderer::AppendHoles(Context& context, const double left, const double bottom,
                                           const double size, const unsigned int iterations,
                                           const double workWeight, double& completedWork)
{
    if (!context.Continue())
        return false;

    if (iterations == 0 || !IsSquareVisible(left, bottom, size) || IsSubpixelSquare(size / 3.0))
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    const double childSize = size / 3.0;
    const double holeLeft = left + childSize;
    const double holeBottom = bottom + childSize;
    if (IsSquareVisible(holeLeft, holeBottom, childSize))
        context.AddRectangle(holeLeft, holeLeft + childSize, holeBottom, holeBottom + childSize,
                             sf::Color::White, false);

    const unsigned int remainingIterations = iterations - 1;
    const double childWork = workWeight / 8.0;
    for (int row = 0; row < 3; row++)
    {
        for (int column = 0; column < 3; column++)
        {
            if (row == 1 && column == 1)
                continue;
            if (!AppendHoles(context, left + column * childSize, bottom + row * childSize, childSize,
                             remainingIterations, childWork, completedWork))
                return false;
        }
    }
    return true;
}

bool SierpinskiCarpetRenderer::IsSquareVisible(const double left, const double bottom, const double size) const
{
    const double viewWidth = std::max(_view._right - _view._left, std::numeric_limits<double>::epsilon());
    const double viewHeight = std::max(_view._top - _view._bottom, std::numeric_limits<double>::epsilon());
    const double marginX = viewWidth / std::max(1u, _screenWidth);
    const double marginY = viewHeight / std::max(1u, _screenHeight);
    return left + size >= _view._left - marginX && left <= _view._right + marginX
        && bottom + size >= _view._bottom - marginY && bottom <= _view._top + marginY;
}

bool SierpinskiCarpetRenderer::IsSubpixelSquare(const double size) const
{
    const double viewWidth = std::max(_view._right - _view._left, std::numeric_limits<double>::epsilon());
    const double viewHeight = std::max(_view._top - _view._bottom, std::numeric_limits<double>::epsilon());
    const double widthPixels = size * std::max(1u, _screenWidth) / viewWidth;
    const double heightPixels = size * std::max(1u, _screenHeight) / viewHeight;
    return std::max(widthPixels, heightPixels) <= 0.75;
}

void SierpinskiCarpetRenderer::CompleteWork(Context& context, const double workWeight, double& completedWork)
{
    completedWork = std::min(1.0, completedWork + workWeight);
    context.SetProgress(static_cast<int>(completedWork * 100.0));
}
