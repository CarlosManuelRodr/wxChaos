#include "FractalCanvas.h"
#include "StringFuncs.h"
#include "SizeDialogSave.h"
#include "Filesystem.h"
#include "global.h"

FractalCanvas* fractalCanvasPtr = nullptr;

// Fractal Canvas
FractalCanvas::FractalCanvas(MainWindowStatus status, PauseContinueButton* pcb, FractalType fractType, wxWindow* Parent, 
                             wxWindowID Id, const wxPoint& Position, const wxSize& Size, long Style)
    : wxSFMLCanvas(Parent, Id, Position, Size, Style)
{
    fractalCanvasPtr = this;

    statusData = status;
    btn = pcb;
    type = fractType;

    // Status variables.
    wSize = Size;
    juliaMode = false;
    kReal = 0;
    kImaginary = 0;
    pointerChange = false;
    keybGuide = false;
    keybGuideMode = false;
    helpImageMode = false;
    orbitMode = false;
    sliderMode = false;
    onUpdate = false;

    // UserFormula
    userFormula.bailout = 2;
    userFormula.julia = false;
    userFormula.userFormula = wxT("z = z^2 + c");
    userFormula.type = FormulaType::Complex;

    // Create fractal.
    fractalHandler.CreateFractal(fractType, this);
    target = fractalHandler.GetFractalPtr();

    fractalHandler.SetFormula(userFormula);
    target->SetOnWxCtrl(true);

    // Initializes GUI elements.
    selection = new SelectRect(this);

    play = new ButtonChange(GetAbsPath({ "Resources", "Play.tga" }), GetAbsPath({ "Resources", "Stop.tga" }), 0, 500, this);
    play->SetAnchorage(false, true, true, false);
    play->Resize(this);

    screenPointer = new ScreenPointer(this);
    keybImage.loadFromFile(GetAbsPath({ "Resources", "keyboard.png" }));
    mouseImage.loadFromFile(GetAbsPath({ "Resources", "mouse.png" }));
    helpImage.loadFromFile(GetAbsPath({ "Resources","HelpImage.png" }));
    outKeyb.SetImage(keybImage);
    outMouse.SetImage(mouseImage);
    outHelp.SetImage(helpImage);
    outKeyb.SetColor(sf::Color(255, 255, 255, 220));
    outMouse.SetColor(sf::Color(255, 255, 255, 220));
    outHelp.SetColor(sf::Color(255, 255, 255, 220));

    this->SetFocus();
    this->setFramerateLimit(31);

    this->Connect(wxEVT_MOTION, wxMouseEventHandler(FractalCanvas::OnMoveMouse));
    this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(FractalCanvas::OnClick));
    this->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(FractalCanvas::OnClick));
    this->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(FractalCanvas::OnUnClick));
    this->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(FractalCanvas::OnUnClick));
    this->Connect(wxEVT_SIZE, wxSizeEventHandler(FractalCanvas::OnResize));
    this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(FractalCanvas::OnKeyDown));
    this->Connect(wxEVT_KEY_UP, wxKeyEventHandler(FractalCanvas::OnKeyUp));
}
FractalCanvas::~FractalCanvas()
{
    // Cleanup.
    fractalHandler.DeleteFractal();
#ifdef __linux__
    juliaHandler.DeleteFractal();
#endif
    delete play;
    delete screenPointer;
}
void FractalCanvas::OnUpdate()
{
    // Handles SFML events.
    while(this->pollEvent(event))
    {
        // Size change event.
        if(event.type == sf::Event::Resized)
        {
            auto pos = sf::Mouse::getPosition(*this);
            this->mapPixelToCoords(pos);

            target->Resize(this);
            play->Resize(this);

            if(screenPointer != nullptr)
                screenPointer->Resize(this);

            if(keybGuide && keybGuideMode)
            {
                if(this->getSize().y > 300 || this->getSize().x > 300)
                {
                    outKeyb.SetPosition(this->getSize().x - 120, this->getSize().y - 80);
                    outMouse.SetPosition(this->getSize().x - 90, 0);
                }
            }

            if(btn->state)
            {
                btn->state = false;
                if(type == FractalType::ScriptFractal)
                    btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt)) + wxT('\t') + wxT("P"));    // Txt: "Abort"
                else
                    btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt)) + wxT('\t') + wxT("P"));    // Txt: "Pause"

                target->DeleteSavedZooms();
            }
        }

        target->HandleEvents(&event);

        // Keyboard event.
        if(event.type == sf::Event::KeyPressed)
        {
            if(!target->IsRendering())
            {
                if(event.key.code == sf::Keyboard::F1)  // Open or close slider.
                {
                    bool modo = !statusData.slider->IsChecked();
                    this->SetSliderMode(modo);
                    statusData.slider->Check(modo);
                }
                if(event.key.code == sf::Keyboard::F2)  // Shows or hides fractal orbit.
                {
                    bool modo = !statusData.showOrbit->IsChecked();
                    this->SetOrbitMode(modo);
                    statusData.showOrbit->Check(modo);
                }
                if(event.key.code == sf::Keyboard::F4)  // Saves image.
                {
                    wxFileDialog * openFileDialog = new wxFileDialog(this, wxT(menuSelectFileTxt), wxT(""),
                                                    wxT("fractal.png"), wxT("PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp"), wxFD_SAVE);
                    wxString fileName;
                    if(openFileDialog->ShowModal() == wxID_OK)
                    {
                        fileName = openFileDialog->GetPath();
                        int ext = openFileDialog->GetFilterIndex();
                        string path = string(fileName.mb_str());

                        SizeDialogSave *diag = new SizeDialogSave(this, path, ext, type, target, this);
                        diag->Show(true);
                    }
                    openFileDialog->Destroy();
                }
            }
            if(event.key.code == sf::Keyboard::F5)  // Redraw canvas.
            {
                target->Redraw();
            }
            if(event.key.code == sf::Keyboard::P)  // Pause shortcut.
            {
                if(btn->state)
                {
                    btn->state = false;
                    if(type == FractalType::ScriptFractal)
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));    // Txt: "Abort"
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));    // Txt: "Pause"
                }
                else
                {
                    btn->state = true;
                    if(type == FractalType::ScriptFractal)
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuRelaunchTxt))+ wxT('\t') + wxT("P"));    // Txt: "Relaunch script"
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuContinueTxt))+ wxT('\t') + wxT("P"));    // Txt: "Continue"
                }
                target->PauseContinue();
            }
        }
    }

    // This is here because the binding between SFML and wxWidgets makes SFML to handle incorrectly a resolution change.
    sf::View View(sf::FloatRect(0, 0, wSize.GetX(), wSize.GetY()));
    this->setView(View);

    // Clears the screen and draw GUI elements and fractal.
    this->clear();

    if(orbitMode)
        target->SetOrbitPoint(kReal, kImaginary);
    if(sliderMode && pointerChange)
        target->SetK(kReal, kImaginary);

    target->Move();
    target->Show(this);

    // Avoid drawing GUI elements if the fractal is rendering.
    if(!target->IsRendering())
    {
        selection->Show(this);
        if(type != FractalType::Logistic && type != FractalType::HenonMap)
            play->Show(this);

        if(btn->pauseContinue->IsEnabled() && !target->IsPaused())
            btn->pauseContinue->Enable(false);

        if(btn->state && !target->IsPaused())
        {
            btn->state = false;
            if(type == FractalType::ScriptFractal)
                btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));    // Txt: "Abort"
            else
                btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));    // Txt: "Pause"
        }

        if(keybGuide && keybGuideMode)
        {
            this->draw(outKeyb);
            this->draw(outMouse);
        }
        if(helpImageMode)
        {
            this->draw(outHelp);
            if(!keybGuide)
                this->draw(outKeyb);
        }

        if(juliaMode || orbitMode || sliderMode) screenPointer->Show(this);

