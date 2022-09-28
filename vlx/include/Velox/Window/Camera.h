#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include <unordered_map>
#include <memory>
#include <concepts>
#include <type_traits>

#include "CameraBehavior.h"

namespace vlx
{
	//////////////////////////////////////////////
	// 
	// Expanded camera containing a variety of functionalities,
	// most notably is the ability to contain a multitude of
	// behaviours. Is treated as a unique entity rather than
	// being part of the ECS design.
	// 
	//////////////////////////////////////////////
	class VELOX_API Camera final : public sf::View, private NonCopyable
	{
	private:
		using Stack = typename std::vector<CameraBehavior::Ptr>;
		using Factory = typename std::unordered_map<CameraBehavior::ID, CameraBehavior::Func>;

		enum class Action
		{
			Push,
			Pop,
			Erase,
			Clear
		};

		struct PendingChange
		{
			VELOX_API explicit PendingChange(const Action& action, const CameraBehavior::ID camera_id = -1);

			template<typename... Args>
			void InsertData(Args&&... args)
			{
				data = cu::PackArray(std::forward<Args>(args)...);
			}

			const Action				action;
			const CameraBehavior::ID	camera_id;

			std::vector<std::byte> data;
		};

	public:
		explicit Camera(CameraBehavior::Context context);

		[[nodiscard]] const sf::Transform& GetViewMatrix() const;
		[[nodiscard]] sf::Vector2f ViewToWorld(const sf::Vector2f& position) const;
		[[nodiscard]] sf::Vector2f GetMouseWorldPosition(const sf::WindowBase& window) const;

		[[nodiscard]] const sf::Vector2f& GetPosition() const noexcept;
		[[nodiscard]] const sf::Vector2f& GetScale() const noexcept;
		[[nodiscard]] const sf::Vector2f& GetSize() const noexcept;

		[[nodiscard]] sf::Vector2f GetOrigin() const;

		[[nodiscard]] const CameraBehavior* GetBehavior(const CameraBehavior::ID camera_id) const;
		[[nodiscard]] CameraBehavior* GetBehavior(const CameraBehavior::ID camera_id);

		void SetPosition(const sf::Vector2f& position);
		void SetScale(const sf::Vector2f& scale);
		void SetSize(const sf::Vector2f& size);

	public:
		void HandleEvent(const sf::Event& event);

		void PreUpdate(const Time& time);
		void Update(const Time& time);
		void FixedUpdate(const Time& time);
		void PostUpdate(const Time& time);

		template<typename... Args>
		void Push(const CameraBehavior::ID camera_id, Args&&... args);
		void Pop();
		void Erase(const CameraBehavior::ID camera_id);
		void Clear();

		template<std::derived_from<CameraBehavior> T, typename... Args>
		void RegisterBehavior(const CameraBehavior::ID camera_id, Args&&... args);

	private:
		CameraBehavior::Ptr CreateBehavior(const CameraBehavior::ID camera_id);
		void ApplyPendingChanges();

	private:
		CameraBehavior::Context		m_context;

		sf::Vector2f				m_position;
		sf::Vector2f				m_scale;	 
		sf::Vector2f				m_size;

		mutable sf::Transform		m_transform;
		mutable bool				m_update_transform{true};

		Stack						m_stack;		// stack of camera behaviours
		Factory						m_factory;		// stores funcs for creating camera behaviours
		std::vector<PendingChange>	m_pending_list;
	};

	template<std::derived_from<CameraBehavior> T, typename... Args>
	inline void Camera::RegisterBehavior(const CameraBehavior::ID camera_id, Args&&... args)
	{
		m_factory[camera_id] = [this, &camera_id, &args...]()
		{
			return CameraBehavior::Ptr(new T(camera_id, *this, m_context, std::forward<Args>(args)...));
		};
	}

	template<typename... Args>
	inline void Camera::Push(const CameraBehavior::ID camera_id, Args&&... args)
	{
		const auto& pending = m_pending_list.emplace_back(Action::Push, camera_id);

		if constexpr (Exists<Args...>)
			pending.InsertData(std::forward<Args>(args)...);
	}
}

