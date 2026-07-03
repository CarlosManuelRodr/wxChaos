#pragma once

#include <string>
#include <vector>
#include <wx/string.h>

namespace AppPaths
{
    wxString ExecutableDir();
    wxString AppFile(const std::vector<wxString>& pathParts);
    std::string AppFileStd(const std::vector<wxString>& pathParts);
    wxString AppDirectory(const std::vector<wxString>& pathParts);
    std::string AppDirectoryStd(const std::vector<wxString>& pathParts);

    wxString ResourceFile(const std::vector<wxString>& pathParts);
    std::string ResourceFileStd(const std::vector<wxString>& pathParts);
    wxString DocFile(const wxString& filename);
    std::string DocFileStd(const wxString& filename);
    wxString ConfigFile();

    wxString ScriptsDir();
    std::string ScriptsDirStd();
    wxString ScriptFile(const wxString& filename);
    std::string ScriptFileStd(const wxString& filename);
    std::vector<std::string> ScriptFilenames();

    wxString FfmpegFile();
    std::string FfmpegFileStd();
    wxString DumpFile();

    wxString Join(const wxString& directory, const wxString& filename);
    std::string JoinStd(const std::string& directory, const std::string& filename);
    wxString BaseName(const wxString& filename);
    std::string BaseName(const std::string& filename);

    bool EnsureDirectory(const wxString& directory);
    bool RemoveFile(const std::string& filename);
    std::string ToStdPath(const wxString& path);
}
