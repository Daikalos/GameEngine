#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	//////////////////////////////////////////////
	// Basic transform that inherits from sf::Transformable
	//////////////////////////////////////////////
	struct VELOX_API Transform
	{ 
	public:
		Transform();
		~Transform();

	public:
		//////////////////////////////////////////////
		// Returns the global representation of the transform
		//////////////////////////////////////////////
		const sf::Transform& GetTransform() const;
		const sf::Transform& GetLocalTransform() const;

		[[nodiscard]] const sf::Vector2f& GetPosition() const;
		[[nodiscard]] const sf::Vector2f& GetScale() const;
		[[nodiscard]] const sf::Angle& GetRotation() const;

		void SetPosition(const sf::Vector2f& position);
		void SetScale(const sf::Vector2f& scale);
		void SetRotation(float degrees);

	public: // parent/child behaviour
		[[nodiscard]] constexpr bool HasParent() const noexcept;

		const Transform& GetParent() const;
		Transform& GetParent();

	public:
		void AttachChild(Transform& child);
		Transform* DetachChild(Transform& child);

	private:
		const Transform* GetTopParent() const;
		void UpdateRequired() const;

	private:
		sf::Transformable		m_transform;

		mutable sf::Transform	m_global_transform; // combined transform of all parents and this
		mutable bool			m_update_transform{true};

		std::vector<Transform*> m_children;
		Transform*				m_parent{nullptr};
	};
}