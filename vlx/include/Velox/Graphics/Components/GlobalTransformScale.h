#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API GlobalTransformScale
	{
	public:
		NODISC const Vector2f& GetScale() const;

	private:
		Vector2f m_scale;

		friend class GlobalTransformSystem;
	};
}