#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class Newton
* @brief Handles the RenderNewton threads.
*/
class Newton : public Fractal
{
private:
    RenderNewton* myRender;
    double minStep;
public:
    Newton(sf::RenderWindow *Window);
    Newton(int width, int height);
    ~Newton();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
