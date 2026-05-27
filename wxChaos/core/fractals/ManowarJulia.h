#pragma once
#include "../Fractal.h"
#include "../renderers/RenderManowarJulia.h"

/*
* @class ManowarJulia
* @brief Handles the RenderManowarJulia threads.
*/
class ManowarJulia : public Fractal
{
    RenderManowarJulia* myRender;
public:
    explicit ManowarJulia(const sf::RenderWindow* window);
    ManowarJulia(int width, int height);
    ~ManowarJulia() override;

    void Render() override;
    void DrawOrbit() override;
};
