#pragma once

#include <Velox/Utility/ArithmeticUtils.h>

namespace vlx
{
	inline constexpr float P_EPSILON					= 0.000001f;

	inline constexpr float P_TIME_TO_SLEEP				= 0.5f;
	inline constexpr float P_VEL_SLEEP_TOLERANCE		= 0.01f;
	inline constexpr float P_ANG_SLEEP_TOLERANCE		= au::ToRadians(2.0f);

	inline constexpr float P_PERCENT					= 0.4f;
	inline constexpr float P_SLOP						= 0.005f;
	inline constexpr float P_MAX_CORRECTION				= 0.2f;
	inline constexpr float P_BAUMGARTE					= 0.2f;

	inline constexpr float P_AABB_INFLATE				= 1.05f;

	inline constexpr float P_POLYGON_RADIUS				= 2.0f * P_SLOP;
}