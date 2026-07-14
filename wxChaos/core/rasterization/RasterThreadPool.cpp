#include "rasterization/RasterThreadPool.h"
#include <algorithm>

RasterThreadPool::RasterThreadPool()
{
    _shutdown = false;
    _running = false;
    _activeWorkers = 0;
    _totalJobs = 0;
    _completedJobs = 0;
}

RasterThreadPool::RasterThreadPool(const unsigned int threadNumber) : RasterThreadPool()
{
    this->SetThreadNumber(threadNumber);
}

RasterThreadPool::~RasterThreadPool()
{
    this->ShutdownWorkers();
}

void RasterThreadPool::SetThreadNumber(const unsigned int threadNumber)
{
    if (_workers.size() == threadNumber)
        return;

    this->ShutdownWorkers();

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _shutdown = false;
        _running = false;
        _activeWorkers = 0;
        _totalJobs = 0;
        _completedJobs = 0;
        _renderers.assign(threadNumber, nullptr);
        _workerActive.assign(threadNumber, false);
    }

    for (unsigned int i = 0; i < threadNumber; i++)
        _workers.emplace_back(&RasterThreadPool::WorkerLoop, this, i);
}

void RasterThreadPool::Render(const std::vector<RenderWorker*>& renderers, const std::vector<RasterJob>& jobs)
{
    this->Stop();
    this->SetThreadNumber(static_cast<unsigned int>(renderers.size()));

    std::lock_guard<std::mutex> lock(_mutex);

    _renderers = renderers;
    _jobs.clear();
    _activeWorkers = 0;
    _totalJobs = 0;
    _completedJobs = 0;
    _workerActive.assign(_workers.size(), false);

    for (const RasterJob& job : jobs)
    {
        if (!job.IsEmpty())
        {
            _jobs.push_back(job);
            _totalJobs++;
        }
    }

    _running = _totalJobs > 0;

    if (_running)
        _workAvailable.notify_all();
    else
        _workFinished.notify_all();
}

void RasterThreadPool::Stop()
{
    std::vector<RenderWorker*> renderers;

    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (!_running && _activeWorkers == 0)
            return;

        _jobs.clear();
        renderers = _renderers;

        if (_activeWorkers == 0)
        {
            _running = false;
            _workFinished.notify_all();
            return;
        }
    }

    for (RenderWorker* renderer : renderers)
    {
        if (renderer != nullptr)
        {
            renderer->PreTerminate();
            renderer->Stop();
        }
    }

    this->Wait();
}

void RasterThreadPool::Wait()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _workFinished.wait(lock, [this] { return !_running && _activeWorkers == 0; });
}

bool RasterThreadPool::IsRunning() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _running || _activeWorkers > 0;
}

int RasterThreadPool::GetProgress() const
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_totalJobs == 0)
        return _running ? 0 : 100;

    unsigned int progress = static_cast<int>(_completedJobs * 100);

    for (unsigned int i = 0; i < _renderers.size(); i++)
    {
        if (_workerActive[i] && _renderers[i] != nullptr)
            progress += _renderers[i]->GetProgress();
    }

    return std::min(100, std::max(0, static_cast<int>(progress / _totalJobs)));
}

void RasterThreadPool::WorkerLoop(const unsigned int workerIndex)
{
    while (true)
    {
        RasterJob job;
        RenderWorker* renderer = nullptr;

        {
            std::unique_lock<std::mutex> lock(_mutex);
            _workAvailable.wait(lock, [this] { return _shutdown || !_jobs.empty(); });

            if (_shutdown)
                return;

            job = _jobs.front();
            _jobs.pop_front();
            renderer = _renderers[workerIndex];
            _workerActive[workerIndex] = true;
            _activeWorkers++;
        }

        if (renderer != nullptr && !job.IsEmpty())
        {
            const RasterRegion& region = job.GetRegion();
            renderer->SetLimits(region.GetLeft(), region.GetTop(), region.GetRight(), region.GetBottom());
            renderer->SetOldHeightOrigin(job.GetProgressOriginY());
            renderer->run();
        }

        {
            std::lock_guard<std::mutex> lock(_mutex);
            _workerActive[workerIndex] = false;
            _activeWorkers--;
            _completedJobs++;

            if (_jobs.empty() && _activeWorkers == 0)
            {
                _running = false;
                _workFinished.notify_all();
            }
        }
    }
}

void RasterThreadPool::ShutdownWorkers()
{
    this->Stop();

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _shutdown = true;
        _jobs.clear();
    }

    _workAvailable.notify_all();

    for (std::thread& worker : _workers)
    {
        if (worker.joinable())
            worker.join();
    }

    _workers.clear();
    _renderers.clear();
    _workerActive.clear();

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _shutdown = false;
        _running = false;
        _activeWorkers = 0;
        _totalJobs = 0;
        _completedJobs = 0;
    }
}
