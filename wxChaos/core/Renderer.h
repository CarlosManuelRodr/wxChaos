#pragma once

#include <limits>
#include <wx/string.h>
#include "types/FractalType.h"
#include "geometry/Vector2Int.h"
#include "Options.h"

class Renderer
{
protected:
    static constexpr unsigned int InvalidColor = std::numeric_limits<unsigned int>::max();

    enum class PointTraceEvent
    {
        Started,
        Iterated,
        Escaped
    };

    struct Point
    {
        double startRe = 0.0;
        double startIm = 0.0;
        double zRe = 0.0;
        double zIm = 0.0;
        double escapedZRe = 0.0;
        double escapedZIm = 0.0;
        double zNorm = 0.0;
        double escapedNorm = 0.0;

        unsigned int iterations = 0;
        bool insideSet = true;

        double mu = 1.0;

        double gaussianDistance = 99.0;
        double previousGaussianDistance = 0.0;
        bool measureGaussianAfterEscape = false;

        double triangleDistance = 0.0;
        double previousTriangleDistance = 0.0;
        unsigned int triangleIterations = 0;

        double orbitTrapDistanceX = 0.0;
        double orbitTrapDistanceY = 0.0;
    };

    bool** _setMap;
    unsigned int** _colorMap;
    unsigned int** _auxMap;
    int _x;
    int _y;
    unsigned int _threadProgress;

    int _widthOrigin;      ///< The starting X-coordinate (left) of the rendering region.
    int _heightOrigin;     ///< The current starting Y-coordinate (top) of the rendering region. This value is updated if rendering is stopped and resumed.
    int _widthFinal;       ///< The ending X-coordinate (right) of the rendering region.
    int _heightFinal;      ///< The ending Y-coordinate (bottom) of the rendering region.
    int _oldHeightOrigin;  ///< Stores the initial Y-coordinate of the region before any rendering started. This is primarily used in AskProgress() to calculate the percentage of completion relative to the original task size, even if _ho has moved.

    bool _threadRunning;
    bool _stopped;
    Options _myOpt;

    FractalType _type;
    double _xFactor;
    double _yFactor;
    double _minX;
    double _maxX;
    double _minY;
    double _maxY;
    double _maxIter;
    PreciseRect _preciseView;
    HighPrecisionReal _preciseXFactor;
    HighPrecisionReal _preciseYFactor;
    bool _useHighPrecision;
    unsigned int _highPrecisionBits;

    double _kReal;
    double _kImaginary;

    static unsigned int ToColorMapValue(double value);
    static double SafeDistance(double distance);
    static double InitialMu();
    static double MuFromNorm(double norm);
    static void MeasureEscapeMu(Point& point, PointTraceEvent event, double zNorm);
    static void MeasureOrbitTrap(Point& point, PointTraceEvent event, double zRe, double zIm);
    static void MeasureGaussianInteger(Point& point, PointTraceEvent event, double zRe, double zIm, bool wasInside);
    static void MeasureTriangleInequality(Point& point, PointTraceEvent event, unsigned int iteration, double zRe, double zIm,
                                          double squaredRe, double squaredIm, bool wasInside);
    static double SmoothEscapeValue(const Point& point);
    static double OrbitTrapValue(const Point& point);
    [[nodiscard]] unsigned int EscapeTimeColor(const Point& point) const;
    [[nodiscard]] unsigned int GaussianIntegerColor(const Point& point) const;
    [[nodiscard]] unsigned int EscapeAngleColor(const Point& point) const;
    [[nodiscard]] unsigned int TriangleInequalityColor(const Point& point) const;

    template<class PixelRenderer>
    void RenderPixels(PixelRenderer pixelRenderer);
    template<class PixelRenderer>
    void RenderPixelsPrecise(PixelRenderer pixelRenderer);
    template<class PixelRenderer>
    void RenderPixelsByPrecision(PixelRenderer pixelRenderer);
    template<class TracePoint, class MeasurePoint>
    void RenderFromPoint(TracePoint tracePoint, unsigned int (Renderer::*colorPoint)(const Point&) const, MeasurePoint measure);
    template<class TracePoint>
    void EscapeTimeRender(TracePoint tracePoint);
    template<class TracePoint>
    void GaussianIntRender(TracePoint tracePoint);
    template<class TracePoint>
    void EscapeAngleRender(TracePoint tracePoint);
    template<class TracePoint>
    void TriangleInequalityRender(TracePoint tracePoint);

public:
    Renderer();
    virtual ~Renderer() = default;

    virtual void Render() = 0;
    static wxString GetAlgorithmName(RenderingAlgorithmType algorithm);
    virtual void Stop();

    void run();

