#pragma once
#include "../Renderer.h"

/*
* @class RenderFractory
* @brief Threaded Fractory rendering routines.
*/
class FractoryRenderer : public Renderer
{
public:
    FractoryRenderer();
    void Render() override;
};
