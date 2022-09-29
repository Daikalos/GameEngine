#pragma once

#if defined(_WIN32)

#define VELOX_SYSTEM_WIN

#ifndef NOMINMAX
#define NOMINMAX
#endif

#endif

#if !defined(NDEBUG)
	#define VELOX_DEBUG
#endif

#if defined(_WIN32)

#define VELOX_API_EXPORT __declspec(dllexport)
#define VELOX_API_IMPORT __declspec(dllimport)

#if defined(DLL_EXPORT)

#define VELOX_API VELOX_API_EXPORT
	
#else

#define VELOX_API VELOX_API_IMPORT

#endif

#endif