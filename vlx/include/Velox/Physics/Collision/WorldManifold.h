#pragma once

#include <array>

#include <Velox/System/Vector2.hpp>

namespace vlx
{
	class LocalManifold;
	class SimpleTransform;

	class WorldManifold
	{
	public:
		void Initialize(const LocalManifold& manifold,
			const SimpleTransform& AW, float AR,
			const SimpleTransform& BW, float BR);

		Vector2f				normal;
		std::array<Vector2f, 2> contacts;
		std::array<float, 2>	penetrations;
	};
}