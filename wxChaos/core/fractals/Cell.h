#pragma once
#include "../Fractal.h"
#include "../renderers/CellRenderer.h"

/**
 * @class Cell
 * @brief Escape-time map using a coupled z and b recurrence.
 *
 * Each pixel is the complex parameter c. The orbit starts with z_0 = c and
 * b_0 = c - sin(c), then iterates b_{n+1} = b_n / c and
 * z_{n+1} = z_n c + b_{n+1} / z_n. The panel bailout is compared as |z|^2
 * > bailout^2.
 */
class Cell : public Fractal
{
private:
    CellRenderer* myRender;
    double bailout;
public:
    Cell(unsigned int width, unsigned int height);
    ~Cell();
    wxString GetName() const override { return "Cell"; }

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
