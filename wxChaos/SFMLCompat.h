#pragma once
#include <SFML/Graphics.hpp>

// Compatibility layer for old SFML 1.6 API used by wxChaos
#define SetImage setTexture
#define SetSubRect setTextureRect
#define SetPosition setPosition
#define SetColor setColor
#define Resize(width, height) setScale(float(width)/getTexture()->getSize().x, float(height)/getTexture()->getSize().y)
#define GetWidth() getSize().x
#define GetHeight() getSize().y
#define Create create
#define LoadFromFile loadFromFile
#define Display display
#define Clear clear
#define Close close
#define SetFramerateLimit setFramerateLimit
#define SetIcon setIcon
#define SetView setView
#define GetEvent pollEvent
#define IsOpened isOpen
#define ConvertCoords mapPixelToCoords
#define Left left
#define Top top
#define Right width
#define Bottom height
#undef GetWidth
#undef GetHeight
#define GetWidth() (width - left)
#define GetHeight() (height - top)
