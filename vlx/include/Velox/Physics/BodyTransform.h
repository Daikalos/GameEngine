#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// Only used for tracking the previous position and angle
	/// 
	class VELOX_API BodyTransform
	{
	public:
		const Vector2f& GetPosition() const noexcept;
		sf::Angle GetRotation() const noexcept;

	private:
		Vector2f	m_position;
		sf::Angle	m_rotation;

		friend class PhysicsSystem;
		friend class RenderSystem;
		friend class CollisionSolver;
	};
}