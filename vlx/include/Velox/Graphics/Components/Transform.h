#pragma once

#include <Velox/ECS/IComponent.h>
#include <Velox/System/Vector2.hpp>
#include <Velox/System/Mat4f.hpp>
#include <Velox/Utility/ArithmeticUtils.h>
#include <Velox/Config.hpp>

namespace vlx
{
	///	Represents the global transform of an object
	///
	class VELOX_API Transform : public IComponent
	{ 
	public:
		NODISC const Mat4f&		GetTransform() const noexcept;
		NODISC const Mat4f&		GetInverseTransform() const;
		NODISC const Vector2f&	GetPosition() const;
		NODISC const Vector2f&	GetScale() const;
		NODISC const sf::Angle&	GetRotation() const;

	private:
		mutable Mat4f		m_transform;			// combined transform of all parents and this (global space)
		mutable Mat4f		m_inverse_transform;

		mutable Vector2f	m_position;
		mutable Vector2f	m_scale;
		mutable sf::Angle	m_rotation;

		mutable bool		m_update_position		{true};
		mutable bool		m_update_scale			{true};
		mutable bool		m_update_rotation		{true};
		mutable bool		m_update_inverse		{true};
		mutable bool		m_dirty					{true};

		friend class TransformSystem;
		friend class PhysicsDirtySystem;
	};
}