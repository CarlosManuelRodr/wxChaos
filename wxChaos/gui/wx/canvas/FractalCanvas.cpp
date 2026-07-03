// ReSharper disable CppDFAUnreachableCode
#include <algorithm>
#include <cmath>
#include <string>
#include <wx/popupwin.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include "AppPaths.h"
#include "canvas/FractalCanvas.h"
#include "export/ImageExportSizeDialog.h"
using namespace std;

wxDEFINE_EVENT(wxEVT_FRACTAL_CANVAS_STATUS_TEXT, wxCommandEvent);

// Fractal Canvas
FractalCanvas::FractalCanvas(const FractalType fractalType, wxWindow* parent, const wxWindowID id,
                             const wxPoint& position, const wxSize& size, const long style)
                             : wxSFMLCanvas(parent, id, position, size, style)
{
    // Status variables.
    _canvasSize = size;
    _juliaMode = false;
    _kReal = 0;
    _kImaginary = 0;
    _prevKReal = 0;
    _prevKImag = 0;
    _coordinateSelectorChange = false;
    _keyboardGuide = false;
    _keyboardGuideMode = false;
    _helpImageMode = false;
    _orbitMode = false;
    _sliderMode = false;
    _onUpdate = false;
    _hasLastMousePosition = false;
    _mouseWheelPanning = false;
    _toolPanning = false;
    _zoomToolDragging = false;
    _showMainCanvasOverlays = true;
    _lastMousePosition = wxPoint(0, 0);
    _lastMouseWheelPanPosition = wxPoint(0, 0);
    _zoomToolStartPosition = wxPoint(0, 0);
    _interactionTool = FractalInteractionTool::Cursor;

    // UserFormula
    _userFormula.bailout = 2;
    _userFormula.julia = false;
    _userFormula.userFormula = "z = z^2 + c";
    _userFormula.type = FormulaType::Complex;

    // Create fractal.
    this->CreateFractal(fractalType);
    _fractal = _fractalFactory.GetFractal();
    AttachFractalStatusHandler();
    _fractalPresenter = new FractalPresenter(_fractal);
    _fractalPresenter->SetHandleRightClickZoomBack(false);

    _fractalFactory.SetFormula(_userFormula);

    // Initialize GUI elements.
    _selectionRect = new SelectionRect();
    UpdateSelectionAspectRatio();

    _coordinateSelector = new CoordinateSelector(this);
    _keyboardImage.loadFromFile(AppPaths::ResourceFileStd({"keyboard.png"}));
    _mouseImage.loadFromFile(AppPaths::ResourceFileStd({"mouse.png"}));
    _helpImage.loadFromFile(AppPaths::ResourceFileStd({"HelpImage.png"}));

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
    this->Bind(wxEVT_MOTION, &FractalCanvas::OnMoveMouse, this);
    this->Bind(wxEVT_LEAVE_WINDOW, &FractalCanvas::OnLeaveMouse, this);
    this->Bind(wxEVT_LEFT_DOWN, &FractalCanvas::OnClick, this);
    this->Bind(wxEVT_RIGHT_DOWN, &FractalCanvas::OnClick, this);
    this->Bind(wxEVT_MIDDLE_DOWN, &FractalCanvas::OnClick, this);
    this->Bind(wxEVT_LEFT_UP, &FractalCanvas::OnReleaseClick, this);
    this->Bind(wxEVT_RIGHT_UP, &FractalCanvas::OnReleaseClick, this);
    this->Bind(wxEVT_MIDDLE_UP, &FractalCanvas::OnReleaseClick, this);
    this->Bind(wxEVT_MOUSEWHEEL, &FractalCanvas::OnMouseWheel, this);
    this->Bind(wxEVT_MOUSE_CAPTURE_LOST, &FractalCanvas::OnMouseCaptureLost, this);
    this->Bind(wxEVT_SIZE, &FractalCanvas::OnResize, this);
    this->Bind(wxEVT_KEY_DOWN, &FractalCanvas::OnKeyDown, this);
    this->Bind(wxEVT_KEY_UP, &FractalCanvas::OnKeyUp, this);
}
FractalCanvas::~FractalCanvas()
{
    // Cleanup.
    delete _fractalPresenter;
    _fractalFactory.DeleteFractal();
    delete _selectionRect;
    delete _coordinateSelector;
    HidePointInfo();
}

