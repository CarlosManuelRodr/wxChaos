#pragma once

#include <SFML/Window/WindowHandle.hpp>

class wxWindow;

namespace Platform
{
    sf::WindowHandle GetNativeWindowHandle(const wxWindow& window);
}
