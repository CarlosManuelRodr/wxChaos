#include <fstream>
#include "global.h"
#include "Filesystem.h"
#include "ConfigParser.h"
using namespace std;

#ifdef _WIN32
#include <direct.h>
#include <Shlobj.h>
#include <windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#elif __linux__
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

bool DirectoryExists(std::string path)
{
    const char* pzPath = path.c_str();
#if defined(__linux__) || defined(__APPLE__)
    if (pzPath == NULL) return false;

    DIR* pDir;
    bool bExists = false;

    pDir = opendir(pzPath);

    if (pDir != NULL)
    {
        bExists = true;
        (void)closedir(pDir);
    }

    return bExists;
#elif defined(_WIN32)
    DWORD ftyp = GetFileAttributesA(pzPath);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;
    return false;
#endif
}
bool FileExists(std::string fileName)
{
    std::ifstream infile(fileName.c_str());
    bool res = infile.is_open();
    if (res)
        infile.close();
    return res;
}
void FSCreateDirectory(std::string directory_name)
{
#if defined(__linux__) || defined(__APPLE__)
    s_mkpath(arg, 0755);
#elif defined(_WIN32)
    std::wstring w_arg = std::wstring(directory_name.begin(), directory_name.end());
    CreateDirectory(w_arg.c_str(), NULL);
#endif
}
void FSDeleteFile(const std::string arg)
{
#if defined(__linux__) || defined(__APPLE__)
    ::remove(arg.c_str());
#elif defined(_WIN32)
    std::ifstream infile(arg);
    if (infile.is_open())
    {
        infile.close();
        std::wstring w_arg = std::wstring(arg.begin(), arg.end());
        DeleteFile(w_arg.c_str());
    }
#endif
}

string GetWorkingDirectory()
{
    string folderPath = "";
#ifdef _WIN32
    // Gets the current directory.
    char cCurrentPath[FILENAME_MAX];
    _getcwd(cCurrentPath, sizeof(cCurrentPath));
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    folderPath += cCurrentPath;
#elif __linux__
    char cCurrentPath[FILENAME_MAX];

    if (!getcwd(cCurrentPath, sizeof(cCurrentPath))) {
        return string("");
    }
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    folderPath += cCurrentPath;
#endif
    return folderPath;
}

string GetAbsPath(std::vector<std::string> pathList)
{
    string path = GetWorkingDirectory();
    for (string element : pathList)
    {
#ifdef __linux__
        path += "/";
#elif _WIN32
        path += "\\";
#endif
        path += element;
    }
    return path;
}
wxString GetWxAbsPath(vector<string> pathList)
{
    string path = GetWorkingDirectory();
    for (string element : pathList)
    {
#ifdef __linux__
        path += "/";
#elif _WIN32
        path += "\\";
#endif
        path += element;
    }
    return wxString(path.c_str(), wxConvUTF8);
}
string GetFileExtension(const string filename)
{
    size_t lastDotPosition = filename.find_last_of(".");
    if (lastDotPosition != string::npos)
        return filename.substr(lastDotPosition + 1);
    else
        return string("");
}
wxString GetFileExtension(const wxString filename)
{
    size_t lastDotPosition = filename.find_last_of(".");
    if (lastDotPosition != wxString::npos)
        return filename.SubString(lastDotPosition, wxString::npos);
    else
        return wxT("");
}
string GetFileBaseName(const string filename)
{
#ifdef __linux__
    const string slash = "/";
#elif _WIN32
    const string slash = "\\";
#endif
    size_t lastDotPosition = filename.find_last_of(".");
    size_t lastSlashPosition = filename.find_last_of(slash);

    if (lastDotPosition != string::npos && lastSlashPosition != string::npos && lastDotPosition > lastSlashPosition)
        return filename.substr(lastSlashPosition + 1, lastDotPosition - lastSlashPosition - 1);
    else
        return string();
}
wxString GetFileBaseName(const wxString filename)
{
    size_t lastDotPosition = filename.find_last_of(".");

    if (lastDotPosition != string::npos)
        return filename.SubString(0, lastDotPosition - 1);
    else
        return wxT("");
}
vector<string> FindFilesWithExtension(string path, string ext)
{
    FileGetter fg(path.c_str());

    // Fills vector with script files.
    vector<string> filesInDirectory;
    string filename;
    while (fg.GetNextFile(filename))
    {
        if (GetFileExtension(filename) == "as")
            filesInDirectory.push_back(filename);
    }
    return filesInDirectory;
}
std::vector<wxString> FindFilesWithExtension(wxString path, std::string ext)
{
    FileGetter fg(path.ToStdString());

    // Fills vector with script files.
    vector<wxString> filesInDirectory;
    string filename;
    while (fg.GetNextFile(filename))
    {
        if (GetFileExtension(filename) == "as")
            filesInDirectory.push_back(wxString(filename));
    }

    return filesInDirectory;
}
std::string FileNameJoin(std::vector<std::string> pathList)
{
    string path = "";
    for (unsigned i = 0; i < pathList.size(); i++)
    {
        if (i != 0)
        {
#ifdef __linux__
            path += "/";
#elif _WIN32
            path += "\\";
#endif
        }
        path += pathList[i];
    }
    return path;
}

// FileGetter
FileGetter::FileGetter(string folder)
{
#ifdef _WIN32
    chk = -1;
    sprintf_s(folderstar, "%s\\*.*", folder.c_str());
    hfind = FindFirstFileA(folderstar, &found);
    FindNextFileA(hfind, &found);
#elif __linux__
    struct dirent* de = NULL;
    DIR* d = NULL;
    d = opendir(folder);

    if (d)
    {
        while (de = readdir(d))
        {
            files.push_back(string(de->d_name));
        }
    }
    i = 0;
#endif
}
int FileGetter::GetNextFile(string& fname)
{
#ifdef _WIN32
    chk = FindNextFileA(hfind, &found);
    if (chk)
        fname = found.cFileName;

    return chk;
#elif __linux__
    if (i < files.size())
    {
        strcpy(fname, files[i].c_str());
        i++;
        return 1;
    }
    else
        return 0;
#endif
}