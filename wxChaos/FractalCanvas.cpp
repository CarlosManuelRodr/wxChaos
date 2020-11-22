#include "FractalCanvas.h"
#include "StringFuncs.h"
#include "SizeDialogSave.h"

FractalCanvas* fractalCanvasPtr = NULL;

// Fractal Canvas
FractalCanvas::FractalCanvas(MainWindowStatus status, PauseContinueButton* pcb, FRACTAL_TYPE fractType, wxWindow* Parent, wxWindowID Id, const wxPoint& Position, const wxSize& Size, long Style)
    : wxSFMLCanvas(Parent, Id, Position, Size, Style)
{
    fractalCanvasPtr = this;

    statusData = status;
    commandTarget = NULL;
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
    userFormula.type = COMPLEX_TYPE;

    // Create fractal.
    fractalHandler.CreateFractal(fractType, this);
    target = fractalHandler.GetTarget();

    fractalHandler.SetFormula(userFormula);
    target->SetOnWxCtrl(true);

    // Updates information in the command console.
    if(consoleState) commandTarget->SetText(target->AskInfo());

    // Initializes GUI elements.
    selection = new SelectRect(this);

    play = new ButtonChange(GetAbsPath("Resources/Play.tga"), GetAbsPath("Resources/Stop.tga"), 0, 500, this);
    play->SetAnchorage(false, true, true, false);
    play->Resize(this);

    pointer = new ScreenPointer(this);
    keybImage.LoadFromFile(GetAbsPath("Resources/keyboard.png"));
    mouseImage.LoadFromFile(GetAbsPath("Resources/mouse.png"));
    helpImage.LoadFromFile(GetAbsPath("Resources/HelpImage.png"));
    outKeyb.SetImage(keybImage);
    outMouse.SetImage(mouseImage);
    outHelp.SetImage(helpImage);
    outKeyb.SetColor(sf::Color(255, 255, 255, 220));
    outMouse.SetColor(sf::Color(255, 255, 255, 220));
    outHelp.SetColor(sf::Color(255, 255, 255, 220));

    this->SetFocus();

    this->SetFramerateLimit(31);
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
    delete pointer;
}
void FractalCanvas::OnUpdate()
{
    // Handles SFML events.
    while(this->GetEvent(event))
    {
        // Size change event.
        if(event.Type == sf::Event::Resized)
        {
            this->ConvertCoords(this->GetInput().GetMouseX(), this->GetInput().GetMouseY());

            target->Resize(this);
            play->Resize(this);
            if(pointer != NULL) pointer->Resize(this);

            if(keybGuide && keybGuideMode)
            {
                if(this->GetHeight() > 300 || this->GetWidth() > 300)
                {
                    outKeyb.SetPosition(this->GetWidth() - 120, this->GetHeight() - 80);
                    outMouse.SetPosition(this->GetWidth() - 90, 0);
                }
            }

            if(btn->state)
            {
                btn->state = false;
                if(type == SCRIPT_FRACTAL)
                    btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));    // Txt: "Abort"
                else
                    btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));    // Txt: "Pause"
                target->DeleteSavedZooms();
            }
        }

        target->HandleEvents(&event);

        // Keyboard event.
        if(event.Type == sf::Event::KeyPressed)
        {
            if(!target->IsRendering())
            {
                if(event.Key.Code == sf::Key::F1)  // Open or close slider.
                {
                    bool modo = !statusData.slider->IsChecked();
                    this->SetSliderMode(modo);
                    statusData.slider->Check(modo);
                }
                if(event.Key.Code == sf::Key::F2)  // Shows or hides fractal orbit.
                {
                    bool modo = !statusData.showOrbit->IsChecked();
                    this->SetOrbitMode(modo);
                    statusData.showOrbit->Check(modo);
                }
                if(event.Key.Code == sf::Key::F4)  // Saves image.
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
            if(event.Key.Code == sf::Key::F5)  // Redraw canvas.
            {
                target->Redraw();
            }
            if(event.Key.Code == sf::Key::P)  // Pause shortcut.
            {
                if(btn->state)
                {
                    btn->state = false;
                    if(type == SCRIPT_FRACTAL)
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));    // Txt: "Abort"
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));    // Txt: "Pause"
                }
                else
                {
                    btn->state = true;
                    if(type == SCRIPT_FRACTAL)
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuRelaunchTxt))+ wxT('\t') + wxT("P"));    // Txt: "Relaunch script"
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuContinueTxt))+ wxT('\t') + wxT("P"));    // Txt: "Continue"
                }
                target->PauseContinue();
            }
        }
    }

    /*This is here because the binding between SFML and wxWidgets makes SFML to handle incorrectly a resolution change.*/
    sf::View View(sf::FloatRect(0, 0, wSize.GetX(), wSize.GetY()));
    this->SetView(View);

    // Clears the screen and draw GUI elements and fractal.
    this->Clear();
    if(orbitMode) target->SetOrbitPoint(kReal, kImaginary);
    if(sliderMode && pointerChange) target->SetK(kReal, kImaginary);
    target->Move();
    target->Show(this);

    // Avoid drawing GUI elements if the fractal is rendering.
    if(!target->IsRendering())
    {
        selection->Show(this);
        if(type != LOGISTIC && type != HENON_MAP) play->Show(this);
        if(btn->pauseContinue->IsEnabled() && !target->IsPaused())
        {
            btn->pauseContinue->Enable(false);
        }

        if(btn->state && !target->IsPaused())
        {
            btn->state = false;
            if(type == SCRIPT_FRACTAL)
                btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));    // Txt: "Abort"
            else
                btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));    // Txt: "Pause"
        }

        if(keybGuide && keybGuideMode)
        {
            this->Draw(outKeyb);
            this->Draw(outMouse);
        }
        if(helpImageMode)
        {
            this->Draw(outHelp);
            if(!keybGuide) this->Draw(outKeyb);
        }

        if(juliaMode || orbitMode || sliderMode) pointer->Show(this);

