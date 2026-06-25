#include <cmath>
#include "NewtonRootRegistry.h"

NewtonRootRegistry::NewtonRootRegistry(const double tolerance) : _tolerance(tolerance) {}

void NewtonRootRegistry::Clear()
{
    _roots.clear();
}

void NewtonRootRegistry::SetTolerance(const double tolerance)
{
    _tolerance = tolerance;
}

unsigned int NewtonRootRegistry::ClassifyOrAdd(const std::complex<double>& rootCandidate)
{
    for (unsigned int i = 0; i < _roots.size(); i++)
    {
        if (std::abs(rootCandidate - _roots[i]) <= _tolerance)
            return i;
    }

    _roots.push_back(rootCandidate);
    return static_cast<unsigned int>(_roots.size() - 1);
}

const std::vector<std::complex<double>>& NewtonRootRegistry::GetRoots() const
{
    return _roots;
}
