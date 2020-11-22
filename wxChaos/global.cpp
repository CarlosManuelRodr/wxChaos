#include "global.h"
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

bool usingLocalDirect = false;

void ConfigureDirectory()
{
#ifdef _WIN32
    if(!FORCE_APP_DIRECT)
    {
        // Gets the user directory.
        string folderPath;
        char path[ MAX_PATH ];
        SHGetFolderPathA( NULL, CSIDL_PROFILE, NULL, 0, path );
        folderPath = path;
        folderPath += "/wxChaos/";
        FileGetter fg(folderPath.c_str());

        bool found = false;
        char text[FILENAME_MAX];
        while(fg.getNextFile(text))
        {
            if(string(text) == "config.ini") found = true;
        }
        if(SETUP_VERSION && !found)
        {
            // To Reimplement
        }
        else if(found) usingLocalDirect = false;
    }
#elif __linux__
    if(SETUP_VERSION) usingLocalDirect = false;
    // Gets the user directory.
    string folderPath = GetWorkingDirectory();
    FileGetter fg(folderPath.c_str());

    bool found = false;
    char text[FILENAME_MAX];
    while(fg.getNextFile(text))
    {
        if(string(text) == "config.ini") found = true;
    }
    if(found)
    {
        string version;
        ConfigParser p(folderPath + string("/config.ini"));
        p.StringArgToVar(version, "APP_VERSION", "1.0.0");
        if(version != APP_VERSION) found = false;
    }
    if(!found)
    {
        int r;
        r = system("mkdir ~/.config/wxChaos"); assert(r >= 0);
        r = system("cp -r /opt/extras.ubuntu.com/wxChaos/Doc ~/.config/wxChaos"); assert(r >= 0);
        r = system("cp -r /opt/extras.ubuntu.com/wxChaos/Resources ~/.config/wxChaos"); assert(r >= 0);
        r = system("cp -r /opt/extras.ubuntu.com/wxChaos/ScriptSamples ~/.config/wxChaos"); assert(r >= 0);
        r = system("cp -r /opt/extras.ubuntu.com/wxChaos/UserScripts ~/.config/wxChaos"); assert(r >= 0);
        r = system("cp /opt/extras.ubuntu.com/wxChaos/config.ini ~/.config/wxChaos"); assert(r >= 0);
    }
#endif
}

string GetWorkingDirectory(const bool opt)
{
    string folderPath = "";
    if(SETUP_VERSION && !opt)
    {
        if(usingLocalDirect)
        {
        #ifdef _WIN32
            // Gets the current directory.
            char cCurrentPath[FILENAME_MAX];
            _getcwd(cCurrentPath, sizeof(cCurrentPath));
            cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
            folderPath += cCurrentPath;
        #elif __linux__
            char cCurrentPath[FILENAME_MAX];

            if(!getcwd(cCurrentPath, sizeof(cCurrentPath))){
                return string("");
            }
            cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
            folderPath += cCurrentPath;
        #endif
        }
        else
        {
        #ifdef _WIN32
            // Gets the user directory.
            char path[ MAX_PATH ];
            SHGetFolderPathA( NULL, CSIDL_PROFILE, NULL, 0, path );
            folderPath = path;
            folderPath += "\\wxChaos";
        #elif __linux__
            struct passwd *pw = getpwuid(getuid());
            const char *homedir = pw->pw_dir;
            folderPath = string(homedir);
            folderPath += "/.config/wxChaos";
        #endif
        }
    }
    else
    {
    #ifdef _WIN32
        // Gets the current directory.
        char cCurrentPath[FILENAME_MAX];
        _getcwd(cCurrentPath, sizeof(cCurrentPath));
        cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
        folderPath += cCurrentPath;
    #elif __linux__
        char cCurrentPath[FILENAME_MAX];

        if(!getcwd(cCurrentPath, sizeof(cCurrentPath))){
            return string("");
        }
        cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
        folderPath += cCurrentPath;
    #endif
    }
    return folderPath;
}

string GetAbsPath(std::string relPath)
{
    string temp = GetWorkingDirectory();
#ifdef __linux__
    temp += "/";
#elif _WIN32
    temp += "\\";
#endif
    temp += relPath;
    return temp;
}
wxString GetWxAbsPath(std::string relPath)
{
    string temp = GetWorkingDirectory();
#ifdef __linux__
    temp += "/";
#elif _WIN32
    temp += "\\";
#endif
    temp += relPath;
    return wxString(temp.c_str(), wxConvUTF8);
}

// FileGetter
FileGetter::FileGetter(const char* folder)
{
#ifdef _WIN32
    sprintf_s(folderstar,"%s\\*.*", folder);
    hfind = FindFirstFileA(folderstar,&found);
    FindNextFileA(hfind,&found);
#elif __linux__
    struct dirent *de=NULL;
    DIR *d=NULL;
    d=opendir(folder);

    if(d)
    {
        while(de = readdir(d))
        {
            files.push_back(string(de->d_name));
        }
    }
    i = 0;
#endif
}
int FileGetter::getNextFile(char* fname)
{
#ifdef _WIN32
    chk=FindNextFileA(hfind, &found);
    if (chk)
        strcpy(fname, found.cFileName);
    return chk;
#elif __linux__
    if(i<files.size())
    {
        strcpy(fname, files[i].c_str());
        i++;
        return 1;
    }
    else return 0;
#endif
}
