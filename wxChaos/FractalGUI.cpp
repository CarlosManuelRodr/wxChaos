#include "FractalClasses.h"
#include <sstream>
#include <cmath>

// SelectRect
SelectRect::SelectRect(sf::RenderWindow* Window)
{
    x = y = -1;
    xSelect = ySelect = -1;

    inSelection = false;

    pos.Top = 0;
    pos.Left = 0;
    pos.Bottom = 0;
    pos.Right = 0;

    color = sf::Color(0, 0, 255, 100);
    texture.create(500, 500, color);
    output.SetImage(texture);
}
void SelectRect::Show(sf::RenderWindow* Window)
{
    if(inSelection)
    {
        if(pos.getSize().x != 0 && pos.getSize().y != 0)
        {
            if(pos.Right >= pos.Left && pos.Bottom >= pos.Top)
            {
                output.Resize(static_cast<float>(pos.getSize().x), static_cast<float>(pos.getSize().y));
                // Draw horizontal borders.
                for(unsigned int x=0; x<texture.getSize().x; x++)
                {
                    texture.setPixel(x, 0, sf::Color(0,0,128));

                    texture.setPixel(x, texture.getSize().y-1, sf::Color(0,0,128));
                }
                // Draw vertical borders.
                for(unsigned int y=0; y<texture.getSize().y; y++)
                {
                    texture.setPixel(0, y, sf::Color(0,0,128));
                    texture.setPixel(texture.getSize().x-1, y, sf::Color(0,0,128));
                }

                output.SetPosition(static_cast<float>(pos.Left), static_cast<float>(pos.Top));
                Window->draw(output);
            }
        }
    }
}
bool SelectRect::HandleEvents(sf::Event Event)
{
    // Clicking event.
    if(Event.type == sf::Event::MouseButtonPressed)
    {
        if(Event.mouseButton.button == sf::Mouse::Left)
        {
            pos.Top = Event.mouseButton.y;
            pos.Left = Event.mouseButton.x;

            pos.Right = Event.mouseButton.x;
            pos.Bottom = Event.mouseButton.y;

            xSelect = Event.mouseButton.x;
            ySelect = Event.mouseButton.y;

            inSelection = true;
        }
    }

    // Mouse movement event.
    if(Event.type == sf::Event::MouseMoved && inSelection)
    {
        x = Event.mouseMove.x;
        y = Event.mouseMove.y;

        if(x >= xSelect && y >= ySelect)
        {
            // Fourth quadrant.
            pos.Left = xSelect;
            pos.Top = ySelect;
            pos.Right = x;
            pos.Bottom = y;
        }
        else if(x >= xSelect && y < ySelect)
        {
            // First quadrant.
            pos.Left = xSelect;
            pos.Top = y;
            pos.Right = x;
            pos.Bottom = ySelect;

        }
        else if(x < xSelect && y >= ySelect)
        {
            // Third quadrant.
            pos.Left = x;
            pos.Top = ySelect;
            pos.Right = xSelect;
            pos.Bottom = y;
        }
        else
        {
            // Second quadrant.
            pos.Left = x;
            pos.Top = y;
            pos.Right = xSelect;
            pos.Bottom = ySelect;
        }
    }

    // Rectangle selected.
    if(Event.type == sf::Event::MouseButtonReleased)
    {
        if(Event.mouseButton.button == sf::Mouse::Left)
        {
            select = pos;
            inSelection = false;
            if(pos.getSize().y != 0 && pos.getSize().x != 0)
            {
                return true;
            }
        }
    }
    return false;
}
void SelectRect::ClickEvent(wxMouseEvent& event)
{
    if(event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        pos.Top = event.GetPosition().y;
        pos.Left = event.GetPosition().x;

        pos.Right = event.GetPosition().x;
        pos.Bottom = event.GetPosition().y;

        xSelect = event.GetPosition().x;
        ySelect = event.GetPosition().y;

        inSelection = true;
    }
}
bool SelectRect::UnClickEvent(wxMouseEvent& event)
{
    if(event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        select = pos;
        inSelection = false;
        if(pos.getSize().y != 0 && pos.getSize().x != 0)
            return true;
    }
    return false;
}
void SelectRect::MoveEvent(wxMouseEvent& event)
{
    if(inSelection)
    {
        x = event.GetPosition().x;
        y = event.GetPosition().y;

        if(x >= xSelect && y >= ySelect)
        {
            // Fourth quadrant.
            pos.Left = xSelect;
            pos.Top = ySelect;
            pos.Right = x;
            pos.Bottom = y;
        }
        else if(x >= xSelect && y < ySelect)
        {
            // First quadrant.
            pos.Left = xSelect;
            pos.Top = y;
            pos.Right = x;
            pos.Bottom = ySelect;

        }
        else if(x < xSelect && y >= ySelect)
        {
            // Third quadrant.
            pos.Left = x;
            pos.Top = ySelect;
            pos.Right = xSelect;
            pos.Bottom = y;
        }
        else
        {
            // Second quadrant.
            pos.Left = x;
            pos.Top = y;
            pos.Right = xSelect;
            pos.Bottom = ySelect;
        }
    }
}
sf::Rect<int> SelectRect::GetSeleccion()
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
    texture.create(screenWidth, screenHeight, sf::Color(255, 255, 255, 255));
    output.SetImage(texture);

    this->Render();
    rendered = true;
    inSelection = false;
}
void ScreenPointer::Show(sf::RenderWindow* Window)
{
    if(!rendered) this->Render();
    Window->draw(output);
}
void ScreenPointer::Render()
{
    texture.create(screenWidth, screenHeight, sf::Color(255, 255, 255, 0));
    // Draw horizontal line.
    for(unsigned int i=0; i< screenWidth; i++)
        texture.setPixel(i, y, color);

    // Draw vertical line.
    for(unsigned int j=0; j< screenHeight; j++)
        texture.setPixel(x, j, color);

    rendered = true;
}
bool ScreenPointer::HandleEvents(sf::Event Event)
{
    if(Event.type == sf::Event::MouseButtonPressed)
    {
        if(Event.mouseButton.button == sf::Mouse::Left)
        {
            y = Event.mouseButton.y;
            x = Event.mouseButton.x;
            rendered = false;
            inSelection = true;
        }
    }

    // Movement while on click.
    if(Event.type == sf::Event::MouseMoved && inSelection)
    {
        x = Event.mouseMove.x;
        y = Event.mouseMove.y;
        rendered = false;
        return true;
    }

    if(Event.type == sf::Event::MouseButtonReleased)
    {
        if(Event.mouseButton.button == sf::Mouse::Left)
        {
            inSelection = false;
            return true;
        }
    }
    return false;
}
bool ScreenPointer::ClickEvent(wxMouseEvent& event)
{
    if(event.ButtonDown(wxMOUSE_BTN_LEFT))
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
    if(event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        inSelection = false;
    }
}
bool ScreenPointer::MoveEvent(wxMouseEvent& event)
{
    if(inSelection)
    {
        if(x != event.GetPosition().x || y != event.GetPosition().y)
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
    sf::Rect<int> Size;
    Size.Right = screenWidth;
    Size.Bottom = screenHeight;
    output.SetSubRect(Size);
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

    texture.loadFromFile(Path.c_str());
    output.SetImage(texture);
    output.SetPosition(static_cast<float>(posX), static_cast<float>(posY));
    output.SetColor(sf::Color(255, 255, 255, 170));
    area.Top = posY;
    area.Left = posX;
    area.Right = texture.getSize().x + (unsigned)posX;
    area.Bottom = texture.getSize().y + (unsigned)posY;
    pressed = false;
    anchorage = false;

    screenWidth = Window->getSize().x;
    screenHeight = Window->getSize().y;

    FY = area.Top/screenHeight;
    FX = area.Left/screenWidth;
    anchorType = 0;
}
Button::Button(int posX, int posY, sf::RenderWindow* Window, string text)
{
    texture.loadFromFile("Resources/Button.tga");
    font.loadFromFile("DiavloFont.otf");
    buttonText.SetFont(font);
    buttonText.SetSize(15);
    buttonText.SetPosition(static_cast<float>(posX+10), static_cast<float>(posY+1));
    buttonText.SetText(text);
    output.SetImage(texture);
    output.SetPosition(static_cast<float>(posX), static_cast<float>(posY));
    output.SetColor(sf::Color(255, 255, 255, 170));
    area.Top = posY;
    area.Left = posX;
    pressed = false;
    anchorage = false;
    thereIsText = true;

    buttonWidth = 9*text.size();
    buttonHeight = texture.getSize().y-8;
    output.Resize(static_cast<float>(buttonWidth), static_cast<float>(buttonHeight));
    area.Right = buttonWidth + posX;
    area.Bottom = buttonHeight + posY;

    screenWidth = Window->getSize().x;
    screenHeight = Window->getSize().y;

    FY = area.Top/screenHeight;
    FX = area.Left/screenWidth;
    anchorType = 0;
}
void Button::Show(sf::RenderWindow *Window)
{
    Window->draw(output);
    if(thereIsText) Window->draw(buttonText);
}
bool Button::HandleEvents(sf::Event Event)
{
    if(Event.type == sf::Event::MouseButtonPressed)
    {
        if(Event.mouseButton.button == sf::Mouse::Left)
        {
            if(Event.mouseButton.x >= area.Left && Event.mouseButton.x <= area.Right)
            {
                if(Event.mouseButton.y >= area.Top && Event.mouseButton.y <= area.Bottom)
                {
                    pressed = !pressed;

                    if(pressed)
                        output.SetColor(sf::Color(0, 255, 0, 100));
                    else
                        output.SetColor(sf::Color(255, 255, 255, 100));

                    return true;
                }
            }
        }
    }
    return false;
}
void Button::Resize(sf::RenderWindow* Window)
{
    if(!anchorage)
    {
        area.Top = FY*Window->getSize().y;
        area.Bottom = area.Top + texture.getSize().y;
        area.Left = FX*Window->getSize().x;
        area.Right = area.Left + texture.getSize().x + FX*texture.getSize().x;
        output.SetPosition(static_cast<float>(area.Left), static_cast<float>(area.Top));
        if(thereIsText) buttonText.SetPosition(static_cast<float>(area.Left+10), static_cast<float>(area.Top+1));
    }
    else
    {
        // TODO: If necessary define more anchor types.
        if(anchorType == 2)
        {
            area.Top = Window->getSize().y - texture.getSize().y;
            area.Bottom = Window->getSize().y;
            output.SetPosition(0.0, static_cast<float>(area.Top));
            if(thereIsText) buttonText.SetPosition(10.0, static_cast<float>(area.Top+1));
        }
        if(anchorType == 3)
        {
            area.Right = Window->getSize().x;
            area.Left = area.Right - buttonWidth;
            area.Top = Window->getSize().y - (unsigned)buttonHeight;
            area.Bottom = Window->getSize().y;
            output.SetPosition(static_cast<float>(area.Left), static_cast<float>(area.Top));
            if(thereIsText) buttonText.SetPosition(static_cast<float>(area.Left+10), static_cast<float>(area.Top+1));
        }
    }
}
void Button::SetAnchorage(bool Top, bool Left, bool Bottom, bool Right)
{
    if(Top && Left) anchorType = 1;
    if(Bottom && Left) anchorType = 2;
    if(Bottom && Right) anchorType = 3;
    if(Top && Right) anchorType = 4;
    if(anchorType != 0) anchorage = true;
}
void Button::ChangeState()
{
    pressed = !pressed;

    if(pressed)
        output.SetColor(sf::Color(0, 255, 0, 100));
    else
        output.SetColor(sf::Color(255, 255, 255, 100));
}

// ButtonChange
ButtonChange::ButtonChange(string Path1, string Path2, int posX, int posY, sf::RenderWindow* Window) : Button(Path1, posX, posY, Window)
{
    texture2.loadFromFile(Path2.c_str());
}
bool ButtonChange::HandleEvents(sf::Event Event)
{
    if(Event.type == sf::Event::MouseButtonPressed)
    {
        if(Event.mouseButton.button == sf::Mouse::Left)
        {
            if(Event.mouseButton.x >= area.Left && Event.mouseButton.x <= area.Right)
            {
                if(Event.mouseButton.y >= area.Top && Event.mouseButton.y <= area.Bottom)
                {
                    pressed = !pressed;

                    if(pressed)
                        output.SetImage(texture2);
                    else
                        output.SetImage(texture);

                    return true;
                }
            }
        }
    }
    return false;
}
bool ButtonChange::ClickEvent(wxMouseEvent& event)
{
    if(event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        if(event.GetPosition().x >= area.Left && event.GetPosition().x <= area.Right)
        {
            if(event.GetPosition().y >= area.Top && event.GetPosition().y <= area.Bottom)
            {
                pressed = !pressed;

                if(pressed)
                    output.SetImage(texture2);
                else
                    output.SetImage(texture);

                return true;
            }
        }
    }
    return false;
}
void ButtonChange::Reset()
{
    pressed = false;
    output.SetImage(texture);
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