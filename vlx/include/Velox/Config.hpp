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

#endif