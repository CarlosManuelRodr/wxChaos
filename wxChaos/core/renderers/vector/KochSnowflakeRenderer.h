#pragma once

#include "../../vector/VectorRenderWorker.h"

/**
 * @class KochSnowflakeRenderer
 * @brief Generates visible Koch snowflake geometry on a background thread.
 */
class KochSnowflakeRenderer : public VectorRenderWorker
{
    unsigned int _iterations{};
    sf::Color _color;

    template<class Real>
    bool AppendKochSegment(Context& context, const Viewport<Real>& view, const Real& x1, const Real& y1,
                           const Real& x2, const Real& y2, unsigned int iterations, double workWeight,
                           double& completedWork);
    template<class Real>
    [[nodiscard]] bool IsCurveVisible(const Viewport<Real>& view, const Real& x1, const Real& y1,
                                      const Real& x2, const Real& y2) const;
    template<class Real>
    [[nodiscard]] bool IsSegmentVisible(const Viewport<Real>& view, const Real& x1, const Real& y1,
                                        const Real& x2, const Real& y2) const;
    template<class Real>
    [[nodiscard]] bool IsSubpixelSegment(const Viewport<Real>& view, const Real& x1, const Real& y1,
                                         const Real& x2, const Real& y2) const;
    template<class Real>
    static bool ClipLine(const Real& direction, const Real& distance, Real& entry, Real& exit);
    template<class Real>
    void AddLine(Context& context, const Viewport<Real>& view, const Real& x1, const Real& y1,
                 const Real& x2, const Real& y2) const;
    template<class Real>
    void RenderTyped(Context& context);
    static void CompleteWork(Context& context, double workWeight, double& completedWork);

protected:
    void RenderGeometry(Context& context) override;

public:
    void Configure(unsigned int iterations, const Options& options, const sf::Color& color);
};
