#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <unordered_map>
#include <memory>

#include "../utilities/VectorUtilities.h"
#include "../utilities/NonCopyable.h"

#include "input/InputHandler.h"
#include "CameraBehaviour.hpp"
#include "CameraBehaviours.h"

namespace fge
{
	class Camera final : public sf::View, NonCopyable
	{
	private:
		using Stack = typename std::vector<CameraBehaviour::Ptr>;
		using Factory = typename std::unordered_map<Cameras::ID, CameraBehaviour::Func>;

	public:
		enum Action
		{
			Push,
			Pop,
			Clear
		};

	public:
		Camera();

	public:
		sf::Transform GetViewMatrix() const;
		sf::Vector2f ViewToWorld(const sf::Vector2f& position) const;
		sf::Vector2f GetMouseWorldPosition(const sf::RenderWindow& window) const;

		sf::Vector2f GetPosition() const noexcept;
		sf::Vector2f GetScale() const noexcept;
		sf::Vector2f GetSize() const noexcept;

		void SetPosition(const sf::Vector2f& position);
		void SetScale(const sf::Vector2f& scale);
		void SetSize(const sf::Vector2f& size);

	public:
		template<class T, typename... Args>
		void RegisterCamera(const Cameras::ID& camera_id, Args&&... args);

		void HandleEvent(const sf::Event& event);

		void PreUpdate();
		void Update();
		void FixedUpdate();
		void PostUpdate();

		void Push(const Cameras::ID& camera_id);
		void Pop();
		void Clear();

	private:
		CameraBehaviour::Ptr CreateCamera(const Cameras::ID& camera_id);
		void ApplyPendingChanges();

		void SetLetterboxView(int width, int height);

	private:
		sf::Vector2f	m_position;
		sf::Vector2f	m_scale;	 
		sf::Vector2f	m_size;

		Stack			m_stack; // stack of camera behaviours
		Factory			m_factory;
	};

	template<class T, typename ...Args>
	void Camera::RegisterCamera(const Cameras::ID& camera_id, Args && ...args)
	{
		m_factories[camera_id] = [this, &args...]()
		{
			return CameraBehaviour::Ptr(new T(*this, _context, std::forward<Args>(args)...));
		};
	}
}

