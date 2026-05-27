#include <algorithm>
#include "SelectionRect.h"

SelectionRect::SelectionRect()
{
    x = -1;
    y = -1;
    xSelect = -1;
    ySelect = -1;
    inSelection = false;
    pos = sf::IntRect(0,0,0,0);
    color = sf::Color(0,0,255,100);
    output.setFillColor(color);
    output.setOutlineColor(sf::Color(0,0,128));
    output.setOutlineThickness(1.f);
}

void SelectionRect::Show(sf::RenderWindow* window)
{
    if (inSelection && pos.width != 0 && pos.height != 0)
    {
        output.setSize(sf::Vector2f(pos.width,pos.height));
        output.setPosition(sf::Vector2f(pos.left,pos.top));
        window->draw(output);
    }
}

bool SelectionRect::HandleEvents(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            pos.top = event.mouseButton.y;
            pos.left = event.mouseButton.x;

            pos.width = 0;
            pos.height = 0;

            xSelect = event.mouseButton.x;
            ySelect = event.mouseButton.y;

            inSelection = true;
        }
    }

    if (event.type == sf::Event::MouseMoved && inSelection)
    {
        x = event.mouseMove.x;
        y = event.mouseMove.y;

        pos.left = std::min(x, xSelect);
        pos.top  = std::min(y, ySelect);
        pos.width  = std::abs(x - xSelect);
        pos.height = std::abs(y - ySelect);
    }

    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            select = pos;
            inSelection = false;
            if (pos.height != 0 && pos.width != 0)
                return true;
        }
    }
    return false;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void SelectionRect::ClickEvent(wxMouseEvent& event)
{
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
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

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool SelectionRect::UnClickEvent(wxMouseEvent& event)
{
    if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        select = pos;
        inSelection = false;
        if (pos.height != 0 && pos.width != 0)
            return true;
    }
    return false;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void SelectionRect::MoveEvent(wxMouseEvent& event)
{
    if (inSelection)
    {
        x = event.GetPosition().x;
        y = event.GetPosition().y;

        pos.left = std::min(x, xSelect);
        pos.top  = std::min(y, ySelect);
        pos.width  = std::abs(x - xSelect);
        pos.height = std::abs(y - ySelect);
    }
}

sf::IntRect SelectionRect::GetSelection() const
{
    return select;
}