#ifdef __linux__
        if(juliaMode)
        {
            if(pointerChange)
            {
                juliaImage = juliaHandler.GetTarget()->GetRenderedImage();
                pointerChange = false;
            }
            this->Draw(outJulia);
        }
#endif
    }
    else if(!btn->pauseContinue->IsEnabled() && type != SIERP_TRIANGLE) // Triangle and Logistic don't use threads so they cannot be paused.
    {
        btn->pauseContinue->Enable(true);
    }
    if(consoleState)
    {
        if(thereIsConsoleText)
        {
            commandTarget->PrepareOutput();
            commandTarget->SetText(wxString(consoleText.c_str(), wxConvUTF8));
            ClearConsoleText();
        }
    }
}
void FractalCanvas::SetWxSize(wxSize size)
{
    wSize = size;

    // Adjust position of the keyboard guide.
    if(keybGuideMode)
    {
        if(this->GetHeight() > 300 || this->GetWidth() > 300)
        {
            outKeyb.SetPosition(this->GetWidth() - 120, this->GetHeight() - 80);
            outMouse.SetPosition(this->GetWidth() - 90, 0);
            keybGuide = true;
        }
        else
        {
            keybGuide = false;
        }
    }
    if(helpImageMode)
    {
        outKeyb.SetPosition(this->GetWidth() - 120, this->GetHeight() - 80);
        outHelp.SetPosition((this->GetWidth()-helpImage.GetWidth())/2, (this->GetHeight()-helpImage.GetHeight())/2);
    }
}
void FractalCanvas::SetJuliaMode(bool mode)
{
    // If Julia mode is activated creates screen pointer.
    if(mode)
    {
        juliaMode = true;
        if(pointer == NULL) pointer = new ScreenPointer(this);
#ifdef __linux__
        FRACTAL_TYPE juliaType;
        switch(type)
        {
        case MANDELBROT:
            juliaType = JULIA;
            break;
        case MANDELBROT_ZN:
            juliaType = JULIA_ZN;
            break;
        case MANOWAR:
            juliaType = MANOWAR_JULIA;
            break;
        case BURNING_SHIP:
            juliaType = BURNING_SHIP_JULIA;
            break;
        default:
            juliaType = JULIA;
        };

        juliaHandler.CreateFractal(juliaType, this->GetWidth()/3, this->GetHeight()/3);
        juliaHandler.GetTarget()->SetOptions(target->GetOptions(), true);
        juliaHandler.GetTarget()->SetK(kReal, kImaginary);
        juliaImage.Create(this->GetWidth()/3, this->GetHeight()/3);
        outJulia.SetImage(juliaImage);
        outJulia.SetPosition(this->GetWidth()-this->GetWidth()/3, this->GetHeight()-this->GetHeight()/3);
        pointerChange = true;

#endif
    }
    // If deactivated, deletes it.
    else
    {
        juliaMode = false;
        if(pointer != NULL && !orbitMode && !sliderMode)
        {
            delete pointer;
            pointer = NULL;
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
    else return prevKReal;
}
double FractalCanvas::GetKImaginary()
{
    if(!onUpdate)
        return kImaginary;
    else return prevKImag;
}
bool FractalCanvas::ChangeInPointer()
{
    if(pointerChange)
    {
        pointerChange = false;
        return true;
    }
    else return false;
}
Fractal *FractalCanvas::GetTarget()
{
    return target;
}
void FractalCanvas::ChangeType(FRACTAL_TYPE _type)
{
    // Deletes old fractal and creates a new one.
    fractalHandler.CreateFractal(_type, this);
    type = _type;
    target = fractalHandler.GetTarget();
    fractalHandler.SetFormula(userFormula);
    target->SetOnWxCtrl(true);

    // Updates command console status.
    if(consoleState)
    {
        commandTarget->SetText(target->AskInfo());
        commandTarget->SetTarget(target);
    }

    // Deletes screen pointer if active.
    if(orbitMode || sliderMode)
    {
        orbitMode = false;
        sliderMode = false;
        if(!juliaMode)
        {
            if(pointer != NULL) delete pointer;
            pointer = NULL;
        }
    }

    play->Reset();

}
void FractalCanvas::ChangeToScript(ScriptData _scriptData)
{
    // Deletes old fractal and creates a new one.
    type = SCRIPT_FRACTAL;
    scriptData = _scriptData;
    fractalHandler.CreateScriptFractal(this, scriptData);
    target = fractalHandler.GetTarget();
    target->SetOnWxCtrl(true);

    // Updates command console status.
    if(consoleState)
    {
        commandTarget->SetText(target->AskInfo());
        commandTarget->SetTarget(target);
    }

    // Deletes screen pointer if active.
    if(orbitMode || sliderMode)
    {
        orbitMode = false;
        sliderMode = false;
        if(!juliaMode)
        {
            if(pointer != NULL) delete pointer;
            pointer = NULL;
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
        if(this->GetHeight() > 300 || this->GetWidth() > 300)
        {
            outKeyb.SetPosition(this->GetWidth() - keybImage.GetWidth(), this->GetHeight() - keybImage.GetHeight());
            outMouse.SetPosition(this->GetWidth() - mouseImage.GetWidth(), 0);
            keybGuide = true;
        }
        else
        {
            keybGuide = false;
        }
    }
    else keybGuide = false;
}
void FractalCanvas::ShowHelpImage()
{
    outKeyb.SetPosition(this->GetWidth() - keybImage.GetWidth(), this->GetHeight() - keybImage.GetHeight());
    outHelp.SetPosition((this->GetWidth()-helpImage.GetWidth())/2, (this->GetHeight()-helpImage.GetHeight())/2);
    helpImageMode = true;
}
void FractalCanvas::Reset()
{
    // Deletes old fractal and creates a new one.
    if(target->IsRendering())
        target->StopRender();

    if(type == SCRIPT_FRACTAL)
        fractalHandler.CreateScriptFractal(this, scriptData);
    else
        fractalHandler.CreateFractal(type, this);

    target = fractalHandler.GetTarget();
    fractalHandler.SetFormula(userFormula);
    target->SetOnWxCtrl(true);

    if(consoleState)
    {
        commandTarget->SetTarget(target);
    }
    play->Reset();

    // Deactivates screen pointer.
    juliaMode = false;
    orbitMode = false;
    sliderMode = false;
    if(pointer != NULL)
    {
        delete pointer;
        pointer = NULL;
    }
}
void FractalCanvas::SetOrbitMode(bool mode)
{
    orbitMode = mode;
    if(orbitMode)
    {
        target->SetOrbitMode(true);
        if(pointer == NULL) pointer = new ScreenPointer(this);
    }
    else
    {
        target->SetOrbitMode(false);
        if(pointer != NULL && !juliaMode && !sliderMode)
        {
            delete pointer;
            pointer = NULL;
        }
    }
}
void FractalCanvas::SetSliderMode(bool mode)
{
    sliderMode = mode;
    if(sliderMode)
    {
        if(pointer == NULL) pointer = new ScreenPointer(this);
        target->SetJuliaMode(true);
    }
    else
    {
        if(pointer != NULL && !juliaMode && !orbitMode)
        {
            delete pointer;
            pointer = NULL;
        }
        target->SetJuliaMode(false);
    }
}
void FractalCanvas::SetCommandConsole(CommandFrame* _infoTarget)
{
    commandTarget = _infoTarget;
}
void FractalCanvas::SetUserFormula(FormulaOpt _userFormula)
{
    userFormula = _userFormula;
}
FormulaOpt FractalCanvas::GetFormula()
{
    return userFormula;
}
void FractalCanvas::OnResize(wxSizeEvent &event)
{
    wSize = event.GetSize();

    // Adjust position of the keyboard guide.
    if(keybGuideMode)
    {
        if(this->GetHeight() > 300 || this->GetWidth() > 300)
        {
            outKeyb.SetPosition(this->GetWidth() - 120, this->GetHeight() - 80);
            outMouse.SetPosition(this->GetWidth() - 90, 0);
            keybGuide = true;
        }
        else
        {
            keybGuide = false;
        }
    }
    if(helpImageMode)
    {
        outKeyb.SetPosition(this->GetWidth() - 120, this->GetHeight() - 80);
        outHelp.SetPosition((this->GetWidth()-helpImage.GetWidth())/2, (this->GetHeight()-helpImage.GetHeight())/2);
    }

#ifdef __linux__
    if(juliaMode)
    {
        outJulia.SetPosition(wSize.GetWidth()-juliaImage.GetWidth(), wSize.GetHeight()-juliaImage.GetHeight());
        pointerChange = true;
    }
#endif
}

void FractalCanvas::OnClick(wxMouseEvent &event)
{
    // Pointer event.
    if(juliaMode || orbitMode || sliderMode)
    {
        if(pointer->ClickEvent(event))
        {
            prevKReal = kReal;
            prevKImag = kImaginary;

            onUpdate = true;
            kReal = pointer->GetX(target);
            kImaginary = pointer->GetY(target);
            pointerChange = true;
            if(orbitMode) target->SetOrbitChange();
            onUpdate = false;

#ifdef __linux__
            if(juliaMode) juliaHandler.GetTarget()->SetK(kReal, kImaginary);
#endif
        }
    }
    // Selection event.
    else if(!target->IsRendering() && !target->IsMoving())
    {
        selection->ClickEvent(event);
    }

    // Play button event.
    if(type != LOGISTIC && type != HENON_MAP)
    {
        if(play->ClickEvent(event))
        {
            target->ChangeVarGradient();
        }
    }

    // Mouse event.
    if(helpImageMode)
    {
        if(event.ButtonDown(wxMOUSE_BTN_LEFT)) helpImageMode = false;
    }

    if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        target->ZoomBack();
        if(btn->state && !target->IsPaused())
        {
            btn->state = false;
            if(type == SCRIPT_FRACTAL)
                btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));    // Txt: "Abort"
            else
                btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));    // Txt: "Pause"
        }
    }
}
void FractalCanvas::OnUnClick(wxMouseEvent &event)
{
    // Selection event.
    if(juliaMode || orbitMode || sliderMode)
    {
        pointer->UnClickEvent(event);
    }
    else
    {
        if(!target->IsRendering() && !target->IsMoving())
        {
            if(selection->UnClickEvent(event))
            {
                if(btn->state)
                {
                    btn->state = false;
                    if(type == SCRIPT_FRACTAL)
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuAbortTxt))+ wxT('\t') + wxT("P"));    // Txt: "Abort"
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT(menuPauseTxt))+ wxT('\t') + wxT("P"));    // Txt: "Pause"
                    target->DeleteSavedZooms();
                }
                target->Resize(selection->GetSeleccion());
            }
        }
    }
}
void FractalCanvas::OnMoveMouse(wxMouseEvent &event)
{
    // Selection event.
    if(juliaMode || orbitMode || sliderMode)
    {
        if(pointer->MoveEvent(event))
        {
            prevKReal = kReal;
            prevKImag = kImaginary;

            onUpdate = true;
            kReal = pointer->GetX(target);
            kImaginary = pointer->GetY(target);
            pointerChange = true;
            if(orbitMode) target->SetOrbitChange();
            onUpdate = false;

#ifdef __linux__
            if(juliaMode) juliaHandler.GetTarget()->SetK(kReal, kImaginary);
#endif
        }
    }
    else
    {
        if(!target->IsRendering() && !target->IsMoving())
        {
            selection->MoveEvent(event);
        }
    }

    // Updates status bar of the MainFrame when the mouse is moved over the fractal canvas.
    wxString text;
    if(type == LOGISTIC)
    {
        text = wxT("a: ");
        text += num_to_string(target->GetX(event.GetPosition().x));
        text += wxT("   x: ");
        text += num_to_string(target->GetY(event.GetPosition().y));
    }
    else if(type == DOUBLE_PENDULUM)
    {
        text = wxT("θ2: ");
        text += num_to_string(target->GetX(event.GetPosition().x));
        text += wxT("   θ1: ");
        text += num_to_string(target->GetY(event.GetPosition().y));
    }
    else if(type == SIERP_TRIANGLE || type == HENON_MAP || type == SCRIPT_FRACTAL)
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
            target->SetMovement(UP);
            break;
        }
    case WXK_DOWN:
        {
            target->SetMovement(DOWN);
            break;
        }
    case WXK_LEFT:
        {
            target->SetMovement(LEFT);
            break;
        }
    case WXK_RIGHT:
        {
            target->SetMovement(RIGHT);
            break;
        }
    default: break;
    }

    wxChar key = event.GetUnicodeKey();
    if(key == wxT('W'))
    {
        target->SetMovement(UP);
    }
    else if(key == wxT('S'))
    {
        target->SetMovement(DOWN);
    }
    else if(key == wxT('A'))
    {
        target->SetMovement(LEFT);
    }
    else if(key == wxT('D'))
    {
        target->SetMovement(RIGHT);
    }
}
void FractalCanvas::OnKeyUp(wxKeyEvent& event)
{
    switch(event.GetKeyCode())
    {
    case WXK_UP:
        {
            target->ReleaseMovement(UP);
            break;
        }
    case WXK_DOWN:
        {
            target->ReleaseMovement(DOWN);
            break;
        }
    case WXK_LEFT:
        {
            target->ReleaseMovement(LEFT);
            break;
        }
    case WXK_RIGHT:
        {
            target->ReleaseMovement(RIGHT);
            break;
        }
    default: break;
    }

    wxChar key = event.GetUnicodeKey();
    if(key == wxT('W'))
    {
        target->ReleaseMovement(UP);
    }
    else if(key == wxT('S'))
    {
        target->ReleaseMovement(DOWN);
    }
    else if(key == wxT('A'))
    {
        target->ReleaseMovement(LEFT);
    }
    else if(key == wxT('D'))
    {
        target->ReleaseMovement(RIGHT);
    }
}
