#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	//////////////////////////////////////////////
	// Basic transform
	//////////////////////////////////////////////
	class VELOX_API Transform : public IComponent
	{ 
	public:
		Transform();
		Transform(const sf::Vector2f& position, const sf::Vector2f& scale, const sf::Angle& rotation);

		~Transform();

	public:
		//////////////////////////////////////////////
		// Returns the global representation of the transform
		//////////////////////////////////////////////
		[[nodiscard]] const sf::Transform& GetTransform() const;
		[[nodiscard]] const sf::Transform& GetInverseTransform() const;
		[[nodiscard]] const sf::Vector2f& GetOrigin() const;
		[[nodiscard]] const sf::Vector2f& GetPosition() const;
		[[nodiscard]] const sf::Vector2f& GetScale() const;
		[[nodiscard]] const sf::Angle& GetRotation() const;

		[[nodiscard]] const sf::Transform& GetLocalTransform() const;
		[[nodiscard]] const sf::Transform& GetInverseLocalTransform() const;
		[[nodiscard]] const sf::Vector2f& GetLocalOrigin() const;
		[[nodiscard]] const sf::Vector2f& GetLocalPosition() const;
		[[nodiscard]] const sf::Vector2f& GetLocalScale() const;
		[[nodiscard]] const sf::Angle& GetLocalRotation() const;

		void SetOrigin(const sf::Vector2f& origin);
		void SetPosition(const sf::Vector2f& position, bool global = true);
		void SetScale(const sf::Vector2f& scale);
		void SetRotation(const sf::Angle angle);

		void Move(const sf::Vector2f& move);
		void Scale(const sf::Vector2f& factor);
		void Rotate(const sf::Angle angle);

	private:
		void UpdateTransforms() const;
		void UpdateRequired() const;

		void ComputeTransform() const;
		void ComputeTransform(const sf::Transform& transform) const;

	private:
		// local space information
		sf::Vector2f			m_origin;
		sf::Vector2f			m_position;
		sf::Vector2f			m_scale;
		sf::Angle				m_rotation;

		mutable sf::Vector2f	m_global_position;
		mutable sf::Vector2f	m_global_scale;
		mutable sf::Angle		m_global_rotation;

		mutable sf::Transform	m_local_transform;
		mutable sf::Transform	m_inverse_local_transform;
		mutable bool			m_update_local_transform		{true};
		mutable bool			m_update_inverse_local_transform{true};

		mutable sf::Transform	m_model_transform; // combined transform of all parents and this (global space)
		mutable sf::Transform	m_inverse_model_transform;
		mutable bool			m_update_model_transform		{true};
		mutable bool			m_update_inverse_model_transform{true};
	};
}