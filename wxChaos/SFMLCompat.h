#pragma once
#include <SFML/Graphics.hpp>

#define SetFramerateLimit setFramerateLimit
#define Display display
#define Clear clear
#define Create create

inline unsigned GetWidth(const sf::RenderWindow& win) { return win.getSize().x; }
inline unsigned GetHeight(const sf::RenderWindow& win) { return win.getSize().y; }

