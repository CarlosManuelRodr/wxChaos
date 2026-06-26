#include "ZoomRenderer.h"

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
        auto* scriptFractalPtr = reinterpret_cast<ScriptFractal*>(fractalCanvas->GetFractalPtr());
        fractalFactory.CreateScriptFractal(width, height, scriptFractalPtr->GetPath());
    }
    else
        fractalFactory.CreateFractal(fractalType, width, height);
}

Rect ZoomRenderer::CreateRecordingFractal(FractalFactory& fractalFactory, FractalCanvas* fractalCanvas, const int width, const int height)
{
    CreateFractalInstance(fractalFactory, fractalCanvas, width, height);
    const Rect defaultView = fractalFactory.GetFractalPtr()->GetView();
    fractalFactory.SetFormula(fractalCanvas->GetFormula());
    fractalFactory.GetFractalPtr()->SetOptions(fractalCanvas->GetFractalPtr()->GetOptions());
    return defaultView;
}

double ZoomRenderer::GetFrameProgress(const int frame, const int totalFrames)
{
    if (totalFrames <= 1)
        return 1.0;

    return static_cast<double>(frame) / static_cast<double>(totalFrames - 1);
}

Rect ZoomRenderer::GetZoomViewport(const Rect& outermostZoom, const Rect& innermostZoom, const double progress)
{
    if (progress <= 0.0)
        return outermostZoom;
    if (progress >= 1.0)
        return innermostZoom;

    const double outermostWidth = outermostZoom._right - outermostZoom._left;
    const double outermostHeight = outermostZoom._top - outermostZoom._bottom;
    const double innermostWidth = innermostZoom._right - innermostZoom._left;
    const double innermostHeight = innermostZoom._top - innermostZoom._bottom;
    const double width = outermostWidth * std::pow(innermostWidth / outermostWidth, progress);
    const double height = outermostHeight * std::pow(innermostHeight / outermostHeight, progress);

    constexpr double epsilon = 1e-12;
    double left;
    if (std::abs(outermostWidth - innermostWidth) < epsilon)
        left = outermostZoom._left + progress * (innermostZoom._left - outermostZoom._left);
    else
    {
        const double targetPosition = (innermostZoom._left - outermostZoom._left) / (outermostWidth - innermostWidth);
        const double target = outermostZoom._left + targetPosition * outermostWidth;
        left = target - targetPosition * width;
    }

    double bottom;
    if (std::abs(outermostHeight - innermostHeight) < epsilon)
        bottom = outermostZoom._bottom + progress * (innermostZoom._bottom - outermostZoom._bottom);
    else
    {
        const double targetPosition = (innermostZoom._bottom - outermostZoom._bottom) / (outermostHeight - innermostHeight);
        const double target = outermostZoom._bottom + targetPosition * outermostHeight;
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
    const Rect outermostZoom = CreateRecordingFractal(fractalHandler, _fractalCanvasPtr, _width, _height);
    const Rect innermostZoom = _fractalCanvasPtr->GetFractalPresenterPtr()->GetCurrentZoom();

    const int outputFileDigits = static_cast<int>(std::log10(_totalFrames) + 1);

    for (_currentFrame = 0; _currentFrame < _totalFrames; _currentFrame++)
    {
        const double t = _currentFrame;
        const double progress = GetFrameProgress(_currentFrame, _totalFrames);
        const Rect viewport = GetZoomViewport(outermostZoom, innermostZoom, progress);

        fractalHandler.GetFractalPtr()->SetView(viewport);

        if (_colorSpeed != -1)
            fractalHandler.GetFractalPtr()->SetVarGradient(static_cast<int>(_colorSpeed * t));
        else
            fractalHandler.GetFractalPtr()->SetVarGradient(0);

        const sf::Image out = fractalHandler.GetFractalPtr()->GetRenderedImage();
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
