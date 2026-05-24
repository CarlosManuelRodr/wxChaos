#pragma once
#include "../RenderFractal.h"

/*
* @class RenderCell
* @brief Threaded Cell rendering routines.
*/
class RenderCell : public RenderFractal
{
    double _bailout;
public:
    RenderCell();

    void Render() override;
    void SetParams(double bailout);
};
