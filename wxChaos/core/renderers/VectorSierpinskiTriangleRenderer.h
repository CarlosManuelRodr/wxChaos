#pragma once

#include "../vector/VectorRenderWorker.h"
#include "geometry/Rect.h"

/**
 * @class VectorSierpinskiTriangleRenderer
 * @brief Generates the visible edges of a recursively subdivided Sierpinski triangle.
 */
class VectorSierpinskiTriangleRenderer : public VectorRenderWorker
{
    struct Vertex
    {
        double x;
        double y;
    };

    unsigned int _iterations{};
    Rect _view;
    unsigned int _screenWidth{};
    unsigned int _screenHeight{};
    sf::Color _color;

    bool AppendHoles(Context& context, const Vertex& first, const Vertex& second, const Vertex& third,
                     unsigned int iterations, double workWeight, double& completedWork);
    void AppendLine(Context& context, const Vertex& first, const Vertex& second) const;
    [[nodiscard]] bool IsTriangleVisible(const Vertex& first, const Vertex& second, const Vertex& third) const;
    [[nodiscard]] bool IsLineVisible(const Vertex& first, const Vertex& second) const;
    [[nodiscard]] bool IsSubpixelTriangle(const Vertex& first, const Vertex& second, const Vertex& third) const;
    static Vertex Midpoint(const Vertex& first, const Vertex& second);
    static void CompleteWork(Context& context, double workWeight, double& completedWork);

protected:
    void RenderGeometry(Context& context) override;

public:
    void Configure(unsigned int iterations, const Rect& view, unsigned int screenWidth, unsigned int screenHeight,
                   const sf::Color& color);
};
