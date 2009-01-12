/* $Id$ */
#ifndef __IMPDECL_H
#define __IMPDECL_H

// The following will ensure that we are exporting our C++ classes when 
// building the DLL and importing the classes when build an application 
// using this DLL.

#include "roint_settings.h"

#ifndef __WINDOWS__
#	if defined(WIN32) || defined(WIN64) || defined(_WINDOWS) || defined(_WIN32) || defined(_WIN64)
#		define __WINDOWS__
#	endif
#endif


#ifndef __WINDOWS__
	#define MYLIB_DLLAPI
#else
	#ifdef ROINT_STATIC
		#define MYLIB_DLLAPI
	#else
		#ifdef _MYLIB_DLLAPI_
		    #define MYLIB_DLLAPI  __declspec( dllexport )
		#else
		    #define MYLIB_DLLAPI  __declspec( dllimport )
		#endif
	#endif

// The following will ensure that when building an application (or another
// DLL) using this DLL, the appropriate .LIB file will automatically be used
// when linking.

	#ifndef _MYLIB_NOAUTOLIB_
		#ifdef ROINT_STATIC
			#pragma comment(lib, "roint.lib")
		#else
			#pragma comment(lib, "roint.dll.lib")
		#endif
	#endif
#endif

// Use XML?
#ifdef ROINT_USE_XML
#	define TIXML_USE_STL
#	include "tinyxml/tinyxml.h"
#endif

// zLib tweak
#ifdef __WINDOWS__
#	define ZLIB_WINAPI
#endif


#endif /* __IMPDECL_H */
