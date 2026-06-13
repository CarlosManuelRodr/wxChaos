#include "JuliaMode.h"
#include "SizeDialogSave.h"
using namespace std;

wxDEFINE_EVENT(wxEVT_JULIA_MODE_CLOSED, wxCommandEvent);

JuliaMode::JuliaMode(wxWindow* parent, FractalCanvas* ptr, const FractalType fractalType, const Options& juliaOpt,
                     const wxSize& size) : _event(), m_thread(&JuliaMode::Run, this), _pendingRendererOptions()
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

    _juliaFractal.CreateFractal(_type, _size.GetWidth(), _size.GetHeight());
    _sfmlFractal = new SFMLFractal(_juliaFractal.GetFractalPtr());
}

JuliaMode::~JuliaMode()
{
    delete _selection;
    delete _play;
    delete _sfmlFractal;
    _juliaFractal.DeleteFractal();
    delete _window;
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void JuliaMode::HandleEvent()
{
    while (_window->pollEvent(_event))
    {
        // Window closed.
        if (_event.type == sf::Event::Closed)
        {
            _juliaFractal.GetFractalPtr()->StopRender();
            _window->close();
        }
        if (_event.type == sf::Event::Resized)
        {
            sf::View View(sf::FloatRect(0, 0, static_cast<float>(_event.size.width), static_cast<float>(_event.size.height)));
            _window->setView(View);
            _sfmlFractal->Resize(_window);
            _play->Resize(_window);
        }

        if (_selection->HandleEvents(_event))
            _sfmlFractal->SetAreaOfView(_selection->GetSelection());
        if (_play->HandleEvents(_event))
            _sfmlFractal->ChangeVarGradient();

        // Keyboad events.
        if (_event.type == sf::Event::KeyPressed)
        {
            if (_event.key.code == sf::Keyboard::F4)
            {
                const auto openFileDialog = new wxFileDialog(nullptr, wxT("Select file name"), wxT(""), wxT("fractal.png"),
                                                             wxT("PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp"), wxFD_SAVE);    // Txt: "Select a file name"
                if (openFileDialog->ShowModal() == wxID_OK)
                {
                    wxString fileName = openFileDialog->GetPath();
                    const int ext = openFileDialog->GetFilterIndex();
                    const auto path = string(fileName.mb_str());
                    const auto diag = new SizeDialogSave(nullptr, path, ext, _type, _juliaFractal.GetFractalPtr(), _parent);
                    diag->Show(true);
                }
                openFileDialog->Destroy();
            }
            if (_event.key.code == sf::Keyboard::F5)  // Redraw fractal.
            {
                _sfmlFractal->Redraw();
            }
            // Handle movement
            switch (_event.key.code)
            {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:    _sfmlFractal->SetMovement(Up); break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:  _sfmlFractal->SetMovement(Down); break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:  _sfmlFractal->SetMovement(Left); break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right: _sfmlFractal->SetMovement(Right); break;
                default: break;
            }
        }

        if (_event.type == sf::Event::KeyReleased)
        {
            // Handle movement stop
            switch (_event.key.code)
            {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:    _sfmlFractal->ReleaseMovement(Up); break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:  _sfmlFractal->ReleaseMovement(Down); break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:  _sfmlFractal->ReleaseMovement(Left); break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right: _sfmlFractal->ReleaseMovement(Right); break;
                default: break;
            }
        }

        _sfmlFractal->HandleEvent(_event);
    }

    if (_target->ChangeInPointer())
    {
        _sfmlFractal->SetK(_target->GetKReal(), _target->GetKImaginary());
    }

    // Updates window.
    _window->clear();
    _sfmlFractal->Move();
    _sfmlFractal->Show(_window);
    _selection->Show(_window);
    _play->Show(_window);
    _window->display();
}

void JuliaMode::Run()
{
    // The window must be created in the same thread that will execute it.
    _window = new sf::RenderWindow(sf::VideoMode(_size.GetWidth(), _size.GetHeight()), "Julia mode");

    // Calculate position using wxWidgets and convert to sf::Vector2i
    const wxPoint parentPos = _parent->GetPosition();
    const wxSize parentSize = _parent->GetSize();
    const sf::Vector2i juliaWindowPos(parentPos.x + parentSize.GetWidth() + 5, parentPos.y);
    _window->setPosition(juliaWindowPos);

    _window->setFramerateLimit(30);
    if (sf::Image icon; icon.loadFromFile("Resources/iconPNG.png"))
        _window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    _juliaFractal.GetFractalPtr()->SetOptions(_myJuliaOpt, true);
    _juliaFractal.GetFractalPtr()->SetJuliaMode(true);

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
            _sfmlFractal->SetK(kReal, kImaginary);

        if (_closeRequested.load())
        {
            _juliaFractal.GetFractalPtr()->StopRender();
            _window->close();
            break;
        }

        HandleEvent();
    }
    wxQueueEvent(_parent, new wxCommandEvent(wxEVT_JULIA_MODE_CLOSED));
}

void JuliaMode::Launch()
{
    m_thread.launch();
}

void JuliaMode::Wait()
{
    m_thread.wait();
}

void JuliaMode::Close()
{
    _closeRequested.store(true);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void JuliaMode::ApplyRendererOptions(const Options& options) const
{
    _sfmlFractal->SetGradient(options.gradient);
    _sfmlFractal->SetGradientSize(options.gradPaletteSize);
    _sfmlFractal->SetVarGradient(options.changeGradient);
    _sfmlFractal->SetAlgorithm(options.alg);
    _sfmlFractal->SetRelativeColor(options.relativeColor);
    _sfmlFractal->SetExteriorColorMode(options.colorMode);
    _sfmlFractal->SetFractalSetColorMode(options.colorSet);
    _sfmlFractal->SetFractalSetColor(options.fSetColor);
    _sfmlFractal->SetOrbitTrapMode(options.orbitTrapMode);
    _sfmlFractal->SetSmoothRender(options.smoothRender);
}

void JuliaMode::SetRendererOptions(const Options& options)
{
    const std::lock_guard lock(_rendererOptionsMutex);
    _pendingRendererOptions = options;
    _rendererOptionsPending = true;
}

void JuliaMode::SetConstant(const double real, const double imaginary)
{
    const std::lock_guard lock(_rendererOptionsMutex);
    _pendingKReal = real;
    _pendingKImaginary = imaginary;
    _constantPending = true;
}
