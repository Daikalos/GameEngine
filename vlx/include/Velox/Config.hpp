#pragma once

#if defined(_WIN32)
#	define VELOX_SYSTEM_WIN

#	define VELOX_API_EXPORT __declspec(dllexport)
#	define VELOX_API_IMPORT __declspec(dllimport)

#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#endif

#if !defined(VELOX_API)
#	if defined(DLL_EXPORT)
#		define VELOX_API VELOX_API_EXPORT
#	else
#		define VELOX_API VELOX_API_IMPORT
#	endif
#endif

#if !defined(NDEBUG)
#	define VELOX_DEBUG
#endif

#if defined(_MSC_VER)
#   define VELOX_PRETTY_FUNCTION __FUNCSIG__
#elif defined(__clang__) || defined(__GNUC__)
#   define VELOX_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif

#define NODISC [[nodiscard]]
#define UNUSED [[maybe_unused]]

#define _USE_MATH_DEFINES

inline constexpr int TRIANGLE_COUNT = 3;
inline constexpr float PHYSICS_EPSILON = 0.0001f;

#include <cstdint>