#pragma once

#include <atomic>
#include <string>
#include <wx/thread.h>
#include "FractalFactory.h"
#include "export/NativeVideoWriter.h"
#include "numeric/PreciseRect.h"

class FractalCanvas;

/**
* @class ZoomRenderer
* @brief Worker thread that renders zoom-recorder frames and encodes them into a video.
*/
class ZoomRenderer : public wxThread
{
    FractalCanvas* _fractalCanvasPtr;
    std::atomic<int> _currentFrame;
    int _totalFrames;
    int _framerate;
    int _width;
    int _height;
    NativeVideoEncodingOptions _encodingOptions;
    double _colorSpeed;
    std::string _outputPath;
    std::string _error;

    static void CreateFractalInstance(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, int width, int height);
    static PreciseRect CreateRecordingFractal(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, int width, int height);

protected:
    ExitCode Entry() override;

public:
    ZoomRenderer(std::string outputPath, FractalCanvas* fractalCanvas, int width, int height, int totalFrames,
                 int framerate, NativeVideoEncodingOptions encodingOptions, double colorSpeed);

    static double GetFrameProgress(int frame, int totalFrames);
    static PreciseRect GetZoomViewport(const PreciseRect& outermostZoom, const PreciseRect& innermostZoom, double progress);

    [[nodiscard]] int GetProgress() const;
    [[nodiscard]] std::string GetError() const;
};
