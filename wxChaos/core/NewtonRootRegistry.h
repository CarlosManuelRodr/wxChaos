#pragma once

#include <complex>
#include <vector>

/**
 * @brief Dynamically classifies numerically discovered Newton roots.
 *
 * The registry stores roots as they are encountered while rendering. A new
 * candidate is assigned to an existing root when it is within the configured
 * tolerance; otherwise it is appended as a new root and receives the next id.
 */
class NewtonRootRegistry
{
    std::vector<std::complex<double>> _roots;
    double _tolerance;

public:
    /**
     * @brief Create an empty registry with the supplied classification radius.
     * @param tolerance Maximum distance for two candidates to be treated as
     * the same numerical root.
     */
    explicit NewtonRootRegistry(double tolerance = 1e-5);

    /** @brief Remove all discovered roots while preserving the tolerance. */
    void Clear();

    /**
     * @brief Change the root matching tolerance.
     * @param tolerance Maximum distance for future classifications.
     */
    void SetTolerance(double tolerance);

    /**
     * @brief Classify a converged Newton endpoint or register it as a new root.
     * @param rootCandidate Numerical root candidate produced by iteration.
     * @return Stable zero-based root id for the matching or newly added root.
     */
    unsigned int ClassifyOrAdd(const std::complex<double>& rootCandidate);

    /**
     * @brief Access the discovered roots in id order.
     * @return Registry-owned roots; index equals the root id.
     */
    [[nodiscard]] const std::vector<std::complex<double>>& GetRoots() const;
};
