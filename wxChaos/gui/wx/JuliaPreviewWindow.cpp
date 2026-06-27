#include <algorithm>
#include <cmath>
#include "JuliaPreviewWindow.h"
#include "ImageExportSizeDialog.h"
using namespace std;

wxDEFINE_EVENT(wxEVT_JULIA_MODE_CLOSED, wxCommandEvent);

JuliaPreviewWindow::JuliaPreviewWindow(wxWindow* parent, FractalCanvas* ptr, const FractalType fractalType, const Options& juliaOpt,
                     const wxSize& size) : _event(), m_thread(&JuliaPreviewWindow::Run, this), _pendingRendererOptions()
{
    _parent = parent;
    _myJuliaOpt = juliaOpt;
    _type = fractalType;
    _size = size;
    _target = ptr;

    _window = nullptr;
    _selection = nullptr;
    _play = nullptr;
    _closeRequested.store(false);
    _rendererOptionsPending = false;
    _pendingKReal = 0.0;
    _pendingKImaginary = 0.0;
    _constantPending = false;
    _mouseWheelPanning = false;
    _lastMouseWheelPanPosition = sf::Vector2i(0, 0);

    _juliaFractal.CreateFractal(_type, _size.GetWidth(), _size.GetHeight());
    _fractalPresenter = new FractalPresenter(_juliaFractal.GetFractal());
}

JuliaPreviewWindow::~JuliaPreviewWindow()
{
    delete _selection;
    delete _play;
    delete _fractalPresenter;
    _juliaFractal.DeleteFractal();
    delete _window;
}

