#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include "../RenderWorker.h"
#include "../geometry/CircleData.h"
#include "../geometry/LineData.h"

/**
 * @class VectorRenderWorker
 * @brief Cancellable background worker for vector-fractal geometry.
 */
class VectorRenderWorker : public RenderWorker
{
public:
    struct Geometry
    {
        std::vector<LineData> lines;
        std::vector<CircleData> circles;
    };

    class Context
    {
        VectorRenderWorker& _worker;
        Geometry& _geometry;

    public:
        Context(VectorRenderWorker& worker, Geometry& geometry);

        bool Continue();
        void SetProgress(int progress);
        void AddLine(double x1, double y1, double x2, double y2, const sf::Color& color);
        void AddCircle(double xCenter, double yCenter, double radius, const sf::Color& color, bool filled);
    };

private:
    std::thread _thread;
    std::atomic<bool> _cancelRequested{false};
    std::atomic<bool> _paused{false};
    std::atomic<bool> _running{false};
    std::atomic<int> _progress{100};
    std::mutex _pauseMutex;
    std::condition_variable _pauseCondition;
    std::mutex _geometryMutex;
    Geometry _completedGeometry;
    bool _hasCompletedGeometry{};

    void WorkerLoop();
    bool WaitUntilRunnable();

protected:
    virtual void RenderGeometry(Context& context) = 0;

public:
    VectorRenderWorker() = default;
    ~VectorRenderWorker() override;
    VectorRenderWorker(const VectorRenderWorker&) = delete;
    VectorRenderWorker& operator=(const VectorRenderWorker&) = delete;

    void Start() override;
    void Stop() override;
    void Wait();
    void SetPaused(bool paused);
    [[nodiscard]] bool IsPaused() const { return _paused; }
    [[nodiscard]] bool IsRunning() const { return _running; }
    [[nodiscard]] unsigned int GetProgress() const override { return _progress; }
    bool TakeCompletedGeometry(Geometry& geometry);
};
