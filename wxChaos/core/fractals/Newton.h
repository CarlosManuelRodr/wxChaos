#pragma once
#include "../Fractal.h"
#include "../renderers/RenderNewton.h"

/*
* @class Newton
* @brief Handles the RenderNewton threads.
*/
class Newton : public Fractal
{
    RenderNewton* myRender;
    double minStep;
public:
    explicit Newton(const sf::RenderWindow* window);
    Newton(int width, int height);
    ~Newton() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
