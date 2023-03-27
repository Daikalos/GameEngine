#pragma once

#include <SFML/Graphics/Transform.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/System/Vector2.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// Local transform of an object. Essential component for many systems.
	///
	class VELOX_API LocalTransform : public IComponent
	{
	public:
		LocalTransform();

		LocalTransform(const Vector2f& position, const Vector2f& scale, const sf::Angle& rotation);
		LocalTransform(const Vector2f& position, const Vector2f& scale);
		LocalTransform(const Vector2f& position, const sf::Angle& rotation);
		LocalTransform(const Vector2f& position);

	public:
		NODISC const sf::Transform&	GetTransform() const;
		NODISC const sf::Transform&	GetInverseTransform() const;
		NODISC const Vector2f&		GetOrigin() const;
		NODISC const Vector2f&		GetPosition() const;
		NODISC const Vector2f&		GetScale() const;
		NODISC const sf::Angle&		GetRotation() const;

		void SetOrigin(const Vector2f& origin);
		void SetPosition(const Vector2f& position);
		void SetScale(const Vector2f& scale);
		void SetRotation(const sf::Angle angle);

		void Move(const Vector2f& move);
		void Scale(const Vector2f& factor);
		void Rotate(const sf::Angle angle);

	private:
		mutable sf::Transform	m_transform;
		mutable sf::Transform	m_inverse_transform;

		Vector2f				m_origin;
		Vector2f				m_position;
		Vector2f				m_scale;
		sf::Angle				m_rotation;

		mutable bool			m_update				{true};
		mutable bool			m_update_inverse		{true};
		mutable bool			m_dirty					{true};

		friend class TransformSystem;
		friend class PhysicsDirtySystem;
	};
}