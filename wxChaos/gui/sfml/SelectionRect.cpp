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
    _aspectRatio = 0.0;
    _color = sf::Color(0,0,255,100);
    _output.setFillColor(_color);
    _output.setOutlineColor(sf::Color(0,0,128));
    _output.setOutlineThickness(1.f);
}

void SelectionRect::SetAspectRatio(const double aspectRatio)
{
    _aspectRatio = aspectRatio > 0.0 ? aspectRatio : 0.0;
}

void SelectionRect::UpdatePosition(const int x, const int y)
{
    _x = x;
    _y = y;

    int width = std::abs(_x - _xSelect);
    int height = std::abs(_y - _ySelect);

    if (_aspectRatio > 0.0 && width > 0 && height > 0)
    {
        if (static_cast<double>(width) / _aspectRatio >= height)
            width = std::max(1, static_cast<int>(std::round(static_cast<double>(height) * _aspectRatio)));
        else
            height = std::max(1, static_cast<int>(std::round(static_cast<double>(width) / _aspectRatio)));
    }

    const int signedWidth = _x >= _xSelect ? width : -width;
    const int signedHeight = _y >= _ySelect ? height : -height;
    _pos.left = std::min(_xSelect, _xSelect + signedWidth);
    _pos.top = std::min(_ySelect, _ySelect + signedHeight);
    _pos.width = width;
    _pos.height = height;
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
        UpdatePosition(event.mouseMove.x, event.mouseMove.y);
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
bool SelectionRect::MoveEvent(wxMouseEvent& event)
{
    if (!_inSelection)
        return false;

    const sf::IntRect previousPosition = _pos;
    UpdatePosition(event.GetPosition().x, event.GetPosition().y);

    return _pos != previousPosition;
}

sf::IntRect SelectionRect::GetSelection() const
{
    return _select;
}
