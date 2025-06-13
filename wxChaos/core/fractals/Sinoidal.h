#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class Sinoidal
* @brief Handles the RenderSinoidal threads.
*/
class Sinoidal : public Fractal
{
private:
    RenderSinoidal* myRender;
public:
    Sinoidal(sf::RenderWindow* Window);
    Sinoidal(int width, int height);
    ~Sinoidal();

    void Render();
    void DrawOrbit();
};
