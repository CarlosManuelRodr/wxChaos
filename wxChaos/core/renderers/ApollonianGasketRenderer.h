#pragma once

#include <complex>
#include "../vector/VectorRenderWorker.h"
#include "geometry/Rect.h"

/**
 * @class ApollonianGasketRenderer
 * @brief Generates visible circles in an Apollonian circle packing.
 */
class ApollonianGasketRenderer : public VectorRenderWorker
{
    struct OrientedCircle
    {
        double curvature;
        std::complex<double> center;
    };

    unsigned int _iterations{};
    Rect _view;
    unsigned int _screenWidth{};
    unsigned int _screenHeight{};
    sf::Color _color;

    bool AppendGap(Context& context, const OrientedCircle& first, const OrientedCircle& second,
                   const OrientedCircle& third, const OrientedCircle& excluded, unsigned int iterations,
                   double workWeight, double& completedWork);
    void AppendCircle(Context& context, const OrientedCircle& circle) const;
    [[nodiscard]] OrientedCircle ReflectCircle(const OrientedCircle& first, const OrientedCircle& second,
                                               const OrientedCircle& third,
                                               const OrientedCircle& excluded) const;
    [[nodiscard]] bool IsGapVisible(const OrientedCircle& first, const OrientedCircle& second,
                                    const OrientedCircle& third) const;
    [[nodiscard]] bool IsCircleVisible(const OrientedCircle& circle) const;
    [[nodiscard]] bool IsSubpixelCircle(const OrientedCircle& circle) const;
    [[nodiscard]] double PixelRadius(const OrientedCircle& circle) const;
    static double Radius(const OrientedCircle& circle);
    static void CompleteWork(Context& context, double workWeight, double& completedWork);

protected:
    void RenderGeometry(Context& context) override;

public:
    void Configure(unsigned int iterations, const Rect& view, unsigned int screenWidth, unsigned int screenHeight,
                   const sf::Color& color);
};
