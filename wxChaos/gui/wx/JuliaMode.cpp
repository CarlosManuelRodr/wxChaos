#include "JuliaMode.h"
#include "SizeDialogSave.h"
#include "global.h"

bool juliaModeState;

JuliaMode::JuliaMode(FractalCanvas* ptr, FractalType fractalType, Options juliaOpt, wxWindow* _parent)
    : m_thread(&JuliaMode::Run, this)
{
    parent = _parent;
    myJuliaOpt = juliaOpt;
    type = fractalType;
    target = ptr;

    window = nullptr;
    selection = nullptr;
    play = nullptr;
}

JuliaMode::~JuliaMode()
{
    delete selection;
    delete play;
    juliaFractal.DeleteFractal();
    delete window;
}

void JuliaMode::Handle_Event()
{
    while (window->pollEvent(event))
    {
        // Window closed.
        if (event.type == sf::Event::Closed)
        {
            juliaFractal.GetFractalPtr()->StopRender();
            window->close();
        }
        if (event.type == sf::Event::Resized)
        {
            sf::View View(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
            window->setView(View);
            juliaFractal.GetFractalPtr()->Resize(window);
            play->Resize(window);
        }

        // The original HandleEvents methods in SelectRect and ButtonChange take SFML 1.6 style events.
        // As we are not supposed to change those files, we'll continue to call them.
        // Fortunately, the event structure members they use are mostly compatible.
        if (selection->HandleEvents(event))
        {
            juliaFractal.GetFractalPtr()->SetAreaOfView(selection->GetSeleccion());
        }
        if (play->HandleEvents(event))
        {
            juliaFractal.GetFractalPtr()->ChangeVarGradient();
        }

        // Keyboad events.
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::F4)
            {
                wxFileDialog* openFileDialog = new wxFileDialog(NULL, wxT(menuSelectFileTxt), wxT(""), wxT("fractal.png"),
                    wxT("PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp"), wxFD_SAVE);    // Txt: "Select a file name"
                wxString fileName;
                if (openFileDialog->ShowModal() == wxID_OK)
                {
                    fileName = openFileDialog->GetPath();
                    int ext = openFileDialog->GetFilterIndex();
                    string path = string(fileName.mb_str());

                    SizeDialogSave* diag = new SizeDialogSave(NULL, path, ext, type, juliaFractal.GetFractalPtr(), parent);
                    diag->Show(true);
                }
                openFileDialog->Destroy();
            }
            if (event.key.code == sf::Keyboard::F5)  // Redraw fractal.
            {
                juliaFractal.GetFractalPtr()->Redraw();
            }
            // Handle movement
            switch (event.key.code)
            {
            case sf::Keyboard::W:
            case sf::Keyboard::Up:    juliaFractal.GetFractalPtr()->SetMovement(Direction::Up); break;
            case sf::Keyboard::S:
            case sf::Keyboard::Down:  juliaFractal.GetFractalPtr()->SetMovement(Direction::Down); break;
            case sf::Keyboard::A:
            case sf::Keyboard::Left:  juliaFractal.GetFractalPtr()->SetMovement(Direction::Left); break;
            case sf::Keyboard::D:
            case sf::Keyboard::Right: juliaFractal.GetFractalPtr()->SetMovement(Direction::Right); break;
            default: break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            // Handle movement stop
            switch (event.key.code)
            {
            case sf::Keyboard::W:
            case sf::Keyboard::Up:    juliaFractal.GetFractalPtr()->ReleaseMovement(Direction::Up); break;
            case sf::Keyboard::S:
            case sf::Keyboard::Down:  juliaFractal.GetFractalPtr()->ReleaseMovement(Direction::Down); break;
            case sf::Keyboard::A:
            case sf::Keyboard::Left:  juliaFractal.GetFractalPtr()->ReleaseMovement(Direction::Left); break;
            case sf::Keyboard::D:
            case sf::Keyboard::Right: juliaFractal.GetFractalPtr()->ReleaseMovement(Direction::Right); break;
            default: break;
            }
        }

        juliaFractal.GetFractalPtr()->HandleEvents(&event);
    }

    if (target->ChangeInPointer())
    {
        juliaFractal.GetFractalPtr()->SetK(target->GetKReal(), target->GetKImaginary());
    }

    // Updates window.
    window->clear();
    juliaFractal.GetFractalPtr()->Move(); // Move no longer takes sf::Input
    juliaFractal.GetFractalPtr()->Show(window);
    selection->Show(window);
    play->Show(window);
    window->display();
}

void JuliaMode::Run()
{
    // The window must be created in the same thread that will execute it.
    window = new sf::RenderWindow(sf::VideoMode(640, 480), menuJuliaModeTxt);    // Txt: "Julia mode"

    // Calculate position using wxWidgets and convert to sf::Vector2i
    wxPoint parentPos = parent->GetPosition();
    wxSize parentSize = parent->GetSize();
    sf::Vector2i juliaWindowPos(parentPos.x + parentSize.GetWidth() + 5, parentPos.y);
    window->setPosition(juliaWindowPos);

    window->setFramerateLimit(30);
    sf::Image icon;
    if (icon.loadFromFile("Resources/iconPNG.png"))
        window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    juliaFractal.CreateFractal(type, window);
    juliaFractal.GetFractalPtr()->SetOptions(myJuliaOpt, true);
    juliaFractal.GetFractalPtr()->SetJuliaMode(true);

    selection = new SelectRect(window);
    play = new ButtonChange("Resources/Play.tga", "Resources/Stop.tga", 0, 450, window);
    play->SetAnchorage(false, true, true, false);

#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
    while (window->isOpen())
    {
        Handle_Event();
    }
    juliaModeState = false; // Signal that the window is closed
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
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

void JuliaMode::Close()
{
    if (window) {
        juliaFractal.GetFractalPtr()->StopRender();
        window->close();
    }
}