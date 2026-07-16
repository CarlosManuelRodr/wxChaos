#include "export/ZoomRenderer.h"

#include <cmath>
#include <utility>
#include "export/NativeVideoWriter.h"
#include "../../../core/fractals/raster/ScriptFractal.h"

void ZoomRenderer::CreateFractalInstance(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, const int width, const int height)
{
    const FractalType fractalType = fractalCanvas->GetFractalType();
    if (fractalType == FractalType::ScriptFractal)
    {
        auto* scriptFractalPtr = reinterpret_cast<ScriptFractal*>(fractalCanvas->GetFractal());
        fractalFactory.CreateScriptFractal(width, height, scriptFractalPtr->GetPath());
    }
    else
        fractalFactory.CreateFractal(fractalType, width, height);
}

PreciseRect ZoomRenderer::CreateRecordingFractal(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, const int width, const int height)
{
    CreateFractalInstance(fractalFactory, fractalCanvas, width, height);
    const PreciseRect defaultView = fractalFactory.GetFractal()->GetPreciseView();
    fractalFactory.SetFormula(fractalCanvas->GetFormula());
    fractalFactory.GetFractal()->SetOptions(fractalCanvas->GetFractal()->GetOptions());
    return defaultView;
}

double ZoomRenderer::GetFrameProgress(const int frame, const int totalFrames)
{
    if (totalFrames <= 1)
        return 1.0;

    return static_cast<double>(frame) / static_cast<double>(totalFrames - 1);
}

PreciseRect ZoomRenderer::GetZoomViewport(const PreciseRect& outermostZoom, const PreciseRect& innermostZoom, const double progress)
{
    if (progress <= 0.0)
        return outermostZoom;
    if (progress >= 1.0)
        return innermostZoom;

    const HighPrecisionReal outermostWidth = outermostZoom.right - outermostZoom.left;
    const HighPrecisionReal outermostHeight = outermostZoom.top - outermostZoom.bottom;
    const HighPrecisionReal innermostWidth = innermostZoom.right - innermostZoom.left;
    const HighPrecisionReal innermostHeight = innermostZoom.top - innermostZoom.bottom;
    const HighPrecisionReal progressValue(progress);
    const HighPrecisionReal width = outermostWidth * pow(innermostWidth / outermostWidth, progressValue);
    const HighPrecisionReal height = outermostHeight * pow(innermostHeight / outermostHeight, progressValue);

    HighPrecisionReal left;
    if (outermostWidth == innermostWidth)
        left = outermostZoom.left + progressValue * (innermostZoom.left - outermostZoom.left);
    else
    {
        const HighPrecisionReal targetPosition = (innermostZoom.left - outermostZoom.left) / (outermostWidth - innermostWidth);
        const HighPrecisionReal target = outermostZoom.left + targetPosition * outermostWidth;
        left = target - targetPosition * width;
    }

    HighPrecisionReal bottom;
    if (outermostHeight == innermostHeight)
        bottom = outermostZoom.bottom + progressValue * (innermostZoom.bottom - outermostZoom.bottom);
    else
    {
        const HighPrecisionReal targetPosition = (innermostZoom.bottom - outermostZoom.bottom) / (outermostHeight - innermostHeight);
        const HighPrecisionReal target = outermostZoom.bottom + targetPosition * outermostHeight;
        bottom = target - targetPosition * height;
    }

    return {left, bottom, left + width, bottom + height};
}

wxThread::ExitCode ZoomRenderer::Entry()
{
    FractalFactory fractalHandler;
    CreateRecordingFractal(fractalHandler, _fractalCanvasPtr, _width, _height);
    const PreciseRect outermostZoom = _fractalCanvasPtr->GetFractalPresenter()->GetPreciseOutermostZoom();
    const PreciseRect innermostZoom = _fractalCanvasPtr->GetFractal()->GetPreciseView();

    NativeVideoWriter videoWriter;
    if (!videoWriter.Open(_outputPath, static_cast<unsigned int>(_width), static_cast<unsigned int>(_height),
                          static_cast<unsigned int>(_framerate)))
    {
        _error = videoWriter.GetError();
        return nullptr;
    }

    for (int frame = 0; frame < _totalFrames; frame++)
    {
        _currentFrame = frame;
        const double t = frame;
        const double progress = GetFrameProgress(frame, _totalFrames);
        const PreciseRect viewport = GetZoomViewport(outermostZoom, innermostZoom, progress);

        fractalHandler.GetFractal()->SetPreciseView(viewport);

        if (_colorSpeed != -1)
            fractalHandler.GetFractal()->SetVarGradient(static_cast<int>(_colorSpeed * t));
        else
            fractalHandler.GetFractal()->SetVarGradient(0);

        const sf::Image out = fractalHandler.GetFractal()->GetRenderedImage();
        if (!videoWriter.WriteFrame(out))
        {
            _error = videoWriter.GetError();
            return nullptr;
        }
    }

    _currentFrame = _totalFrames;
    if (!videoWriter.Close())
        _error = videoWriter.GetError();

    return nullptr;
}

ZoomRenderer::ZoomRenderer(std::string outputPath, FractalCanvas* fractalCanvas, const int width, const int height,
                           const int totalFrames, const int framerate, const double colorSpeed) : wxThread(wxTHREAD_JOINABLE)
{
    _outputPath = std::move(outputPath);
    _fractalCanvasPtr = fractalCanvas;
    _currentFrame = 0;
    _totalFrames = totalFrames;
    _framerate = framerate;
    _width = width;
    _height = height;
    _colorSpeed = colorSpeed;
}

int ZoomRenderer::GetProgress() const
{
    return _currentFrame.load();
}

std::string ZoomRenderer::GetError() const
{
    return _error;
}
