#pragma once

#include "../vector/VectorRenderWorker.h"
#include "geometry/Rect.h"

/**
 * @class KochSnowflakeRenderer
 * @brief Generates visible Koch snowflake geometry on a background thread.
 */
class KochSnowflakeRenderer : public VectorRenderWorker
{
    unsigned int _iterations{};
    Rect _view;
    unsigned int _screenWidth{};
    unsigned int _screenHeight{};
    sf::Color _color;

    bool AppendKochSegment(Context& context, double x1, double y1, double x2, double y2,
                           unsigned int iterations, double workWeight, double& completedWork);
    [[nodiscard]] bool IsCurveVisible(double x1, double y1, double x2, double y2) const;
    [[nodiscard]] bool IsSegmentVisible(double x1, double y1, double x2, double y2) const;
    [[nodiscard]] bool IsSubpixelSegment(double x1, double y1, double x2, double y2) const;
    static bool ClipLine(double direction, double distance, double& entry, double& exit);
    static void CompleteWork(Context& context, double workWeight, double& completedWork);

protected:
    void RenderGeometry(Context& context) override;

public:
    void Configure(unsigned int iterations, const Rect& view, unsigned int screenWidth, unsigned int screenHeight,
                   const sf::Color& color);
};
