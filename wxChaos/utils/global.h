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
const char APP_VERSION[] = "1.2.0";
#define wxcLANG_ENGLISH
//#define wxcLANG_SPANISH

// End app parameters.
// ===================

// ===================
// Set up language


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