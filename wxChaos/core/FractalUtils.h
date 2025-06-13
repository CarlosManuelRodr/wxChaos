#pragma once
#include <cmath>

const int trapFactor = 1;

inline double minVal(double a, double b) {
    return (a > b ? b : a);
}

inline double gaussianIntDist(double x, double y) {
    double gint_x = floor(x * trapFactor + 0.5) / trapFactor;
    double gint_y = floor(y * trapFactor + 0.5) / trapFactor;
    return sqrt((x - gint_x) * (x - gint_x) + (y - gint_y) * (y - gint_y));
}

inline double TIA(double z_re, double z_im, double c_re, double c_im,
                  double tia_prev_x, double tia_prev_y) {
    double z_mag = sqrt(tia_prev_x * tia_prev_x + tia_prev_y * tia_prev_y);
    double c_mag = sqrt(c_re * c_re + c_im * c_im);
    double mn = abs(z_mag - c_mag);
    double Mn = z_mag + c_mag;
    double num = sqrt(z_re * z_re + z_im * z_im) - mn;
    double den = Mn - mn;
    return num / den;
}
