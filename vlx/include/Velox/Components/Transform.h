#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "Relation.h"

namespace vlx
{
	static constexpr int MATRIX_WIDTH = 4;

	/// <summary>
	///		Represents the transform
	/// </summary>
	class VELOX_API Transform : public IComponent
	{ 
	public:
		Transform();

		Transform(const sf::Vector2f& position, const sf::Vector2f& scale, const sf::Angle& rotation);
		Transform(const sf::Vector2f& position, const sf::Vector2f& scale);
		Transform(const sf::Vector2f& position, const sf::Angle& rotation);
		Transform(const sf::Vector2f& position);

	public:
		/// <summary>
		///		Returns the global representation of the transform
		/// </summary>
		[[nodiscard]] const sf::Transform&	GetTransform() const;
		[[nodiscard]] const sf::Transform&	GetInverseTransform() const;
		[[nodiscard]] const sf::Vector2f&	GetOrigin() const;
		[[nodiscard]] const sf::Vector2f&	GetPosition() const;
		[[nodiscard]] const sf::Vector2f&	GetScale() const;
		[[nodiscard]] const sf::Angle&		GetRotation() const;

		[[nodiscard]] const sf::Transform&	GetLocalTransform() const;
		[[nodiscard]] const sf::Transform&	GetInverseLocalTransform() const;
		[[nodiscard]] const sf::Vector2f&	GetLocalPosition() const;
		[[nodiscard]] const sf::Vector2f&	GetLocalScale() const;
		[[nodiscard]] const sf::Angle&		GetLocalRotation() const;

		void SetOrigin(const sf::Vector2f& origin);
		void SetPosition(const sf::Vector2f& position);
		void SetScale(const sf::Vector2f& scale);
		void SetRotation(const sf::Angle angle);

		void Move(const sf::Vector2f& move);
		void Scale(const sf::Vector2f& factor);
		void Rotate(const sf::Angle angle);

	private:
		// local space information
		sf::Vector2f			m_origin;
		sf::Vector2f			m_position;
		sf::Vector2f			m_scale;
		sf::Angle				m_rotation;

		mutable sf::Vector2f	m_global_position;
		mutable sf::Vector2f	m_global_scale;
		mutable sf::Angle		m_global_rotation;

		mutable bool			m_update_position			{true};
		mutable bool			m_update_scale				{true};
		mutable bool			m_update_rotation			{true};

		mutable sf::Transform	m_local_transform;
		mutable sf::Transform	m_local_inverse_transform;
		mutable bool			m_update_local				{true};
		mutable bool			m_update_local_inverse		{true};

		mutable sf::Transform	m_global_transform;			// combined transform of all parents and this (global space)
		mutable sf::Transform	m_global_inverse_transform;
		mutable bool			m_update_global				{true};
		mutable bool			m_update_global_inverse		{true};

		mutable bool			m_dirty						{true};

		friend class TransformSystem;
	};
}