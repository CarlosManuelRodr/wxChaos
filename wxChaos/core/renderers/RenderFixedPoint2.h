#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class RenderFixedPoint2
* @brief Threaded FixedPoint2 rendering routines.
*/
class RenderFixedPoint2 : public RenderFractal
{
private:
    double minStep;
public:
    RenderFixedPoint2();

    void Render();
    void SetParams(double _minStep);
};
