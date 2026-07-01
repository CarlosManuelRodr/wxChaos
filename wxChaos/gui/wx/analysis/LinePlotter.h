#pragma once

#include <mathplot.h>

struct LineParams
{
    double m, b;
};

class LinePlotter : public mpFX
{
    LineParams _params;
public:
    explicit LinePlotter(LineParams params);
    double GetY(double x) override;
};