#ifdef __linux__
        if(juliaMode)
        {
            if(pointerChange)
            {
                juliaImage = juliaHandler.GetTarget()->GetRenderedImage();
                pointerChange = false;
            }
            this->draw(outJulia);
        }
#endif
    }
    else if(!btn->pauseContinue->IsEnabled() && type != FractalType::SierpinskyTriangle) // Triangle and Logistic don't use threads so they cannot be paused.
    {
        btn->pauseContinue->Enable(true);
    }
}
void FractalCanvas::SetWxSize(wxSize size)
{
    wSize = size;

    // Adjust position of the keyboard guide.
    if(keybGuideMode)
    {
        if(this->getSize().y > 300 || this->getSize().x > 300)
        {
            outKeyb.SetPosition(this->getSize().x - 120, this->getSize().y - 80);
            outMouse.SetPosition(this->getSize().x - 90, 0);
            keybGuide = true;
        }
        else
            keybGuide = false;
    }
    if(helpImageMode)
    {
        outKeyb.SetPosition(this->getSize().x - 120, this->getSize().y - 80);
        outHelp.SetPosition((this->getSize().x-helpImage.getSize().x)/2, (this->getSize().y-helpImage.getSize().y)/2);
    }
}
void FractalCanvas::SetJuliaMode(bool mode)
{
    // If Julia mode is activated creates screen pointer.
    if(mode)
    {
        juliaMode = true;
        if(screenPointer == nullptr)
            screenPointer = new ScreenPointer(this);
#ifdef __linux__
        FRACTAL_TYPE juliaType;
        switch(type)
        {
        case Mandelbrot:
            juliaType = Julia;
            break;
        case MandelbrotZN:
            juliaType = JuliaZN;
            break;
        case Manowar:
            juliaType = ManowarJulia;
            break;
        case BurningShip:
            juliaType = BurningShipJulia;
            break;
        default:
            juliaType = Julia;
            break;
        };

        juliaHandler.CreateFractal(juliaType, this->getSize().x/3, this->getSize().y/3);
        juliaHandler.GetTarget()->SetOptions(target->GetOptions(), true);
        juliaHandler.GetTarget()->SetK(kReal, kImaginary);
        juliaImage.create(this->getSize().x/3, this->getSize().y/3);
        outJulia.SetImage(juliaImage);
        outJulia.SetPosition(this->getSize().x-this->getSize().x/3, this->getSize().y-this->getSize().y/3);
        pointerChange = true;

#endif
    }
    // If deactivated, deletes it.
    else
    {
        juliaMode = false;
        if(screenPointer != nullptr && !orbitMode && !sliderMode)
        {
            delete screenPointer;
            screenPointer = nullptr;
        }
#ifdef __linux__
        juliaHandler.DeleteFractal();
#endif
    }
}
double FractalCanvas::GetKReal()
{
    if(!onUpdate)
        return kReal;
    else
        return prevKReal;
}
double FractalCanvas::GetKImaginary()
{
    if(!onUpdate)
        return kImaginary;
    else
        return prevKImag;
}
bool FractalCanvas::ChangeInPointer()
{
    if(pointerChange)
    {
        pointerChange = false;
        return true;
    }
    else
        return false;
}
Fractal* FractalCanvas::GetFractalPtr()
{
    return target;
}
FractalType FractalCanvas::GetFractalType()
{
    return type;
}
void FractalCanvas::ChangeType(FractalType _type)
{
    // Deletes old fractal and creates a new one.
    fractalHandler.CreateFractal(_type, this);
    type = _type;
    target = fractalHandler.GetFractalPtr();
    fractalHandler.SetFormula(userFormula);
    target->SetOnWxCtrl(true);

    // Deletes screen pointer if active.
    if(orbitMode || sliderMode)
    {
        orbitMode = false;
        sliderMode = false;
        if(!juliaMode)
        {
            if(screenPointer != nullptr)
                delete screenPointer;

            screenPointer = nullptr;
        }
    }

    play->Reset();

}
void FractalCanvas::ChangeToScript(ScriptData _scriptData)
{
    // Deletes old fractal and creates a new one.
    type = FractalType::ScriptFractal;
    scriptData = _scriptData;
    fractalHandler.CreateScriptFractal(this, scriptData);
    target = fractalHandler.GetFractalPtr();
    target->SetOnWxCtrl(true);

    // Deletes screen pointer if active.
    if(orbitMode || sliderMode)
    {
        orbitMode = false;
        sliderMode = false;
        if(!juliaMode)
        {
            if(screenPointer != nullptr)
                delete screenPointer;

            screenPointer = nullptr;
        }
    }

    play->Reset();
}
void FractalCanvas::SetKeybGuide(bool mode)
{
    keybGuideMode = mode;
    if(keybGuideMode)
    {
        // Adjust position of the keyboard guide.
        if(this->getSize().y > 300 || this->getSize().x > 300)
        {
            outKeyb.SetPosition(this->getSize().x - keybImage.getSize().x, this->getSize().y - keybImage.getSize().y);
            outMouse.SetPosition(this->getSize().x - mouseImage.getSize().x, 0);
            keybGuide = true;
        }
        else
            keybGuide = false;
    }
    else
        keybGuide = false;
}
void FractalCanvas::ShowHelpImage()
{
    outKeyb.SetPosition(this->getSize().x - keybImage.getSize().x, this->getSize().y - keybImage.getSize().y);
    outHelp.SetPosition((this->getSize().x - helpImage.getSize().x)/2, (this->getSize().y - helpImage.getSize().y)/2);
    helpImageMode = true;
}
void FractalCanvas::Reset()
{
    // Deletes old fractal and creates a new one.
    if(target->IsRendering())
        target->StopRender();

    if(type == FractalType::ScriptFractal)
        fractalHandler.CreateScriptFractal(this, scriptData);
    else
        fractalHandler.CreateFractal(type, this);

    target = fractalHandler.GetFractalPtr();
    fractalHandler.SetFormula(userFormula);
    target->SetOnWxCtrl(true);
    play->Reset();

    // Deactivates screen pointer.
    juliaMode = false;
    orbitMode = false;
    sliderMode = false;
    if(screenPointer != nullptr)
    {
        delete screenPointer;
        screenPointer = nullptr;
    }
}
void FractalCanvas::SetOrbitMode(bool mode)
{
    orbitMode = mode;
    if(orbitMode)
    {
        target->SetOrbitMode(true);
        if(screenPointer == nullptr)
            screenPointer = new ScreenPointer(this);
    }
    else
    {
        target->SetOrbitMode(false);
        if(screenPointer != nullptr && !juliaMode && !sliderMode)
        {
            delete screenPointer;
            screenPointer = nullptr;
        }
    }
}
void FractalCanvas::SetSliderMode(bool mode)
{
    sliderMode = mode;
    if(sliderMode)
    {
        if(screenPointer == nullptr)
            screenPointer = new ScreenPointer(this);
        target->SetJuliaMode(true);
    }
    else
    {
        if(screenPointer != nullptr && !juliaMode && !orbitMode)
        {
            delete screenPointer;
            screenPointer = nullptr;
        }
        target->SetJuliaMode(false);
    }
}
void FractalCanvas::SetUserFormula(FormulaOpt _userFormula)
{
    userFormula = _userFormula;
}
FormulaOpt FractalCanvas::GetFormula()
{
    return userFormula;
}
void FractalCanvas::OnResize(wxSizeEvent& event)
{
    wSize = event.GetSize();

    // Adjust position of the keyboard guide.
    if(keybGuideMode)
    {
        if(this->getSize().y > 300 || this->getSize().x > 300)
        {
            outKeyb.SetPosition(this->getSize().x - 120, this->getSize().y - 80);
            outMouse.SetPosition(this->getSize().x - 90, 0);
            keybGuide = true;
        }
        else
            keybGuide = false;
    }
    if(helpImageMode)
    {
        outKeyb.SetPosition(this->getSize().x - 120, this->getSize().y - 80);
        outHelp.SetPosition((this->getSize().x - helpImage.getSize().x)/2, (this->getSize().y - helpImage.getSize().y)/2);
    }

#ifdef __linux__
    if(juliaMode)
    {
        outJulia.SetPosition(wSize.getSize().x-juliaImage.getSize().x, wSize.getSize().y-juliaImage.getSize().y);
        pointerChange = true;
    }
#endif
}

