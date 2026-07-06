#include "export/ZoomRenderer.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <utility>
#include "AppPaths.h"
#include "fractals/ScriptFractal.h"

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

std::string ZoomRenderer::FixedLengthToString(const int i, const int length)
{
    std::ostringstream ostr;
    if (i < 0)
        ostr << '-';

    ostr << std::setfill('0') << std::setw(length) << (i < 0 ? -i : i);
    return ostr.str();
}

std::string ZoomRenderer::QuoteCommandArg(const std::string& value)
{
    std::string quoted = "\"";
    for (const char ch : value)
    {
        if (ch == '"')
            quoted += "\\\"";
        else
            quoted += ch;
    }
    quoted += "\"";
    return quoted;
}

wxThread::ExitCode ZoomRenderer::Entry()
{
    FractalFactory fractalHandler;
    CreateRecordingFractal(fractalHandler, _fractalCanvasPtr, _width, _height);
    const PreciseRect outermostZoom = _fractalCanvasPtr->GetFractalPresenter()->GetPreciseOutermostZoom();
    const PreciseRect innermostZoom = _fractalCanvasPtr->GetFractal()->GetPreciseView();

    const int outputFileDigits = static_cast<int>(std::log10(_totalFrames) + 1);

    for (_currentFrame = 0; _currentFrame < _totalFrames; _currentFrame++)
    {
        const double t = _currentFrame;
        const double progress = GetFrameProgress(_currentFrame, _totalFrames);
        const PreciseRect viewport = GetZoomViewport(outermostZoom, innermostZoom, progress);

        fractalHandler.GetFractal()->SetPreciseView(viewport);

        if (_colorSpeed != -1)
            fractalHandler.GetFractal()->SetVarGradient(static_cast<int>(_colorSpeed * t));
        else
            fractalHandler.GetFractal()->SetVarGradient(0);

        const sf::Image out = fractalHandler.GetFractal()->GetRenderedImage();
        const std::string filename = "frame_" + FixedLengthToString(_currentFrame, outputFileDigits) + ".jpg";
        const std::string fullPath = AppPaths::JoinStd(_filepath, filename);

        // ReSharper disable once CppExpressionWithoutSideEffects
        out.saveToFile(fullPath);
    }

    const std::string ffmpegPath = AppPaths::FfmpegFileStd();
    const std::string fileTemplate = "frame_%0" + std::to_string(outputFileDigits) + "d.jpg";
    const std::string inputFrames = AppPaths::JoinStd(_filepath, fileTemplate);
    const std::string outputVideo = AppPaths::JoinStd(_filepath, "Zoom.mp4");
    const std::string renderVideoCommand = QuoteCommandArg(ffmpegPath) + " -i " + QuoteCommandArg(inputFrames) +
        " -c:v libx264 -vf fps=30 -vf \"crop = trunc(iw / 2) * 2:trunc(ih / 2) * 2\" -pix_fmt yuv420p " + QuoteCommandArg(outputVideo);

    system(renderVideoCommand.c_str());

    return nullptr;
}

ZoomRenderer::ZoomRenderer(std::string filepath, FractalCanvas* fractalCanvas, const int width, const int height, const int totalFrames,
                           const double colorSpeed)
{
    _filepath = std::move(filepath);
    _fractalCanvasPtr = fractalCanvas;
    _currentFrame = 0;
    _totalFrames = totalFrames;
    _width = width;
    _height = height;
    _colorSpeed = colorSpeed;
}

int ZoomRenderer::GetProgress() const
{
    return _currentFrame;
}
