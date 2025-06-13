#include "ScreenPointer.h"
#include "FractalClasses.h"

ScreenPointer::ScreenPointer(sf::RenderWindow* Window)
{
    screenWidth = Window->getSize().x;
    screenHeight = Window->getSize().y;
    x = Window->getSize().x / 2;
    y = Window->getSize().y / 2;

    color = sf::Color(0, 0, 0);
    textureImage.create(screenWidth, screenHeight, sf::Color(255, 255, 255, 0));
    texture.loadFromImage(textureImage);
    output.setTexture(texture);

    this->Render();
    rendered = true;
    inSelection = false;
}

void ScreenPointer::Show(sf::RenderWindow* Window)
{
    if (!rendered)
        this->Render();
    Window->draw(output);
}

void ScreenPointer::Render()
{
    textureImage.create(screenWidth, screenHeight, sf::Color(255, 255, 255, 0));
    for (unsigned int i = 0; i < screenWidth; i++)
        textureImage.setPixel(i, y, color);
    for (unsigned int j = 0; j < screenHeight; j++)
        textureImage.setPixel(x, j, color);
    texture.loadFromImage(textureImage);
    rendered = true;
}

bool ScreenPointer::HandleEvents(sf::Event Event)
{
    if (Event.type == sf::Event::MouseButtonPressed)
    {
        if (Event.mouseButton.button == sf::Mouse::Left)
        {
            y = Event.mouseButton.y;
            x = Event.mouseButton.x;
            rendered = false;
            inSelection = true;
        }
    }

    if (Event.type == sf::Event::MouseMoved && inSelection)
    {
        x = Event.mouseMove.x;
        y = Event.mouseMove.y;
        rendered = false;
        return true;
    }

    if (Event.type == sf::Event::MouseButtonReleased)
    {
        if (Event.mouseButton.button == sf::Mouse::Left)
        {
            inSelection = false;
            return true;
        }
    }
    return false;
}

bool ScreenPointer::ClickEvent(wxMouseEvent& event)
{
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        y = event.GetPosition().y;
        x = event.GetPosition().x;
        rendered = false;
        inSelection = true;
        return true;
    }
    return false;
}

void ScreenPointer::UnClickEvent(wxMouseEvent& event)
{
    if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        inSelection = false;
    }
}

bool ScreenPointer::MoveEvent(wxMouseEvent& event)
{
    if (inSelection)
    {
        if (x != event.GetPosition().x || y != event.GetPosition().y)
        {
            y = event.GetPosition().y;
            x = event.GetPosition().x;
            rendered = false;
            return true;
        }
    }
    return false;
}

void ScreenPointer::Resize(sf::RenderWindow* Window)
{
    screenWidth = Window->getSize().x;
    screenHeight = Window->getSize().y;
    x = screenWidth / 2;
    y = screenHeight / 2;
    this->Render();
    inSelection = false;

    sf::IntRect Size;
    Size.width = screenWidth;
    Size.height = screenHeight;
    output.setTextureRect(Size);
}

double ScreenPointer::GetX(Fractal* target)
{
    return target->GetX(x);
}

double ScreenPointer::GetY(Fractal* target)
{
    return target->GetY(y);
}

void ScreenPointer::AdjustPosition(Fractal* target, double numX, double numY)
{
    x = target->GetPixelX(numX);
    y = target->GetPixelY(numY);
}
