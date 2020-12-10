#include "JuliaMode.h"
#include "SizeDialogSave.h"
#include "global.h"

bool juliaModeState;

JuliaMode::JuliaMode(FractalCanvas* ptr, FractalType fractalType, Options juliaOpt, wxWindow* _parent)
{
    parent = _parent;
    myJuliaOpt = juliaOpt;
    type = fractalType;
    resizing = false;
    target = ptr;
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
    while(window->GetEvent(event))
    {
        // Window closed.
        if(event.Type == sf::Event::Closed)
        {
            juliaFractal.GetFractalPtr()->StopRender();
            juliaModeState = false;
        }
        if(event.Type == sf::Event::Resized)
        {
            // Ignore subsequent resize events.
            if(!resizing)
            {
                sf::View View(sf::FloatRect(0, 0, window->GetWidth(), window->GetHeight()));
                window->SetView(View);
                window->ConvertCoords(window->GetInput().GetMouseX(), window->GetInput().GetMouseY());
                juliaFractal.GetFractalPtr()->Resize(window);
                play->Resize(window);
                resizing = true;
            }
        }

        // GUI events.
        if(selection->HandleEvents(event))
        {
            juliaFractal.GetFractalPtr()->Resize(selection->GetSeleccion());
        }
        if(play->HandleEvents(event))
        {
            juliaFractal.GetFractalPtr()->ChangeVarGradient();
        }

        // Keyboad events.
        if(event.Type == sf::Event::KeyPressed)
        {
            if(event.Key.Code == sf::Key::F4)
            {
                wxFileDialog * openFileDialog = new wxFileDialog(NULL, wxT(menuSelectFileTxt), wxT(""), wxT("fractal.png"), 
                                                    wxT("PNG file (*.png)|*.png|JPG file (*.jpg)|*.jpg|BMP file (*.bmp)|*.bmp"), wxFD_SAVE);    // Txt: "Select a file name"
                wxString fileName;
                if(openFileDialog->ShowModal() == wxID_OK)
                {
                    fileName = openFileDialog->GetPath();
                    int ext = openFileDialog->GetFilterIndex();
                    string path = string(fileName.mb_str());

                    SizeDialogSave *diag = new SizeDialogSave(NULL, path, ext, type, juliaFractal.GetFractalPtr(), parent);
                    diag->Show(true);
                }
                openFileDialog->Destroy();
            }
            if(event.Key.Code == sf::Key::F5)  // Redraw fractal.
            {
                juliaFractal.GetFractalPtr()->Redraw();
            }
        }

        juliaFractal.GetFractalPtr()->HandleEvents(&event);
    }
    resizing = false;
    if(target->ChangeInPointer())
    {
        juliaFractal.GetFractalPtr()->SetK(target->GetKReal(), target->GetKImaginary());
    }

    // Updates window.
    window->Clear();
    juliaFractal.GetFractalPtr()->Move(window->GetInput());
    juliaFractal.GetFractalPtr()->Show(window);
    selection->Show(window);
    play->Show(window);
    window->Display();
}
void JuliaMode::Run()
{
    // The window must be created in the same thread that will execute it.
    window = new sf::RenderWindow(sf::VideoMode(640, 480, 32), menuJuliaModeTxt);    // Txt: "Julia mode"
    window->SetPosition(parent->GetPosition().x+parent->GetSize().GetWidth()+5, parent->GetPosition().y);
    window->SetFramerateLimit(30);
    sf::Image icon;
    icon.LoadFromFile("Resources/iconPNG.png");
    window->SetIcon(icon.GetWidth(), icon.GetHeight(), icon.GetPixelsPtr());

    juliaFractal.CreateFractal(type, window);
    juliaFractal.GetFractalPtr()->SetOptions(myJuliaOpt, true);
    juliaFractal.GetFractalPtr()->SetJuliaMode(true);

    selection = new SelectRect(window);
    play = new ButtonChange("Resources/Play.tga", "Resources/Stop.tga", 0, 450, window);
    play->SetAnchorage(false, true, true, false);

#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
    while(window->IsOpened())
    {
        Handle_Event();
    }
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}
void JuliaMode::Close()
{
    juliaFractal.GetFractalPtr()->StopRender();
    window->Close();
}