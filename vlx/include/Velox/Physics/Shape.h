#pragma once

namespace vlx
{
	struct Shape
	{
		enum Type : short
		{
			Circle,
			AABB,
			Convex,

			Count // always keep last
		};

		virtual constexpr Type GetType() const noexcept = 0;
	};
}