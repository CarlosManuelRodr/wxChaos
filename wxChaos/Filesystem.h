#pragma once
#include <string>
#include <vector>
#include <wx/string.h>

#ifdef _WIN32
    #include <WTypes.h>
#elif __linux__
    #include <vector>
#endif

void ConfigureDirectory();

std::string GetWorkingDirectory(bool opt = false);
std::string GetAbsPath(std::string relPath);
wxString GetWxAbsPath(std::vector<std::string> pathList);

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
    FileGetter(const char* folder);
    int getNextFile(char* fname);
};