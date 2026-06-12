#pragma once
#include "../Fractal.h"
#include "../renderers/CellRenderer.h"

/*
* @class Cell
* @brief Handles the RenderCell threads.
*/
class Cell : public Fractal
{
private:
    CellRenderer* myRender;
    double bailout;
public:
    Cell(unsigned int width, unsigned int height);
    ~Cell();
    wxString GetName() const override { return wxT("Cell"); }

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
