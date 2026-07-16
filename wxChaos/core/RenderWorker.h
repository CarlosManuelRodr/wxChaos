#pragma once

/**
 * @class RenderWorker
 * @brief Minimal execution contract shared by raster and vector render workers.
 */
class RenderWorker
{
public:
    virtual ~RenderWorker() = default;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    [[nodiscard]] virtual unsigned int GetProgress() const = 0;
};
