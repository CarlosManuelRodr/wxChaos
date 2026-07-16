#pragma once

#include "../../vector/VectorRenderWorker.h"
#include "geometry/Rect.h"

/** @brief Generates the visible removed-square construction of a Sierpinski carpet. */
class SierpinskiCarpetRenderer : public VectorRenderWorker
{
    unsigned int _iterations{};       ///< Requested subdivision depth.
    Rect _view;                       ///< Visible world-coordinate rectangle.
    unsigned int _screenWidth{};      ///< Render width used for subpixel pruning.
    unsigned int _screenHeight{};     ///< Render height used for subpixel pruning.
    sf::Color _setColor;              ///< Color of the retained carpet.

    /** @brief Appends visible center holes recursively and reports weighted progress. */
    bool AppendHoles(Context& context, double left, double bottom, double size, unsigned int iterations,
                     double workWeight, double& completedWork);
    /** @brief Returns whether a square intersects the view plus a one-pixel margin. */
    [[nodiscard]] bool IsSquareVisible(double left, double bottom, double size) const;
    /** @brief Returns whether a square is too small to affect the rendered image reliably. */
    [[nodiscard]] bool IsSubpixelSquare(double size) const;
    /** @brief Marks a pruned or completed recursive branch as finished work. */
    static void CompleteWork(Context& context, double workWeight, double& completedWork);

protected:
    /** @brief Builds the outer square followed by every visible recursive hole. */
    void RenderGeometry(Context& context) override;

public:
    /** @brief Stores the render parameters consumed by the next background render. */
    void Configure(unsigned int iterations, const Rect& view, unsigned int screenWidth, unsigned int screenHeight,
                   const sf::Color& setColor);
};