unsigned int FractalCanvas::GetStatusCoordinateSignificantDigits() const
{
    constexpr unsigned int defaultSignificantDigits = 15;
    constexpr unsigned int maximumSignificantDigits = 80;
    if (!_fractal->IsHighPrecisionRenderActive())
        return defaultSignificantDigits;

    const auto decimalDigits = static_cast<unsigned int>(std::ceil(
        static_cast<double>(_fractal->GetHighPrecisionRenderBits()) * std::log10(2.0))) + 2;
    return std::clamp(decimalDigits, defaultSignificantDigits, maximumSignificantDigits);
}

wxString FractalCanvas::FormatStatusCoordinate(const HighPrecisionReal& value) const
{
    return wxString::FromUTF8(value.ToString(GetStatusCoordinateSignificantDigits()));
}

void FractalCanvas::CreateFractal(const FractalType type)
{
    const sf::Vector2u size = GetCurrentRenderSize();
    _fractalFactory.CreateFractal(type, size.x, size.y);
}

void FractalCanvas::CreateScriptFractal(const ScriptData& scriptData)
{
    const sf::Vector2u size = GetCurrentRenderSize();
    _fractalFactory.CreateScriptFractal(size.x, size.y, scriptData);
}

void FractalCanvas::AttachFractalStatusHandler() const
{
    if (_fractal == nullptr)
        return;

    _fractal->SetPrecisionStatusChangedCallback([this](bool, unsigned int)
    {
        EmitStatusText();
    });
}

wxString FractalCanvas::BuildStatusText() const
{
    wxString text;
    if (_fractal == nullptr)
        return text;

    if (_hasLastMousePosition)
    {
        if (const FractalType fractalType = GetFractalType(); fractalType == FractalType::DoublePendulum)
        {
            text = "θ2: ";
            text += FormatStatusCoordinate(_fractal->GetPreciseX(_lastMousePosition.x));
            text += "   θ1: ";
            text += FormatStatusCoordinate(_fractal->GetPreciseY(_lastMousePosition.y));
        }
        else if (fractalType == FractalType::SierpinskiTriangle || fractalType == FractalType::ScriptFractal)
        {
            text = "x: ";
            text += FormatStatusCoordinate(_fractal->GetPreciseX(_lastMousePosition.x));
            text += "   y: ";
            text += FormatStatusCoordinate(_fractal->GetPreciseY(_lastMousePosition.y));
        }
        else
        {
            text = "Real: ";
            text += FormatStatusCoordinate(_fractal->GetPreciseX(_lastMousePosition.x));
            text += "   Imaginary: ";
            text += FormatStatusCoordinate(_fractal->GetPreciseY(_lastMousePosition.y));
        }
    }

    if (_fractal->IsHighPrecisionRenderActive())
    {
        if (!text.empty())
            text += "   ";

        text += "Multiprecision: ";
        text += wxString::Format("%u", _fractal->GetHighPrecisionRenderBits());
        text += "-bit";
    }

    return text;
}

void FractalCanvas::EmitStatusText() const
{
    wxCommandEvent statusEvent(wxEVT_FRACTAL_CANVAS_STATUS_TEXT);
    statusEvent.SetEventObject(const_cast<FractalCanvas*>(this));
    statusEvent.SetString(BuildStatusText());
    GetParent()->GetEventHandler()->ProcessEvent(statusEvent);
}

sf::Vector2u FractalCanvas::GetCurrentRenderSize() const
{
    if (IsSfmlWindowCreated())
    {
        const sf::Vector2u sfmlSize = this->getSize();
        if (sfmlSize.x > 0 && sfmlSize.y > 0)
            return sfmlSize;
    }

    const int width = std::max(1, _canvasSize.GetWidth());
    const int height = std::max(1, _canvasSize.GetHeight());
    return {static_cast<unsigned int>(width), static_cast<unsigned int>(height)};
}

void FractalCanvas::ResizePresentation(const wxSize size)
{
    const int width = size.GetWidth();
    const int height = size.GetHeight();
    if (width <= 0 || height <= 0)
        return;

    _canvasSize = size;

    const sf::Vector2u sfmlSize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    if (!EnsureSfmlWindowCreated())
        return;

    if (this->getSize() != sfmlSize)
        this->setSize(sfmlSize);

    _fractalPresenter->Resize(this);
    UpdateSelectionAspectRatio();

    if (_coordinateSelector != nullptr)
        _coordinateSelector->Resize(this);
}

