#pragma once

#include "../../vector/VectorRenderWorker.h"

/**
 * @class VectorSierpinskiTriangleRenderer
 * @brief Generates the visible edges of a recursively subdivided Sierpinski triangle.
 */
class VectorSierpinskiTriangleRenderer : public VectorRenderWorker
{
    template<class Real>
    struct Vertex
    {
        Real x;
        Real y;
    };

    unsigned int _iterations{};
    sf::Color _color;

    template<class Real>
    bool AppendHoles(Context& context, const Viewport<Real>& view, const Vertex<Real>& first,
                     const Vertex<Real>& second, const Vertex<Real>& third, unsigned int iterations,
                     double workWeight, double& completedWork);
    template<class Real>
    void AppendLine(Context& context, const Viewport<Real>& view, const Vertex<Real>& first,
                    const Vertex<Real>& second) const;
    template<class Real>
    [[nodiscard]] bool IsTriangleVisible(const Viewport<Real>& view, const Vertex<Real>& first,
                                         const Vertex<Real>& second, const Vertex<Real>& third) const;
    template<class Real>
    [[nodiscard]] bool IsLineVisible(const Viewport<Real>& view, const Vertex<Real>& first,
                                     const Vertex<Real>& second) const;
    template<class Real>
    [[nodiscard]] bool IsSubpixelTriangle(const Viewport<Real>& view, const Vertex<Real>& first,
                                          const Vertex<Real>& second, const Vertex<Real>& third) const;
    template<class Real>
    static Vertex<Real> Midpoint(const Vertex<Real>& first, const Vertex<Real>& second);
    template<class Real>
    static bool ClipDirection(const Real& direction, const Real& distance, Real& entry, Real& exit);
    template<class Real>
    void RenderTyped(Context& context);
    static void CompleteWork(Context& context, double workWeight, double& completedWork);

protected:
    void RenderGeometry(Context& context) override;

public:
    void Configure(unsigned int iterations, const Options& options, const sf::Color& color);
};
