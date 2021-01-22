#pragma once
#include <string>
#include <vector>
#include <wx/string.h>

#ifdef _WIN32
#include <WTypes.h>
#endif

// File and directory manipulation
bool DirectoryExists(std::string path);
bool FileExists(std::string fileName);
void FSCreateDirectory(std::string directory_name);
void FSDeleteFile(const std::string arg);


// Filepath operations
std::string GetWorkingDirectory();
std::string GetFileExtension(const std::string filename);
wxString GetFileExtension(const wxString filename);
std::string GetFileBaseName(const std::string filename);
wxString GetFileBaseName(const wxString filename);
std::string GetAbsPath(std::vector<std::string> pathList);
wxString GetWxAbsPath(std::vector<std::string> pathList);
std::vector<std::string> FindFilesWithExtension(std::string path, std::string ext);
std::vector<wxString> FindFilesWithExtension(wxString path, std::string ext);
std::string FileNameJoin(std::vector<std::string> pathList);

/**
* @class FileGetter
* @brief Gets all the files within a directory.
*/
class FileGetter
{
#ifdef _WIN32
    WIN32_FIND_DATAA found;
    HANDLE hfind;
    char folderstar[FILENAME_MAX];
    int chk;
#elif __linux__
    std::vector<std::string> files;
    int i;
#endif

public:
    ///@param folder Folder to scan.
    FileGetter(std::string folder);
    int GetNextFile(std::string& fname);
};