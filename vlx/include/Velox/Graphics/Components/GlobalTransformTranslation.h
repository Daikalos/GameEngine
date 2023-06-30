#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API GlobalTransformTranslation
	{
	public:
		NODISC const Vector2f& GetPosition() const;

	private:
		Vector2f m_position;

		friend class TransformSystem;
	};
}