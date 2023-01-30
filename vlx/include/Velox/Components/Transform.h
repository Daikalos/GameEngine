#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "Relation.h"

namespace vlx
{
	/// <summary>
	///		Represents the final transform
	/// </summary>
	class VELOX_API Transform : public IComponent
	{ 
	public:
		[[nodiscard]] const sf::Transform&	GetTransform() const;
		[[nodiscard]] const sf::Transform&	GetInverseTransform() const;
		[[nodiscard]] const sf::Vector2f&	GetPosition() const;
		[[nodiscard]] const sf::Vector2f&	GetScale() const;
		[[nodiscard]] const sf::Angle&		GetRotation() const;

	private:
		mutable sf::Vector2f	m_position;
		mutable sf::Vector2f	m_scale;
		mutable sf::Angle		m_rotation;

		mutable bool			m_update_position		{true};
		mutable bool			m_update_scale			{true};
		mutable bool			m_update_rotation		{true};

		mutable sf::Transform	m_transform;			// combined transform of all parents and this (global space)
		mutable sf::Transform	m_inverse_transform;
		mutable bool			m_update_inverse		{true};

		mutable bool			m_dirty					{true};

		friend class TransformSystem;
	};
}