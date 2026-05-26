#pragma once
#ifndef RENDER_THREAD_POOL_H
#define RENDER_THREAD_POOL_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include "RenderFractal.h"
#include "rendering/RenderJob.h"

/**
* @class RenderThreadPool
* @brief Reusable worker pool that renders queued RenderJob objects.
*
* RenderThreadPool keeps a fixed set of worker threads alive and assigns each
* worker one RenderFractal instance. Workers pull RenderJob objects from a
* shared queue, so a worker that finishes early can immediately help with the
* remaining render area.
*/
class RenderThreadPool
{
    std::vector<std::thread> _workers;
    std::vector<RenderFractal*> _renderers;
    std::vector<char> _workerActive;
    std::deque<RenderJob> _jobs;

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
    RenderThreadPool();
    explicit RenderThreadPool(unsigned int threadNumber);
    ~RenderThreadPool();

    ///@brief Sets the worker thread count.
    ///@param threadNumber New worker count.
    void SetThreadNumber(unsigned int threadNumber);

    ///@brief Starts rendering the supplied jobs with the supplied renderers.
    ///@param renderers Renderer instances, one for each worker.
    ///@param jobs Jobs to render.
    void Render(const std::vector<RenderFractal*>& renderers, const std::vector<RenderJob>& jobs);

    ///@brief Stops all active work and waits for workers to become idle.
    void Stop();

    ///@brief Waits until the current batch is complete.
    void Wait();

    ///@brief Informs if the pool is actively rendering.
    ///@return true if there is active work.
    bool IsRunning() const;

    ///@brief Returns render progress for the current batch.
    ///@return A value from 0 to 100.
    int GetProgress();
};

#endif
