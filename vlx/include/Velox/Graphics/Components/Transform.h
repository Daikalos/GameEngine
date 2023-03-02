#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transform.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	///	Represents the global transform of an object
	///
	class VELOX_API Transform : public IComponent
	{ 
	public:
		NODISC const sf::Transform&	GetTransform() const noexcept;
		NODISC const sf::Transform&	GetInverseTransform() const;
		NODISC const sf::Vector2f&	GetPosition() const;
		NODISC const sf::Vector2f&	GetScale() const;
		NODISC const sf::Angle&		GetRotation() const;

	private:
		mutable sf::Vector2f	m_position;
		mutable sf::Vector2f	m_scale;
		mutable sf::Angle		m_rotation;

		mutable sf::Transform	m_transform;			// combined transform of all parents and this (global space)
		mutable sf::Transform	m_inverse_transform;

		mutable bool			m_update_position		{true};
		mutable bool			m_update_scale			{true};
		mutable bool			m_update_rotation		{true};
		mutable bool			m_update_inverse		{true};
		mutable bool			m_dirty					{true};

		friend class TransformSystem;
	};
}