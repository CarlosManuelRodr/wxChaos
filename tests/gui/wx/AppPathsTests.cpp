#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <wx/filename.h>
#include <wx/init.h>
#include "AppPaths.h"

class TemporaryAppDirectories
{
public:
    TemporaryAppDirectories()
    {
        const auto uniqueName = "wxchaos_app_paths_" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count());
        root = std::filesystem::temp_directory_path() / uniqueName;
        installed = root / "installed";
        roaming = root / "roaming";
        local = root / "local";
        std::filesystem::create_directories(installed / "Scripts");
    }

    ~TemporaryAppDirectories()
    {
        std::error_code error;
        std::filesystem::remove_all(root, error);
    }

    void WriteInstalledFile(const std::filesystem::path& relativePath, const std::string& contents) const
    {
        WriteFile(installed / relativePath, contents);
    }

    void WriteRoamingFile(const std::filesystem::path& relativePath, const std::string& contents) const
    {
        WriteFile(roaming / relativePath, contents);
    }

    std::filesystem::path root;
    std::filesystem::path installed;
    std::filesystem::path roaming;
    std::filesystem::path local;

private:
    static void WriteFile(const std::filesystem::path& path, const std::string& contents)
    {
        std::ofstream file(path);
        file << contents;
    }
};

TEST_CASE("User data initialization migrates config and seeds scripts")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    TemporaryAppDirectories directories;
    directories.WriteInstalledFile("config.ini", "legacy=true\n");
    directories.WriteInstalledFile("Scripts/Mandelbrot.as", "default script\n");

    const AppPaths::UserDataInitializer initializer(
        directories.installed.wstring(), directories.roaming.wstring(), directories.local.wstring());

    REQUIRE(initializer.Initialize());
    CHECK(std::filesystem::exists(directories.roaming / "config.ini"));
    CHECK(std::filesystem::exists(directories.roaming / "Scripts" / "Mandelbrot.as"));
    CHECK(std::filesystem::exists(directories.local));
}

TEST_CASE("User data initialization never overwrites existing user files")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    TemporaryAppDirectories directories;
    std::filesystem::create_directories(directories.roaming / "Scripts");
    directories.WriteInstalledFile("config.ini", "installed config\n");
    directories.WriteInstalledFile("Scripts/Mandelbrot.as", "installed script\n");
    directories.WriteRoamingFile("config.ini", "user config\n");
    directories.WriteRoamingFile("Scripts/Mandelbrot.as", "user script\n");

    const AppPaths::UserDataInitializer initializer(
        directories.installed.wstring(), directories.roaming.wstring(), directories.local.wstring());

    REQUIRE(initializer.Initialize());

    std::ifstream config(directories.roaming / "config.ini");
    std::ifstream script(directories.roaming / "Scripts" / "Mandelbrot.as");
    CHECK(std::string(std::istreambuf_iterator<char>(config), std::istreambuf_iterator<char>()) == "user config\n");
    CHECK(std::string(std::istreambuf_iterator<char>(script), std::istreambuf_iterator<char>()) == "user script\n");
}

TEST_CASE("User data initialization imports legacy UserScripts")
{
    wxInitializer wx;
    REQUIRE(wx.IsOk());

    TemporaryAppDirectories directories;
    std::filesystem::create_directories(directories.installed / "UserScripts");
    directories.WriteInstalledFile("UserScripts/Legacy.as", "legacy script\n");

    const AppPaths::UserDataInitializer initializer(
        directories.installed.wstring(), directories.roaming.wstring(), directories.local.wstring());

    REQUIRE(initializer.Initialize());
    CHECK(std::filesystem::exists(directories.roaming / "Scripts" / "Legacy.as"));
}
