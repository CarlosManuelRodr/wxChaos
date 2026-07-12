#include "AppPaths.h"

#include <algorithm>
#include <wx/dir.h>
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

    wxString ExecutableDir()
    {
        const wxFileName executable(wxStandardPaths::Get().GetExecutablePath());
        return executable.GetPath();
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
        return AppFile({"config.ini"});
    }

    wxString ScriptsDir()
    {
        return AppDirectory({"Scripts"});
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
        return AppFile({"Scripts", filename});
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
        return AppFile({"dump.csv"});
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
