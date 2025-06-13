#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class ManowarJulia
* @brief Handles the RenderManowarJulia threads.
*/
class ManowarJulia : public Fractal
{
private:
    RenderManowarJulia* myRender;
public:
    ManowarJulia(sf::RenderWindow* Window);
    ManowarJulia(int width, int height);
    ~ManowarJulia();

    void Render();
    void DrawOrbit();
};
