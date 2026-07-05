#include <doctest/doctest.h>

#include <filesystem>
#include <wx/init.h>
#include "config/AppConfigStore.h"

TEST_CASE("AppConfigStore persists anti-aliasing scale")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    const std::filesystem::path path = std::filesystem::temp_directory_path() / "wxchaos_app_config_store_test.ini";
    std::filesystem::remove(path);

    AppConfig config;
    config.antiAliasingScale = 4;

    const AppConfigStore store(path.string());
    store.Save(config);

    const AppConfig loaded = store.Load();
    CHECK(loaded.antiAliasingScale == 4);

    config.antiAliasingScale = 8;
    store.Save(config);

    const AppConfig normalized = store.Load();
    CHECK(normalized.antiAliasingScale == 1);

    std::filesystem::remove(path);
}