void FractalCanvas::UpdateSelectionAspectRatio() const
{
    if (_selectionRect == nullptr || _fractal == nullptr)
        return;

    const sf::Vector2u renderSize = _fractal->GetScreenSize();
    if (renderSize.x == 0 || renderSize.y == 0)
        return;

    _selectionRect->SetAspectRatio(static_cast<double>(renderSize.x) / static_cast<double>(renderSize.y));
}

void FractalCanvas::UpdateCoordinateSelectorValue()
{
    if (_coordinateSelector == nullptr || !(_juliaMode || _orbitMode || _sliderMode))
        return;

    const double nextKReal = _coordinateSelector->GetX(_fractal);
    const double nextKImaginary = _coordinateSelector->GetY(_fractal);
    if (_kReal == nextKReal && _kImaginary == nextKImaginary)
        return;

    _prevKReal = _kReal;
    _prevKImag = _kImaginary;
    _onUpdate = true;
    _kReal = nextKReal;
    _kImaginary = nextKImaginary;
    _coordinateSelectorChange = true;

    if (_orbitMode)
        _fractal->SetOrbitChange();

    _onUpdate = false;
}

void FractalCanvas::SeedCoordinateSelectorValue()
{
    if (_coordinateSelector == nullptr)
        return;

    _prevKReal = _kReal;
    _prevKImag = _kImaginary;
    _onUpdate = true;
    _kReal = _coordinateSelector->GetX(_fractal);
    _kImaginary = _coordinateSelector->GetY(_fractal);
    _coordinateSelectorChange = true;
    _fractal->SetOrbitChange();
    _onUpdate = false;
}

void FractalCanvas::BeginMousePanAt(const wxPoint position)
{
    if (_fractalPresenter->IsZoomPreviewActive() || _fractal->IsRendering() || !_fractal->IsRendered())
        return;

    _mouseWheelPanning = true;
    _lastMouseWheelPanPosition = position;
    _fractalPresenter->BeginMousePan();

    if (!HasCapture())
        CaptureMouse();
}

void FractalCanvas::ContinueMousePanAt(const wxPoint position)
{
    if (!_mouseWheelPanning)
        return;

    if (const wxPoint delta = position - _lastMouseWheelPanPosition; delta.x != 0 || delta.y != 0)
    {
        _fractalPresenter->PanByMousePixels(delta.x, delta.y);
        UpdateCoordinateSelectorValue();
        _lastMouseWheelPanPosition = position;
        Refresh(false);
        Update();
    }
}

void FractalCanvas::EndMousePanGesture()
{
    if (!_mouseWheelPanning)
    {
        _toolPanning = false;
        return;
    }

    _mouseWheelPanning = false;
    _toolPanning = false;
    _fractalPresenter->EndMousePan();

    if (HasCapture())
        ReleaseMouse();
}

double FractalCanvas::CalculateZoomToolScale(const wxPoint position) const
{
    constexpr double pixelsPerZoomDoubling = 240.0;
    constexpr double maximumDragZoomScale = 16.0;
    const int dragY = position.y - _zoomToolStartPosition.y;
    return std::clamp(
        std::pow(2.0, static_cast<double>(dragY) / pixelsPerZoomDoubling),
        1.0 / maximumDragZoomScale,
        maximumDragZoomScale);
}

void FractalCanvas::CommitZoomToolDrag(const wxPoint endPosition) const
{
    if (!_zoomToolDragging || _fractalPresenter->IsMoving())
        return;

    _fractalPresenter->CommitInteractiveZoom(CalculateZoomToolScale(endPosition));
}

wxString FractalCanvas::InspectPointAt(const wxPoint position) const
{
    if (_fractal == nullptr)
        return "No fractal loaded.";

    return _fractal->InspectPoint(_fractal->GetX(position.x), _fractal->GetY(position.y));
}