    void SetLimits(int widthOrigin, int heightOrigin, int widthFinal, int heightFinal);
    void SetOldHeightOrigin(int oldHeightOrigin);
    void UpdateLimits(int heightOrigin);
    void SetOptions(const Options& opt);
    Options GetOptions();
    void SetRenderOut(bool** outSetMap, unsigned int** outColorMap, unsigned int** outAux = nullptr);
    void SetK(double re, double im);
    void Reset();
    virtual void PreTerminate();
    [[nodiscard]] Vector2Int GetCoords() const;
    [[nodiscard]] Vector2Int GetStartPoints() const;
    [[nodiscard]] Vector2Int GetEndPoints() const;
    [[nodiscard]] bool IsRunning() const;
    virtual unsigned int GetProgress();
};

template<class PixelRenderer> void Renderer::RenderPixels(PixelRenderer pixelRenderer)
{
    for (_y = _heightOrigin; _y < _heightFinal; _y++)
    {
        const double pixelIm = _maxY - _y * _yFactor;
        for (_x = _widthOrigin; _x < _widthFinal; _x++)
        {
            const double pixelRe = _minX + _x * _xFactor;
            pixelRenderer(pixelRe, pixelIm);
        }
    }
}

template<class PixelRenderer> void Renderer::RenderPixelsPrecise(PixelRenderer pixelRenderer)
{
    HighPrecisionReal::PrecisionScope precision(_highPrecisionBits);
    const HighPrecisionReal top = HighPrecisionReal::WithCurrentPrecision(_preciseView.top);
    const HighPrecisionReal left = HighPrecisionReal::WithCurrentPrecision(_preciseView.left);
    const HighPrecisionReal xFactor = HighPrecisionReal::WithCurrentPrecision(_preciseXFactor);
    const HighPrecisionReal yFactor = HighPrecisionReal::WithCurrentPrecision(_preciseYFactor);
    HighPrecisionReal pixelIm = top - HighPrecisionReal(_heightOrigin) * yFactor;

    for (_y = _heightOrigin; _y < _heightFinal; _y++)
    {
        HighPrecisionReal pixelRe = left + HighPrecisionReal(_widthOrigin) * xFactor;
        for (_x = _widthOrigin; _x < _widthFinal; _x++)
        {
            pixelRenderer(pixelRe, pixelIm);
            pixelRe += xFactor;
        }
        pixelIm -= yFactor;
    }
}

template<class PixelRenderer> void Renderer::RenderPixelsByPrecision(PixelRenderer pixelRenderer)
{
    if (_useHighPrecision)
        RenderPixelsPrecise(pixelRenderer);
    else
        RenderPixels(pixelRenderer);
}

template<class TracePoint, class MeasurePoint>
void Renderer::RenderFromPoint(TracePoint tracePoint, unsigned int (Renderer::*colorPoint)(const Point&) const, MeasurePoint measure)
{
    const auto renderPixel = [this, tracePoint, colorPoint, measure](const auto& pixelRe, const auto& pixelIm)
    {
        const Point point = tracePoint(pixelRe, pixelIm, measure);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    };

    RenderPixelsByPrecision(renderPixel);
}

template<class TracePoint>
void Renderer::EscapeTimeRender(TracePoint tracePoint)
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm, double, double, double, bool)
        {
            MeasureOrbitTrap(point, event, zRe, zIm);
        };
        RenderFromPoint(tracePoint, &Renderer::EscapeTimeColor, measure);
        return;
    }

    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(tracePoint, &Renderer::EscapeTimeColor, measure);
}

template<class TracePoint>
void Renderer::GaussianIntRender(TracePoint tracePoint)
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                                const double zNorm, double, double, const bool wasInside)
        {
            MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
            MeasureOrbitTrap(point, event, zRe, zIm);
            MeasureEscapeMu(point, event, zNorm);
        };
        RenderFromPoint(tracePoint, &Renderer::GaussianIntegerColor, measure);
        return;
    }

    const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                            const double zNorm, double, double, const bool wasInside)
    {
        MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    RenderFromPoint(tracePoint, &Renderer::GaussianIntegerColor, measure);
}

template<class TracePoint>
void Renderer::EscapeAngleRender(TracePoint tracePoint)
{
    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(tracePoint, &Renderer::EscapeAngleColor, measure);
}

template<class TracePoint>
void Renderer::TriangleInequalityRender(TracePoint tracePoint)
{
    const auto measure = [](Point& point, const PointTraceEvent event, const unsigned int iteration, const double zRe, const double zIm,
                            const double zNorm, const double squaredRe, const double squaredIm, const bool wasInside)
    {
        MeasureTriangleInequality(point, event, iteration, zRe, zIm, squaredRe, squaredIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    RenderFromPoint(tracePoint, &Renderer::TriangleInequalityColor, measure);
}
