#pragma once
#include "../Renderer.h"

/*
* @class RenderCell
* @brief Threaded Cell rendering routines.
*/
class CellRenderer : public Renderer
{
    double _bailout;
public:
    CellRenderer();

    void Render() override;
    void SetParams(double bailout);
};
