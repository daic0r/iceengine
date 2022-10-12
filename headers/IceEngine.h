#pragma once

#pragma warning( disable : 4251 )

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WINDOWS_SYSTEM
#endif

#ifdef WINDOWS_SYSTEM

#ifdef ICEENGINE_DLL_EXPORTS
#define __declspec(dllexport)
#else
#define __declspec(dllimport)
#endif

#else

#define

#endif