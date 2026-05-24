#pragma once
#include "../RenderFractal.h"

/*
* @class RenderMedusa
* @brief Threaded Medusa rendering routines.
*/
class RenderMedusa : public RenderFractal
{
public:
    RenderMedusa();

    void Render() override;
    void SpecialRender() override;
};