void FractalCanvas::OnClick(wxMouseEvent& event)
{
    // Pointer event.
    if(juliaMode || orbitMode || sliderMode)
    {
        if(screenPointer->ClickEvent(event))
        {
            prevKReal = kReal;
            prevKImag = kImaginary;

            onUpdate = true;
            kReal = screenPointer->GetX(target);
            kImaginary = screenPointer->GetY(target);
            pointerChange = true;

            if(orbitMode)
                target->SetOrbitChange();

            onUpdate = false;

#ifdef __linux__
            if(juliaMode) juliaHandler.GetTarget()->SetK(kReal, kImaginary);
#endif
        }
    }
    // Selection event.
    else if(!target->IsRendering() && !target->IsMoving())
        selection->ClickEvent(event);

    // Play button event.
    if(type != FractalType::Logistic && type != FractalType::HenonMap)
    {
        if(play->ClickEvent(event))
            target->ChangeVarGradient();
    }

    // Mouse event.
    if(helpImageMode)
    {
        if(event.ButtonDown(wxMOUSE_BTN_LEFT))
            helpImageMode = false;
    }

    if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        target->ZoomBack();
        if(btn->state && !target->IsPaused())
        {
            btn->state = false;
            if(type == FractalType::ScriptFractal)
                btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));    // Txt: "Abort"
            else
                btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));    // Txt: "Pause"
        }
    }
}
void FractalCanvas::OnUnClick(wxMouseEvent& event)
{
    // Selection event.
    if(juliaMode || orbitMode || sliderMode)
        screenPointer->UnClickEvent(event);
    else
    {
        if(!target->IsRendering() && !target->IsMoving())
        {
            if(selection->UnClickEvent(event))
            {
                if(btn->state)
                {
                    btn->state = false;
                    if(type == FractalType::ScriptFractal)
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt)) + wxT('\t') + wxT("P"));    // Txt: "Abort"
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt)) + wxT('\t') + wxT("P"));    // Txt: "Pause"
                    target->DeleteSavedZooms();
                }
                target->SetAreaOfView(selection->GetSeleccion());
            }
        }
    }
}
void FractalCanvas::OnMoveMouse(wxMouseEvent &event)
{
    // Selection event.
    if(juliaMode || orbitMode || sliderMode)
    {
        if(screenPointer->MoveEvent(event))
        {
            prevKReal = kReal;
            prevKImag = kImaginary;

            onUpdate = true;
            kReal = screenPointer->GetX(target);
            kImaginary = screenPointer->GetY(target);
            pointerChange = true;

            if(orbitMode)
                target->SetOrbitChange();

            onUpdate = false;

#ifdef __linux__
            if(juliaMode)
                juliaHandler.GetTarget()->SetK(kReal, kImaginary);
#endif
        }
    }
    else
    {
        if(!target->IsRendering() && !target->IsMoving())
            selection->MoveEvent(event);
    }

    // Updates status bar of the MainFrame when the mouse is moved over the fractal canvas.
    wxString text;
    if(type == FractalType::Logistic)
    {
        text = wxT("a: ");
        text += num_to_string(target->GetX(event.GetPosition().x));
        text += wxT("   x: ");
        text += num_to_string(target->GetY(event.GetPosition().y));
    }
    else if(type == FractalType::DoublePendulum)
    {
        text = wxT("θ2: ");
        text += num_to_string(target->GetX(event.GetPosition().x));
        text += wxT("   θ1: ");
        text += num_to_string(target->GetY(event.GetPosition().y));
    }
    else if(type == FractalType::SierpinskyTriangle || type == FractalType::HenonMap || type == FractalType::ScriptFractal)
    {
        text = wxT("x: ");
        text += num_to_string(target->GetX(event.GetPosition().x));
        text += wxT("   y: ");
        text += num_to_string(target->GetY(event.GetPosition().y));
    }
    else
    {
        text = wxT(realLabelTxt);    // Txt: "Real: "
        text += num_to_string(target->GetX(event.GetPosition().x));
        text += wxT(imagLabelTxt);    // Txt: "   Imaginary: "
        text += num_to_string(target->GetY(event.GetPosition().y));
    }
    statusData.status->SetStatusText(text);
}
void FractalCanvas::OnKeyDown(wxKeyEvent& event)
{
    switch(event.GetKeyCode())
    {
    case WXK_UP:
        {
            target->SetMovement(Direction::Up);
            break;
        }
    case WXK_DOWN:
        {
            target->SetMovement(Direction::Down);
            break;
        }
    case WXK_LEFT:
        {
            target->SetMovement(Direction::Left);
            break;
        }
    case WXK_RIGHT:
        {
            target->SetMovement(Direction::Right);
            break;
        }
    default: break;
    }

    wxChar key = event.GetUnicodeKey();

    if(key == wxT('W'))
        target->SetMovement(Direction::Up);
    else if(key == wxT('S'))
        target->SetMovement(Direction::Down);
    else if(key == wxT('A'))
        target->SetMovement(Direction::Left);
    else if(key == wxT('D'))
        target->SetMovement(Direction::Right);
}
void FractalCanvas::OnKeyUp(wxKeyEvent& event)
{
    switch(event.GetKeyCode())
    {
    case WXK_UP:
        {
            target->ReleaseMovement(Direction::Up);
            break;
        }
    case WXK_DOWN:
        {
            target->ReleaseMovement(Direction::Down);
            break;
        }
    case WXK_LEFT:
        {
            target->ReleaseMovement(Direction::Left);
            break;
        }
    case WXK_RIGHT:
        {
            target->ReleaseMovement(Direction::Right);
            break;
        }
    default: break;
    }

    wxChar key = event.GetUnicodeKey();
    if(key == wxT('W'))
        target->ReleaseMovement(Direction::Up);
    else if(key == wxT('S'))
        target->ReleaseMovement(Direction::Down);
    else if(key == wxT('A'))
        target->ReleaseMovement(Direction::Left);
    else if(key == wxT('D'))
        target->ReleaseMovement(Direction::Right);
}