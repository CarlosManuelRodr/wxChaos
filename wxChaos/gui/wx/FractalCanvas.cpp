#include "FractalCanvas.h"
#include "StringFuncs.h"
#include "SizeDialogSave.h"
#include "Filesystem.h"
using namespace std;

FractalCanvas* fractalCanvasPtr = nullptr;

// Fractal Canvas
FractalCanvas::FractalCanvas(const MainWindowStatus &status, PauseContinueButton* pcb, const FractalType fractalType,
                             wxWindow* parent, const wxWindowID id, const wxPoint& position, const wxSize& size,
                             const long style) : wxSFMLCanvas(parent, id, position, size, style)
{
    fractalCanvasPtr = this;

    statusData = status;
    btn = pcb;
    _type = fractalType;

    // Status variables.
    _canvasSize = size;
    _juliaMode = false;
    _kReal = 0;
    _kImaginary = 0;
    _pointerChange = false;
    _keyboardGuide = false;
    _keyboardGuideMode = false;
    _helpImageMode = false;
    _orbitMode = false;
    _sliderMode = false;
    _onUpdate = false;

    // UserFormula
    _userFormula.bailout = 2;
    _userFormula.julia = false;
    _userFormula.userFormula = wxT("z = z^2 + c");
    _userFormula.type = FormulaType::Complex;

    // Create fractal.
    _fractalHandler.CreateFractal(fractalType, this);
    _target = _fractalHandler.GetFractalPtr();
    _sfmlFractal.SetFractal(_target);

    _fractalHandler.SetFormula(_userFormula);
    _target->SetOnWxCtrl(true);

    // Initializes GUI elements.
    _selection = new SelectionRect();

    _play = new ToggleButton(GetAbsPath({ "Resources", "Play.tga" }), GetAbsPath({ "Resources", "Stop.tga" }), 0, 4, this);
    _play->SetAnchor(false, true, true, false);
    _play->Resize(this);

    _screenPointer = new CoordinateSelector(this);
    _keyboardImage.loadFromFile(GetAbsPath({ "Resources", "keyboard.png" }));
    _mouseImage.loadFromFile(GetAbsPath({ "Resources", "mouse.png" }));
    _helpImage.loadFromFile(GetAbsPath({ "Resources","HelpImage.png" }));

    _keyboardTexture.loadFromImage(_keyboardImage);
    _mouseTexture.loadFromImage(_mouseImage);
    _helpTexture.loadFromImage(_helpImage);

    _outKeyboard.setTexture(_keyboardTexture);
    _outMouse.setTexture(_mouseTexture);
    _outHelp.setTexture(_helpTexture);

    _outKeyboard.setColor(sf::Color(255, 255, 255, 220));
    _outMouse.setColor(sf::Color(255, 255, 255, 220));
    _outHelp.setColor(sf::Color(255, 255, 255, 220));

    this->wxWindow::SetFocus();
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
    _fractalHandler.DeleteFractal();
    delete _play;
    delete _screenPointer;
}
void FractalCanvas::OnUpdate()
{
    // Handles SFML events.
    while (this->pollEvent(_event))
    {
        // Size change event.
        if (_event.type == sf::Event::Resized)
        {
            _sfmlFractal.Resize(this);
            _play->Resize(this);

            if (_screenPointer != nullptr)
                _screenPointer->Resize(this);

            if (_keyboardGuide && _keyboardGuideMode)
            {
                if (this->getSize().y > 300 || this->getSize().x > 300)
                {
                    _outKeyboard.setPosition(this->getSize().x - 120, this->getSize().y - 80);
                    _outMouse.setPosition(this->getSize().x - 90, 0);
                }
            }

            if (btn->state)
            {
                btn->state = false;
                if (_type == FractalType::ScriptFractal)
                    btn->pauseContinue->SetItemLabel(wxString(wxT("Abort")) + wxT('\t') + wxT("P"));
                else
                    btn->pauseContinue->SetItemLabel(wxString(wxT("Pause")) + wxT('\t') + wxT("P"));

                _sfmlFractal.Redraw();
            }
        }

        _sfmlFractal.HandleEvent(_event);
        if (!_target->IsRendering() && _play->HandleEvents(_event))
            _sfmlFractal.ChangeVarGradient();

        // Keyboard event.
        if (_event.type == sf::Event::KeyPressed)
        {
            if (!_target->IsRendering())
            {
                if (_event.key.code == sf::Keyboard::F1)  // Open or close slider.
                {
                    const bool mode = !statusData.slider->IsChecked();
                    this->SetSliderMode(mode);
                    statusData.slider->Check(mode);
                }
                if (_event.key.code == sf::Keyboard::F2)  // Shows or hides fractal orbit.
                {
                    const bool mode = !statusData.showOrbit->IsChecked();
                    this->SetOrbitMode(mode);
                    statusData.showOrbit->Check(mode);
                }
                if (_event.key.code == sf::Keyboard::F4)  // Saves image.
                {
                    const auto openFileDialog = new wxFileDialog(this, wxT("Select file name"), wxT(""),
                        wxT("fractal.png"), wxT("PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp"), wxFD_SAVE);
                    if (openFileDialog->ShowModal() == wxID_OK)
                    {
                        wxString fileName;
                        fileName = openFileDialog->GetPath();
                        const int ext = openFileDialog->GetFilterIndex();
                        auto path = string(fileName.mb_str());
                        const auto diag = new SizeDialogSave(this, path, ext, _type, _target, this);
                        diag->Show(true);
                    }
                    openFileDialog->Destroy();
                }
            }
            if (_event.key.code == sf::Keyboard::F5)  // Redraw canvas.
            {
                _sfmlFractal.Redraw();
            }
            if (_event.key.code == sf::Keyboard::P)  // Pause shortcut.
            {
                if (btn->state)
                {
                    btn->state = false;
                    if (_type == FractalType::ScriptFractal)
                        btn->pauseContinue->SetItemLabel(wxString(wxT("Abort")) + wxT('\t') + wxT("P"));
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT("Pause")) + wxT('\t') + wxT("P"));
                }
                else
                {
                    btn->state = true;
                    if (_type == FractalType::ScriptFractal)
                        btn->pauseContinue->SetItemLabel(wxString(wxT("Relaunch script")) + wxT('\t') + wxT("P"));
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT("Continue")) + wxT('\t') + wxT("P"));
                }
                _target->PauseContinue();
            }
        }
    }

    // This is here because the binding between SFML and wxWidgets makes SFML to incorrectly handle a resolution change.
    const sf::View view(sf::FloatRect(0, 0, _canvasSize.GetX(), _canvasSize.GetY()));
    this->setView(view);

    // Clears the screen and draw GUI elements and fractal.
    this->clear();

    if (_orbitMode)
        _target->SetOrbitPoint(_kReal, _kImaginary);
    if (_sliderMode && _pointerChange)
        _sfmlFractal.SetK(_kReal, _kImaginary);

    _target->Move();
    _sfmlFractal.Show(this);

    // Avoid drawing GUI elements if the fractal is rendering.
    if (!_target->IsRendering())
    {
        _selection->Show(this);

        if (btn->pauseContinue->IsEnabled() && !_target->IsPaused())
            btn->pauseContinue->Enable(false);

        if (btn->state && !_target->IsPaused())
        {
            btn->state = false;
            if (_type == FractalType::ScriptFractal)
                btn->pauseContinue->SetItemLabel(wxString(wxT("Abort")) + wxT('\t') + wxT("P"));
            else
                btn->pauseContinue->SetItemLabel(wxString(wxT("Pause")) + wxT('\t') + wxT("P"));
        }

        if (_keyboardGuide && _keyboardGuideMode)
        {
            this->draw(_outKeyboard);
            this->draw(_outMouse);
        }

        if (_helpImageMode)
        {
            this->draw(_outHelp);
            if (!_keyboardGuide)
                this->draw(_outKeyboard);
        }

        if (_juliaMode || _orbitMode || _sliderMode)
            _screenPointer->Show(this);
    }
    else if (!btn->pauseContinue->IsEnabled() && _type != FractalType::SierpinskyTriangle) // Triangle and Logistic don't use threads, so they cannot be paused.
    {
        btn->pauseContinue->Enable(true);
    }

    _play->Show(this);
}
void FractalCanvas::SetWxSize(const wxSize size)
{
    _canvasSize = size;

    // Adjust position of the keyboard guide.
    if (_keyboardGuideMode)
    {
        if (this->getSize().y > 300 || this->getSize().x > 300)
        {
            _outKeyboard.setPosition(this->getSize().x - 120, this->getSize().y - 80);
            _outMouse.setPosition(this->getSize().x - 90, 0);
            _keyboardGuide = true;
        }
        else
            _keyboardGuide = false;
    }
    if (_helpImageMode)
    {
        _outKeyboard.setPosition(this->getSize().x - _keyboardImage.getSize().x, this->getSize().y - _keyboardImage.getSize().y);
        _outHelp.setPosition((this->getSize().x - _helpImage.getSize().x) / 2, (this->getSize().y - _helpImage.getSize().y) / 2);
    }
}
void FractalCanvas::SetJuliaMode(const bool mode)
{
    // If Julia mode is activated creates screen pointer.
    if (mode)
    {
        _juliaMode = true;
        if (_screenPointer == nullptr)
            _screenPointer = new CoordinateSelector(this);
    }
    // If deactivated, deletes it.
    else
    {
        _juliaMode = false;
        if (_screenPointer != nullptr && !_orbitMode && !_sliderMode)
        {
            delete _screenPointer;
            _screenPointer = nullptr;
        }
    }
}
double FractalCanvas::GetKReal() const
{
    return !_onUpdate ? _kReal : _prevKReal;
}

