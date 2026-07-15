#pragma once

class BoxCountMap;

/**
 * @class BoxCountWorker
 * @brief Counts occupied grid boxes within one horizontal slice of an occupancy map.
 *
 * DimensionFrame creates one worker per thread. All workers read the same immutable
 * BoxCountMap while each worker owns a disjoint range of pixel rows.
 */
class BoxCountWorker
{
    int _ho;                    ///< First pixel row assigned to this worker.
    int _hf;                    ///< One-past-the-last pixel row assigned to this worker.
    const BoxCountMap* _map;    ///< Shared immutable fractal occupancy map.
    int _size;                  ///< Width and height of the square occupancy map.
    int _div;                   ///< Number of grid divisions along each axis.
    bool _running;              ///< true while Run() is counting boxes.
    int _boxCountN;             ///< Number of occupied boxes found in this worker's slice.

public:
    /** @brief Creates an idle worker without an assigned map. */
    BoxCountWorker();

    /**
     * @brief Assigns the occupancy map and pixel-row range processed by this worker.
     * @param map Shared map that remains valid until the worker finishes.
     * @param ho First assigned pixel row.
     * @param hf One-past-the-last assigned pixel row.
     */
    void SetMap(const BoxCountMap* map, int ho, int hf);

    /** @brief Sets the number of box-grid divisions along each image axis. */
    void SetDiv(int div);

    /** @brief Counts occupied boxes synchronously until complete or terminated. */
    void Run();

    /** @brief Returns the number of occupied boxes found by the most recent run. */
    [[nodiscard]] int GetBoxCount() const;

    /** @brief Returns true while Run() is actively processing the map. */
    [[nodiscard]] bool IsRunning() const;

    /** @brief Requests cooperative termination of the active counting loop. */
    void Terminate();
};
