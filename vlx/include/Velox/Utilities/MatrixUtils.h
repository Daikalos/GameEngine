#pragma once

#include <SFML/Graphics/Transform.hpp>

#include <Velox/Config.hpp>

namespace vlx::mu
{
	static constexpr int M = 4; // width
	static constexpr int N = 4; // height

	NODISC static constexpr int IX(const int x, const int y)
	{
		return x + y * M;
	}

	NODISC static constexpr float MV(const float* m, const int x, const int y)
	{
		return m[IX(x, y)];
	}

	NODISC static constexpr sf::Transform Transpose(const sf::Transform& transform)
	{
		const float* m = transform.getMatrix();

		return sf::Transform(
			MV(m, 0, 0), MV(m, 1, 0), MV(m, 3, 0),
			MV(m, 0, 1), MV(m, 1, 1), MV(m, 3, 1),
			MV(m, 0, 3), MV(m, 1, 3), MV(m, 3, 3));
	}

}