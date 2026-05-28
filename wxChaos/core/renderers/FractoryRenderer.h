#pragma once
#include "../Renderer.h"

/*
* @class RenderFractory
* @brief Threaded Fractory rendering routines.
*/
class FractoryRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
public:
    FractoryRenderer();
    void Render() override;
};
