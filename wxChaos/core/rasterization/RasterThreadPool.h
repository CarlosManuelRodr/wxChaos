#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include "RasterRenderWorker.h"
#include "rasterization/RasterJob.h"

/**
* @class RasterThreadPool
* @brief Reusable worker pool that renders queued RasterJob objects.
*
* RasterThreadPool keeps a fixed set of worker threads alive and assigns each
* worker one Renderer instance. Workers pull RasterJob objects from a
* shared queue, so a worker that finishes early can immediately help with the
* remaining render area.
*/
class RasterThreadPool
{
    std::vector<std::thread> _workers;
    std::vector<RasterRenderWorker*> _renderers;
    std::vector<char> _workerActive;
    std::deque<RasterJob> _jobs;

    mutable std::mutex _mutex;
    std::condition_variable _workAvailable;
    std::condition_variable _workFinished;

    bool _shutdown;
    bool _running;
    unsigned int _activeWorkers;
    unsigned int _totalJobs;
    unsigned int _completedJobs;

    void WorkerLoop(unsigned int workerIndex);
    void ShutdownWorkers();

public:
    RasterThreadPool();
    explicit RasterThreadPool(unsigned int threadNumber);
    ~RasterThreadPool();

    ///@brief Sets the worker thread count.
    ///@param threadNumber New worker count.
    void SetThreadNumber(unsigned int threadNumber);

    ///@brief Starts rendering the supplied jobs with the supplied renderers.
    ///@param renderers Renderer instances, one for each worker.
    ///@param jobs Jobs to render.
    void Render(const std::vector<RasterRenderWorker*>& renderers, const std::vector<RasterJob>& jobs);

    ///@brief Stops all active work and waits for workers to become idle.
    void Stop();

    ///@brief Waits until the current batch is complete.
    void Wait();

    ///@brief Informs if the pool is actively rendering.
    ///@return true if there is active work.
    [[nodiscard]] bool IsRunning() const;

    ///@brief Returns render progress for the current batch.
    ///@return A value from 0 to 100.
    [[nodiscard]] int GetProgress() const;
};
