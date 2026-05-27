#pragma once

constexpr int trapFactor = 1;

inline double minVal(const double a, const double b)
{
    return (a > b ? b : a);
}

inline double gaussianIntDist(const double x, const double y)
{
    const double gint_x = floor(x * trapFactor + 0.5) / trapFactor;
    const double gint_y = floor(y * trapFactor + 0.5) / trapFactor;
    return sqrt((x - gint_x) * (x - gint_x) + (y - gint_y) * (y - gint_y));
}

inline double TIA(const double zRe, double zIm, const double cRe, const double cIm,
                  const double tiaPrevX, const double tiaPrevY)
{
    const double z_mag = sqrt(tiaPrevX * tiaPrevX + tiaPrevY * tiaPrevY);
    const double c_mag = sqrt(cRe * cRe + cIm * cIm);
    const double mn = abs(z_mag - c_mag);
    const double Mn = z_mag + c_mag;
    const double num = sqrt(zRe * zRe + zIm * zIm) - mn;
    const double den = Mn - mn;
    return num / den;
}
