#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include <unordered_map>
#include <memory>
#include <concepts>
#include <type_traits>

#include "CameraBehaviour.hpp"
#include "Cameras.h"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Expanded camera containing a variety of functionalities,
	// most notably is the ability to contain a multitude of
	// behaviours. Is treated as a unique entity rather than
	// being part of the ECS design.
	////////////////////////////////////////////////////////////
	class VELOX_API_EXPORT Camera final : public sf::View, private NonCopyable
	{
	private:
		using Stack = typename std::vector<CameraBehaviour::Ptr>;
		using Factory = typename std::unordered_map<camera::ID, CameraBehaviour::Func>;

		enum class Action
		{
			Push,
			Pop,
			Erase,
			Clear
		};

		struct PendingChange
		{
			explicit PendingChange(const Action& action, const camera::ID& camera_id = camera::ID::None)
				: action(action), camera_id(camera_id) {}

			const Action		action;
			const camera::ID	camera_id;
		};

	public:
		explicit Camera(CameraBehaviour::Context context);

		[[nodiscard]] constexpr sf::Transform GetViewMatrix() const;
		[[nodiscard]] constexpr sf::Vector2f ViewToWorld(const sf::Vector2f& position) const;
		[[nodiscard]] constexpr sf::Vector2f GetMouseWorldPosition(const sf::WindowBase& window) const;

		[[nodiscard]] constexpr sf::Vector2f GetPosition() const noexcept;
		[[nodiscard]] constexpr sf::Vector2f GetScale() const noexcept;
		[[nodiscard]] constexpr sf::Vector2f GetSize() const noexcept;

		[[nodiscard]] constexpr sf::Vector2f GetOrigin() const;

		[[nodiscard]] CameraBehaviour* GetBehaviour(camera::ID camera_id);
		[[nodiscard]] const CameraBehaviour* GetBehaviour(camera::ID camera_id) const;

		void SetPosition(const sf::Vector2f& position);
		void SetScale(const sf::Vector2f& scale);
		void SetSize(const sf::Vector2f& size);

	public:
		void HandleEvent(const sf::Event& event);

		void PreUpdate(const Time& time);
		void Update(const Time& time);
		void FixedUpdate(const Time& time);
		void PostUpdate(const Time& time, float interp);

		void Push(const camera::ID& camera_id);
		void Pop();
		void Erase(const camera::ID& camera_id);
		void Clear();

		template<std::derived_from<CameraBehaviour> T, typename... Args>
		void RegisterBehaviour(const camera::ID& camera_id, Args&&... args);

	private:
		CameraBehaviour::Ptr CreateBehaviour(const camera::ID& camera_id);
		void ApplyPendingChanges();

	private:
		CameraBehaviour::Context	m_context;

		sf::Vector2f				m_position;
		sf::Vector2f				m_scale;	 
		sf::Vector2f				m_size;

		Stack						m_stack;		// stack of camera behaviours
		Factory						m_factory;		// stores funcs for creating camera behaviours
		std::vector<PendingChange>	m_pending_list; // TODO: ALLOW FOR STORING VARIADIC PARAMETERS AND PASS THEM TO ONCREATE FOR BEHAVIOUR CHILDREN, dunno if possible even, should reconsider structure if not
	};

	template<std::derived_from<CameraBehaviour> T, typename... Args>
	inline void Camera::RegisterBehaviour(const camera::ID& camera_id, Args&&... args)
	{
		m_factory[camera_id] = [this, &camera_id, &args...]()
		{
			return CameraBehaviour::Ptr(new T(camera_id, *this, m_context, std::forward<Args>(args)...));
		};
	}
}

