#pragma once
#include "../FractalClasses.h"

/*
* @class RenderCell
* @brief Threaded Cell rendering routines.
*/
class RenderCell : public RenderFractal
{
private:
    double bailout;
public:
    RenderCell();

    void Render();
    void SetParams(double _bailout);
};
