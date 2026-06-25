#include "gui/wx/MainFrame.h"
#include "gui/wx/AppTheme.h"
#include "gui/wx/config/AppConfigStore.h"
#include "utils/AppPaths.h"

wxApp::Appearance ToWxAppearance(const AppAppearance appearance)
{
    switch (appearance)
    {
        case AppAppearance::Dark:
            return wxApp::Appearance::Dark;
        case AppAppearance::Light:
            return wxApp::Appearance::Light;
        case AppAppearance::System:
            return wxApp::Appearance::System;
    }

    return wxApp::Appearance::System;
}

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
* @class App
* @brief wxWidgets application object for wxChaos startup and shutdown.
*
* App enables Windows DPI awareness, loads persisted appearance settings,
* installs the global theme filter, creates the MainFrame, and removes the
* theme filter during application exit.
*/
class App : public wxApp
{
public:
    bool OnInit() override
    {
#ifdef _WIN32
        EnableHighDpiSupport();
#endif
        const AppConfig config = AppConfigStore(AppPaths::ToStdPath(AppPaths::ConfigFile())).Load();
        if (SetAppearance(ToWxAppearance(config.appearance)) == AppearanceResult::Failure)
            wxLogWarning("The requested application appearance is not available.");

        AppTheme::SetAppearance(config.appearance);
        AppTheme::Install();
        const auto main = new MainFrame;
        main->Show();
        return true;
    }

    /// @brief Removes the global theme event filter during application shutdown.
    /// @return Base wxWidgets exit status.
    int OnExit() override
    {
        AppTheme::Uninstall();
        return wxApp::OnExit();
    }
};

// wxWidgets entry point.
wxIMPLEMENT_APP(App); // NOLINT(*-pro-type-static-cast-downcast)