void JuliaPreviewWindow::ZoomAtMousePosition(const sf::Vector2i& position) const
{
    constexpr double zoomScale = 0.75;

    const sf::Vector2u screenSize = _juliaFractal.GetFractal()->GetScreenSize();
    const auto screenWidth = static_cast<int>(screenSize.x);
    const auto screenHeight = static_cast<int>(screenSize.y);

    if (screenWidth <= 0 || screenHeight <= 0)
        return;

    const int zoomWidth = std::max(1, static_cast<int>(std::round(static_cast<double>(screenWidth) * zoomScale)));
    const int zoomHeight = std::max(1, static_cast<int>(std::round(static_cast<double>(screenHeight) * zoomScale)));
    const int left = std::clamp(position.x - zoomWidth / 2, 0, screenWidth - zoomWidth);
    const int top = std::clamp(position.y - zoomHeight / 2, 0, screenHeight - zoomHeight);

    _fractalPresenter->SetAreaOfView(sf::IntRect(left, top, zoomWidth, zoomHeight));
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void JuliaPreviewWindow::HandleEvent()
{
    while (_window->pollEvent(_event))
    {
        // Window closed.
        if (_event.type == sf::Event::Closed)
        {
            _juliaFractal.GetFractal()->StopRender();
            _window->close();
        }
        if (_event.type == sf::Event::Resized)
        {
            sf::View View(sf::FloatRect(0, 0, static_cast<float>(_event.size.width), static_cast<float>(_event.size.height)));
            _window->setView(View);
            _fractalPresenter->Resize(_window);
            _play->Resize(_window);
        }

        if (_selection->HandleEvents(_event))
            _fractalPresenter->SetAreaOfView(_selection->GetSelection());
        if (_play->HandleEvents(_event))
            _fractalPresenter->ToggleColorRotation();

        if (_event.type == sf::Event::MouseWheelScrolled && !_fractalPresenter->IsMoving())
        {
            if (_event.mouseWheelScroll.delta > 0.0F)
            {
                if (_juliaFractal.GetFractal()->StopRender())
                    _juliaFractal.GetFractal()->MarkRenderInterrupted();

                ZoomAtMousePosition(sf::Vector2i(_event.mouseWheelScroll.x, _event.mouseWheelScroll.y));
            }
            else if (_event.mouseWheelScroll.delta < 0.0F)
                _fractalPresenter->ZoomBack();
        }

        if (_event.type == sf::Event::MouseButtonPressed && _event.mouseButton.button == sf::Mouse::Middle)
        {
            if (!_juliaFractal.GetFractal()->IsRendering() && _juliaFractal.GetFractal()->IsRendered())
            {
                _mouseWheelPanning = true;
                _lastMouseWheelPanPosition = sf::Vector2i(_event.mouseButton.x, _event.mouseButton.y);
                _fractalPresenter->BeginMousePan();
            }
        }

        if (_event.type == sf::Event::MouseButtonReleased && _event.mouseButton.button == sf::Mouse::Middle)
        {
            _mouseWheelPanning = false;
            _fractalPresenter->EndMousePan();
        }

        if (_event.type == sf::Event::MouseMoved && _mouseWheelPanning)
        {
            const sf::Vector2i currentPosition(_event.mouseMove.x, _event.mouseMove.y);
            const sf::Vector2i delta = currentPosition - _lastMouseWheelPanPosition;

            if (delta.x != 0 || delta.y != 0)
            {
                _fractalPresenter->PanByMousePixels(delta.x, delta.y);
                _lastMouseWheelPanPosition = currentPosition;
            }
        }

        // Keyboad events.
        if (_event.type == sf::Event::KeyPressed)
        {
            if (_event.key.code == sf::Keyboard::F4)
            {
                const auto openFileDialog = new wxFileDialog(nullptr, "Select file name", "", "fractal.png",
                                                             "PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp", wxFD_SAVE);    // Txt: "Select a file name"
                if (openFileDialog->ShowModal() == wxID_OK)
                {
                    wxString fileName = openFileDialog->GetPath();
                    const int ext = openFileDialog->GetFilterIndex();
                    const auto path = string(fileName.mb_str());
                    const auto diag = new ImageExportSizeDialog(nullptr, path, ext, _type, _juliaFractal.GetFractal(), _parent);
                    diag->Show(true);
                }
                openFileDialog->Destroy();
            }
            if (_event.key.code == sf::Keyboard::F5)  // Redraw fractal.
            {
                _fractalPresenter->Redraw();
            }
            // Handle movement
            switch (_event.key.code)
            {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:    _fractalPresenter->SetMovement(Up); break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:  _fractalPresenter->SetMovement(Down); break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:  _fractalPresenter->SetMovement(Left); break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right: _fractalPresenter->SetMovement(Right); break;
                default: break;
            }
        }

        if (_event.type == sf::Event::KeyReleased)
        {
            // Handle movement stop
            switch (_event.key.code)
            {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:    _fractalPresenter->ReleaseMovement(Up); break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:  _fractalPresenter->ReleaseMovement(Down); break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:  _fractalPresenter->ReleaseMovement(Left); break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right: _fractalPresenter->ReleaseMovement(Right); break;
                default: break;
            }
        }

        _fractalPresenter->HandleEvent(_event);
    }

    if (_target->ChangeInPointer())
    {
        _fractalPresenter->SetK(_target->GetKReal(), _target->GetKImaginary());
    }

    // Updates window.
    _window->clear();
    const double elapsedSeconds = _movementClock.restart().asSeconds();
    _fractalPresenter->Move(elapsedSeconds);
    _fractalPresenter->Show(_window, elapsedSeconds);
    _selection->Show(_window);
    _play->Show(_window);
    _window->display();
}

void JuliaPreviewWindow::Run()
{
    // The window must be created in the same thread that will execute it.
    _window = new sf::RenderWindow(sf::VideoMode(_size.GetWidth(), _size.GetHeight()), "Julia mode");

    // Calculate position using wxWidgets and convert to sf::Vector2i
    const wxPoint parentPos = _parent->GetPosition();
    const wxSize parentSize = _parent->GetSize();
    const sf::Vector2i juliaWindowPos(parentPos.x + parentSize.GetWidth() + 5, parentPos.y);
    _window->setPosition(juliaWindowPos);

    if (sf::Image icon; icon.loadFromFile("Resources/iconPNG.png"))
        _window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    _juliaFractal.GetFractal()->SetOptions(_myJuliaOpt, true);
    _juliaFractal.GetFractal()->SetJuliaMode(true);

    _selection = new SelectionRect();
    _play = new ToggleButton("Resources/Play.tga", "Resources/Stop.tga", 0, 4, _window);
    _play->SetAnchor(false, true, true, false);
    _play->Resize(_window);

    while (_window->isOpen())
    {
        Options rendererOptions;
        bool applyRendererOptions = false;
        double kReal = 0.0;
        double kImaginary = 0.0;
        bool applyConstant = false;
        {
            const std::lock_guard lock(_rendererOptionsMutex);
            if (_rendererOptionsPending)
            {
                rendererOptions = _pendingRendererOptions;
                _rendererOptionsPending = false;
                applyRendererOptions = true;
            }
            if (_constantPending)
            {
                kReal = _pendingKReal;
                kImaginary = _pendingKImaginary;
                _constantPending = false;
                applyConstant = true;
            }
        }
        if (applyRendererOptions)
            ApplyRendererOptions(rendererOptions);
        if (applyConstant)
            _fractalPresenter->SetK(kReal, kImaginary);

        if (_closeRequested.load())
        {
            _juliaFractal.GetFractal()->StopRender();
            _window->close();
            break;
        }

        HandleEvent();
    }
    wxQueueEvent(_parent, new wxCommandEvent(wxEVT_JULIA_MODE_CLOSED));
}

void JuliaPreviewWindow::Launch()
{
    m_thread.launch();
}

void JuliaPreviewWindow::Wait()
{
    m_thread.wait();
}

void JuliaPreviewWindow::Close()
{
    _closeRequested.store(true);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void JuliaPreviewWindow::ApplyRendererOptions(const Options& options) const
{
    _fractalPresenter->SetGradient(options.gradient);
    _fractalPresenter->SetGradientSize(options.gradPaletteSize);
    _fractalPresenter->SetColorVariationOffset(options.changeGradient);
    _fractalPresenter->SetColorRotationSpeed(options.colorRotationSpeed);
    _fractalPresenter->SetAlgorithm(options.alg);
    _fractalPresenter->SetRenderingPrecisionMode(options.renderingPrecisionMode);
    _fractalPresenter->SetRelativeColor(options.relativeColor);
    _fractalPresenter->SetExteriorColorMode(options.colorMode);
    _fractalPresenter->SetFractalSetColorMode(options.colorSet);
    _fractalPresenter->SetFractalSetColor(options.fSetColor);
    _fractalPresenter->SetOrbitTrapMode(options.orbitTrapMode);
    _fractalPresenter->SetSmoothRender(options.smoothRender);
}

void JuliaPreviewWindow::SetRendererOptions(const Options& options)
{
    const std::lock_guard lock(_rendererOptionsMutex);
    _pendingRendererOptions = options;
    _rendererOptionsPending = true;
}

void JuliaPreviewWindow::SetConstant(const double real, const double imaginary)
{
    const std::lock_guard lock(_rendererOptionsMutex);
    _pendingKReal = real;
    _pendingKImaginary = imaginary;
    _constantPending = true;
}
