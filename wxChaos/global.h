/**
* @file global.h
* @brief Global functions and variables.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 9/22/2012
*/

#pragma once
#ifndef _global
#define _global

#ifdef _WIN32
#include <WTypes.h>
#elif __linux__
#include <vector>
#include <string>
#endif

// ===================
// Begin app parameters.
const char APP_VERSION[] = "1.1.0";

#define SETUP_VERSION false
#define FORCE_APP_DIRECT true

#define wxcLANG_ENGLISH
//#define wxcLANG_SPANISH

// End app parameters.
// ===================

// ===================
// Various config fixes.
#ifndef USE_BOOST
#ifdef _WIN32
#undef SETUP_VERSION
#undef FORCE_APP_DIRECT
#define SETUP_VERSION false
#define FORCE_APP_DIRECT true
#endif
#endif

#ifdef BUILD_SETUP
#undef SETUP_VERSION
#define SETUP_VERSION true
#undef FORCE_APP_DIRECT
#define FORCE_APP_DIRECT false
#endif

#ifdef USE_LANG_ENGLISH
#undef wxcLANG_SPANISH
#define wxcLANG_ENGLISH
#endif

#ifdef USE_LANG_SPANISH
#undef wxcLANG_ENGLISH
#define wxcLANG_SPANISH
#endif

#include "Lang.h"
// ===================

#endif