#pragma once

#include <string>
#include <wx/thread.h>
#include "FractalCanvas.h"

/**
* @class ZoomRenderer
* @brief Worker thread that renders zoom-recorder frames and encodes them into a video.
*/
class ZoomRenderer : public wxThread
{
    FractalCanvas* _fractalCanvasPtr;
    int _currentFrame;
    int _totalFrames;
    int _width;
    int _height;
    double _colorSpeed;
    std::string _filepath;

    static void CreateFractalInstance(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, int width, int height);
    static PreciseRect CreateRecordingFractal(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, int width, int height);
    static std::string FixedLengthToString(int i, int length);
    static std::string QuoteCommandArg(const std::string& value);

protected:
    ExitCode Entry() override;

public:
    ZoomRenderer(std::string filepath, FractalCanvas* fractalCanvas, int width, int height, int totalFrames, double colorSpeed);

    static double GetFrameProgress(int frame, int totalFrames);
    static PreciseRect GetZoomViewport(const PreciseRect& outermostZoom, const PreciseRect& innermostZoom, double progress);

    [[nodiscard]] int GetProgress() const;
};
