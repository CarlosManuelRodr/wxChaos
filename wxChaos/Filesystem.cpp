#include "global.h"
#include "Filesystem.h"
#include "ConfigParser.h"
using namespace std;

#ifdef _WIN32
#include <direct.h>
#include <Shlobj.h>

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
bool GetFileExtension(const string filename, const string ext)
{
    for (unsigned int i = 0; i < filename.length(); i++)
    {
        if (filename[i] == '.' && i + 1 != filename.length())
        {
            if (filename.substr(i + 1, filename.size() - 1 - i) == ext)
                return true;
        }
    }
    return false;
}
vector<string> FindFilesWithExtension(string path, string ext)
{
    FileGetter fg(path.c_str());

    // Fills vector with script files.
    vector<string> filesInDirectory;
    string filename;
    while (fg.GetNextFile(filename))
    {
        if (GetFileExtension(filename, "as"))
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
        if (GetFileExtension(filename, "as"))
            filesInDirectory.push_back(wxString(filename));
    }

    return filesInDirectory;
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