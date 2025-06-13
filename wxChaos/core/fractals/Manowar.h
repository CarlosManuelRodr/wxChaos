#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class Manowar
* @brief Handles the RenderManowar threads.
*/
class Manowar : public Fractal
{
private:
    RenderManowar* myRender;
public:
    Manowar(sf::RenderWindow* Window);
    Manowar(int width, int height);
    ~Manowar();

    void Render();
    void DrawOrbit();
};
