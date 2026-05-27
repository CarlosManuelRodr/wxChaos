#include "JuliaMode.h"
#include "SizeDialogSave.h"
using namespace std;

bool juliaModeState;

JuliaMode::JuliaMode(FractalCanvas* ptr, FractalType fractalType, const Options& juliaOpt, wxWindow* parent)
                    : _event(), m_thread(&JuliaMode::Run, this)
{
    _parent = parent;
    _myJuliaOpt = juliaOpt;
    _type = fractalType;
    _target = ptr;

    _window = nullptr;
    _selection = nullptr;
    _play = nullptr;
}

JuliaMode::~JuliaMode()
{
    delete _selection;
    delete _play;
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
            _sfmlFractal.Resize(_window);
            _play->Resize(_window);
        }

        // The original HandleEvents methods in SelectRect and ButtonChange take SFML 1.6 style events.
        // As we are not supposed to change those files, we'll continue to call them.
        // Fortunately, the event structure members they use are mostly compatible.
        if (_selection->HandleEvents(_event))
        {
            _sfmlFractal.SetAreaOfView(_selection->GetSelection());
        }
        if (_play->HandleEvents(_event))
        {
            _sfmlFractal.ChangeVarGradient();
        }

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
                _sfmlFractal.Redraw();
            }
            // Handle movement
            switch (_event.key.code)
            {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:    _juliaFractal.GetFractalPtr()->SetMovement(Up); break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:  _juliaFractal.GetFractalPtr()->SetMovement(Down); break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:  _juliaFractal.GetFractalPtr()->SetMovement(Left); break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right: _juliaFractal.GetFractalPtr()->SetMovement(Right); break;
                default: break;
            }
        }

        if (_event.type == sf::Event::KeyReleased)
        {
            // Handle movement stop
            switch (_event.key.code)
            {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:    _juliaFractal.GetFractalPtr()->ReleaseMovement(Up); break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:  _juliaFractal.GetFractalPtr()->ReleaseMovement(Down); break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:  _juliaFractal.GetFractalPtr()->ReleaseMovement(Left); break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right: _juliaFractal.GetFractalPtr()->ReleaseMovement(Right); break;
                default: break;
            }
        }

        _sfmlFractal.HandleEvent(_event);
    }

    if (_target->ChangeInPointer())
    {
        _sfmlFractal.SetK(_target->GetKReal(), _target->GetKImaginary());
    }

    // Updates window.
    _window->clear();
    _juliaFractal.GetFractalPtr()->Move(); // Move no longer takes sf::Input
    _sfmlFractal.Show(_window);
    _selection->Show(_window);
    _play->Show(_window);
    _window->display();
}

void JuliaMode::Run()
{
    // The window must be created in the same thread that will execute it.
    _window = new sf::RenderWindow(sf::VideoMode(640, 480), "Julia mode");

    // Calculate position using wxWidgets and convert to sf::Vector2i
    const wxPoint parentPos = _parent->GetPosition();
    const wxSize parentSize = _parent->GetSize();
    const sf::Vector2i juliaWindowPos(parentPos.x + parentSize.GetWidth() + 5, parentPos.y);
    _window->setPosition(juliaWindowPos);

    _window->setFramerateLimit(30);
    sf::Image icon;
    if (icon.loadFromFile("Resources/iconPNG.png"))
        _window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    _juliaFractal.CreateFractal(_type, _window);
    _sfmlFractal.SetFractal(_juliaFractal.GetFractalPtr());
    _juliaFractal.GetFractalPtr()->SetOptions(_myJuliaOpt, true);
    _juliaFractal.GetFractalPtr()->SetJuliaMode(true);

    _selection = new SelectionRect();
    _play = new ToggleButton("Resources/Play.tga", "Resources/Stop.tga", 0, 4, _window);
    _play->SetAnchor(false, true, true, false);

    while (_window->isOpen())
    {
        HandleEvent();
    }
    juliaModeState = false; // Signal that the window is closed
}

void JuliaMode::Launch()
{
    m_thread.launch();
}

void JuliaMode::Wait()
{
    m_thread.wait();
}

void JuliaMode::Terminate()
{
    m_thread.terminate();
}

void JuliaMode::Close() const
{
    if (_window)
        {
        _juliaFractal.GetFractalPtr()->StopRender();
        _window->close();
    }
}
