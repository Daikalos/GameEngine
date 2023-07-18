#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <concepts>
#include <type_traits>

#include <SFML/Graphics/View.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <SFML/Window/Event.hpp>

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Mat4f.hpp>

#include <Velox/Utility/ContainerUtils.h>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "CameraBehavior.h"

namespace vlx
{
	/// Expanded camera containing a variety of functionalities, most notably is the ability to contain 
	/// a multitude of behaviours. Treated as a unique entity rather than being part of the ECS design.
	/// 
	class VELOX_API Camera final : public sf::View, private NonCopyable
	{
	private:
		using BehaviorID		= typename CameraBehavior::ID;
		using BehaviorPtr		= typename CameraBehavior::Ptr;
		using BehaviorFunc		= typename CameraBehavior::Func;
		using BehaviorContext	= typename CameraBehavior::Context;

		using Stack = typename std::vector<BehaviorPtr>;
		using Factory = typename std::unordered_map<BehaviorID, BehaviorFunc>;

		enum class Action : int8
		{
			None = -1,
			Push,
			Pop,
			Erase,
			Clear
		};

		struct PendingChange
		{
			VELOX_API explicit PendingChange(Action action, BehaviorID behavior_id = 0);

			template<typename... Args>
			void InsertData(Args&&... args)
			{
				data = cu::PackArray(std::forward<Args>(args)...);
			}

			Action		action		{Action::None};
			BehaviorID	behavior_id	{0};

			std::vector<std::byte> data;
		};

		using PendingList = std::vector<PendingChange>;

	public:
		explicit Camera(const BehaviorContext& context);

		NODISC const Mat4f& GetViewMatrix() const;
		NODISC Vector2f ViewToWorld(const Vector2f& position) const;
		NODISC Vector2f LocalToWorldPosition(const Vector2f& local_pos) const;
		NODISC Vector2i LocalToWorldPosition(const Vector2i& local_pos) const;
		NODISC Vector2f MouseWorldPosition(const sf::WindowBase& window) const;

		NODISC const Vector2f& GetPosition() const noexcept;
		NODISC const Vector2f& GetScale() const noexcept;
		NODISC const Vector2f& GetSize() const noexcept;

		NODISC Vector2f GetOrigin() const;

		NODISC CameraBehavior* GetBehavior(BehaviorID behavior_id);
		NODISC const CameraBehavior* GetBehavior(BehaviorID behavior_id) const;

		void SetPosition(const Vector2f& position);
		void SetScale(const Vector2f& scale);
		void SetSize(const Vector2f& size);

	public:
		void HandleEvent(const sf::Event& event);

		void Start(const Time& time);
		void PreUpdate(const Time& time);
		void Update(const Time& time);
		void FixedUpdate(const Time& time);
		void PostUpdate(const Time& time);

		template<typename... Args>
		void Push(BehaviorID behavior_id, Args&&... args);
		void Pop();
		void Erase(BehaviorID behavior_id);
		void Clear();

		void ApplyPendingChanges();

		template<std::derived_from<CameraBehavior> T, typename... Args>
			requires std::constructible_from<T, typename Camera::BehaviorID, Camera&, const typename Camera::BehaviorContext&, Args...> // looks stupid but works
		void RegisterBehavior(BehaviorID behavior_id, Args&&... args);

	private:
		auto CreateBehavior(BehaviorID behavior_id) -> BehaviorPtr;

	private:
		BehaviorContext	m_context;

		Vector2f		m_position;
		Vector2f		m_scale;	 
		Vector2f		m_size;

		mutable Mat4f	m_transform;
		mutable bool	m_update_transform{true};

		Stack			m_stack;		// stack of camera behaviours
		Factory			m_factory;		// stores funcs for creating camera behaviours
		PendingList		m_pending_list;
	};

	template<std::derived_from<CameraBehavior> T, typename... Args>
		requires std::constructible_from<T, typename Camera::BehaviorID, Camera&, const typename Camera::BehaviorContext&, Args...>
	inline void Camera::RegisterBehavior(BehaviorID behavior_id, Args&&... args)
	{
		m_factory[behavior_id] = [this, behavior_id, ... args = std::forward<Args>(args)]()
		{
			return std::make_unique<T>(behavior_id, *this, m_context, args...);
		};
	}

	template<typename... Args>
	inline void Camera::Push(BehaviorID behavior_id, Args&&... args)
	{
		const auto& pending = m_pending_list.emplace_back(Action::Push, behavior_id);

		if constexpr (Exists<Args...>)
			pending.InsertData(std::forward<Args>(args)...);
	}
}