void FractalCanvas::ShowPointInfo(const wxPoint position, const wxString& text)
{
    const wxPoint screenPosition = ClientToScreen(position + wxPoint(20, 18));

    if (_pointInfoPopup == nullptr)
    {
        _pointInfoPopup = new wxPopupWindow(this, wxBORDER_SIMPLE);
        _pointInfoPopup->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

        _pointInfoText = new wxStaticText(_pointInfoPopup, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
        _pointInfoText->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
        _pointInfoText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

        const auto sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(_pointInfoText, 0, wxALL, 8);
        _pointInfoPopup->SetSizer(sizer);
    }

    if (_lastPointInfoText != text)
    {
        constexpr int popupMaxWidth = 520;
        _lastPointInfoText = text;
        _pointInfoText->SetLabel(text);
        _pointInfoText->Wrap(popupMaxWidth);
        _pointInfoPopup->Fit();
    }

    _pointInfoPopup->Move(screenPosition);
    if (!_pointInfoPopup->IsShown())
        _pointInfoPopup->Show();
}

void FractalCanvas::HidePointInfo()
{
    _lastPointInfoText.clear();
    _pointInfoText = nullptr;

    if (_pointInfoPopup == nullptr)
        return;

    _pointInfoPopup->Destroy();
    _pointInfoPopup = nullptr;
}

void FractalCanvas::CancelToolGestures()
{
    _fractalPresenter->CancelInteractiveZoom();
    _zoomToolDragging = false;
    EndMousePanGesture();
}

void FractalCanvas::OnUpdate()
{
    if (const wxSize clientSize = GetClientSize(); clientSize.GetWidth() <= 0 || clientSize.GetHeight() <= 0)
        return;

    // Handles SFML events.
    while (this->pollEvent(_event))
    {
        // Size change event.
        if (_event.type == sf::Event::Resized)
        {
            ResizePresentation(wxSize(
                static_cast<int>(_event.size.width),
                static_cast<int>(_event.size.height)));

            if (_keyboardGuide && _keyboardGuideMode)
            {
                if (this->getSize().y > 300 || this->getSize().x > 300)
                {
                    _outKeyboard.setPosition(
                        static_cast<float>(this->getSize().x - 120),
                        static_cast<float>(this->getSize().y - 80)
                        );
                    _outMouse.setPosition(static_cast<float>(this->getSize().x - 90), 0);
                }
            }

        }

        _fractalPresenter->HandleEvent(_event);

        // Keyboard event.
        if (_event.type == sf::Event::KeyPressed)
        {
            if (!_fractal->IsRendering())
            {
                if (_event.key.code == sf::Keyboard::F1)  // Open or close slider.
                    this->SetSliderMode(!_sliderMode);
                if (_event.key.code == sf::Keyboard::F2)  // Shows or hides fractal orbit.
                    this->SetOrbitMode(!_orbitMode);
                if (_event.key.code == sf::Keyboard::F4)  // Saves image.
                {
                    const auto diag = new ImageExportSizeDialog(this, GetFractalType(), _fractal, this);
                    diag->ShowModal();
                    diag->Destroy();
                }
            }
            if (_event.key.code == sf::Keyboard::F5)  // Redraw canvas.
                _fractalPresenter->Redraw();
            if (_event.key.code == sf::Keyboard::P)  // Abort shortcut.
                this->AbortRender();
        }
    }

    // This is here because the binding between SFML and wxWidgets makes SFML to incorrectly handle a resolution change.
    const sf::View view(sf::FloatRect(0, 0, static_cast<float>(_canvasSize.GetX()), static_cast<float>(_canvasSize.GetY())));
    this->setView(view);

    // Clears the screen and draw GUI elements and fractal.
    this->clear();

    const double elapsedSeconds = _movementClock.restart().asSeconds();
    const bool wasMoving = _fractalPresenter->IsMoving();
    _fractalPresenter->Move(elapsedSeconds);
    if (wasMoving || _fractalPresenter->IsMoving())
        UpdateCoordinateSelectorValue();

    if (_orbitMode)
        _fractal->SetOrbitPoint(_kReal, _kImaginary);
    if (_sliderMode && _coordinateSelectorChange)
        _fractalPresenter->SetK(_kReal, _kImaginary);

    _fractalPresenter->Show(this, elapsedSeconds);

    // Avoid drawing GUI elements if the fractal is rendering.
    if (!_fractal->IsRendering())
    {
        _selectionRect->Show(this);

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
            _coordinateSelector->Show(this);
    }
}
void FractalCanvas::SetWxSize(const wxSize size)
{
    _canvasSize = size;

    // Adjust position of the keyboard guide.
    if (_keyboardGuideMode)
    {
        if (this->getSize().y > 300 || this->getSize().x > 300)
        {
            _outKeyboard.setPosition(static_cast<float>(this->getSize().x - 120), static_cast<float>(this->getSize().y - 80));
            _outMouse.setPosition(static_cast<float>(this->getSize().x - 90), 0);
            _keyboardGuide = true;
        }
        else
            _keyboardGuide = false;
    }
    if (_helpImageMode)
    {
        _outKeyboard.setPosition(
            static_cast<float>(this->getSize().x - _keyboardImage.getSize().x),
            static_cast<float>(this->getSize().y - _keyboardImage.getSize().y)
            );
        _outHelp.setPosition(
            static_cast<float>((this->getSize().x - _helpImage.getSize().x) / 2.0),
            static_cast<float>((this->getSize().y - _helpImage.getSize().y) / 2.0)
            );
    }
}

void FractalCanvas::PrepareForClose()
{
    StopSfmlRefresh();
    if (_fractal != nullptr)
        _fractal->StopRender();
}

void FractalCanvas::SetJuliaMode(const bool mode)
{
    // If Julia mode is activated creates screen pointer.
    if (mode)
    {
        _juliaMode = true;
        if (_coordinateSelector == nullptr)
            _coordinateSelector = new CoordinateSelector(this);
    }
    // If deactivated, deletes it.
    else
    {
        _juliaMode = false;
        if (_coordinateSelector != nullptr && !_orbitMode && !_sliderMode)
        {
            delete _coordinateSelector;
            _coordinateSelector = nullptr;
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

bool FractalCanvas::ChangeInCoordinateSelector()
{
    if (_coordinateSelectorChange)
    {
        _coordinateSelectorChange = false;
        return true;
    }
    return false;
}
Fractal* FractalCanvas::GetFractal() const
{
    return _fractal;
}
FractalPresenter* FractalCanvas::GetFractalPresenter() const
{
    return _fractalPresenter;
}
void FractalCanvas::SetInteractionTool(const FractalInteractionTool tool)
{
    CancelToolGestures();
    if (_interactionTool == FractalInteractionTool::PointPicker && tool != FractalInteractionTool::PointPicker)
        HidePointInfo();

    _interactionTool = tool;

    switch (_interactionTool)
    {
        case FractalInteractionTool::Hand:
            SetCursor(wxCursor(wxCURSOR_HAND));
            break;
        case FractalInteractionTool::Zoom:
            SetCursor(wxCursor(wxCURSOR_CROSS));
            break;
        case FractalInteractionTool::PointPicker:
            SetCursor(wxCursor(wxCURSOR_QUESTION_ARROW));
            break;
        case FractalInteractionTool::Cursor:
        default:
            SetCursor(wxCursor(wxCURSOR_ARROW));
            break;
    }
}
FractalInteractionTool FractalCanvas::GetInteractionTool() const
{
    return _interactionTool;
}
FractalType FractalCanvas::GetFractalType() const
{
    return _fractal != nullptr ? _fractal->GetType() : FractalType::Undefined;
}
bool FractalCanvas::CanAbortRender() const
{
    return _fractal != nullptr && _fractal->IsRendering() && GetFractalType() != FractalType::SierpinskiTriangle;
}
void FractalCanvas::AbortRender() const
{
    if (!this->CanAbortRender())
        return;

    _fractal->StopRender();
    _fractal->MarkRenderAborted();
}
void FractalCanvas::ChangeType(const FractalType type)
{
    // Deletes old fractal and creates a new one.
    this->CreateFractal(type);
    _fractal = _fractalFactory.GetFractal();
    AttachFractalStatusHandler();
    _fractalPresenter->SetFractal(_fractal);
    _fractalPresenter->SetHandleRightClickZoomBack(false);
    _fractalFactory.SetFormula(_userFormula);
    UpdateSelectionAspectRatio();

    // Deletes screen pointer if active.
    if (_orbitMode || _sliderMode)
    {
        _orbitMode = false;
        _sliderMode = false;
        if (!_juliaMode)
        {
            delete _coordinateSelector;
            _coordinateSelector = nullptr;
        }
    }
}
void FractalCanvas::ChangeToScript(const ScriptData &scriptData)
{
    // Deletes old fractal and creates a new one.
    _scriptData = scriptData;
    this->CreateScriptFractal(_scriptData);
    _fractal = _fractalFactory.GetFractal();
    AttachFractalStatusHandler();
    _fractalPresenter->SetFractal(_fractal);
    _fractalPresenter->SetHandleRightClickZoomBack(false);
    UpdateSelectionAspectRatio();

    // Deletes screen pointer if active.
    if (_orbitMode || _sliderMode)
    {
        _orbitMode = false;
        _sliderMode = false;
        if (!_juliaMode)
        {
            delete _coordinateSelector;
            _coordinateSelector = nullptr;
        }
    }
}
void FractalCanvas::SetKeyboardGuide(const bool mode)
{
    _keyboardGuideMode = mode;
    if (_keyboardGuideMode)
    {
        // Adjust position of the keyboard guide.
        if (this->getSize().y > 300 || this->getSize().x > 300)
        {
            _outKeyboard.setPosition(
                static_cast<float>(this->getSize().x - _keyboardImage.getSize().x),
                static_cast<float>(this->getSize().y - _keyboardImage.getSize().y)
                );
            _outMouse.setPosition(static_cast<float>(this->getSize().x - _mouseImage.getSize().x), 0);
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
    _outKeyboard.setPosition(
        static_cast<float>(this->getSize().x - _keyboardImage.getSize().x),
        static_cast<float>(this->getSize().y - _keyboardImage.getSize().y)
        );
    _outHelp.setPosition(
        static_cast<float>((this->getSize().x - _helpImage.getSize().x) / 2.0),
        static_cast<float>((this->getSize().y - _helpImage.getSize().y) / 2.0)
        );
    _helpImageMode = true;
}
void FractalCanvas::Reset()
{
    // Deletes old fractal and creates a new one.
    if (_fractal->IsRendering())
        _fractal->StopRender();

    const FractalType fractalType = GetFractalType();
    if (fractalType == FractalType::ScriptFractal)
        this->CreateScriptFractal(_scriptData);
    else
        this->CreateFractal(fractalType);

    _fractal = _fractalFactory.GetFractal();
    AttachFractalStatusHandler();
    _fractalPresenter->SetFractal(_fractal);
    _fractalPresenter->SetHandleRightClickZoomBack(false);
    _fractalFactory.SetFormula(_userFormula);
    UpdateSelectionAspectRatio();

    // Deactivates screen pointer.
    _juliaMode = false;
    _orbitMode = false;
    _sliderMode = false;
    if (_coordinateSelector != nullptr)
    {
        delete _coordinateSelector;
        _coordinateSelector = nullptr;
    }
}
void FractalCanvas::SetOrbitMode(const bool mode)
{
    _orbitMode = mode;
    if (_orbitMode)
    {
        _fractal->SetOrbitMode(true);
        if (_coordinateSelector == nullptr)
            _coordinateSelector = new CoordinateSelector(this);
        SeedCoordinateSelectorValue();
        _fractal->SetOrbitPoint(_kReal, _kImaginary);
        Refresh(false);
    }
    else
    {
        _fractal->SetOrbitMode(false);
        if (_coordinateSelector != nullptr && !_juliaMode && !_sliderMode)
        {
            delete _coordinateSelector;
            _coordinateSelector = nullptr;
        }
    }
}
bool FractalCanvas::IsOrbitMode() const
{
    return _orbitMode;
}
void FractalCanvas::SetSliderMode(const bool mode)
{
    _sliderMode = mode;
    if (_sliderMode)
    {
        if (_coordinateSelector == nullptr)
            _coordinateSelector = new CoordinateSelector(this);
        _fractal->SetJuliaMode(true);
    }
    else
    {
        if (_coordinateSelector != nullptr && !_juliaMode && !_orbitMode)
        {
            delete _coordinateSelector;
            _coordinateSelector = nullptr;
        }
        _fractal->SetJuliaMode(false);
    }
}
bool FractalCanvas::IsSliderMode() const
{
    return _sliderMode;
}
void FractalCanvas::SetUserFormula(const FormulaOptions &userFormula)
{
    _userFormula = userFormula;
}
FormulaOptions FractalCanvas::GetFormula()
{
    return _userFormula;
}

void FractalCanvas::SetMainCanvasOverlaysVisible(const bool show)
{
    _showMainCanvasOverlays = show;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void FractalCanvas::OnResize(wxSizeEvent& event)
{
    ResizePresentation(event.GetSize());

    // Adjust position of the keyboard guide.
    if (_keyboardGuideMode)
    {
        if (this->getSize().y > 300 || this->getSize().x > 300)
        {
            _outKeyboard.setPosition(static_cast<float>(this->getSize().x - 120), static_cast<float>(this->getSize().y - 80));
            _outMouse.setPosition(static_cast<float>(this->getSize().x - 90), 0);
            _keyboardGuide = true;
        }
        else
            _keyboardGuide = false;
    }
    if (_helpImageMode)
    {
        _outKeyboard.setPosition(static_cast<float>(this->getSize().x - 120), static_cast<float>(this->getSize().y - 80));
        _outHelp.setPosition(
            static_cast<float>((this->getSize().x - _helpImage.getSize().x) / 2.0),
            static_cast<float>((this->getSize().y - _helpImage.getSize().y) / 2.0)
            );
    }
}

void FractalCanvas::OnClick(wxMouseEvent& event)
{
    if (_zoomToolDragging)
        return;

    if (event.ButtonDown(wxMOUSE_BTN_MIDDLE))
    {
        BeginMousePanAt(event.GetPosition());
        return;
    }

    if (_interactionTool == FractalInteractionTool::Hand)
    {
        if (event.ButtonDown(wxMOUSE_BTN_LEFT))
        {
            _toolPanning = true;
            BeginMousePanAt(event.GetPosition());
        }
        return;
    }

    if (_interactionTool == FractalInteractionTool::Zoom)
    {
        if (event.ButtonDown(wxMOUSE_BTN_LEFT) && !_fractal->IsRendering() && !_fractalPresenter->IsMoving())
        {
            _zoomToolStartPosition = event.GetPosition();
            _zoomToolDragging = _fractalPresenter->BeginInteractiveZoomAtPixel(_zoomToolStartPosition.x, _zoomToolStartPosition.y);

            if (_zoomToolDragging && !HasCapture())
                CaptureMouse();
        }
        else if (event.ButtonDown(wxMOUSE_BTN_RIGHT) && !_fractalPresenter->IsMoving())
            _fractalPresenter->ZoomBack();
        return;
    }

    if (_interactionTool == FractalInteractionTool::PointPicker)
        return;

    // Pointer event.
    if (_juliaMode || _orbitMode || _sliderMode)
    {
        if (_coordinateSelector->ClickEvent(event))
        {
            _prevKReal = _kReal;
            _prevKImag = _kImaginary;

            _onUpdate = true;
            _kReal = _coordinateSelector->GetX(_fractal);
            _kImaginary = _coordinateSelector->GetY(_fractal);
            _coordinateSelectorChange = true;

            if (_orbitMode)
                _fractal->SetOrbitChange();

            _onUpdate = false;
        }
    }
    // Selection event.
    else if (!_fractal->IsRendering() && !_fractalPresenter->IsMoving())
        _selectionRect->ClickEvent(event);

    // Mouse event.
    if (_helpImageMode)
    {
        if (event.ButtonDown(wxMOUSE_BTN_LEFT))
            _helpImageMode = false;
    }

    if (event.ButtonDown(wxMOUSE_BTN_RIGHT) && !_fractalPresenter->IsMoving())
        _fractalPresenter->ZoomBack();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void FractalCanvas::OnReleaseClick(wxMouseEvent& event)
{
    if (event.ButtonUp(wxMOUSE_BTN_MIDDLE) || (event.ButtonUp(wxMOUSE_BTN_LEFT) && _toolPanning))
    {
        EndMousePanGesture();
        return;
    }

    if (event.ButtonUp(wxMOUSE_BTN_LEFT) && _zoomToolDragging)
    {
        CommitZoomToolDrag(event.GetPosition());
        _zoomToolDragging = false;

        if (HasCapture())
            ReleaseMouse();
        return;
    }

    // Selection event.
    if (_juliaMode || _orbitMode || _sliderMode)
        _coordinateSelector->ReleaseClickEvent(event);
    else
    {
        if (!_fractal->IsRendering() && !_fractalPresenter->IsMoving())
        {
            if (_selectionRect->UnClickEvent(event))
                _fractalPresenter->SetAreaOfView(_selectionRect->GetSelection());
        }
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void FractalCanvas::OnMouseWheel(wxMouseEvent& event)
{
    if (_zoomToolDragging)
        return;

    if (_fractalPresenter->IsMoving())
    {
        event.Skip();
        return;
    }

    if (const int rotation = event.GetWheelRotation(); rotation > 0)
    {
        const wxPoint position = event.GetPosition();
        _fractalPresenter->ZoomAtPixel(position.x, position.y);
        UpdateCoordinateSelectorValue();
    }
    else if (rotation < 0)
    {
        _fractalPresenter->ZoomBack();
        UpdateCoordinateSelectorValue();
    }
}

void FractalCanvas::OnMouseCaptureLost(wxMouseCaptureLostEvent& event)
{
    _zoomToolDragging = false;
    _toolPanning = false;
    _mouseWheelPanning = false;
    _fractalPresenter->EndMousePan();
    event.Skip();
}

void FractalCanvas::OnMoveMouse(wxMouseEvent& event)
{
    if (_mouseWheelPanning)
    {
        const wxPoint currentPosition = event.GetPosition();
        ContinueMousePanAt(currentPosition);

        _lastMousePosition = currentPosition;
        _hasLastMousePosition = true;
        EmitStatusText();
        return;
    }

    if (_zoomToolDragging)
    {
        const wxPoint currentPosition = event.GetPosition();
        _fractalPresenter->UpdateInteractiveZoom(CalculateZoomToolScale(currentPosition));
        Refresh(false);
        Update();
        _lastMousePosition = currentPosition;
        _hasLastMousePosition = true;
        EmitStatusText();
        return;
    }

    if (_interactionTool == FractalInteractionTool::PointPicker)
    {
        const wxPoint currentPosition = event.GetPosition();
        ShowPointInfo(currentPosition, InspectPointAt(currentPosition));
        _lastMousePosition = currentPosition;
        _hasLastMousePosition = true;
        EmitStatusText();
        return;
    }

    // Selection event.
    if (_juliaMode || _orbitMode || _sliderMode)
    {
        if (_coordinateSelector->MoveEvent(event))
        {
            _prevKReal = _kReal;
            _prevKImag = _kImaginary;

            _onUpdate = true;
            _kReal = _coordinateSelector->GetX(_fractal);
            _kImaginary = _coordinateSelector->GetY(_fractal);
            _coordinateSelectorChange = true;

            if (_orbitMode)
                _fractal->SetOrbitChange();

            _onUpdate = false;
        }
    }
    else
    {
        if (!_fractal->IsRendering() && !_fractalPresenter->IsMoving())
        {
            if (_selectionRect->MoveEvent(event))
            {
                // We do this to make the selection rectangle feel responsive.
                Refresh(false);
                Update();
            }
        }
    }

    // Updates status bar of the MainFrame when the mouse is moved over the fractal canvas.
    _lastMousePosition = event.GetPosition();
    _hasLastMousePosition = true;
    EmitStatusText();
}

void FractalCanvas::OnLeaveMouse(wxMouseEvent& event)
{
    if (_interactionTool == FractalInteractionTool::PointPicker)
        HidePointInfo();

    event.Skip();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void FractalCanvas::OnKeyDown(wxKeyEvent& event)
{
    if (_fractal->IsRendering())
    {
        event.Skip();
        return;
    }

    switch (event.GetKeyCode())
    {
        case WXK_UP:
            _fractalPresenter->SetMovement(Up);
            break;
        case WXK_DOWN:
            _fractalPresenter->SetMovement(Down);
            break;
        case WXK_LEFT:
            _fractalPresenter->SetMovement(Left);
            break;
        case WXK_RIGHT:
            _fractalPresenter->SetMovement(Right);
            break;
        default: break;
    }

    switch (event.GetUnicodeKey())
    {
        case 'W':
        case 'w':
            _fractalPresenter->SetMovement(Up);
            break;
        case 'S':
        case 's':
            _fractalPresenter->SetMovement(Down);
            break;
        case 'A':
        case 'a':
            _fractalPresenter->SetMovement(Left);
            break;
        case 'D':
        case 'd':
            _fractalPresenter->SetMovement(Right);
            break;
        default: break;
    }
}
// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void FractalCanvas::OnKeyUp(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_UP:
            _fractalPresenter->ReleaseMovement(Up);
            break;
        case WXK_DOWN:
            _fractalPresenter->ReleaseMovement(Down);
            break;
        case WXK_LEFT:
            _fractalPresenter->ReleaseMovement(Left);
            break;
        case WXK_RIGHT:
            _fractalPresenter->ReleaseMovement(Right);
            break;
        default: break;
    }

    switch (event.GetUnicodeKey())
    {
        case 'W':
        case 'w':
            _fractalPresenter->ReleaseMovement(Up);
            break;
        case 'S':
        case 's':
            _fractalPresenter->ReleaseMovement(Down);
            break;
        case 'A':
        case 'a':
            _fractalPresenter->ReleaseMovement(Left);
            break;
        case 'D':
        case 'd':
            _fractalPresenter->ReleaseMovement(Right);
            break;
        default: break;
    }
}
