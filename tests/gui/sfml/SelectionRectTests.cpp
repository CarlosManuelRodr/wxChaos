#include <doctest/doctest.h>

#include "sfml/SelectionRect.h"

namespace
{
    sf::Event LeftMousePress(const int x, const int y)
    {
        sf::Event event{};
        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = x;
        event.mouseButton.y = y;
        return event;
    }

    sf::Event MouseMove(const int x, const int y)
    {
        sf::Event event{};
        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = x;
        event.mouseMove.y = y;
        return event;
    }

    sf::Event LeftMouseRelease(const int x, const int y)
    {
        sf::Event event{};
        event.type = sf::Event::MouseButtonReleased;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = x;
        event.mouseButton.y = y;
        return event;
    }
}

TEST_CASE("SelectionRect returns normalized drag rectangle")
{
    SelectionRect selection;

    CHECK_FALSE(selection.HandleEvents(LeftMousePress(100, 80)));
    CHECK_FALSE(selection.HandleEvents(MouseMove(60, 120)));
    CHECK(selection.HandleEvents(LeftMouseRelease(60, 120)));

    const sf::IntRect result = selection.GetSelection();
    CHECK(result.left == 60);
    CHECK(result.top == 80);
    CHECK(result.width == 40);
    CHECK(result.height == 40);
}

TEST_CASE("SelectionRect handles drags toward the upper-left")
{
    SelectionRect selection;

    selection.HandleEvents(LeftMousePress(150, 140));
    selection.HandleEvents(MouseMove(90, 70));
    CHECK(selection.HandleEvents(LeftMouseRelease(90, 70)));

    const sf::IntRect result = selection.GetSelection();
    CHECK(result.left == 90);
    CHECK(result.top == 70);
    CHECK(result.width == 60);
    CHECK(result.height == 70);
}

TEST_CASE("SelectionRect can constrain selections to an aspect ratio")
{
    SelectionRect selection;
    selection.SetAspectRatio(2.0);

    selection.HandleEvents(LeftMousePress(0, 0));
    selection.HandleEvents(MouseMove(100, 100));
    CHECK(selection.HandleEvents(LeftMouseRelease(100, 100)));

    const sf::IntRect result = selection.GetSelection();
    CHECK(result.left == 0);
    CHECK(result.top == 0);
    CHECK(result.width == 100);
    CHECK(result.height == 50);
}

TEST_CASE("SelectionRect preserves aspect ratio while dragging toward the upper-left")
{
    SelectionRect selection;
    selection.SetAspectRatio(2.0);

    selection.HandleEvents(LeftMousePress(100, 100));
    selection.HandleEvents(MouseMove(0, 0));
    CHECK(selection.HandleEvents(LeftMouseRelease(0, 0)));

    const sf::IntRect result = selection.GetSelection();
    CHECK(result.left == 0);
    CHECK(result.top == 50);
    CHECK(result.width == 100);
    CHECK(result.height == 50);
}

TEST_CASE("SelectionRect ignores zero-area selections")
{
    SelectionRect selection;

    selection.HandleEvents(LeftMousePress(30, 40));
    CHECK_FALSE(selection.HandleEvents(LeftMouseRelease(30, 40)));

    const sf::IntRect result = selection.GetSelection();
    CHECK(result.left == 30);
    CHECK(result.top == 40);
    CHECK(result.width == 0);
    CHECK(result.height == 0);
}

TEST_CASE("SelectionRect ignores non-left mouse button presses")
{
    SelectionRect selection;
    sf::Event press = LeftMousePress(10, 20);
    press.mouseButton.button = sf::Mouse::Right;

    CHECK_FALSE(selection.HandleEvents(press));
    CHECK_FALSE(selection.HandleEvents(MouseMove(50, 60)));
    CHECK_FALSE(selection.HandleEvents(LeftMouseRelease(50, 60)));
}

TEST_CASE("SelectionRect uses normalized wx event coordinates without applying another offset")
{
    SelectionRect selection;
    wxMouseEvent press(wxEVT_LEFT_DOWN);
    press.SetPosition(wxPoint(120, 75));
    wxMouseEvent move(wxEVT_MOTION);
    move.SetPosition(wxPoint(180, 115));
    wxMouseEvent release(wxEVT_LEFT_UP);
    release.SetPosition(wxPoint(180, 115));

    selection.ClickEvent(press);
    CHECK(selection.MoveEvent(move));
    CHECK(selection.UnClickEvent(release));

    const sf::IntRect result = selection.GetSelection();
    CHECK(result.left == 120);
    CHECK(result.top == 75);
    CHECK(result.width == 60);
    CHECK(result.height == 40);
}
