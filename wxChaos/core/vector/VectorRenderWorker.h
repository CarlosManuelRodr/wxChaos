#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>
#include "../RenderWorker.h"
#include "../geometry/CircleData.h"
#include "../geometry/LineData.h"
#include "../geometry/RectangleData.h"
#include "../numeric/PreciseRect.h"
#include "../Options.h"

/**
 * @class VectorRenderWorker
 * @brief Cancellable background worker for vector-fractal geometry.
 */
class VectorRenderWorker : public RenderWorker
{
public:
    struct Geometry
    {
        std::vector<LineData> lines;              ///< Completed line primitives.
        std::vector<CircleData> circles;          ///< Completed circle primitives.
        std::vector<RectangleData> rectangles;    ///< Completed filled or cleared rectangle primitives.
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
        /** @brief Appends a line whose coordinates are already expressed in output pixels. */
        void AddScreenLine(double x1, double y1, double x2, double y2, const sf::Color& color);
        void AddCircle(double xCenter, double yCenter, double radius, const sf::Color& color, bool filled);
        /** @brief Appends a filled rectangle that either adds to or removes from the represented set. */
        void AddRectangle(double left, double right, double bottom, double top, const sf::Color& color,
                          bool belongsToSet);
        /** @brief Appends a filled or cleared rectangle expressed in output pixels. */
        void AddScreenRectangle(double left, double right, double bottom, double top, const sf::Color& color,
                                bool belongsToSet);
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
    template<class Real>
    struct Viewport
    {
        Real left;
        Real bottom;
        Real right;
        Real top;
    };

    Rect _doubleView;                    ///< Fast viewport used while double precision remains sufficient.
    PreciseRect _preciseView;            ///< Authoritative viewport used after the adaptive precision transition.
    unsigned int _screenWidth{};         ///< Output width used for world-to-pixel conversion.
    unsigned int _screenHeight{};        ///< Output height used for world-to-pixel conversion.
    bool _useHighPrecision{};            ///< Selects MPFR geometry generation for the next render.
    unsigned int _highPrecisionBits{};   ///< MPFR precision selected from the current zoom depth.

    /** @brief Stores the common viewport and adaptive-precision parameters for the next render. */
    void ConfigureViewport(const Options& options);

    /** @brief Creates a viewport whose values use the requested numeric representation. */
    template<class Real>
    [[nodiscard]] Viewport<Real> GetViewport() const
    {
        if constexpr (std::is_same_v<Real, double>)
            return {_doubleView._left, _doubleView._bottom, _doubleView._right, _doubleView._top};
        else
            return {HighPrecisionReal::WithCurrentPrecision(_preciseView.left),
                    HighPrecisionReal::WithCurrentPrecision(_preciseView.bottom),
                    HighPrecisionReal::WithCurrentPrecision(_preciseView.right),
                    HighPrecisionReal::WithCurrentPrecision(_preciseView.top)};
    }

    /** @brief Converts a world X coordinate to an output-pixel coordinate. */
    template<class Real>
    [[nodiscard]] double ToScreenX(const Real& x, const Viewport<Real>& view) const
    {
        const Real pixelMaximum(_screenWidth > 1 ? _screenWidth - 1 : 0);
        return ToDouble((x - view.left) * pixelMaximum / (view.right - view.left));
    }

    /** @brief Converts a world Y coordinate to an output-pixel coordinate. */
    template<class Real>
    [[nodiscard]] double ToScreenY(const Real& y, const Viewport<Real>& view) const
    {
        const Real pixelMaximum(_screenHeight > 1 ? _screenHeight - 1 : 0);
        return ToDouble((view.top - y) * pixelMaximum / (view.top - view.bottom));
    }

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
