/*
*   DSFML - SFML Library binding in D language.
*   Copyright (C) 2008 Julien Dagorn (sirjulio13@gmail.com)
*
*   This software is provided 'as-is', without any express or
*   implied warranty. In no event will the authors be held
*   liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose,
*   including commercial applications, and to alter it and redistribute
*   it freely, subject to the following restrictions:
*
*   1.  The origin of this software must not be misrepresented;
*       you must not claim that you wrote the original software.
*       If you use this software in a product, an acknowledgment
*       in the product documentation would be appreciated but
*       is not required.
*
*   2.  Altered source versions must be plainly marked as such,
*       and must not be misrepresented as being the original software.
*
*   3.  This notice may not be removed or altered from any
*       source distribution.
*/

module dsfml.system.dllloader;

import dsfml.system.stringutil;

version (Tango)
{
    import tango.io.Console;
    import tango.sys.SharedLib;
}
else
{
    import std.stdio;
    
    version (Windows)
    {
        import std.c.windows.windows;
        alias HMODULE MODULEHANDLE; 
    }
    version (linux)
    {
        import std.c.linux.linux;
        alias void* MODULEHANDLE;
        
        const int RTLD_NOW = 0x00002;
        const int RTLD_GLOBAL = 0x00100;
    }
}

static this()
{
    version (Tango)
    {
        SharedLib.throwExceptions = false;
    }
}

private void report(char[] msg, char[] lib, char[] symb)
{
    char[] str = "Loading error. Reason : " ~ msg ~ " (library : " ~ lib ~ ", symbol : " ~ symb ~ ")";
    version (Tango)
    {
        Cerr(str).newline; 
    }
    else
    {
        fwritefln(stderr, str);
    }
}


/**
*   Simple Dll loader.
*/
class DllLoader
{  
    static DllLoader load(char[] library)
    {
        version (Windows)
        {
            char[] libraryName = library ~ ".dll";
        }
        version (linux)
        {
            char[] libraryName = "lib" ~ library ~ ".so";
        }
        
        if (libraryName in alreadyLoaded)
        {
            return alreadyLoaded[libraryName];
        }
        else
        {
            DllLoader temp = new DllLoader(libraryName);
            alreadyLoaded[libraryName] = temp;
            return temp;
        }       
    }
    
    ~this()
    {
        close();
    }

    void* getSymbol(char[] symbolName)
    {
        void* symb;
        version (Tango)
        {
            symb = m_lib.getSymbol(toStringz(symbolName));
        }
        else
        {                
            version (Windows)
            {
                symb = GetProcAddress(m_lib, toStringz(symbolName));
            }
            version (linux)
            {
                symb = dlsym(m_lib, toStringz(symbolName));
            }
        }
    
        if (symb is null)
            report( "Symbol cannot be found in specified library", m_libPath, symbolName);
            
        return symb;
    }

    void close()
    {
        version (Tango)
        {
            m_lib.unload();
        }
        else
        {
            version (Windows)
            {
                FreeLibrary(m_lib);
            }
            version (linux)
            {
                dlclose(m_lib);
            }
            alreadyLoaded.remove(this.m_libPath);
        }
    }

private:
    this(char[] libraryPath)
    {
        m_libPath = libraryPath;
        
        version (Tango)
        {
            m_lib = SharedLib.load(libraryPath);
        }
        else
        {
            version (Windows)
            {
                m_lib = LoadLibraryA(toStringz(libraryPath));
            }
            version (linux)
            {
                m_lib = dlopen(toStringz(libraryPath), RTLD_NOW | RTLD_GLOBAL);
            }
        }
        if (m_lib is null)
            report("Cannot open library", m_libPath, null);
    }
    
    version (Tango)
    {
        SharedLib m_lib;
    }
    else
    {
        MODULEHANDLE m_lib; 
    }
    
    static DllLoader[char[]] alreadyLoaded;
    char[] m_libPath;
}
