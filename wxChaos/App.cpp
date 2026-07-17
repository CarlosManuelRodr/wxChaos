#include "gui/wx/main/MainFrame.h"
#include "gui/wx/common/AppTheme.h"
#include "gui/wx/common/AppLocalization.h"
#include "gui/wx/config/AppConfigStore.h"
#include "core/SystemUtilities.h"
#include "utils/AppPaths.h"
#include <angelscript.h>

#if wxCHECK_VERSION(3, 3, 0)
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
#endif

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
        Platform::EnableHighDpiSupport();
        if (asPrepareMultithread() < 0)
            return false;

        const AppConfig config = AppConfigStore(AppPaths::ToStdPath(AppPaths::ConfigFile())).Load();
        const AppAppearance appearance = config.appearance;
#if wxCHECK_VERSION(3, 3, 0)
        if (SetAppearance(ToWxAppearance(appearance)) == AppearanceResult::Failure)
            wxLogWarning("The requested application appearance is not available.");
#endif

        AppLocalization::Initialize(config.language);
        AppTheme::SetAppearance(appearance);
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
        asThreadCleanup();
        asUnprepareMultithread();
        return wxApp::OnExit();
    }
};

// wxWidgets entry point. Linux must select the GUI backend before GTK initializes.
#if defined(__linux__)
wxIMPLEMENT_APP_NO_MAIN(App); // NOLINT(*-pro-type-static-cast-downcast)

int main(int argc, char** argv)
{
    Platform::ConfigureGuiBackend();
    return wxEntry(argc, argv);
}
#else
wxIMPLEMENT_APP(App); // NOLINT(*-pro-type-static-cast-downcast)
#endif