double FractalCanvas::GetKImaginary() const
{
    return !_onUpdate ? _kImaginary : _prevKImag;
}

bool FractalCanvas::ChangeInPointer()
{
    if (_pointerChange)
    {
        _pointerChange = false;
        return true;
    }
    return false;
}
Fractal* FractalCanvas::GetFractalPtr() const
{
    return _target;
}
SFMLFractal* FractalCanvas::GetSFMLFractalPtr()
{
    return &_sfmlFractal;
}
FractalType FractalCanvas::GetFractalType() const
{
    return _type;
}
void FractalCanvas::ChangeType(const FractalType type)
{
    // Deletes old fractal and creates a new one.
    _fractalHandler.CreateFractal(type, this);
    _type = type;
    _target = _fractalHandler.GetFractalPtr();
    _sfmlFractal.SetFractal(_target);
    _fractalHandler.SetFormula(_userFormula);
    _target->SetOnWxCtrl(true);

    // Deletes screen pointer if active.
    if (_orbitMode || _sliderMode)
    {
        _orbitMode = false;
        _sliderMode = false;
        if (!_juliaMode)
        {
            if (_screenPointer != nullptr)
                delete _screenPointer;

            _screenPointer = nullptr;
        }
    }

    _play->Reset();

}
void FractalCanvas::ChangeToScript(const ScriptData &scriptData)
{
    // Deletes old fractal and creates a new one.
    _type = FractalType::ScriptFractal;
    _scriptData = scriptData;
    _fractalHandler.CreateScriptFractal(this, _scriptData);
    _target = _fractalHandler.GetFractalPtr();
    _sfmlFractal.SetFractal(_target);
    _target->SetOnWxCtrl(true);

    // Deletes screen pointer if active.
    if (_orbitMode || _sliderMode)
    {
        _orbitMode = false;
        _sliderMode = false;
        if (!_juliaMode)
        {
            if (_screenPointer != nullptr)
                delete _screenPointer;

            _screenPointer = nullptr;
        }
    }

    _play->Reset();
}
void FractalCanvas::SetKeyboardGuide(const bool mode)
{
    _keyboardGuideMode = mode;
    if (_keyboardGuideMode)
    {
        // Adjust position of the keyboard guide.
        if (this->getSize().y > 300 || this->getSize().x > 300)
        {
            _outKeyboard.setPosition(this->getSize().x - _keyboardImage.getSize().x, this->getSize().y - _keyboardImage.getSize().y);
            _outMouse.setPosition(this->getSize().x - _mouseImage.getSize().x, 0);
            _keyboardGuide = true;
        }
        else
            _keyboardGuide = false;
    }
    else
        _keyboardGuide = false;
}
void FractalCanvas::ShowHelpImage()
{
    _outKeyboard.setPosition(this->getSize().x - _keyboardImage.getSize().x, this->getSize().y - _keyboardImage.getSize().y);
    _outHelp.setPosition((this->getSize().x - _helpImage.getSize().x) / 2, (this->getSize().y - _helpImage.getSize().y) / 2);
    _helpImageMode = true;
}
void FractalCanvas::Reset()
{
    // Deletes old fractal and creates a new one.
    if (_target->IsRendering())
        _target->StopRender();

    if (_type == FractalType::ScriptFractal)
        _fractalHandler.CreateScriptFractal(this, _scriptData);
    else
        _fractalHandler.CreateFractal(_type, this);

    _target = _fractalHandler.GetFractalPtr();
    _sfmlFractal.SetFractal(_target);
    _fractalHandler.SetFormula(_userFormula);
    _target->SetOnWxCtrl(true);
    _play->Reset();

    // Deactivates screen pointer.
    _juliaMode = false;
    _orbitMode = false;
    _sliderMode = false;
    if (_screenPointer != nullptr)
    {
        delete _screenPointer;
        _screenPointer = nullptr;
    }
}
void FractalCanvas::SetOrbitMode(const bool mode)
{
    _orbitMode = mode;
    if (_orbitMode)
    {
        _target->SetOrbitMode(true);
        if (_screenPointer == nullptr)
            _screenPointer = new CoordinateSelector(this);
    }
    else
    {
        _target->SetOrbitMode(false);
        if (_screenPointer != nullptr && !_juliaMode && !_sliderMode)
        {
            delete _screenPointer;
            _screenPointer = nullptr;
        }
    }
}
void FractalCanvas::SetSliderMode(const bool mode)
{
    _sliderMode = mode;
    if (_sliderMode)
    {
        if (_screenPointer == nullptr)
            _screenPointer = new CoordinateSelector(this);
        _target->SetJuliaMode(true);
    }
    else
    {
        if (_screenPointer != nullptr && !_juliaMode && !_orbitMode)
        {
            delete _screenPointer;
            _screenPointer = nullptr;
        }
        _target->SetJuliaMode(false);
    }
}
void FractalCanvas::SetUserFormula(const FormulaOpt &userFormula)
{
    _userFormula = userFormula;
}
FormulaOpt FractalCanvas::GetFormula()
{
    return _userFormula;
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void FractalCanvas::OnResize(wxSizeEvent& event)
{
    _canvasSize = event.GetSize();

    // Adjust position of the keyboard guide.
    if (_keyboardGuideMode)
    {
        if (this->getSize().y > 300 || this->getSize().x > 300)
        {
            _outKeyboard.setPosition(this->getSize().x - 120, this->getSize().y - 80);
            _outMouse.setPosition(this->getSize().x - 90, 0);
            _keyboardGuide = true;
        }
        else
            _keyboardGuide = false;
    }
    if (_helpImageMode)
    {
        _outKeyboard.setPosition(this->getSize().x - 120, this->getSize().y - 80);
        _outHelp.setPosition((this->getSize().x - _helpImage.getSize().x) / 2, (this->getSize().y - _helpImage.getSize().y) / 2);
    }
}

void FractalCanvas::OnClick(wxMouseEvent& event)
{
    // Pointer event.
    if (_juliaMode || _orbitMode || _sliderMode)
    {
        if (_screenPointer->ClickEvent(event))
        {
            _prevKReal = _kReal;
            _prevKImag = _kImaginary;

            _onUpdate = true;
            _kReal = _screenPointer->GetX(_target);
            _kImaginary = _screenPointer->GetY(_target);
            _pointerChange = true;

            if (_orbitMode)
                _target->SetOrbitChange();

            _onUpdate = false;
        }
    }
    // Selection event.
    else if (!_target->IsRendering() && !_target->IsMoving())
        _selection->ClickEvent(event);

    // Mouse event.
    if (_helpImageMode)
    {
        if (event.ButtonDown(wxMOUSE_BTN_LEFT))
            _helpImageMode = false;
    }

    if (event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        _sfmlFractal.ZoomBack();
        if (btn->state && !_target->IsPaused())
        {
            btn->state = false;
            if (_type == FractalType::ScriptFractal)
                btn->pauseContinue->SetItemLabel(wxString(wxT("Abort")) + wxT('\t') + wxT("P"));
            else
                btn->pauseContinue->SetItemLabel(wxString(wxT("Pause")) + wxT('\t') + wxT("P"));
        }
    }
}
void FractalCanvas::OnUnClick(wxMouseEvent& event)
{
    // Selection event.
    if (_juliaMode || _orbitMode || _sliderMode)
        _screenPointer->UnClickEvent(event);
    else
    {
        if (!_target->IsRendering() && !_target->IsMoving())
        {
            if (_selection->UnClickEvent(event))
            {
                if (btn->state)
                {
                    btn->state = false;
                    if (_type == FractalType::ScriptFractal)
                        btn->pauseContinue->SetItemLabel(wxString(wxT("Abort")) + wxT('\t') + wxT("P"));
                    else
                        btn->pauseContinue->SetItemLabel(wxString(wxT("Pause")) + wxT('\t') + wxT("P"));
                    _sfmlFractal.Redraw();
                }
                _sfmlFractal.SetAreaOfView(_selection->GetSelection());
            }
        }
    }
}
void FractalCanvas::OnMoveMouse(wxMouseEvent& event)
{
    // Selection event.
    if (_juliaMode || _orbitMode || _sliderMode)
    {
        if (_screenPointer->MoveEvent(event))
        {
            _prevKReal = _kReal;
            _prevKImag = _kImaginary;

            _onUpdate = true;
            _kReal = _screenPointer->GetX(_target);
            _kImaginary = _screenPointer->GetY(_target);
            _pointerChange = true;

            if (_orbitMode)
                _target->SetOrbitChange();

            _onUpdate = false;
        }
    }
    else
    {
        if (!_target->IsRendering() && !_target->IsMoving())
            _selection->MoveEvent(event);
    }

    // Updates status bar of the MainFrame when the mouse is moved over the fractal canvas.
    wxString text;
    if (_type == FractalType::DoublePendulum)
    {
        text = wxT("θ2: ");
        text += num_to_string(_target->GetX(event.GetPosition().x));
        text += wxT("   θ1: ");
        text += num_to_string(_target->GetY(event.GetPosition().y));
    }
    else if (_type == FractalType::SierpinskyTriangle || _type == FractalType::ScriptFractal)
    {
        text = wxT("x: ");
        text += num_to_string(_target->GetX(event.GetPosition().x));
        text += wxT("   y: ");
        text += num_to_string(_target->GetY(event.GetPosition().y));
    }
    else
    {
        text = wxT("Real: ");
        text += num_to_string(_target->GetX(event.GetPosition().x));
        text += wxT("   Imaginary: ");
        text += num_to_string(_target->GetY(event.GetPosition().y));
    }
    statusData.status->SetStatusText(text);
}
// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void FractalCanvas::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_UP:
        {
            _target->SetMovement(Up);
            break;
        }
        case WXK_DOWN:
        {
            _target->SetMovement(Down);
            break;
        }
        case WXK_LEFT:
        {
            _target->SetMovement(Left);
            break;
        }
        case WXK_RIGHT:
        {
            _target->SetMovement(Right);
            break;
        }
        default: break;
    }

    const wxChar key = event.GetUnicodeKey();
    if (key == wxT('W') || key == wxT('w'))
        _target->SetMovement(Up);
    else if (key == wxT('S') || key == wxT('s'))
        _target->SetMovement(Down);
    else if (key == wxT('A') || key == wxT('a'))
        _target->SetMovement(Left);
    else if (key == wxT('D') || key == wxT('d'))
        _target->SetMovement(Right);
}
// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void FractalCanvas::OnKeyUp(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_UP:
        {
            _target->ReleaseMovement(Up);
            break;
        }
        case WXK_DOWN:
        {
            _target->ReleaseMovement(Down);
            break;
        }
        case WXK_LEFT:
        {
            _target->ReleaseMovement(Left);
            break;
        }
        case WXK_RIGHT:
        {
            _target->ReleaseMovement(Right);
            break;
        }
        default: break;
    }

    const wxChar key = event.GetUnicodeKey();
    if (key == wxT('W') || key == wxT('w'))
        _target->ReleaseMovement(Up);
    else if (key == wxT('S') || key == wxT('s'))
        _target->ReleaseMovement(Down);
    else if (key == wxT('A') || key == wxT('a'))
        _target->ReleaseMovement(Left);
    else if (key == wxT('D') || key == wxT('d'))
        _target->ReleaseMovement(Right);
}
