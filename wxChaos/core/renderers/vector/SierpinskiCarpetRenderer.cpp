#include "renderers/vector/SierpinskiCarpetRenderer.h"
#include <algorithm>

void SierpinskiCarpetRenderer::Configure(const unsigned int iterations, const Options& options,
                                         const sf::Color& setColor)
{
    Stop();
    _iterations = iterations;
    _setColor = setColor;
    ConfigureViewport(options);
}

template<class Real>
void SierpinskiCarpetRenderer::RenderTyped(Context& context)
{
    const Viewport<Real> view = GetViewport<Real>();
    const Real left(-1);
    const Real bottom(-1);
    const Real size(2);
    if (!IsSquareVisible(view, left, bottom, size))
    {
        context.SetProgress(100);
        return;
    }

    AddRectangle(context, view, left, bottom, size, _setColor, true);
    if (_iterations == 0)
    {
        context.SetProgress(100);
        return;
    }

    double completedWork = 0.0;
    AppendHoles(context, view, left, bottom, size, _iterations, 1.0, completedWork);
}

template<class Real>
bool SierpinskiCarpetRenderer::AppendHoles(Context& context, const Viewport<Real>& view, const Real& left,
                                           const Real& bottom, const Real& size, const unsigned int iterations,
                                           const double workWeight, double& completedWork)
{
    if (!context.Continue())
        return false;

    const Real childSize = size / Real(3);
    if (iterations == 0 || !IsSquareVisible(view, left, bottom, size) || IsSubpixelSquare(view, childSize))
    {
        CompleteWork(context, workWeight, completedWork);
        return true;
    }

    const Real holeLeft = left + childSize;
    const Real holeBottom = bottom + childSize;
    if (IsSquareVisible(view, holeLeft, holeBottom, childSize))
        AddRectangle(context, view, holeLeft, holeBottom, childSize, sf::Color::White, false);

    const unsigned int remainingIterations = iterations - 1;
    const double childWork = workWeight / 8.0;
    for (int row = 0; row < 3; row++)
    {
        for (int column = 0; column < 3; column++)
        {
            if (row == 1 && column == 1)
                continue;
            if (!AppendHoles(context, view, left + Real(column) * childSize, bottom + Real(row) * childSize,
                             childSize, remainingIterations, childWork, completedWork))
                return false;
        }
    }
    return true;
}

template<class Real>
bool SierpinskiCarpetRenderer::IsSquareVisible(const Viewport<Real>& view, const Real& left, const Real& bottom,
                                               const Real& size) const
{
    const Real marginX = (view.right - view.left) / Real(std::max(1u, _screenWidth));
    const Real marginY = (view.top - view.bottom) / Real(std::max(1u, _screenHeight));
    return left + size >= view.left - marginX && left <= view.right + marginX
        && bottom + size >= view.bottom - marginY && bottom <= view.top + marginY;
}

template<class Real>
bool SierpinskiCarpetRenderer::IsSubpixelSquare(const Viewport<Real>& view, const Real& size) const
{
    const double widthPixels = ToDouble(size * Real(std::max(1u, _screenWidth)) / (view.right - view.left));
    const double heightPixels = ToDouble(size * Real(std::max(1u, _screenHeight)) / (view.top - view.bottom));
    return std::max(widthPixels, heightPixels) <= 0.75;
}

template<class Real>
void SierpinskiCarpetRenderer::AddRectangle(Context& context, const Viewport<Real>& view, const Real& left,
                                            const Real& bottom, const Real& size, const sf::Color& color,
                                            const bool belongsToSet) const
{
    if constexpr (std::is_same_v<Real, double>)
    {
        context.AddRectangle(left, left + size, bottom, bottom + size, color, belongsToSet);
    }
    else
    {
        const Real clippedLeft = std::max(left, view.left);
        const Real clippedRight = std::min(left + size, view.right);
        const Real clippedBottom = std::max(bottom, view.bottom);
        const Real clippedTop = std::min(bottom + size, view.top);
        if (clippedLeft >= clippedRight || clippedBottom >= clippedTop)
            return;
        context.AddScreenRectangle(ToScreenX(clippedLeft, view), ToScreenX(clippedRight, view),
                                   ToScreenY(clippedBottom, view), ToScreenY(clippedTop, view), color, belongsToSet);
    }
}

void SierpinskiCarpetRenderer::RenderGeometry(Context& context)
{
    if (_useHighPrecision)
    {
        HighPrecisionReal::PrecisionScope precision(std::max(_highPrecisionBits, 64U));
        RenderTyped<HighPrecisionReal>(context);
    }
    else
        RenderTyped<double>(context);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void SierpinskiCarpetRenderer::CompleteWork(Context& context, const double workWeight, double& completedWork)
{
    completedWork = std::min(1.0, completedWork + workWeight);
    context.SetProgress(static_cast<int>(completedWork * 100.0));
}
