#pragma once
#include "../RenderFractal.h"

/*
* @class RenderTricorn
* @brief Threaded Tricorn rendering routines.
*/
class RenderTricorn : public RenderFractal
{
public:
    RenderTricorn();
    void Render() override;
};
