////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef SFML_WINDOWHANDLE_H
#define SFML_WINDOWHANDLE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Config.h>


////////////////////////////////////////////////////////////
/// Define a low-level window handle type, specific to
/// each platform
////////////////////////////////////////////////////////////
#if defined(CSFML_SYSTEM_WINDOWS)

    // Windows defines a void* handle (HWND)
    typedef void* sfWindowHandle;

#elif defined(CSFML_SYSTEM_LINUX) || defined(CSFML_SYSTEM_FREEBSD)

    // Unix - X11 defines an unsigned integer handle (Window)
    typedef unsigned long sfWindowHandle;

#elif defined(CSFML_SYSTEM_MACOS)

    // Mac OS X defines a void* handle (NSWindow)
	typedef void * sfWindowHandle;

#endif


#endif // SFML_WINDOWHANDLE_H
