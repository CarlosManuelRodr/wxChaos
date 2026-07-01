#include "analysis/LinePlotter.h"

LinePlotter::LinePlotter(const LineParams params) : mpFX("Fit"), _params(params) {}

double LinePlotter::GetY(const double x)
{
    return _params.m * x + _params.b;
}
