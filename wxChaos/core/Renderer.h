#pragma once
#ifndef RENDER_FRACTAL_H
#define RENDER_FRACTAL_H

#include <cmath>
#include <limits>
#include "types/FractalType.h"
#include "geometry/Vector2Int.h"
#include "Options.h"

class Renderer
{
protected:
    static constexpr unsigned int InvalidColor = std::numeric_limits<unsigned int>::max();

    struct EscapePoint
    {
        unsigned int iterations = 0;
        bool insideSet = true;
        double zRe = 0.0;
        double zIm = 0.0;
        double zNorm = 0.0;
        double trapDistanceX = 0.0;
        double trapDistanceY = 0.0;
    };

    struct GaussianIntegerPoint
    {
        bool insideSet = true;
        double distance = 99.0;
        double previousDistance = 0.0;
        double mu = 0.0;
        double trapDistanceX = 0.0;
        double trapDistanceY = 0.0;
    };

    struct TriangleInequalityPoint
    {
        bool insideSet = true;
        unsigned int iterations = 0;
        double distance = 0.0;
        double previousDistance = 0.0;
        double mu = 0.0;
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

    double _kReal;
    double _kImaginary;

    static unsigned int ToColorMapValue(double value);
    static double SafeTrapDistance(double distance);
    static double OrbitTrapColorOffset(double distanceX, double distanceY);
    static double SmoothEscapeOffset(double zNorm);
    static double InitialGaussianMu();
    static double EscapedGaussianMu(double zNorm);
    unsigned int GaussianIntegerColor(const GaussianIntegerPoint& point, unsigned int paletteSize) const;
    static unsigned int EscapeAngleColor(const EscapePoint& point, unsigned int paletteSize);
    static unsigned int TriangleInequalityColor(const TriangleInequalityPoint& point);

    template<class PixelRenderer>
    void RenderPixels(PixelRenderer pixelRenderer);

public:
    Renderer();
    virtual ~Renderer() = default;

    virtual void Render() = 0;
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
    Vector2Int GetCoords() const;
    Vector2Int GetStartPoints() const;
    Vector2Int GetEndPoints() const;
    bool IsRunning() const;
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

#endif
