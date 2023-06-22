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

		Transform(const Vector2f& position, const Vector2f& scale, const sf::Angle& rotation);
		Transform(const Vector2f& position, const Vector2f& scale);
		Transform(const Vector2f& position, const sf::Angle& rotation);
		Transform(const Vector2f& position);

	public:
		NODISC const Mat4f&		GetTransform() const;
		NODISC const Mat4f&		GetInverseTransform() const;
		NODISC const Vector2f&	GetOrigin() const;
		NODISC const Vector2f&	GetPosition() const;
		NODISC const Vector2f&	GetScale() const;
		NODISC sf::Angle		GetRotation() const;

		void SetOrigin(const Vector2f& origin);
		void SetPosition(const Vector2f& position);
		void SetScale(const Vector2f& scale);
		void SetRotation(const sf::Angle angle);

		void Move(const Vector2f& move);
		void Scale(const Vector2f& factor);
		void Rotate(const sf::Angle angle);

	private:
		mutable Mat4f	m_transform;			// TODO: maybe remove inverse, or create new component that caches the transforms, dunno what way is best
		mutable Mat4f	m_inverse_transform;

		Vector2f		m_origin;
		Vector2f		m_position;
		Vector2f		m_scale;
		sf::Angle		m_rotation;

		mutable bool	m_update				{true};
		mutable bool	m_update_inverse		{true};
		mutable bool	m_dirty					{true};

		friend class TransformSystem;
		friend class PhysicsDirtySystem;
	};
}