#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>
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

TEST_CASE("AppConfigStore preserves Logistic Map fractal type")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    const std::filesystem::path path = std::filesystem::temp_directory_path() / "wxchaos_logistic_config_store_test.ini";
    std::filesystem::remove(path);

    AppConfig config;
    config.type = FractalType::LogisticMap;

    const AppConfigStore store(path.string());
    store.Save(config);

    const AppConfig loaded = store.Load();
    CHECK(loaded.type == FractalType::LogisticMap);

    std::filesystem::remove(path);
}

TEST_CASE("AppConfigStore preserves vector fractal types")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    const std::filesystem::path path = std::filesystem::temp_directory_path() / "wxchaos_vector_config_store_test.ini";
    std::filesystem::remove(path);
    const AppConfigStore store(path.string());

    AppConfig config;
    config.type = FractalType::VectorSierpinskiTriangle;
    store.Save(config);
    CHECK(store.Load().type == FractalType::VectorSierpinskiTriangle);

    std::filesystem::remove(path);
}

TEST_CASE("AppConfigStore preserves the Sierpinski carpet type")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    const std::filesystem::path path = std::filesystem::temp_directory_path() / "wxchaos_carpet_config_store_test.ini";
    std::filesystem::remove(path);
    const AppConfigStore store(path.string());

    AppConfig config;
    config.type = FractalType::SierpinskiCarpet;
    store.Save(config);
    CHECK(store.Load().type == FractalType::SierpinskiCarpet);

    std::filesystem::remove(path);
}

TEST_CASE("AppConfigStore persists application language")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    const std::filesystem::path path = std::filesystem::temp_directory_path() / "wxchaos_language_config_store_test.ini";
    std::filesystem::remove(path);

    AppConfig config;
    CHECK(config.language == AppLanguage::System);
    config.language = AppLanguage::Japanese;

    const AppConfigStore store(path.string());
    store.Save(config);

    const AppConfig loaded = store.Load();
    CHECK(loaded.language == AppLanguage::Japanese);

    std::filesystem::remove(path);
}

TEST_CASE("AppConfigStore persists and clamps target frame rate")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "wxchaos_frame_rate_config_store_test.ini";
    std::filesystem::remove(path);
    const AppConfigStore store(path.string());

    AppConfig config;
    CHECK(config.targetFrameRate == 120);
    config.targetFrameRate = 144;
    store.Save(config);
    CHECK(store.Load().targetFrameRate == 144);

    config.targetFrameRate = 10;
    store.Save(config);
    CHECK(store.Load().targetFrameRate == AppConfig::MinimumTargetFrameRate);

    std::filesystem::remove(path);
}

TEST_CASE("AppConfigStore falls back to system language for invalid values")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    const std::filesystem::path path = std::filesystem::temp_directory_path() / "wxchaos_invalid_language_config_test.ini";
    {
        std::ofstream file(path);
        file << "[General]\n";
        file << "language=Klingon\n";
    }

    const AppConfig loaded = AppConfigStore(path.string()).Load();
    CHECK(loaded.language == AppLanguage::System);

    std::filesystem::remove(path);
}

TEST_CASE("AppConfigStore legacy config keeps system language by default")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    const std::filesystem::path path = std::filesystem::temp_directory_path() / "wxchaos_legacy_language_config_test.ini";
    {
        std::ofstream file(path);
        file << "FRACTAL_TYPE=Mandelbrot\n";
    }

    const AppConfig loaded = AppConfigStore(path.string()).Load();
    CHECK(loaded.language == AppLanguage::System);

    std::filesystem::remove(path);
}
