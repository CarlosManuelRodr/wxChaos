#include "FractalClasses.h"
#include <sstream>
#include <cmath>

// SelectRect
SelectRect::SelectRect(sf::RenderWindow* Window)
{
    x = y = -1;
    xSelect = ySelect = -1;

    inSelection = false;

    pos.top = 0;
    pos.left = 0;
    pos.height = 0;
    pos.width = 0;

    color = sf::Color(0, 0, 255, 100);
    output.setFillColor(color);
    output.setOutlineColor(sf::Color(0, 0, 128));
    output.setOutlineThickness(1.f);
}
void SelectRect::Show(sf::RenderWindow* Window)
{
    if (inSelection)
    {
        if (pos.width != 0 && pos.height != 0)
        {
            output.setSize(sf::Vector2f(pos.width, pos.height));
            output.setPosition(sf::Vector2f(pos.left, pos.top));
            Window->draw(output);
        }
    }
}
bool SelectRect::HandleEvents(sf::Event Event)
{
    // Clicking event.
    if (Event.type == sf::Event::MouseButtonPressed)
    {
        if (Event.mouseButton.button == sf::Mouse::Left)
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

    // Mouse movement event.
    if (Event.type == sf::Event::MouseMoved && inSelection)
    {
        x = Event.mouseMove.x;
        y = Event.mouseMove.y;

        pos.left = std::min(x, xSelect);
        pos.top = std::min(y, ySelect);
        pos.width = std::abs(x - xSelect);
        pos.height = std::abs(y - ySelect);
    }

    // Rectangle selected.
    if (Event.type == sf::Event::MouseButtonReleased)
    {
        if (Event.mouseButton.button == sf::Mouse::Left)
        {
            select = pos;
            inSelection = false;
            if (pos.height != 0 && pos.width != 0)
            {
                return true;
            }
        }
    }
    return false;
}
void SelectRect::ClickEvent(wxMouseEvent& event)
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
bool SelectRect::UnClickEvent(wxMouseEvent& event)
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
void SelectRect::MoveEvent(wxMouseEvent& event)
{
    if (inSelection)
    {
        x = event.GetPosition().x;
        y = event.GetPosition().y;

        pos.left = std::min(x, xSelect);
        pos.top = std::min(y, ySelect);
        pos.width = std::abs(x - xSelect);
        pos.height = std::abs(y - ySelect);
    }
}
sf::IntRect SelectRect::GetSeleccion()
{
    return select;
}

// ScreenPointer
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
    if (!rendered) this->Render();
    Window->draw(output);
}
void ScreenPointer::Render()
{
    textureImage.create(screenWidth, screenHeight, sf::Color(255, 255, 255, 0));
    // Draw horizontal line.
    for (unsigned int i = 0; i < screenWidth; i++)
        textureImage.setPixel(i, y, color);

    // Draw vertical line.
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

    // Movement while on click.
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
    else return false;
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

    // Change output layer properties.
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

// Button
Button::Button(string Path, int posX, int posY, sf::RenderWindow* Window)
{
    buttonHeight = buttonWidth = -1;
    thereIsText = false;

    textureImage.loadFromFile(Path.c_str());
    texture.loadFromImage(textureImage);
    output.setTexture(texture);
    output.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    output.setColor(sf::Color(255, 255, 255, 170));

    area.top = posY;
    area.left = posX;
    area.width = texture.getSize().x;
    area.height = texture.getSize().y;

    pressed = false;
    anchorage = false;

    screenWidth = Window->getSize().x;
    screenHeight = Window->getSize().y;

    FY = area.top / screenHeight;
    FX = area.left / screenWidth;
    anchorType = 0;
}
Button::Button(int posX, int posY, sf::RenderWindow* Window, string text)
{
    textureImage.loadFromFile("Resources/Button.tga");
    texture.loadFromImage(textureImage);
    font.loadFromFile("DiavloFont.otf");
    buttonText.setFont(font);
    buttonText.setCharacterSize(15);
    buttonText.setPosition(static_cast<float>(posX + 10), static_cast<float>(posY + 1));
    buttonText.setString(text);
    output.setTexture(texture);
    output.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    output.setColor(sf::Color(255, 255, 255, 170));
    area.top = posY;
    area.left = posX;
    pressed = false;
    anchorage = false;
    thereIsText = true;

    buttonWidth = 9 * text.size();
    buttonHeight = texture.getSize().y - 8;
    output.setScale((float)buttonWidth / texture.getSize().x, (float)buttonHeight / texture.getSize().y);
    area.width = buttonWidth;
    area.height = buttonHeight;

    screenWidth = Window->getSize().x;
    screenHeight = Window->getSize().y;

    FY = area.top / screenHeight;
    FX = area.left / screenWidth;
    anchorType = 0;
}
void Button::Show(sf::RenderWindow* Window)
{
    Window->draw(output);
    if (thereIsText) Window->draw(buttonText);
}
bool Button::HandleEvents(sf::Event Event)
{
    if (Event.type == sf::Event::MouseButtonPressed)
    {
        if (Event.mouseButton.button == sf::Mouse::Left)
        {
            if (area.contains(Event.mouseButton.x, Event.mouseButton.y))
            {
                pressed = !pressed;

                if (pressed)
                    output.setColor(sf::Color(0, 255, 0, 100));
                else
                    output.setColor(sf::Color(255, 255, 255, 100));

                return true;
            }
        }
    }
    return false;
}
bool Button::ClickEvent(wxMouseEvent& event)
{
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        if (area.contains(event.GetPosition().x, event.GetPosition().y))
        {
            pressed = !pressed;
            if (pressed)
                output.setColor(sf::Color(0, 255, 0, 100));
            else
                output.setColor(sf::Color(255, 255, 255, 100));

            return true;
        }
    }
    return false;
}

