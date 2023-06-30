#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// Only used for tracking the previous position and angle
	/// 
	class VELOX_API PhysicsBodyTransform
	{
	public:
		const Vector2f& GetLastPosition() const noexcept;
		sf::Angle GetLastRotation() const noexcept;

	private:
		Vector2f	m_last_pos;
		sf::Angle	m_last_rot;

		friend class PhysicsSystem;
		friend class RenderSystem;
		friend class CollisionArbiter;
	};
}