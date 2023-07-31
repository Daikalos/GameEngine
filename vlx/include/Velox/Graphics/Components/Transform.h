#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Mat4f.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// Local transform of an object.
	///
	class VELOX_API Transform
	{
	public:
		Transform();

		Transform(const Vector2f& position, const Vector2f& scale, sf::Angle rotation);
		Transform(const Vector2f& position, const Vector2f& scale);
		Transform(const Vector2f& position, sf::Angle rotation);
		Transform(const Vector2f& position);

	public:
		NODISC const Vector2f&	GetOrigin() const;
		NODISC const Vector2f&	GetPosition() const;
		NODISC const Vector2f&	GetScale() const;
		NODISC sf::Angle		GetRotation() const;

		void SetOrigin(const Vector2f& origin);
		void SetPosition(const Vector2f& position);
		void SetScale(const Vector2f& scale);
		void SetRotation(sf::Angle angle);

		void Move(const Vector2f& move);
		void Scale(const Vector2f& factor);
		void Rotate(sf::Angle angle);

	private:
		Vector2f	m_origin;
		Vector2f	m_position;
		Vector2f	m_scale;
		sf::Angle	m_rotation;

		bool		m_update_pos		{true};
		bool		m_update_rot		{true};
		bool		m_dirty				{true};

		friend class LocalTransformSystem;
		friend class GlobalTransformSystem;
		friend class PhysicsDirtySystem;
	};
}