void Button::Resize(sf::RenderWindow* Window)
{
    if (!anchorage)
    {
        area.top = FY * Window->getSize().y;
        area.left = FX * Window->getSize().x;
        area.width = texture.getSize().x * (FX + 1);
        area.height = texture.getSize().y;
        output.setPosition(area.left, area.top);
        if (thereIsText) buttonText.setPosition(area.left + 10, area.top + 1);
    }
    else
    {
        // TODO: If necessary define more anchor types.
        if (anchorType == 2)
        {
            area.top = Window->getSize().y - texture.getSize().y;
            area.height = texture.getSize().y;
            output.setPosition(0.0, area.top);
            if (thereIsText) buttonText.setPosition(10.0f, area.top + 1);
        }
        if (anchorType == 3)
        {
            area.left = Window->getSize().x - buttonWidth;
            area.width = buttonWidth;
            area.top = Window->getSize().y - (unsigned)buttonHeight;
            area.height = buttonHeight;
            output.setPosition(area.left, area.top);
            if (thereIsText) buttonText.setPosition(area.left + 10, area.top + 1);
        }
    }
}
void Button::SetAnchorage(bool Top, bool Left, bool Bottom, bool Right)
{
    if (Top && Left) anchorType = 1;
    if (Bottom && Left) anchorType = 2;
    if (Bottom && Right) anchorType = 3;
    if (Top && Right) anchorType = 4;
    if (anchorType != 0) anchorage = true;
}
void Button::ChangeState()
{
    pressed = !pressed;

    if (pressed)
        output.setColor(sf::Color(0, 255, 0, 100));
    else
        output.setColor(sf::Color(255, 255, 255, 100));
}

// ButtonChange
ButtonChange::ButtonChange(string Path1, string Path2, int posX, int posY, sf::RenderWindow* Window) : Button(Path1, posX, posY, Window)
{
    textureImage2.loadFromFile(Path2.c_str());
    texture2.loadFromImage(textureImage2);
}
bool ButtonChange::HandleEvents(sf::Event Event)
{
    if (Event.type == sf::Event::MouseButtonPressed)
    {
        if (Event.mouseButton.button == sf::Mouse::Left)
        {
            if (area.contains(Event.mouseButton.x, Event.mouseButton.y))
            {
                pressed = !pressed;

                if (pressed)
                    output.setTexture(texture2);
                else
                    output.setTexture(texture);

                return true;
            }
        }
    }
    return false;
}
bool ButtonChange::ClickEvent(wxMouseEvent& event)
{
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        if (area.contains(event.GetPosition().x, event.GetPosition().y))
        {
            pressed = !pressed;

            if (pressed)
                output.setTexture(texture2);
            else
                output.setTexture(texture);

            return true;
        }
    }
    return false;
}
void ButtonChange::Reset()
{
    pressed = false;
    output.setTexture(texture);
}

// PanelOptions
PanelOptions::PanelOptions()
{
    forceShow = false;
}
void PanelOptions::LinkInt(PanelOptionType pType, wxString labelTxt, int* linkInt, wxString defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    linkTo.push_back(LinkTo::ToInt);
    intTarget.push_back(linkInt);
    defaults.push_back(defaultVal);
}
void PanelOptions::LinkDbl(PanelOptionType pType, wxString labelTxt, double* linkDbl, wxString defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    linkTo.push_back(LinkTo::ToDouble);
    dblTarget.push_back(linkDbl);
    defaults.push_back(defaultVal);
}
void PanelOptions::LinkBool(PanelOptionType pType, wxString labelTxt, bool* linkBool, wxString defaultVal)
{
    type.push_back(pType);
    label.push_back(labelTxt);
    linkTo.push_back(LinkTo::ToBool);
    boolTarget.push_back(linkBool);
    defaults.push_back(defaultVal);
}
int PanelOptions::GetElementsSize()
{
    return label.size();
}
LinkTo PanelOptions::GetLinkType(int index)
{
    return linkTo.at(index);
}
wxString PanelOptions::GetLabelElement(int index)
{
    return label.at(index);
}
int* PanelOptions::GetIntElement(int index)
{
    return intTarget.at(index);
}
double* PanelOptions::GetDoubleElement(int index)
{
    return dblTarget.at(index);
}
bool* PanelOptions::GetBoolElement(int index)
{
    return boolTarget.at(index);
}
PanelOptionType PanelOptions::GetPanelOptType(int index)
{
    return type.at(index);
}
wxString PanelOptions::GetDefault(int index)
{
    return defaults.at(index);
}
void PanelOptions::SetForceShow(bool mode)
{
    forceShow = mode;
}
bool PanelOptions::GetForceShow()
{
    return forceShow;
}