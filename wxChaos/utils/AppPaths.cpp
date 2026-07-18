#include "AppPaths.h"

#include <algorithm>
#include <utility>
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

namespace AppPaths
{
    static wxString BuildFilePath(const wxString& baseDir, const std::vector<wxString>& pathParts)
    {
        wxFileName fileName;
        fileName.AssignDir(baseDir);

        if (pathParts.empty())
            return fileName.GetPath();

        for (size_t i = 0; i + 1 < pathParts.size(); ++i)
            fileName.AppendDir(pathParts[i]);

        fileName.SetFullName(pathParts.back());
        return fileName.GetFullPath();
    }

    static wxString BuildDirectoryPath(const wxString& baseDir, const std::vector<wxString>& pathParts)
    {
        wxFileName directory;
        directory.AssignDir(baseDir);

        for (const wxString& part : pathParts)
            directory.AppendDir(part);

        return directory.GetPath();
    }

    UserDataInitializer::UserDataInitializer(wxString installedDataDir, wxString userDataDir, wxString localDataDir)
        : _installedDataDir(std::move(installedDataDir)),
          _userDataDir(std::move(userDataDir)),
          _localDataDir(std::move(localDataDir))
    {
    }

    bool UserDataInitializer::Initialize() const
    {
        if (!EnsureDirectory(_userDataDir) || !EnsureDirectory(_localDataDir))
            return false;

        return CopyLegacyConfig() && SeedScripts();
    }

    bool UserDataInitializer::CopyLegacyConfig() const
    {
        const wxString userConfig = BuildFilePath(_userDataDir, {"config.ini"});
        const wxString legacyConfig = BuildFilePath(_installedDataDir, {"config.ini"});
        if (userConfig == legacyConfig || wxFileExists(userConfig) || !wxFileExists(legacyConfig))
            return true;

        return wxCopyFile(legacyConfig, userConfig, false);
    }

    bool UserDataInitializer::SeedScripts() const
    {
        const wxString userScripts = BuildDirectoryPath(_userDataDir, {"Scripts"});
        if (wxDirExists(userScripts))
            return true;
        if (!EnsureDirectory(userScripts))
            return false;

        const wxString bundledScripts = BuildDirectoryPath(_installedDataDir, {"Scripts"});
        if (!CopyScriptsFrom(bundledScripts, userScripts))
            return false;

        const wxString legacyScripts = BuildDirectoryPath(_installedDataDir, {"UserScripts"});
        return CopyScriptsFrom(legacyScripts, userScripts);
    }

    bool UserDataInitializer::CopyScriptsFrom(const wxString& sourceDirectory,
                                              const wxString& destinationDirectory) const
    {
        const wxDir directory(sourceDirectory);
        if (!directory.IsOpened())
            return true;

        wxString filename;
        bool hasFile = directory.GetFirst(&filename, "*.as", wxDIR_FILES);
        while (hasFile)
        {
            const wxString source = BuildFilePath(sourceDirectory, {filename});
            const wxString destination = BuildFilePath(destinationDirectory, {filename});
            if (!wxFileExists(destination) && !wxCopyFile(source, destination, false))
                return false;
            hasFile = directory.GetNext(&filename);
        }

        return true;
    }

    wxString ExecutableDir()
    {
        const wxFileName executable(wxStandardPaths::Get().GetExecutablePath());
        return executable.GetPath();
    }

    wxString UserDataDir()
    {
        return IsPortable() ? ExecutableDir() : wxStandardPaths::Get().GetUserDataDir();
    }

    wxString LocalUserDataDir()
    {
        return IsPortable() ? ExecutableDir() : wxStandardPaths::Get().GetUserLocalDataDir();
    }

    bool IsPortable()
    {
        return wxFileExists(AppFile({"portable.flag"}));
    }

    bool InitializeUserData()
    {
        return UserDataInitializer(ExecutableDir(), UserDataDir(), LocalUserDataDir()).Initialize();
    }

    wxString AppFile(const std::vector<wxString>& pathParts)
    {
        return BuildFilePath(ExecutableDir(), pathParts);
    }

    std::string AppFileStd(const std::vector<wxString>& pathParts)
    {
        return ToStdPath(AppFile(pathParts));
    }

    wxString AppDirectory(const std::vector<wxString>& pathParts)
    {
        return BuildDirectoryPath(ExecutableDir(), pathParts);
    }

    std::string AppDirectoryStd(const std::vector<wxString>& pathParts)
    {
        return ToStdPath(AppDirectory(pathParts));
    }

    wxString ResourceFile(const std::vector<wxString>& pathParts)
    {
        std::vector<wxString> appPath = {"Resources"};
        appPath.insert(appPath.end(), pathParts.begin(), pathParts.end());
        return AppFile(appPath);
    }

    std::string ResourceFileStd(const std::vector<wxString>& pathParts)
    {
        return ToStdPath(ResourceFile(pathParts));
    }

    wxString DocFile(const wxString& filename)
    {
        return AppFile({"Doc", filename});
    }

    std::string DocFileStd(const wxString& filename)
    {
        return ToStdPath(DocFile(filename));
    }

    wxString ConfigFile()
    {
        return BuildFilePath(UserDataDir(), {"config.ini"});
    }

    wxString ScriptsDir()
    {
        return BuildDirectoryPath(UserDataDir(), {"Scripts"});
    }

    std::string ScriptsDirStd()
    {
        return ToStdPath(ScriptsDir());
    }

    wxString ScriptSamplesDir()
    {
        return AppDirectory({"ScriptSamples"});
    }

    wxString ScriptFile(const wxString& filename)
    {
        return BuildFilePath(ScriptsDir(), {filename});
    }

    std::string ScriptFileStd(const wxString& filename)
    {
        return ToStdPath(ScriptFile(filename));
    }

    std::vector<std::string> ScriptFilenames()
    {
        std::vector<std::string> scriptFiles;
        const wxDir directory(ScriptsDir());

        if (!directory.IsOpened())
            return scriptFiles;

        wxString filename;
        bool hasFile = directory.GetFirst(&filename, "*.as", wxDIR_FILES);
        while (hasFile)
        {
            scriptFiles.push_back(ToStdPath(filename));
            hasFile = directory.GetNext(&filename);
        }

        std::sort(scriptFiles.begin(), scriptFiles.end());
        return scriptFiles;
    }

    wxString DumpFile()
    {
        return BuildFilePath(LocalUserDataDir(), {"dump.csv"});
    }

    wxString Join(const wxString& directory, const wxString& filename)
    {
        wxFileName fileName(directory, filename);
        return fileName.GetFullPath();
    }

    std::string JoinStd(const std::string& directory, const std::string& filename)
    {
        return ToStdPath(Join(wxString::FromUTF8(directory.c_str()), wxString::FromUTF8(filename.c_str())));
    }

    wxString BaseName(const wxString& filename)
    {
        return wxFileName(filename).GetName();
    }

    std::string BaseName(const std::string& filename)
    {
        return ToStdPath(BaseName(wxString::FromUTF8(filename.c_str())));
    }

    bool EnsureDirectory(const wxString& directory)
    {
        if (wxDirExists(directory))
            return true;

        return wxFileName::Mkdir(directory, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }

    bool RemoveFile(const std::string& filename)
    {
        const wxString wxFilename = wxString::FromUTF8(filename.c_str());
        return wxFileExists(wxFilename) && wxRemoveFile(wxFilename);
    }

    std::string ToStdPath(const wxString& path)
    {
        const wxScopedCharBuffer buffer = path.ToUTF8();
        return buffer ? std::string(buffer.data()) : std::string();
    }
}
