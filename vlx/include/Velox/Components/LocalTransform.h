#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API LocalTransform : public IComponent
	{
	public:
		LocalTransform();

		LocalTransform(const sf::Vector2f& position, const sf::Vector2f& scale, const sf::Angle& rotation);
		LocalTransform(const sf::Vector2f& position, const sf::Vector2f& scale);
		LocalTransform(const sf::Vector2f& position, const sf::Angle& rotation);
		LocalTransform(const sf::Vector2f& position);

	public:
		[[nodiscard]] const sf::Transform&	GetTransform() const;
		[[nodiscard]] const sf::Transform&	GetInverseTransform() const;
		[[nodiscard]] const sf::Vector2f&	GetOrigin() const;
		[[nodiscard]] const sf::Vector2f&	GetPosition() const;
		[[nodiscard]] const sf::Vector2f&	GetScale() const;
		[[nodiscard]] const sf::Angle&		GetRotation() const;

		void SetOrigin(const sf::Vector2f& origin);
		void SetPosition(const sf::Vector2f& position);
		void SetScale(const sf::Vector2f& scale);
		void SetRotation(const sf::Angle angle);

		void Move(const sf::Vector2f& move);
		void Scale(const sf::Vector2f& factor);
		void Rotate(const sf::Angle angle);

	private:
		sf::Vector2f			m_origin;
		sf::Vector2f			m_position;
		sf::Vector2f			m_scale;
		sf::Angle				m_rotation;

		mutable sf::Transform	m_transform;
		mutable sf::Transform	m_inverse_transform;
		mutable bool			m_update				{true};
		mutable bool			m_update_inverse		{true};

		mutable bool			m_dirty					{true};

		friend class TransformSystem;
	};
}