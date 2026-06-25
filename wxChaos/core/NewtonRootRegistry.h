#pragma once

#include <complex>
#include <vector>

class NewtonRootRegistry
{
    std::vector<std::complex<double>> _roots;
    double _tolerance;

public:
    explicit NewtonRootRegistry(double tolerance = 1e-5);

    void Clear();
    void SetTolerance(double tolerance);
    unsigned int ClassifyOrAdd(const std::complex<double>& rootCandidate);
    [[nodiscard]] const std::vector<std::complex<double>>& GetRoots() const;
};
