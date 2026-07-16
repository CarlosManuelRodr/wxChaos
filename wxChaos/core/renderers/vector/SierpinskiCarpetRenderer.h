#pragma once

#include "../../vector/VectorRenderWorker.h"

/** @brief Generates the visible removed-square construction of a Sierpinski carpet. */
class SierpinskiCarpetRenderer : public VectorRenderWorker
{
    unsigned int _iterations{};       ///< Requested subdivision depth.
    sf::Color _setColor;              ///< Color of the retained carpet.

    /** @brief Appends visible center holes recursively and reports weighted progress. */
    template<class Real>
    bool AppendHoles(Context& context, const Viewport<Real>& view, const Real& left, const Real& bottom,
                     const Real& size, unsigned int iterations, double workWeight, double& completedWork);
    /** @brief Returns whether a square intersects the view plus a one-pixel margin. */
    template<class Real>
    [[nodiscard]] bool IsSquareVisible(const Viewport<Real>& view, const Real& left, const Real& bottom,
                                       const Real& size) const;
    /** @brief Returns whether a square is too small to affect the rendered image reliably. */
    template<class Real>
    [[nodiscard]] bool IsSubpixelSquare(const Viewport<Real>& view, const Real& size) const;
    /** @brief Publishes one square in world or screen coordinates according to the numeric path. */
    template<class Real>
    void AddRectangle(Context& context, const Viewport<Real>& view, const Real& left, const Real& bottom,
                      const Real& size, const sf::Color& color, bool belongsToSet) const;
    /** @brief Generates carpet geometry using one numeric representation. */
    template<class Real>
    void RenderTyped(Context& context);
    /** @brief Marks a pruned or completed recursive branch as finished work. */
    static void CompleteWork(Context& context, double workWeight, double& completedWork);

protected:
    /** @brief Builds the outer square followed by every visible recursive hole. */
    void RenderGeometry(Context& context) override;

public:
    /** @brief Stores the render parameters consumed by the next background render. */
    void Configure(unsigned int iterations, const Options& options, const sf::Color& setColor);
};
