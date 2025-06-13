#include "SelectRect.h"
#include <algorithm>

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
    if(Event.type == sf::Event::MouseButtonPressed)
    {
        if(Event.mouseButton.button == sf::Mouse::Left)
        {
            pos.top = Event.mouseButton.y;
            pos.left = Event.mouseButton.x;

            pos.width = 0;
            pos.height = 0;

            xSelect = Event.mouseButton.x;
            ySelect = Event.mouseButton.y;

            inSelection = true;
        }
    }

    if(Event.type == sf::Event::MouseMoved && inSelection)
    {
        x = Event.mouseMove.x;
        y = Event.mouseMove.y;

        pos.left = std::min(x, xSelect);
        pos.top  = std::min(y, ySelect);
        pos.width  = std::abs(x - xSelect);
        pos.height = std::abs(y - ySelect);
    }

    if(Event.type == sf::Event::MouseButtonReleased)
    {
        if(Event.mouseButton.button == sf::Mouse::Left)
        {
            select = pos;
            inSelection = false;
            if(pos.height != 0 && pos.width != 0)
                return true;
        }
    }
    return false;
}

void SelectRect::ClickEvent(wxMouseEvent& event)
{
    if(event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        pos.top = event.GetPosition().y;
        pos.left = event.GetPosition().x;

        pos.width = 0;
        pos.height = 0;

        xSelect = event.GetPosition().x;
        ySelect = event.GetPosition().y;

        inSelection = true;
    }
}

bool SelectRect::UnClickEvent(wxMouseEvent& event)
{
    if(event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        select = pos;
        inSelection = false;
        if(pos.height != 0 && pos.width != 0)
            return true;
    }
    return false;
}

void SelectRect::MoveEvent(wxMouseEvent& event)
{
    if(inSelection)
    {
        x = event.GetPosition().x;
        y = event.GetPosition().y;

        pos.left = std::min(x, xSelect);
        pos.top  = std::min(y, ySelect);
        pos.width  = std::abs(x - xSelect);
        pos.height = std::abs(y - ySelect);
    }
}

sf::IntRect SelectRect::GetSeleccion()
{
    return select;
}
