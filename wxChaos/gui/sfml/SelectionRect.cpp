#include <algorithm>
#include "SelectionRect.h"

SelectionRect::SelectionRect()
{
    _x = -1;
    _y = -1;
    _xSelect = -1;
    _ySelect = -1;
    _inSelection = false;
    _pos = sf::IntRect(0,0,0,0);
    _color = sf::Color(0,0,255,100);
    _output.setFillColor(_color);
    _output.setOutlineColor(sf::Color(0,0,128));
    _output.setOutlineThickness(1.f);
}

void SelectionRect::Show(sf::RenderWindow* window)
{
    if (_inSelection && _pos.width != 0 && _pos.height != 0)
    {
        _output.setSize(sf::Vector2f(static_cast<float>(_pos.width), static_cast<float>(_pos.height)));
        _output.setPosition(sf::Vector2f(static_cast<float>(_pos.left), static_cast<float>(_pos.top)));
        window->draw(_output);
    }
}

bool SelectionRect::HandleEvents(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            _pos.top = event.mouseButton.y;
            _pos.left = event.mouseButton.x;

            _pos.width = 0;
            _pos.height = 0;

            _xSelect = event.mouseButton.x;
            _ySelect = event.mouseButton.y;

            _inSelection = true;
        }
    }

    if (event.type == sf::Event::MouseMoved && _inSelection)
    {
        _x = event.mouseMove.x;
        _y = event.mouseMove.y;

        _pos.left = std::min(_x, _xSelect);
        _pos.top  = std::min(_y, _ySelect);
        _pos.width  = std::abs(_x - _xSelect);
        _pos.height = std::abs(_y - _ySelect);
    }

    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            _select = _pos;
            _inSelection = false;
            if (_pos.height != 0 && _pos.width != 0)
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
        _pos.top = event.GetPosition().y;
        _pos.left = event.GetPosition().x;

        _pos.width = 0;
        _pos.height = 0;

        _xSelect = event.GetPosition().x;
        _ySelect = event.GetPosition().y;

        _inSelection = true;
    }
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool SelectionRect::UnClickEvent(wxMouseEvent& event)
{
    if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        _select = _pos;
        _inSelection = false;
        if (_pos.height != 0 && _pos.width != 0)
            return true;
    }
    return false;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void SelectionRect::MoveEvent(wxMouseEvent& event)
{
    if (_inSelection)
    {
        _x = event.GetPosition().x;
        _y = event.GetPosition().y;

        _pos.left = std::min(_x, _xSelect);
        _pos.top  = std::min(_y, _ySelect);
        _pos.width  = std::abs(_x - _xSelect);
        _pos.height = std::abs(_y - _ySelect);
    }
}

sf::IntRect SelectionRect::GetSelection() const
{
    return _select;
}
