#pragma once

class BoxCountWorker
{
    int _ho, _hf;      ///< Work area of the thread.
    bool** _map;       ///< Fractal target.
    int _size, _div;
    bool _running;
    int _boxCountN;    ///< Number of counted boxes.

public:
    BoxCountWorker();

    void SetMap(bool** map, int size, int ho, int hf);
    void SetDiv(int div);
    void Run();
    [[nodiscard]] int GetBoxCount() const;
    [[nodiscard]] bool IsRunning() const;
    void Terminate();
};
