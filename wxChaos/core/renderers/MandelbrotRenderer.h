#pragma once
#include <vector>
#include "../Renderer.h"

/*
* @class RenderMandelbrot
* @brief Threaded Mandelbrot rendering routines.
*/
class MandelbrotRenderer : public Renderer
{
    struct PerturbationReference
    {
        HighPrecisionReal centerRe;
        HighPrecisionReal centerIm;
        double centerReDouble = 0.0;
        double centerImDouble = 0.0;
        std::vector<double> orbitRe;
        std::vector<double> orbitIm;
    };

    int _buddhaRandomP;

    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;
    [[nodiscard]] bool ShouldUsePerturbationRender() const;
    [[nodiscard]] PerturbationReference BuildPerturbationReference() const;
    template<class MeasurePoint>
    Point TracePerturbationPoint(const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm,
                                 const PerturbationReference& reference, MeasurePoint measure) const;
    template<class PixelRenderer>
    void RenderPerturbationPixels(const PerturbationReference& reference, PixelRenderer pixelRenderer);
    template<class MeasurePoint>
    void PerturbationRenderFromPoint(double (MandelbrotRenderer::*colorPoint)(const Point&) const, MeasurePoint measure);
    void PerturbationEscapeTimeRender();
    void PerturbationGaussianIntRender();
    void PerturbationEscapeAngleRender();
    void PerturbationTriangleInequalityRender();
    void BuddhabrotRender();

public:
    MandelbrotRenderer();

    void Render() override;
    unsigned int GetProgress() override;

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
