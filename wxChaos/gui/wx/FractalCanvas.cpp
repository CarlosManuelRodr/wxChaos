#include <algorithm>
#include <cmath>
#include <string>
#include "AppPaths.h"
#include "FractalCanvas.h"
#include "ImageExportSizeDialog.h"
using namespace std;

wxDEFINE_EVENT(wxEVT_FRACTAL_CANVAS_STATUS_TEXT, wxCommandEvent);

wxString FractalCanvas::FormatStatusCoordinate(const double value)
{
    return wxString::Format("%.15g", value);
}

// Fractal Canvas
FractalCanvas::FractalCanvas(const FractalType fractalType, wxWindow* parent, const wxWindowID id,
                             const wxPoint& position, const wxSize& size, const long style)
                             : wxSFMLCanvas(parent, id, position, size, style)
{
    _fractalType = fractalType;

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
    _iterationsOverlayDirty = true;
    _renderingOverlayDirty = true;
    _hasLastMousePosition = false;
    _mouseWheelPanning = false;
    _toolPanning = false;
    _zoomToolDragging = false;
    _lastMousePosition = wxPoint(0, 0);
    _lastMouseWheelPanPosition = wxPoint(0, 0);
    _zoomToolStartPosition = wxPoint(0, 0);
    _displayedIterations = 0;
    _spinnerFrame = 0;
    _spinnerCenter = sf::Vector2f(0.0F, 0.0F);
    _spinnerRadius = 0.0F;
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
    EnsureFontLoaded();

    _fractalFactory.SetFormula(_userFormula);

    // Initialize GUI elements.
    _selectionRect = new SelectionRect();

    _playToggleButton = new ToggleButton(
        AppPaths::ResourceFileStd({"Play.tga"}),
        AppPaths::ResourceFileStd({"Stop.tga"}),
        0,
        4,
        this);
    _playToggleButton->SetAnchor(false, true, true, false);
    _playToggleButton->Resize(this);

    _screenPointer = new CoordinateSelector(this);
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
    delete _playToggleButton;
    delete _screenPointer;
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

void FractalCanvas::EnsureFontLoaded()
{
    if (!_font.getInfo().family.empty())
        return;

    _font.loadFromFile(AppPaths::ResourceFileStd({"PublicSans-Regular.otf"}));
    _iterationsText.setFont(_font);
    _renderingText.setFont(_font);
}

void FractalCanvas::UpdateIterationsOverlay()
{
    constexpr float horizontalPadding = 8.0F;
    constexpr float verticalPadding = 4.0F;

    _displayedIterations = _fractal->GetIterations();
    _iterationsText.setCharacterSize(25);
    _iterationsText.setString("Iterations: " + std::to_string(_displayedIterations));

    const sf::FloatRect textBounds = _iterationsText.getLocalBounds();
    const auto overlayWidth = static_cast<unsigned int>(std::ceil(textBounds.width + horizontalPadding * 2.0F));
    const auto overlayHeight = static_cast<unsigned int>(std::ceil(textBounds.height + verticalPadding * 2.0F));

    _iterationsOverlay.setFillColor(sf::Color(0, 0, 0, 100));
    _iterationsOverlay.setSize(sf::Vector2f(static_cast<float>(overlayWidth), static_cast<float>(overlayHeight)));
    _iterationsOverlay.setPosition(0.0F, 0.0F);

    _iterationsText.setPosition(horizontalPadding - textBounds.left, verticalPadding - textBounds.top);
    _iterationsOverlayDirty = false;
    _renderingOverlayDirty = true;
}

void FractalCanvas::UpdateRenderingOverlay()
{
    constexpr float horizontalPadding = 8.0F;
    constexpr float verticalPadding = 4.0F;
    constexpr float spinnerDiameter = 18.0F;
    constexpr float spinnerGap = 8.0F;

    _renderingText.setCharacterSize(25);
    _renderingText.setString("Rendering...");

    const sf::FloatRect textBounds = _renderingText.getLocalBounds();
    const float contentHeight = std::max(spinnerDiameter, textBounds.height);
    const auto overlayWidth = static_cast<unsigned int>(
        std::ceil(spinnerDiameter + spinnerGap + textBounds.width + horizontalPadding * 2.0F));
    const auto overlayHeight = static_cast<unsigned int>(std::ceil(contentHeight + verticalPadding * 2.0F));

    _iterationsOverlay.setFillColor(sf::Color(0, 0, 0, 100));
    _iterationsOverlay.setSize(sf::Vector2f(static_cast<float>(overlayWidth), static_cast<float>(overlayHeight)));
    _iterationsOverlay.setPosition(0.0F, 0.0F);

    const float contentCenterY = verticalPadding + contentHeight / 2.0F;
    _spinnerRadius = spinnerDiameter / 2.0F;
    _spinnerCenter = sf::Vector2f(horizontalPadding + _spinnerRadius, contentCenterY);
    _renderingText.setPosition(
        horizontalPadding + spinnerDiameter + spinnerGap - textBounds.left,
        contentCenterY - textBounds.height / 2.0F - textBounds.top);
    _renderingOverlayDirty = false;
}

void FractalCanvas::DrawLoadingSpinner(sf::RenderWindow* window)
{
    constexpr int dotCount = 12;

    for (int i = 0; i < dotCount; ++i)
    {
        constexpr float dotRadius = 2.2F;
        constexpr float pi = 3.14159265358979323846F;
        const int age = (i + static_cast<int>(_spinnerFrame)) % dotCount;
        const auto alpha = static_cast<sf::Uint8>(55 + age * 16);
        const float angle = 2.0F * pi * static_cast<float>(i) / static_cast<float>(dotCount);
        sf::CircleShape dot(dotRadius);
        dot.setOrigin(dotRadius, dotRadius);
        dot.setFillColor(sf::Color(255, 255, 255, alpha));
        dot.setPosition(
            _spinnerCenter.x + std::cos(angle) * _spinnerRadius,
            _spinnerCenter.y + std::sin(angle) * _spinnerRadius
            );
        window->draw(dot);
    }

    _spinnerFrame = (_spinnerFrame + 1) % dotCount;
}

void FractalCanvas::DrawRenderingOverlay(sf::RenderWindow* window)
{
    if (_renderingOverlayDirty)
        UpdateRenderingOverlay();

    window->draw(_iterationsOverlay);
    DrawLoadingSpinner(window);
    window->draw(_renderingText);
    _iterationsOverlayDirty = true;
}

void FractalCanvas::DrawIterationsOverlay(sf::RenderWindow* window)
{
    if (_fractal->IsSnapshotActive())
        return;

    if (_fractal->IsRenderStarted() || _fractal->IsRendering())
    {
        DrawRenderingOverlay(window);
        return;
    }

    if (_iterationsOverlayDirty || _displayedIterations != _fractal->GetIterations())
        UpdateIterationsOverlay();

    window->draw(_iterationsOverlay);
    window->draw(_iterationsText);
}

wxString FractalCanvas::BuildStatusText() const
{
    wxString text;
    if (_fractal == nullptr)
        return text;

    if (_hasLastMousePosition)
    {
        if (_fractalType == FractalType::DoublePendulum)
        {
            text = "θ2: ";
            text += FormatStatusCoordinate(_fractal->GetX(_lastMousePosition.x));
            text += "   θ1: ";
            text += FormatStatusCoordinate(_fractal->GetY(_lastMousePosition.y));
        }
        else if (_fractalType == FractalType::SierpinskiTriangle || _fractalType == FractalType::ScriptFractal)
        {
            text = "x: ";
            text += FormatStatusCoordinate(_fractal->GetX(_lastMousePosition.x));
            text += "   y: ";
            text += FormatStatusCoordinate(_fractal->GetY(_lastMousePosition.y));
        }
        else
        {
            text = "Real: ";
            text += FormatStatusCoordinate(_fractal->GetX(_lastMousePosition.x));
            text += "   Imaginary: ";
            text += FormatStatusCoordinate(_fractal->GetY(_lastMousePosition.y));
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
    const sf::Vector2u sfmlSize = this->getSize();
    if (sfmlSize.x > 0 && sfmlSize.y > 0)
        return sfmlSize;

    const int width = std::max(1, _canvasSize.GetWidth());
    const int height = std::max(1, _canvasSize.GetHeight());
    return {static_cast<unsigned int>(width), static_cast<unsigned int>(height)};
}

void FractalCanvas::ResizePresentation(const wxSize size)
{
    _canvasSize = size;

    const int width = size.GetWidth();
    const int height = size.GetHeight();
    if (width <= 0 || height <= 0)
        return;

    const sf::Vector2u sfmlSize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    if (this->getSize() != sfmlSize)
        this->setSize(sfmlSize);

    _fractalPresenter->Resize(this);
    _playToggleButton->Resize(this);

    if (_screenPointer != nullptr)
        _screenPointer->Resize(this);
}

void FractalCanvas::BeginMousePanAt(const wxPoint position)
{
    if (_fractal->IsRendering() || !_fractal->IsRendered())
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

void FractalCanvas::CommitZoomToolDrag(const wxPoint endPosition)
{
    if (!_zoomToolDragging || _fractalPresenter->IsMoving())
        return;

    _fractalPresenter->CommitInteractiveZoom(CalculateZoomToolScale(endPosition));
}

void FractalCanvas::CancelToolGestures()
{
    _fractalPresenter->CancelInteractiveZoom();
    _zoomToolDragging = false;
    EndMousePanGesture();
}

void FractalCanvas::OnUpdate()
{
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
        if (!_fractal->IsRendering() && _playToggleButton->HandleEvents(_event))
            _fractalPresenter->ToggleColorRotation();

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
                    const auto openFileDialog = new wxFileDialog(
                        this,
                        "Select file name",
                        "",
                        "fractal.png",
                        "PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp",
                        wxFD_SAVE
                        );
                    if (openFileDialog->ShowModal() == wxID_OK)
                    {
                        wxString fileName;
                        fileName = openFileDialog->GetPath();
                        const int ext = openFileDialog->GetFilterIndex();
                        auto path = string(fileName.mb_str());
                        const auto diag = new ImageExportSizeDialog(this, path, ext, _fractalType, _fractal, this);
                        diag->Show(true);
                    }
                    openFileDialog->Destroy();
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

    if (_orbitMode)
        _fractal->SetOrbitPoint(_kReal, _kImaginary);
    if (_sliderMode && _coordinateSelectorChange)
        _fractalPresenter->SetK(_kReal, _kImaginary);

    const double elapsedSeconds = _movementClock.restart().asSeconds();
    _fractalPresenter->Move(elapsedSeconds);
    _fractalPresenter->Show(this, elapsedSeconds);
    DrawIterationsOverlay(this);

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
            _screenPointer->Show(this);
    }
    _playToggleButton->Show(this);
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
    _interactionTool = tool;

    switch (_interactionTool)
    {
        case FractalInteractionTool::Hand:
            SetCursor(wxCursor(wxCURSOR_HAND));
            break;
        case FractalInteractionTool::Zoom:
            SetCursor(wxCursor(wxCURSOR_CROSS));
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
    return _fractalType;
}
bool FractalCanvas::CanAbortRender() const
{
    return _fractal != nullptr && _fractal->IsRendering() && _fractalType != FractalType::SierpinskiTriangle;
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
    _fractalType = type;
    _fractal = _fractalFactory.GetFractal();
    AttachFractalStatusHandler();
    _fractalPresenter->SetFractal(_fractal);
    _fractalPresenter->SetHandleRightClickZoomBack(false);
    _iterationsOverlayDirty = true;
    _fractalFactory.SetFormula(_userFormula);

    // Deletes screen pointer if active.
    if (_orbitMode || _sliderMode)
    {
        _orbitMode = false;
        _sliderMode = false;
        if (!_juliaMode)
        {
            delete _screenPointer;
            _screenPointer = nullptr;
        }
    }

    _playToggleButton->Reset();

}
void FractalCanvas::ChangeToScript(const ScriptData &scriptData)
{
    // Deletes old fractal and creates a new one.
    _fractalType = FractalType::ScriptFractal;
    _scriptData = scriptData;
    this->CreateScriptFractal(_scriptData);
    _fractal = _fractalFactory.GetFractal();
    AttachFractalStatusHandler();
    _fractalPresenter->SetFractal(_fractal);
    _fractalPresenter->SetHandleRightClickZoomBack(false);
    _iterationsOverlayDirty = true;

    // Deletes screen pointer if active.
    if (_orbitMode || _sliderMode)
    {
        _orbitMode = false;
        _sliderMode = false;
        if (!_juliaMode)
        {
            delete _screenPointer;
            _screenPointer = nullptr;
        }
    }

    _playToggleButton->Reset();
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

    if (_fractalType == FractalType::ScriptFractal)
        this->CreateScriptFractal(_scriptData);
    else
        this->CreateFractal(_fractalType);

    _fractal = _fractalFactory.GetFractal();
    AttachFractalStatusHandler();
    _fractalPresenter->SetFractal(_fractal);
    _fractalPresenter->SetHandleRightClickZoomBack(false);
    _iterationsOverlayDirty = true;
    _fractalFactory.SetFormula(_userFormula);
    _playToggleButton->Reset();

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
        _fractal->SetOrbitMode(true);
        if (_screenPointer == nullptr)
            _screenPointer = new CoordinateSelector(this);
    }
    else
    {
        _fractal->SetOrbitMode(false);
        if (_screenPointer != nullptr && !_juliaMode && !_sliderMode)
        {
            delete _screenPointer;
            _screenPointer = nullptr;
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
        if (_screenPointer == nullptr)
            _screenPointer = new CoordinateSelector(this);
        _fractal->SetJuliaMode(true);
    }
    else
    {
        if (_screenPointer != nullptr && !_juliaMode && !_orbitMode)
        {
            delete _screenPointer;
            _screenPointer = nullptr;
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

    // Pointer event.
    if (_juliaMode || _orbitMode || _sliderMode)
    {
        if (_screenPointer->ClickEvent(event))
        {
            _prevKReal = _kReal;
            _prevKImag = _kImaginary;

            _onUpdate = true;
            _kReal = _screenPointer->GetX(_fractal);
            _kImaginary = _screenPointer->GetY(_fractal);
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
        _screenPointer->ReleaseClickEvent(event);
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
    if (_fractalPresenter->IsMoving())
    {
        event.Skip();
        return;
    }

    if (const int rotation = event.GetWheelRotation(); rotation > 0)
    {
        const wxPoint position = event.GetPosition();
        _fractalPresenter->ZoomAtPixel(position.x, position.y);
    }
    else if (rotation < 0)
        _fractalPresenter->ZoomBack();
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

    // Selection event.
    if (_juliaMode || _orbitMode || _sliderMode)
    {
        if (_screenPointer->MoveEvent(event))
        {
            _prevKReal = _kReal;
            _prevKImag = _kImaginary;

            _onUpdate = true;
            _kReal = _screenPointer->GetX(_fractal);
            _kImaginary = _screenPointer->GetY(_fractal);
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
        {
            _fractalPresenter->SetMovement(Up);
            break;
        }
        case WXK_DOWN:
        {
            _fractalPresenter->SetMovement(Down);
            break;
        }
        case WXK_LEFT:
        {
            _fractalPresenter->SetMovement(Left);
            break;
        }
        case WXK_RIGHT:
        {
            _fractalPresenter->SetMovement(Right);
            break;
        }
        default: break;
    }

    const wxChar key = event.GetUnicodeKey();
    if (key == 'W' || key == 'w')
        _fractalPresenter->SetMovement(Up);
    else if (key == 'S' || key == 's')
        _fractalPresenter->SetMovement(Down);
    else if (key == 'A' || key == 'a')
        _fractalPresenter->SetMovement(Left);
    else if (key == 'D' || key == 'd')
        _fractalPresenter->SetMovement(Right);
}
// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void FractalCanvas::OnKeyUp(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_UP:
        {
            _fractalPresenter->ReleaseMovement(Up);
            break;
        }
        case WXK_DOWN:
        {
            _fractalPresenter->ReleaseMovement(Down);
            break;
        }
        case WXK_LEFT:
        {
            _fractalPresenter->ReleaseMovement(Left);
            break;
        }
        case WXK_RIGHT:
        {
            _fractalPresenter->ReleaseMovement(Right);
            break;
        }
        default: break;
    }

    const wxChar key = event.GetUnicodeKey();
    if (key == 'W' || key == 'w')
        _fractalPresenter->ReleaseMovement(Up);
    else if (key == 'S' || key == 's')
        _fractalPresenter->ReleaseMovement(Down);
    else if (key == 'A' || key == 'a')
        _fractalPresenter->ReleaseMovement(Left);
    else if (key == 'D' || key == 'd')
        _fractalPresenter->ReleaseMovement(Right);
}
