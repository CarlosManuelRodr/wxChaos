#pragma once
#ifndef RENDER_FRACTAL_H
#define RENDER_FRACTAL_H

#include "types/FractalType.h"
#include "geometry/Vector2Int.h"
#include "Options.h"

class RenderFractal
{
protected:
    bool** setMap;
    int** colorMap;
    unsigned int** auxMap;
    int x;
    int y;
    unsigned int threadProgress;

    int wo;
    int ho;
    int wf;
    int hf;
    int oldHo;

    bool threadRunning;
    bool stopped;
    bool specialRenderMode;
    Options myOpt;

    FractalType type;
    double xFactor;
    double yFactor;
    double minX;
    double maxX;
    double minY;
    double maxY;
    double maxIter;

    double kReal;
    double kImaginary;

public:
    virtual ~RenderFractal() = default;

    virtual void Render() = 0;
    virtual void SpecialRender() {}
    virtual void Stop();
    RenderFractal();

    void run();

    void SetLimits(int widthO, int heightO, int widthF, int heightF);
    void SetOldHo(int _oldHo);
    void UpdateLimits(int heightO);
    void SetSpecialRenderMode(bool mode);
    void SetOpt(Options opt);
    Options GetOpt();
    void SetRenderOut(bool** outSetMap, int** outColorMap, unsigned int** outAux = nullptr);
    void SetK(double re, double im);
    void Reset();
    virtual void PreTerminate();
    Vector2Int GetCoords();
    Vector2Int GetStartPoints();
    Vector2Int GetEndPoints();
    bool IsRunning();
    virtual int AskProgress();
};

#endif
