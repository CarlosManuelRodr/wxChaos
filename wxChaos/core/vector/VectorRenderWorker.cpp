#include "VectorRenderWorker.h"
#include <algorithm>

VectorRenderWorker::Context::Context(VectorRenderWorker& worker, Geometry& geometry)
    : _worker(worker), _geometry(geometry)
{
}

bool VectorRenderWorker::Context::Continue()
{
    return _worker.WaitUntilRunnable();
}

void VectorRenderWorker::Context::SetProgress(const int progress)
{
    _worker._progress = std::clamp(progress, 0, 100);
}

void VectorRenderWorker::Context::AddLine(const double x1, const double y1, const double x2, const double y2,
                                          const sf::Color& color)
{
    _geometry.lines.push_back({x1, y1, x2, y2, color});
}

void VectorRenderWorker::Context::AddScreenLine(const double x1, const double y1, const double x2, const double y2,
                                                const sf::Color& color)
{
    _geometry.lines.push_back({x1, y1, x2, y2, color, true});
}

void VectorRenderWorker::Context::AddCircle(const double xCenter, const double yCenter, const double radius,
                                            const sf::Color& color, const bool filled)
{
    _geometry.circles.push_back({xCenter, yCenter, radius, color, filled});
}

void VectorRenderWorker::Context::AddRectangle(const double left, const double right, const double bottom,
                                               const double top, const sf::Color& color,
                                               const bool belongsToSet)
{
    _geometry.rectangles.push_back({left, right, bottom, top, color, belongsToSet});
}

void VectorRenderWorker::Context::AddScreenRectangle(const double left, const double right, const double bottom,
                                                      const double top, const sf::Color& color,
                                                      const bool belongsToSet)
{
    _geometry.rectangles.push_back({left, right, bottom, top, color, belongsToSet, true});
}

void VectorRenderWorker::ConfigureViewport(const Options& options)
{
    _doubleView = {options.minX, options.minY, options.maxX, options.maxY};
    _preciseView = options.hasPreciseView ? options.preciseView : PreciseRect(_doubleView);
    _screenWidth = options.screenWidth;
    _screenHeight = options.screenHeight;
    _useHighPrecision = options.useHighPrecision;
    _highPrecisionBits = options.highPrecisionBits;
}

VectorRenderWorker::~VectorRenderWorker()
{
    Stop();
}

void VectorRenderWorker::Start()
{
    Stop();

    {
        std::lock_guard<std::mutex> lock(_geometryMutex);
        _completedGeometry = {};
        _hasCompletedGeometry = false;
    }

    _cancelRequested = false;
    _paused = false;
    _progress = 0;
    _running = true;
    _thread = std::thread(&VectorRenderWorker::WorkerLoop, this);
}

void VectorRenderWorker::Stop()
{
    _cancelRequested = true;
    _paused = false;
    _pauseCondition.notify_all();
    Wait();
}

void VectorRenderWorker::Wait()
{
    if (_thread.joinable())
        _thread.join();
}

void VectorRenderWorker::SetPaused(const bool paused)
{
    _paused = paused;
    if (!paused)
        _pauseCondition.notify_all();
}

bool VectorRenderWorker::TakeCompletedGeometry(Geometry& geometry)
{
    if (_running)
        return false;

    Wait();
    std::lock_guard<std::mutex> lock(_geometryMutex);
    if (!_hasCompletedGeometry)
        return false;

    geometry = std::move(_completedGeometry);
    _completedGeometry = {};
    _hasCompletedGeometry = false;
    return true;
}

void VectorRenderWorker::WorkerLoop()
{
    Geometry geometry;
    Context context(*this, geometry);

    try
    {
        RenderGeometry(context);
    }
    catch (...)
    {
        _cancelRequested = true;
    }

    if (!_cancelRequested)
    {
        std::lock_guard<std::mutex> lock(_geometryMutex);
        _completedGeometry = std::move(geometry);
        _hasCompletedGeometry = true;
        _progress = 100;
    }

    _running = false;
}

bool VectorRenderWorker::WaitUntilRunnable()
{
    if (_cancelRequested)
        return false;

    if (_paused)
    {
        std::unique_lock<std::mutex> lock(_pauseMutex);
        _pauseCondition.wait(lock, [this] { return !_paused || _cancelRequested; });
    }

    return !_cancelRequested;
}
