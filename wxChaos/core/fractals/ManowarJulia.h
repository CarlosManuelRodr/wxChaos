#pragma once
#include "../Fractal.h"
#include "../renderers/ManowarJuliaRenderer.h"

/**
 * @class ManowarJulia
 * @brief Julia variant of the Manowar recurrence.
 *
 * Each pixel supplies z_0 and the UI Julia constant supplies k. The orbit
 * starts with m_0 = z_0, then iterates z_{n+1} = z_n^2 + m_n + k and
 * m_{n+1} = z_n. Points escape when |z|^2 exceeds 4.
 */
class ManowarJulia : public Fractal
{
    ManowarJuliaRenderer* myRender;
public:
    ManowarJulia(unsigned int width, unsigned int height);
    ~ManowarJulia() override;
    wxString GetName() const override { return wxT("Manowar Julia"); }

    void Render() override;
    void DrawOrbit() override;
};
