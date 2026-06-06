#include "gui/wx/MainWindow.h"

enum ProcessDpiAwarenessValue
{
    ProcessDpiUnaware [[maybe_unused]] = 0,
    ProcessSystemDpiAware [[maybe_unused]] = 1,
    ProcessPerMonitorDpiAware = 2
};

void EnableHighDpiSupport()
{
    const HMODULE user32 = LoadLibraryW(L"user32.dll");
    if (user32)
    {
        using SetProcessDpiAwarenessContextFn = BOOL(WINAPI*)(HANDLE);
        const auto setDpiAwarenessContext = reinterpret_cast<SetProcessDpiAwarenessContextFn>(
            GetProcAddress(user32, "SetProcessDpiAwarenessContext"));

        if (setDpiAwarenessContext &&
            setDpiAwarenessContext(reinterpret_cast<HANDLE>(-4))) // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        {
            FreeLibrary(user32);
            return;
        }
    }

    if (const HMODULE shcore = LoadLibraryW(L"Shcore.dll"))
    {
        using SetProcessDpiAwarenessFn = HRESULT(WINAPI*)(ProcessDpiAwarenessValue);
        const auto setProcessDpiAwareness = reinterpret_cast<SetProcessDpiAwarenessFn>(
            GetProcAddress(shcore, "SetProcessDpiAwareness"));

        if (setProcessDpiAwareness)
            setProcessDpiAwareness(ProcessPerMonitorDpiAware);

        FreeLibrary(shcore);
        if (user32)
            FreeLibrary(user32);
        return;
    }

    if (user32)
    {
        using SetProcessDPIAwareFn = BOOL(WINAPI*)();
        const auto setProcessDPIAware = reinterpret_cast<SetProcessDPIAwareFn>(
            GetProcAddress(user32, "SetProcessDPIAware"));

        if (setProcessDPIAware)
            setProcessDPIAware();

        FreeLibrary(user32);
    }
}

/**
* @class MainApp
* @brief Starts the execution of the program.
*/
class MainApp : public wxApp
{
public:
    bool OnInit() override
    {
#ifdef _WIN32
        EnableHighDpiSupport();
#endif
        const auto main = new MainFrame;
        main->Show();
        return true;
    }
};

// wxWidgets entry point.
wxIMPLEMENT_APP(MainApp); // NOLINT(*-pro-type-static-cast-downcast)
