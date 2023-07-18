#pragma once

#include <array>

#include <Velox/System/Vector2.hpp>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API LocalManifold
	{
	public:
		struct Point
		{
			Vector2f	point;
			float		impulse_normal	{0.0f};
			float		impulse_tangent {0.0f};
		};

		enum class Type
		{
			None = -1,
			Circles,
			CircleBox,
			BoxCircle,
			FaceA,
			FaceB
		};

		std::array<Point, 2>	contacts;
		Vector2f				normal;
		Vector2f				point;
		int32					contacts_count	{0};
		Type					type			{Type::None};
	};
}