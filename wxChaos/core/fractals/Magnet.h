#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class Magnet
* @brief Handles the RenderMagnet threads.
*/

class Magnet : public Fractal
{
private:
    RenderMagnet* myRender;
public:
    Magnet(sf::RenderWindow* Window);
    Magnet(int width, int height);
    ~Magnet();

    void Render();
    void DrawOrbit();
};
