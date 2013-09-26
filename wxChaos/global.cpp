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

#ifdef USE_BOOST
#include <boost/filesystem.hpp>
#endif

bool usingLocalDirect = false;


#ifdef USE_BOOST
#ifdef _WIN32
void copyDir(const boost::filesystem::path& source, const boost::filesystem::path& dest)
{
    namespace fs = boost::filesystem;
    try
	{
        // Check whether the function call is valid.
        if(!fs::exists(source) || !fs::is_directory(source))
		{
            return;
        }
        if(!fs::exists(dest))
		{
            if(!fs::create_directory(dest))
			{
				return;
			}
        }
    }
    catch(fs::filesystem_error&)
	{
        return;
    }
    // Iterate through the source directory.
    for(fs::directory_iterator it(source); it != fs::directory_iterator(); it++)
	{
        fs::path current(it->path());
        if(fs::is_directory(current))
		{
            // Found directory: Recursion.
            copyDir(current,
                dest.string() + "/" + current.filename().string());
        }
        else
		{
            // Found file: Copy.
            fs::copy_file(current,
                fs::path(dest.string() + "/" + current.filename().string()), fs::copy_option::overwrite_if_exists);
        }
    }
}
#endif
#endif //USE_BOOST

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
		#ifdef USE_BOOST
			// Gets the current directory.
			string sourcePath = "";
			char cCurrentPath[FILENAME_MAX];
			_getcwd(cCurrentPath, sizeof(cCurrentPath));
			cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
			sourcePath += cCurrentPath;

			_mkdir(folderPath.c_str());
			copyDir(boost::filesystem::path(sourcePath+string("/Doc/")), boost::filesystem::path(folderPath+string("/Doc/")));
			copyDir(boost::filesystem::path(sourcePath+string("/Resources/")), boost::filesystem::path(folderPath+string("/Resources/")));
			copyDir(boost::filesystem::path(sourcePath+string("/UserScripts/")), boost::filesystem::path(folderPath+string("/UserScripts/")));
			copyDir(boost::filesystem::path(sourcePath+string("/ScriptSamples/")), boost::filesystem::path(folderPath+string("/ScriptSamples/")));
			boost::filesystem::copy(boost::filesystem::path(sourcePath+string("/config.ini")), boost::filesystem::path(folderPath+string("/config.ini")));
			usingLocalDirect = false;
		#endif
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
