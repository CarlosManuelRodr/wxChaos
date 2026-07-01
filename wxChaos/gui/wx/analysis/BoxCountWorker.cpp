#include "analysis/BoxCountWorker.h"

BoxCountWorker::BoxCountWorker() : _ho(0), _hf(0), _map(nullptr), _size(0), _div(0)
{
    _running = false;
    _boxCountN = 0;
}

void BoxCountWorker::SetMap(bool** map, const int size, const int ho, const int hf)
{
    _map = map;
    _size = size;
    _ho = ho;
    _hf = hf;
}

void BoxCountWorker::SetDiv(const int div)
{
    _div = div;
}

void BoxCountWorker::Run()
{
    _running = true;

    _boxCountN = 0;
    const double epsilon = static_cast<double>(_size) / static_cast<double>(_div);
    const int ey_init = static_cast<int>(static_cast<double>(_ho) / epsilon);
    const int ey_end = static_cast<int>(static_cast<double>(_hf) / epsilon);

    for (int ey = ey_init; ey < ey_end; ey++)
    {
        for (int ex = 0; ex < _div; ex++)
        {
            if (!_running) return;
            bool found = false;

            for (int w = static_cast<int>(ex * epsilon); w < (ex + 1) * epsilon && !found; w++)
            {
                for (int h = static_cast<int>(ey * epsilon); h < (ey + 1) * epsilon; h++)
                {
                    if (w < _size && h < _size)
                    {
                        if (_map[w][h] == true)
                        {
                            found = true;
                            _boxCountN++;
                            break;
                        }
                    }
                    else break;
                }
            }
        }
    }
    _running = false;
}

int BoxCountWorker::GetBoxCount() const
{
    return _boxCountN;
}

bool BoxCountWorker::IsRunning() const
{
    return _running;
}

void BoxCountWorker::Terminate()
{
    _running = false;
}
