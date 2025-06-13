#include "SelectRect.h"

SelectRect::SelectRect(sf::RenderWindow* Window)
    : x(-1), y(-1), xSelect(-1), ySelect(-1), inSelection(false)
{
    pos = sf::IntRect(0,0,0,0);
    color = sf::Color(0,0,255,100);
    output.setFillColor(color);
    output.setOutlineColor(sf::Color(0,0,128));
    output.setOutlineThickness(1.f);
}

void SelectRect::Show(sf::RenderWindow* Window)
{
    if(inSelection && pos.width != 0 && pos.height != 0)
    {
        output.setSize(sf::Vector2f(pos.width,pos.height));
        output.setPosition(sf::Vector2f(pos.left,pos.top));
        Window->draw(output);
    }
}

bool SelectRect::HandleEvents(sf::Event Event)
{
    return false;
}

void SelectRect::ClickEvent(wxMouseEvent& event){}
bool SelectRect::UnClickEvent(wxMouseEvent& event){return false;}
void SelectRect::MoveEvent(wxMouseEvent& event){}

sf::IntRect SelectRect::GetSeleccion()
{
    return select;
}
