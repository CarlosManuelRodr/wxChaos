#include "Button.h"
#include <sstream>
#include <cmath>

void Button::Resize(sf::RenderWindow* Window)
{
    if (FX != 0.0)
        area.left = Window->getSize().x - FX;
    if (FY != 0.0)
        area.top = Window->getSize().y - FY;
}

Button::Button(std::string Path, int posX, int posY, sf::RenderWindow* Window)
{
    textureImage.loadFromFile(Path);
    texture.loadFromImage(textureImage);
    output.setTexture(texture);
    output.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    area = output.getGlobalBounds();
    pressed = false;
    thereIsText = false;
}

Button::Button(int posX, int posY, sf::RenderWindow* Window, std::string text)
{
    font.loadFromFile("arial.ttf");
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(14);
    buttonText.setFillColor(sf::Color::White);
    output.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    area = output.getGlobalBounds();
    pressed = false;
    thereIsText = true;
}

void Button::Show(sf::RenderWindow* Window)
{
    Window->draw(output);
    if (thereIsText)
    {
        buttonText.setPosition(output.getPosition());
        Window->draw(buttonText);
    }
}

void Button::SetAnchorage(bool Top, bool Left, bool Bottom, bool Right)
{
    // Implementation placeholder
}

void Button::ChangeState()
{
    pressed = !pressed;
}

bool Button::HandleEvents(sf::Event Event)
{
    return false;
}

bool Button::ClickEvent(wxMouseEvent& event)
{
    return false;
}
