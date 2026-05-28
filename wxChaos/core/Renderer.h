#pragma once
#ifndef RENDER_FRACTAL_H
#define RENDER_FRACTAL_H

#include "types/FractalType.h"
#include "geometry/Vector2Int.h"
#include "Options.h"

class Renderer
{
protected:
    bool** _setMap;
    int** _colorMap;
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
    void SetRenderOut(bool** outSetMap, int** outColorMap, unsigned int** outAux = nullptr);
    void SetK(double re, double im);
    void Reset();
    virtual void PreTerminate();
    Vector2Int GetCoords() const;
    Vector2Int GetStartPoints() const;
    Vector2Int GetEndPoints() const;
    bool IsRunning() const;
    virtual unsigned int GetProgress();
};

#endif
