#include "gui/wx/main/MainFrame.h"
#include "gui/wx/common/AppTheme.h"
#include "gui/wx/common/AppLocalization.h"
#include "gui/wx/config/AppConfigStore.h"
#include "core/SystemUtilities.h"
#include "utils/AppPaths.h"
#include <angelscript.h>

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
        if (SetAppearance(ToWxAppearance(appearance)) == AppearanceResult::Failure)
            wxLogWarning("The requested application appearance is not available.");

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

// wxWidgets entry point.
wxIMPLEMENT_APP(App); // NOLINT(*-pro-type-static-cast-downcast)
